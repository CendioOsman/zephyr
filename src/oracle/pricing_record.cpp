// Copyright (c) 2019, Haven Protocol
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Portions of this code based upon code Copyright (c) 2019, The Monero Project

#include "pricing_record.h"

#include "serialization/keyvalue_serialization.h"
#include "storages/portable_storage.h"

#include "string_tools.h"
namespace oracle
{

  namespace
  {
    struct pr_serialized
    {
      uint64_t spot;
      uint64_t moving_average;
      uint64_t stable;
      uint64_t stable_ma;
      uint64_t reserve;
      uint64_t reserve_ma;
      uint64_t timestamp;
      std::string signature;

      BEGIN_KV_SERIALIZE_MAP()
        KV_SERIALIZE(spot)
        KV_SERIALIZE(moving_average)
        KV_SERIALIZE(stable)
        KV_SERIALIZE(stable_ma)
        KV_SERIALIZE(reserve)
        KV_SERIALIZE(reserve_ma)
        KV_SERIALIZE(timestamp)
        KV_SERIALIZE(signature)
      END_KV_SERIALIZE_MAP()
    };
  }
  
  pricing_record::pricing_record() noexcept
    : spot(0)
    , moving_average(0)
    , stable(0)
    , stable_ma(0)
    , reserve(0)
    , reserve_ma(0)
    , timestamp(0)
  {
    std::memset(signature, 0, sizeof(signature));
  }

  bool pricing_record::_load(epee::serialization::portable_storage& src, epee::serialization::section* hparent)
  {
    pr_serialized in{};
    if (in._load(src, hparent))
    {
      // Copy everything into the local instance
      spot = in.spot;
      moving_average = in.moving_average;
      stable = in.stable;
      stable_ma = in.stable_ma;
      reserve = in.reserve;
      reserve_ma = in.reserve_ma;
      timestamp = in.timestamp;
      for (unsigned int i = 0; i < in.signature.length(); i += 2) {
        std::string byteString = in.signature.substr(i, 2);
        signature[i>>1] = (char) strtol(byteString.c_str(), NULL, 16);
      }
      return true;
    }

    // Report error here?
    return false;
  }

  bool pricing_record::store(epee::serialization::portable_storage& dest, epee::serialization::section* hparent) const
  {
    std::string sig_hex;
    for (unsigned int i=0; i<64; i++) {
      std::stringstream ss;
      ss << std::hex << std::setw(2) << std::setfill('0') << (0xff & signature[i]);
      sig_hex += ss.str();
    }
    const pr_serialized out{spot,moving_average,stable,stable_ma,reserve,reserve_ma,timestamp,sig_hex};
    return out.store(dest, hparent);
  }

  pricing_record::pricing_record(const pricing_record& orig) noexcept
    : spot(orig.spot)
    , moving_average(orig.moving_average)
    , stable(orig.stable)
    , stable_ma(orig.stable_ma)
    , reserve(orig.reserve)
    , reserve_ma(orig.reserve_ma)
    , timestamp(orig.timestamp)
  {
    std::memcpy(signature, orig.signature, sizeof(signature));
  }

  pricing_record& pricing_record::operator=(const pricing_record& orig) noexcept
  {
    spot = orig.spot;
    moving_average = orig.moving_average;
    stable = orig.stable;
    stable_ma = orig.stable_ma;
    reserve = orig.reserve;
    reserve_ma = orig.reserve_ma;
    timestamp = orig.timestamp;
    ::memcpy(signature, orig.signature, sizeof(signature));
    return *this;
  }
  
  bool pricing_record::equal(const pricing_record& other) const noexcept
  {
    return ((spot == other.spot) &&
      (moving_average == other.moving_average) &&
      (stable == other.stable) &&
      (stable_ma == other.stable_ma) &&
      (reserve == other.reserve) &&
      (reserve_ma == other.reserve_ma) &&
	    (timestamp == other.timestamp) &&
      !::memcmp(signature, other.signature, sizeof(signature)));
  }

  bool pricing_record::empty() const noexcept
  {
    const pricing_record empty_pr = oracle::pricing_record();
    return (*this).equal(empty_pr);
  }

  bool pricing_record::verifySignature(const std::string& public_key) const
  {
    CHECK_AND_ASSERT_THROW_MES(!public_key.empty(), "Pricing record verification failed. NULL public key. PK Size: " << public_key.size()); // TODO: is this necessary or the one below already covers this case, meannin it will produce empty pubkey?

    // extract the key
    EVP_PKEY* pubkey;
    BIO* bio = BIO_new_mem_buf(public_key.c_str(), public_key.size());
    if (!bio) {
      return false;
    }
    pubkey = PEM_read_bio_PUBKEY(bio, NULL, NULL, NULL);
    BIO_free(bio);
    CHECK_AND_ASSERT_THROW_MES(pubkey != NULL, "Pricing record verification failed. NULL public key.");

    // Convert our internal 64-byte binary representation into 128-byte hex string
    std::string sig_hex;
    for (unsigned int i=0; i<64; i++) {
      std::stringstream ss;
      ss << std::hex << std::setw(2) << std::setfill('0') << (0xff & signature[i]);
      sig_hex += ss.str();
    }

    // Build the JSON string, so that we can verify the signature
    std::ostringstream oss;
    oss << "{\"spot\":" << spot;
    oss << ",\"moving_average\":" << moving_average;
    oss << ",\"timestamp\":" << timestamp;
    oss << "}";
    std::string message = oss.str();

    // Create a verify digest from the message
    EVP_MD_CTX *ctx = EVP_MD_CTX_create();
    int ret = 0;
    if (ctx) {
      ret = EVP_DigestVerifyInit(ctx, NULL, EVP_sha256(), NULL, pubkey);
      if (ret == 1) {
        ret = EVP_DigestVerifyUpdate(ctx, message.data(), message.length());
        if (ret == 1) {
          ret = EVP_DigestVerifyFinal(ctx, (const unsigned char *)signature, 64);
        }
      }
    }

    // Cleanup the context we created
    EVP_MD_CTX_destroy(ctx);
    // Cleanup the openssl stuff
    EVP_PKEY_free(pubkey);

    if (ret == 1)
      return true;

    // Get the errors from OpenSSL
    ERR_print_errors_fp (stderr);

    return false;
  }

  bool pricing_record::has_missing_rates() const noexcept
  {
    return (spot == 0) || (moving_average == 0) || (stable == 0) || (stable_ma == 0) || (reserve == 0) || (reserve_ma == 0);
  }

  // overload for pr validation for block
  bool pricing_record::valid(cryptonote::network_type nettype, uint32_t hf_version, uint64_t bl_timestamp, uint64_t last_bl_timestamp) const 
  {
    if (hf_version < HF_VERSION_DJED) {
      if (!this->empty())
        return false;
    }

    if (this->empty())
        return true;

    if (this->has_missing_rates()) {
      LOG_ERROR("Pricing record has missing rates.");
      return false;
    }

    if (!verifySignature(get_config(nettype).ORACLE_PUBLIC_KEY)) {
      LOG_ERROR("Invalid pricing record signature.");
      return false;
    }

    // validate the timestmap
    if (this->timestamp > bl_timestamp + PRICING_RECORD_VALID_TIME_DIFF_FROM_BLOCK) {
      LOG_ERROR("Pricing record timestamp is too far in the future.");
      return false;
    }

    if (this->timestamp <= last_bl_timestamp) {
      LOG_ERROR("Pricing record timestamp: " << this->timestamp << ", block timestamp: " << bl_timestamp);
      LOG_ERROR("Pricing record timestamp is too old.");
      return false;
    }

    return true;
  }
}
