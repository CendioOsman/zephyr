// Copyright (c) 2023, Zephyr Protocol
// Portions copyright (c) 2019-2021, Haven Protocol
// Portions copyright (c) 2016-2019, The Monero Project
// 
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

#include "gtest/gtest.h"
#include "oracle/pricing_record.h"

TEST(oracle, empty_pricing_record_valid)
{
  oracle::pricing_record pr;
  EXPECT_TRUE(pr.valid(cryptonote::network_type::TESTNET, 3, 1691041762, 1691040762));
}

TEST(oracle, fail_if_empty_signature)
{
  oracle::pricing_record pr;
  pr.spot = 1;
  pr.moving_average = 1;
  pr.stable = 1;
  pr.stable_ma = 1;
  pr.reserve = 1;
  pr.reserve_ma = 1;
  pr.timestamp = 1;
  EXPECT_FALSE(pr.valid(cryptonote::network_type::TESTNET, 3, 1691030038, 1691029038));
}

TEST(oracle, pricing_record_from_oracle_valid_signature)
{
  oracle::pricing_record pr;
  pr.spot = 2915484310000;
  pr.moving_average = 2924650120000;
  pr.timestamp = 1691040826;
  std::string sig = "a4eebd24d684240635f8f0dae4347a87f951ff8220495f6982e4e52359bc1fb8028b11e02e4ddea503b3c175984836e90e4f65599ab2b1fa632ccb4a915a95f9";
  int j=0;
  for (unsigned int i = 0; i < sig.size(); i += 2) {
    std::string byteString = sig.substr(i, 2);
    pr.signature[j++] = (char) strtol(byteString.c_str(), NULL, 16);
  }

  EXPECT_TRUE(pr.verifySignature(get_config(cryptonote::network_type::TESTNET).ORACLE_PUBLIC_KEY));
}

TEST(oracle, modified_pricing_record_from_oracle_invalid_signature)
{
  oracle::pricing_record pr;
  pr.spot = 2915484310000;
  pr.moving_average = 2924650120000;
  pr.timestamp = 1691040826;
  std::string sig = "a4eebd24d684240635f8f0dae4347a87f951ff8220495f6982e4e52359bc1fb8028b11e02e4ddea503b3c175984836e90e4f65599ab2b1fa632ccb4a915a95f9";
  int j=0;
  for (unsigned int i = 0; i < sig.size(); i += 2) {
    std::string byteString = sig.substr(i, 2);
    pr.signature[j++] = (char) strtol(byteString.c_str(), NULL, 16);
  }

  EXPECT_TRUE(pr.verifySignature(get_config(cryptonote::network_type::TESTNET).ORACLE_PUBLIC_KEY));

  // modified spot price
  pr.spot = 1;
  EXPECT_FALSE(pr.verifySignature(get_config(cryptonote::network_type::TESTNET).ORACLE_PUBLIC_KEY));
  pr.spot = 2915484310000; // revert back

  // modified signature
  pr.signature[0] = 0x2e;
  EXPECT_FALSE(pr.verifySignature(get_config(cryptonote::network_type::TESTNET).ORACLE_PUBLIC_KEY));
}

TEST(oracle, pricing_record_values_valid)
{
  oracle::pricing_record pr;
  pr.spot = 2915484310000;
  pr.moving_average = 2924650120000;
  pr.timestamp = 1691040826;
  std::string sig = "a4eebd24d684240635f8f0dae4347a87f951ff8220495f6982e4e52359bc1fb8028b11e02e4ddea503b3c175984836e90e4f65599ab2b1fa632ccb4a915a95f9";
  int j=0;
  for (unsigned int i = 0; i < sig.size(); i += 2) {
    std::string byteString = sig.substr(i, 2);
    pr.signature[j++] = (char) strtol(byteString.c_str(), NULL, 16);
  }

  EXPECT_TRUE(pr.verifySignature(get_config(cryptonote::network_type::TESTNET).ORACLE_PUBLIC_KEY));

  pr.stable = 1;
  pr.stable_ma = 1;
  pr.reserve = 1;
  pr.reserve_ma = 1;

  EXPECT_TRUE(pr.valid(cryptonote::network_type::TESTNET, 3, 1691041762, 1691040762));
}

TEST(oracle, pricing_record_missing_values_invalid)
{
  oracle::pricing_record pr;
  pr.spot = 2915484310000;
  pr.moving_average = 2924650120000;
  pr.timestamp = 1691040826;
  std::string sig = "a4eebd24d684240635f8f0dae4347a87f951ff8220495f6982e4e52359bc1fb8028b11e02e4ddea503b3c175984836e90e4f65599ab2b1fa632ccb4a915a95f9";
  int j=0;
  for (unsigned int i = 0; i < sig.size(); i += 2) {
    std::string byteString = sig.substr(i, 2);
    pr.signature[j++] = (char) strtol(byteString.c_str(), NULL, 16);
  }

  EXPECT_TRUE(pr.verifySignature(get_config(cryptonote::network_type::TESTNET).ORACLE_PUBLIC_KEY));

  pr.stable = 1;
  pr.stable_ma = 1;
  pr.reserve = 0;
  pr.reserve_ma = 0;

  EXPECT_FALSE(pr.valid(cryptonote::network_type::TESTNET, 3, 1691041762, 1691040762));
}

