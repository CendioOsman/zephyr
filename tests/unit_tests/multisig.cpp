// Copyright (c) 2017-2023, The Monero Project
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

#include "crypto/crypto.h"
#include "multisig/multisig_account.h"
#include "multisig/multisig_kex_msg.h"
#include "ringct/rctOps.h"
#include "wallet/wallet2.h"

#include "gtest/gtest.h"

#include <cstdint>

static const struct
{
  const char *address;
  const char *spendkey;
} test_addresses[] =
{
  {
    "ZPHTjb4k3912KDTR2ZPeVbH6BRenVM3F9hr2cKo7W5Rp9kDNkMU6o5MTwCmGxZbmPBC9uUUm7fnJZPz8hVUqxpPBM4bjQUJY6Fs",
    "579666108d714c1e883aba4b2aee3937e5137fb696923f9a0cf6db84bf2d5f01"
  },
  {
    "ZPHTjc4Zw7TCZ9bFD7Crw6KB1vm4xpJiBNrsgN5UTo1E8ak22P5YN2118myxXPjamANZ9mEjDVHnpBwSKHW3p74AfdUgji5wAMQ",
    "2fae0c45f101f2702fc454db0bacccac05a8ba465719d74ad12c37d7df7b6f02"
  },
  {
    "ZPHTjankvTzJhzEDogX6ct57HQpnb1MmEeY4wWtqrQ472jDK7X7h5Ze8csQdf6pspeS8dWrhjXTtjGubWVkHefhcQNZQgrp6KU1",
    "cb8c1d130af3bf7c38ef775eb5ba5a6ec140c9b424f2ac441303ab1830b4a001"
  },
  {
    "ZPHTjdeZpTGLPnRVY4WxKSJNKeA2PZLDfgrQg66CCTo19FVjCY3eg1vag6e9rvn41i7x4Pde61UGdG9iRMtJztniARVSTrRis2J",
    "59c86cfa3618592ebe70c2acfb95e4611c98d4fb7f2228dd1d97af468c2a080b"
  },
  {
    "ZPHTjfWr5db4Kh5AuMz28D36uGXLYnrnfcaZ8xq7nRAn9nzrQBzCTf8VHAouCxfKKQ2fqpbxxQqouf3Sxpny5PmdfGeFQESoJ27",
    "b8ac8ccf0273a6f7f9c754e5932532867b9a2a7f27f90ed53d85be8174820f0c"
  }
};

static const size_t KEYS_COUNT = 5;

static void make_wallet(unsigned int idx, tools::wallet2 &wallet)
{
  ASSERT_TRUE(idx < sizeof(test_addresses) / sizeof(test_addresses[0]));

  crypto::secret_key spendkey;
  epee::string_tools::hex_to_pod(test_addresses[idx].spendkey, spendkey);

  try
  {
    wallet.init("", boost::none, "", 0, true, epee::net_utils::ssl_support_t::e_ssl_support_disabled);
    wallet.set_subaddress_lookahead(1, 1);
    wallet.generate("", "", spendkey, true, false);
    ASSERT_TRUE(test_addresses[idx].address == wallet.get_account().get_public_address_str(cryptonote::TESTNET));
    wallet.decrypt_keys("");
    ASSERT_TRUE(test_addresses[idx].spendkey == epee::string_tools::pod_to_hex(wallet.get_account().get_keys().m_spend_secret_key));
    wallet.encrypt_keys("");
  }
  catch (const std::exception &e)
  {
    MFATAL("Error creating test wallet: " << e.what());
    ASSERT_TRUE(0);
  }
}

static std::vector<std::string> exchange_round(std::vector<tools::wallet2>& wallets, const std::vector<std::string>& infos)
{
  std::vector<std::string> new_infos;
  new_infos.reserve(infos.size());

  for (size_t i = 0; i < wallets.size(); ++i)
    new_infos.push_back(wallets[i].exchange_multisig_keys("", infos));

  return new_infos;
}

