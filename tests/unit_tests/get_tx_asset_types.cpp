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
#include "cryptonote_core/cryptonote_tx_utils.cpp"
#include "cryptonote_basic/cryptonote_basic.h"
#include "vector"

#define TEST_SUCCESSFUL_TRANSFER(asset_type1) \
    TEST(get_tx_asset_types, successful_on_1_input_type_1_output_type_##asset_type1) \
    { \
        cryptonote::transaction tx; \
        tx.version = 3; \
        \
        cryptonote::txin_zephyr_key in_key; \
        in_key.asset_type = #asset_type1; \
        tx.vin.push_back(in_key); \
        \
        cryptonote::tx_out out; \
        cryptonote::tx_out out1; \
        cryptonote::txout_zephyr_tagged_key out_key; \
        out_key.asset_type = #asset_type1; \
        out.target = out_key; \
        out1.target = out_key; \
        \
        tx.vout.push_back(out); \
        tx.vout.push_back(out1); \
        \
        std::string source; \
        std::string dest; \
        EXPECT_TRUE(get_tx_asset_types(tx, tx.hash, source, dest, false)); \
        \
        EXPECT_EQ(source, #asset_type1); \
        EXPECT_EQ(dest, #asset_type1); \
    }

#define TEST_SUCCESSFUL_CONVERSION(asset_type1, asset_type2) \
    TEST(get_tx_asset_types, successful_on_1_input_type_1_output_type_##asset_type1##_##asset_type2) \
    { \
        cryptonote::transaction tx; \
        tx.version = 3; \
        \
        cryptonote::txin_zephyr_key in_key; \
        in_key.asset_type = #asset_type1; \
        tx.vin.push_back(in_key); \
        \
        cryptonote::tx_out out; \
        cryptonote::tx_out out1; \
        cryptonote::txout_zephyr_tagged_key out_key; \
        cryptonote::txout_zephyr_tagged_key out_key_change; \
        out_key.asset_type = #asset_type2; \
        out_key_change.asset_type = #asset_type1; \
        out.target = out_key; \
        out1.target = out_key_change; \
        \
        tx.vout.push_back(out); \
        tx.vout.push_back(out1); \
        \
        std::string source; \
        std::string dest; \
        EXPECT_TRUE(get_tx_asset_types(tx, tx.hash, source, dest, false)); \
        \
        EXPECT_EQ(source, #asset_type1); \
        EXPECT_EQ(dest, #asset_type2); \
    }

TEST_SUCCESSFUL_TRANSFER(ZEPH)
TEST_SUCCESSFUL_TRANSFER(ZEPHUSD)
TEST_SUCCESSFUL_TRANSFER(ZEPHRSV)

TEST_SUCCESSFUL_CONVERSION(ZEPH, ZEPHUSD) // mint_stable
TEST_SUCCESSFUL_CONVERSION(ZEPHUSD, ZEPH) // redeem stable
TEST_SUCCESSFUL_CONVERSION(ZEPH, ZEPHRSV) // mint_reserve
TEST_SUCCESSFUL_CONVERSION(ZEPHRSV, ZEPH) // redeem reserve

// fail on multiple source asset types - 2 or more
TEST(get_tx_asset_types, fail_on_multiple_input_types_2_or_more)
{
    cryptonote::transaction tx;
    tx.version = 3;
    cryptonote::txin_zephyr_key in_key1;
    cryptonote::txin_zephyr_key in_key2;

    in_key1.asset_type = "ZEPH";
    in_key2.asset_type = "ZEPHUSD";
    tx.vin.push_back(in_key1);
    tx.vin.push_back(in_key2);

    cryptonote::tx_out out;
    cryptonote::tx_out out1;
    cryptonote::txout_zephyr_tagged_key out_key1;
    cryptonote::txout_zephyr_tagged_key out_key2;
    out_key1.asset_type = "ZEPH";
    out_key2.asset_type = "ZEPHUSD";
    out.target = out_key1;
    out1.target = out_key2;

    tx.vout.push_back(out);
    tx.vout.push_back(out1);

    std::string source;
    std::string dest;
    EXPECT_FALSE(get_tx_asset_types(tx, tx.hash, source, dest, false));
}

// fail with more than 2 output asset types
TEST(get_tx_asset_types, fail_more_than_2_output_types)
{
    cryptonote::transaction tx;
    tx.version = 3;
    cryptonote::txin_zephyr_key in_key;
    in_key.asset_type = "ZEPH";
    tx.vin.push_back(in_key);

    cryptonote::txout_zephyr_tagged_key out_key1;
    cryptonote::txout_zephyr_tagged_key out_key2;
    cryptonote::txout_zephyr_tagged_key out_key3;
    out_key1.asset_type = "ZEPHUSD";
    out_key2.asset_type = "ZEPHRSV";
    out_key3.asset_type = "ZEPH";

    cryptonote::tx_out out;
    cryptonote::tx_out out1;
    cryptonote::tx_out out2;
    out.target = out_key1;
    out1.target = out_key2;
    out2.target = out_key3;

    tx.vout.push_back(out);
    tx.vout.push_back(out1);
    tx.vout.push_back(out2);

    std::string source;
    std::string dest;
    EXPECT_FALSE(get_tx_asset_types(tx, tx.hash, source, dest, false));
}

// fail on single input types & single output types & they are not equal
TEST(get_tx_asset_types, fail_single_input_single_output_types_are_not_equal)
{
    cryptonote::transaction tx;
    tx.version = 3;
    cryptonote::txin_zephyr_key in_key;
    in_key.asset_type = "ZEPH";
    tx.vin.push_back(in_key);

    cryptonote::txout_zephyr_tagged_key out_key1;
    cryptonote::txout_zephyr_tagged_key out_key2;
    out_key1.asset_type = "ZEPHUSD";
    out_key2.asset_type = "ZEPHUSD";

    cryptonote::tx_out out;
    cryptonote::tx_out out1;
    out.target = out_key1;
    out1.target = out_key2;
    tx.vout.push_back(out);
    tx.vout.push_back(out1);

    std::string source;
    std::string dest;
    EXPECT_FALSE(get_tx_asset_types(tx, tx.hash, source, dest, false));
}

// fail on single input types & 2 output types & none of the outputs matches inputs
TEST(get_tx_asset_types, none_of_output_matches_input)
{
    cryptonote::transaction tx;
    tx.version = 3;
    cryptonote::txin_zephyr_key in_key;
    in_key.asset_type = "ZEPH";
    tx.vin.push_back(in_key);
    tx.vin.push_back(in_key);

    cryptonote::txout_zephyr_tagged_key out_key1;
    cryptonote::txout_zephyr_tagged_key out_key2;
    out_key1.asset_type = "ZEPHUSD";
    out_key2.asset_type = "ZEPHRSV";

    cryptonote::tx_out out;
    cryptonote::tx_out out1;
    out.target = out_key1;
    out1.target = out_key2;
    tx.vout.push_back(out);
    tx.vout.push_back(out1);

    std::string source;
    std::string dest;
    EXPECT_FALSE(get_tx_asset_types(tx, tx.hash, source, dest, false));
}

// pass on single input types & 2 output types & 1 of the outputs matches inputs e.g. ZEPHUSD -> ZEPHRSV
// This case is expected to be caught by get_tx_type()
TEST(get_tx_asset_types, successful_on_logical_input_output_but_not_allowed)
{
    cryptonote::transaction tx;
    tx.version = 3;
    cryptonote::txin_zephyr_key in_key;
    in_key.asset_type = "ZEPHUSD";
    tx.vin.push_back(in_key);
    tx.vin.push_back(in_key);

    cryptonote::txout_zephyr_tagged_key out_key1;
    cryptonote::txout_zephyr_tagged_key out_key2;
    out_key1.asset_type = "ZEPHRSV";
    out_key2.asset_type = "ZEPHUSD";

    cryptonote::tx_out out;
    cryptonote::tx_out out1;
    out.target = out_key1;
    out1.target = out_key2;
    tx.vout.push_back(out);
    tx.vout.push_back(out1);

    std::string source;
    std::string dest;
    EXPECT_TRUE(get_tx_asset_types(tx, tx.hash, source, dest, false));

    EXPECT_EQ(source, "ZEPHUSD");
    EXPECT_EQ(dest, "ZEPHRSV");
}

// fail single input and output asset types ZEPHUSD -> ZEPHRSV
TEST(get_tx_asset_types, fail_on_single_input_output_zephusd_zephrsv)
{
    cryptonote::transaction tx;
    tx.version = 3;
    
    cryptonote::txin_zephyr_key in_key;
    in_key.asset_type = "ZEPHUSD";
    tx.vin.push_back(in_key);
    tx.vin.push_back(in_key);
    tx.vin.push_back(in_key);

    cryptonote::tx_out out;
    cryptonote::tx_out out1;
    cryptonote::txout_zephyr_tagged_key out_key1;
    cryptonote::txout_zephyr_tagged_key out_key2;
    out_key1.asset_type = "ZEPHRSV";
    out_key2.asset_type = "ZEPHRSV";
    out.target = out_key1;
    out1.target = out_key2;

    tx.vout.push_back(out);
    tx.vout.push_back(out1);

    std::string source;
    std::string dest;
    EXPECT_FALSE(get_tx_asset_types(tx, tx.hash, source, dest, false));
}

// fail on unknown asset types
TEST(get_tx_asset_types, fail_on_unknown_asset_type)
{
    cryptonote::transaction tx;
    tx.version = 3;
    
    cryptonote::txin_zephyr_key in_key;
    in_key.asset_type = "ZEPHEUR";
    tx.vin.push_back(in_key);
    tx.vin.push_back(in_key);
    tx.vin.push_back(in_key);

    cryptonote::tx_out out;
    cryptonote::tx_out out1;
    cryptonote::txout_zephyr_tagged_key out_key1;
    cryptonote::txout_zephyr_tagged_key out_key2;
    out_key1.asset_type = "ZEPHEUR";
    out_key2.asset_type = "ZEPHEUR";
    out.target = out_key1;
    out1.target = out_key2;

    tx.vout.push_back(out);
    tx.vout.push_back(out1);

    std::string source;
    std::string dest;
    EXPECT_FALSE(get_tx_asset_types(tx, tx.hash, source, dest, false));
}

// fail on unknown asset types
TEST(get_tx_asset_types, fail_on_2_unknown_asset_types_and_multiple_outs)
{
    cryptonote::transaction tx;
    tx.version = 3;
    
    cryptonote::txin_zephyr_key in_key;
    in_key.asset_type = "ZEPHEUR";
    tx.vin.push_back(in_key);
    tx.vin.push_back(in_key);
    tx.vin.push_back(in_key);

    cryptonote::tx_out out;
    cryptonote::tx_out out1;
    cryptonote::txout_zephyr_tagged_key out_key1;
    cryptonote::txout_zephyr_tagged_key out_key2;
    out_key1.asset_type = "ZEPHEUR";
    out_key2.asset_type = "ZEPHGBP";
    out.target = out_key1;
    out1.target = out_key2;

    tx.vout.push_back(out);
    tx.vout.push_back(out1);

    std::string source;
    std::string dest;
    EXPECT_FALSE(get_tx_asset_types(tx, tx.hash, source, dest, false));
}

// fail on unknown asset types
TEST(get_tx_asset_types, fail_on_1_unknown_asset_type)
{
    cryptonote::transaction tx;
    tx.version = 3;
    
    cryptonote::txin_zephyr_key in_key;
    in_key.asset_type = "ZEPH";
    tx.vin.push_back(in_key);
    tx.vin.push_back(in_key);
    tx.vin.push_back(in_key);

    cryptonote::tx_out out;
    cryptonote::tx_out out1;
    cryptonote::txout_zephyr_tagged_key out_key1;
    cryptonote::txout_zephyr_tagged_key out_key2;
    out_key1.asset_type = "ZEPH";
    out_key2.asset_type = "ZEPHGBP";
    out.target = out_key1;
    out1.target = out_key2;

    tx.vout.push_back(out);
    tx.vout.push_back(out1);

    std::string source;
    std::string dest;
    EXPECT_FALSE(get_tx_asset_types(tx, tx.hash, source, dest, false));
}