static std::vector<std::string> exchange_round_force_update(std::vector<tools::wallet2>& wallets,
  const std::vector<std::string>& infos,
  const std::size_t round_in_progress)
{
  EXPECT_TRUE(wallets.size() == infos.size());
  std::vector<std::string> new_infos;
  std::vector<std::string> temp_force_update_infos;
  new_infos.reserve(infos.size());

  // when force-updating, we only need at most 'num_signers - 1 - (round - 1)' messages from other signers
  size_t num_other_messages_required{wallets.size() - 1 - (round_in_progress - 1)};
  if (num_other_messages_required > wallets.size())
    num_other_messages_required = 0;  //overflow case for post-kex verification round of 1-of-N

  for (size_t i = 0; i < wallets.size(); ++i)
  {
    temp_force_update_infos.clear();
    temp_force_update_infos.reserve(num_other_messages_required + 1);
    temp_force_update_infos.push_back(infos[i]);  //always include the local signer's message for this round

    size_t infos_collected{0};
    for (size_t wallet_index = 0; wallet_index < wallets.size(); ++wallet_index)
    {
      // skip the local signer's message
      if (wallet_index == i)
        continue;

      temp_force_update_infos.push_back(infos[wallet_index]);
      ++infos_collected;

      if (infos_collected == num_other_messages_required)
        break;
    }

    new_infos.push_back(wallets[i].exchange_multisig_keys("", temp_force_update_infos, true));
  }

  return new_infos;
}

static void check_results(const std::vector<std::string> &intermediate_infos,
  std::vector<tools::wallet2>& wallets,
  const std::uint32_t M)
{
  // check results
  std::unordered_set<crypto::secret_key> unique_privkeys;
  rct::key composite_pubkey = rct::identity();

  wallets[0].decrypt_keys("");
  crypto::public_key spend_pubkey = wallets[0].get_account().get_keys().m_account_address.m_spend_public_key;
  crypto::secret_key view_privkey = wallets[0].get_account().get_keys().m_view_secret_key;
  crypto::public_key view_pubkey;
  EXPECT_TRUE(crypto::secret_key_to_public_key(view_privkey, view_pubkey));
  wallets[0].encrypt_keys("");

  for (size_t i = 0; i < wallets.size(); ++i)
  {
    EXPECT_TRUE(!intermediate_infos[i].empty());
    bool ready;
    uint32_t threshold, total;
    EXPECT_TRUE(wallets[i].multisig(&ready, &threshold, &total));
    EXPECT_TRUE(ready);
    EXPECT_TRUE(threshold == M);
    EXPECT_TRUE(total == wallets.size());

    wallets[i].decrypt_keys("");

    if (i != 0)
    {
      // "equals" is transitive relation so we need only to compare first wallet's address to each others' addresses.
      // no need to compare 0's address with itself.
      EXPECT_TRUE(wallets[0].get_account().get_public_address_str(cryptonote::TESTNET) ==
        wallets[i].get_account().get_public_address_str(cryptonote::TESTNET));
      
      EXPECT_EQ(spend_pubkey, wallets[i].get_account().get_keys().m_account_address.m_spend_public_key);
      EXPECT_EQ(view_privkey, wallets[i].get_account().get_keys().m_view_secret_key);
      EXPECT_EQ(view_pubkey, wallets[i].get_account().get_keys().m_account_address.m_view_public_key);
    }

    // sum together unique multisig keys
    for (const auto &privkey : wallets[i].get_account().get_keys().m_multisig_keys)
    {
      EXPECT_NE(privkey, crypto::null_skey);

      if (unique_privkeys.find(privkey) == unique_privkeys.end())
      {
        unique_privkeys.insert(privkey);
        crypto::public_key pubkey;
        crypto::secret_key_to_public_key(privkey, pubkey);
        EXPECT_NE(privkey, crypto::null_skey);
        EXPECT_NE(pubkey, crypto::null_pkey);
        EXPECT_NE(pubkey, rct::rct2pk(rct::identity()));
        rct::addKeys(composite_pubkey, composite_pubkey, rct::pk2rct(pubkey));
      }
    }
    wallets[i].encrypt_keys("");
  }

  // final key via sums should equal the wallets' public spend key
  wallets[0].decrypt_keys("");
  EXPECT_EQ(wallets[0].get_account().get_keys().m_account_address.m_spend_public_key, rct::rct2pk(composite_pubkey));
  wallets[0].encrypt_keys("");
}

static void make_wallets(const unsigned int M, const unsigned int N, const bool force_update)
{
  std::vector<tools::wallet2> wallets(N);
  ASSERT_TRUE(wallets.size() > 1 && wallets.size() <= KEYS_COUNT);
  ASSERT_TRUE(M <= wallets.size());
  std::uint32_t total_rounds_required = multisig::multisig_setup_rounds_required(wallets.size(), M);
  std::uint32_t rounds_complete{0};

  // initialize wallets, get first round multisig kex msgs
  std::vector<std::string> initial_infos(wallets.size());

  for (size_t i = 0; i < wallets.size(); ++i)
  {
    make_wallet(i, wallets[i]);

    wallets[i].decrypt_keys("");
    initial_infos[i] = wallets[i].get_multisig_first_kex_msg();
    wallets[i].encrypt_keys("");
  }

  // wallets should not be multisig yet
  for (const auto &wallet: wallets)
  {
    ASSERT_FALSE(wallet.multisig());
  }

  // make wallets multisig, get second round kex messages (if appropriate)
  std::vector<std::string> intermediate_infos(wallets.size());

  for (size_t i = 0; i < wallets.size(); ++i)
  {
    intermediate_infos[i] = wallets[i].make_multisig("", initial_infos, M);
  }

  ++rounds_complete;

  // perform kex rounds until kex is complete
  bool ready;
  wallets[0].multisig(&ready);
  while (!ready)
  {
    if (force_update)
      intermediate_infos = exchange_round_force_update(wallets, intermediate_infos, rounds_complete + 1);
    else
      intermediate_infos = exchange_round(wallets, intermediate_infos);

    wallets[0].multisig(&ready);
    ++rounds_complete;
  }

  EXPECT_EQ(total_rounds_required, rounds_complete);

  check_results(intermediate_infos, wallets, M);
}

TEST(multisig, make_1_2)
{
  make_wallets(1, 2, false);
  make_wallets(1, 2, true);
}

TEST(multisig, make_1_3)
{
  make_wallets(1, 3, false);
  make_wallets(1, 3, true);
}

TEST(multisig, make_2_2)
{
  make_wallets(2, 2, false);
  make_wallets(2, 2, true);
}

TEST(multisig, make_3_3)
{
  make_wallets(3, 3, false);
  make_wallets(3, 3, true);
}

TEST(multisig, make_2_3)
{
  make_wallets(2, 3, false);
  make_wallets(2, 3, true);
}

TEST(multisig, make_2_4)
{
  make_wallets(2, 4, false);
  make_wallets(2, 4, true);
}

TEST(multisig, multisig_kex_msg)
{
  using namespace multisig;

  crypto::public_key pubkey1;
  crypto::public_key pubkey2;
  crypto::public_key pubkey3;
  crypto::secret_key_to_public_key(rct::rct2sk(rct::skGen()), pubkey1);
  crypto::secret_key_to_public_key(rct::rct2sk(rct::skGen()), pubkey2);
  crypto::secret_key_to_public_key(rct::rct2sk(rct::skGen()), pubkey3);

  crypto::secret_key signing_skey = rct::rct2sk(rct::skGen());
  crypto::public_key signing_pubkey;
  while(!crypto::secret_key_to_public_key(signing_skey, signing_pubkey))
  {
    signing_skey = rct::rct2sk(rct::skGen());
  }

  const crypto::secret_key ancillary_skey{rct::rct2sk(rct::skGen())};

  // misc. edge cases
  EXPECT_NO_THROW((multisig_kex_msg{}));
  EXPECT_ANY_THROW((multisig_kex_msg{multisig_kex_msg{}.get_msg()}));
  EXPECT_ANY_THROW((multisig_kex_msg{"abc"}));
  EXPECT_ANY_THROW((multisig_kex_msg{0, crypto::null_skey, std::vector<crypto::public_key>{}, crypto::null_skey}));
  EXPECT_ANY_THROW((multisig_kex_msg{1, crypto::null_skey, std::vector<crypto::public_key>{}, crypto::null_skey}));
  EXPECT_ANY_THROW((multisig_kex_msg{1, signing_skey, std::vector<crypto::public_key>{}, crypto::null_skey}));
  EXPECT_ANY_THROW((multisig_kex_msg{1, crypto::null_skey, std::vector<crypto::public_key>{}, ancillary_skey}));

  // test that messages are both constructible and reversible

  // round 1
  EXPECT_NO_THROW((multisig_kex_msg{
      multisig_kex_msg{1, signing_skey, std::vector<crypto::public_key>{}, ancillary_skey}.get_msg()
    }));
  EXPECT_NO_THROW((multisig_kex_msg{
      multisig_kex_msg{1, signing_skey, std::vector<crypto::public_key>{pubkey1}, ancillary_skey}.get_msg()
    }));

  // round 2
  EXPECT_NO_THROW((multisig_kex_msg{
      multisig_kex_msg{2, signing_skey, std::vector<crypto::public_key>{pubkey1}, ancillary_skey}.get_msg()
    }));
  EXPECT_NO_THROW((multisig_kex_msg{
      multisig_kex_msg{2, signing_skey, std::vector<crypto::public_key>{pubkey1}, crypto::null_skey}.get_msg()
    }));
  EXPECT_NO_THROW((multisig_kex_msg{
      multisig_kex_msg{2, signing_skey, std::vector<crypto::public_key>{pubkey1, pubkey2}, ancillary_skey}.get_msg()
    }));
  EXPECT_NO_THROW((multisig_kex_msg{
      multisig_kex_msg{2, signing_skey, std::vector<crypto::public_key>{pubkey1, pubkey2, pubkey3}, crypto::null_skey}.get_msg()
    }));

  // test that keys can be recovered if stored in a message and the message's reverse

  // round 1
  const multisig_kex_msg msg_rnd1{1, signing_skey, std::vector<crypto::public_key>{pubkey1}, ancillary_skey};
  const multisig_kex_msg msg_rnd1_reverse{msg_rnd1.get_msg()};
  EXPECT_EQ(msg_rnd1.get_round(), 1);
  EXPECT_EQ(msg_rnd1.get_round(), msg_rnd1_reverse.get_round());
  EXPECT_EQ(msg_rnd1.get_signing_pubkey(), signing_pubkey);
  EXPECT_EQ(msg_rnd1.get_signing_pubkey(), msg_rnd1_reverse.get_signing_pubkey());
  EXPECT_EQ(msg_rnd1.get_msg_pubkeys().size(), 0);
  EXPECT_EQ(msg_rnd1.get_msg_pubkeys().size(), msg_rnd1_reverse.get_msg_pubkeys().size());
  EXPECT_EQ(msg_rnd1.get_msg_privkey(), ancillary_skey);
  EXPECT_EQ(msg_rnd1.get_msg_privkey(), msg_rnd1_reverse.get_msg_privkey());

  // round 2
  const multisig_kex_msg msg_rnd2{2, signing_skey, std::vector<crypto::public_key>{pubkey1, pubkey2}, ancillary_skey};
  const multisig_kex_msg msg_rnd2_reverse{msg_rnd2.get_msg()};
  EXPECT_EQ(msg_rnd2.get_round(), 2);
  EXPECT_EQ(msg_rnd2.get_round(), msg_rnd2_reverse.get_round());
  EXPECT_EQ(msg_rnd2.get_signing_pubkey(), signing_pubkey);
  EXPECT_EQ(msg_rnd2.get_signing_pubkey(), msg_rnd2_reverse.get_signing_pubkey());
  ASSERT_EQ(msg_rnd2.get_msg_pubkeys().size(), 2);
  ASSERT_EQ(msg_rnd2.get_msg_pubkeys().size(), msg_rnd2_reverse.get_msg_pubkeys().size());
  EXPECT_EQ(msg_rnd2.get_msg_pubkeys()[0], pubkey1);
  EXPECT_EQ(msg_rnd2.get_msg_pubkeys()[1], pubkey2);
  EXPECT_EQ(msg_rnd2.get_msg_pubkeys()[0], msg_rnd2_reverse.get_msg_pubkeys()[0]);
  EXPECT_EQ(msg_rnd2.get_msg_pubkeys()[1], msg_rnd2_reverse.get_msg_pubkeys()[1]);
  EXPECT_EQ(msg_rnd2.get_msg_privkey(), crypto::null_skey);
  EXPECT_EQ(msg_rnd2.get_msg_privkey(), msg_rnd2_reverse.get_msg_privkey());
}
