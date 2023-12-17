// Copyright (c) 2023, Zephyr Protocol
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
#include "cryptonote_core/cryptonote_tx_utils.h"
#include "cryptonote_basic/cryptonote_basic.h"
#include "oracle/pricing_record.h"
#include "vector"

using tt = cryptonote::transaction_type;

#define INIT_PROTOCOL_STATE_600_PERCENT(circ_amounts, pr) \
    circ_amounts.push_back(std::make_pair("ZEPH",    "1000000000000000")); /* 1000 * 10^12 */ \
    circ_amounts.push_back(std::make_pair("ZEPHUSD", "1000000000000000")); /* 1000 * 10^12 */ \
    circ_amounts.push_back(std::make_pair("ZEPHRSV", "1000000000000000")); /* 1000 * 10^12 */ \
    std::string sig = "a4eebd24d684240635f8f0dae4347a87f951ff8220495f6982e4e52359bc1fb8028b11e02e4ddea503b3c175984836e90e4f65599ab2b1fa632ccb4a915a95f9"; \
    int j=0; \
    for (unsigned int i = 0; i < sig.size(); i += 2) { \
        std::string byteString = sig.substr(i, 2); \
        pr.signature[j++] = (char) strtol(byteString.c_str(), NULL, 16); \
    } \
    pr.timestamp = 1691040826; \
    pr.spot = 6 * COIN; \
    pr.moving_average = 6 * COIN; \
    pr.stable = cryptonote::get_stable_coin_price(circ_amounts, pr.spot); \
    pr.stable_ma = cryptonote::get_stable_coin_price(circ_amounts, pr.moving_average); \
    pr.reserve = cryptonote::get_reserve_coin_price(circ_amounts, pr.spot); \
    pr.reserve_ma = cryptonote::get_reserve_coin_price(circ_amounts, pr.moving_average); \

#define SET_PROTOCOL_STATE_X_PERCENT(pr, percent) \
    pr.spot = percent * COIN; \
    pr.moving_average = percent * COIN; \
    pr.stable = cryptonote::get_stable_coin_price(circ_amounts, pr.spot); \
    pr.stable_ma = cryptonote::get_stable_coin_price(circ_amounts, pr.moving_average); \
    pr.reserve = cryptonote::get_reserve_coin_price(circ_amounts, pr.spot); \
    pr.reserve_ma = cryptonote::get_reserve_coin_price(circ_amounts, pr.moving_average); \

TEST(get_reserve_ratio, reserve_ratio_600_percent)
{
    std::vector<std::pair<std::string, std::string>> circ_amounts;
    oracle::pricing_record pr;
    INIT_PROTOCOL_STATE_600_PERCENT(circ_amounts, pr);
    EXPECT_EQ(cryptonote::get_reserve_ratio(circ_amounts, pr.spot), 6.0);
}

/*
* MINT_STABLE
*/
TEST(reserve_ratio_satisfied, mint_stable_above_400_percent_success)
{
    std::vector<std::pair<std::string, std::string>> circ_amounts;
    oracle::pricing_record pr;
    INIT_PROTOCOL_STATE_600_PERCENT(circ_amounts, pr);

    boost::multiprecision::int128_t tally_zeph = 100 * COIN;
    boost::multiprecision::int128_t tally_stables = cryptonote::zeph_to_zephusd((uint64_t)tally_zeph, pr);
    boost::multiprecision::int128_t tally_reserves = 0;

    EXPECT_TRUE(cryptonote::reserve_ratio_satisfied(circ_amounts, pr, tt::MINT_STABLE, tally_zeph, tally_stables, tally_reserves));
}

TEST(reserve_ratio_satisfied, mint_stable_below_400_percent_fails)
{
    std::vector<std::pair<std::string, std::string>> circ_amounts;
    oracle::pricing_record pr;
    INIT_PROTOCOL_STATE_600_PERCENT(circ_amounts, pr);
    SET_PROTOCOL_STATE_X_PERCENT(pr, 1.0);
    EXPECT_EQ(cryptonote::get_reserve_ratio(circ_amounts, pr.spot), 1.0);

    boost::multiprecision::int128_t tally_zeph = 100 * COIN;
    boost::multiprecision::int128_t tally_stables = cryptonote::zeph_to_zephusd((uint64_t)tally_zeph, pr);
    boost::multiprecision::int128_t tally_reserves = 0;

    EXPECT_FALSE(cryptonote::reserve_ratio_satisfied(circ_amounts, pr, tt::MINT_STABLE, tally_zeph, tally_stables, tally_reserves));
}

/*
* REDEEM_STABLE
*/
TEST(reserve_ratio_satisfied, redeem_stable_above_400_percent_success)
{
    std::vector<std::pair<std::string, std::string>> circ_amounts;
    oracle::pricing_record pr;
    INIT_PROTOCOL_STATE_600_PERCENT(circ_amounts, pr);

    int64_t tx_amount = 100 * COIN;
    boost::multiprecision::int128_t tally_stables = -tx_amount;
    boost::multiprecision::int128_t tally_zeph = -(cryptonote::zephusd_to_zeph((uint64_t)tx_amount, pr));
    boost::multiprecision::int128_t tally_reserves = 0;

    EXPECT_TRUE(cryptonote::reserve_ratio_satisfied(circ_amounts, pr, tt::REDEEM_STABLE, tally_zeph, tally_stables, tally_reserves));
}

TEST(reserve_ratio_satisfied, redeem_stable_below_400_percent_success)
{
    std::vector<std::pair<std::string, std::string>> circ_amounts;
    oracle::pricing_record pr;
    INIT_PROTOCOL_STATE_600_PERCENT(circ_amounts, pr);
    SET_PROTOCOL_STATE_X_PERCENT(pr, 1.0);
    EXPECT_EQ(cryptonote::get_reserve_ratio(circ_amounts, pr.spot), 1.0);

    int64_t tx_amount = 100 * COIN;
    boost::multiprecision::int128_t tally_stables = -tx_amount;
    boost::multiprecision::int128_t tally_zeph = -(cryptonote::zephusd_to_zeph((uint64_t)tx_amount, pr));
    boost::multiprecision::int128_t tally_reserves = 0;

    EXPECT_TRUE(cryptonote::reserve_ratio_satisfied(circ_amounts, pr, tt::REDEEM_STABLE, tally_zeph, tally_stables, tally_reserves));
}

TEST(reserve_ratio_satisfied, redeem_stable_fails_if_reserve_below_zero)
{
    std::vector<std::pair<std::string, std::string>> circ_amounts;
    oracle::pricing_record pr;
    INIT_PROTOCOL_STATE_600_PERCENT(circ_amounts, pr);
    SET_PROTOCOL_STATE_X_PERCENT(pr, 1.0);
    EXPECT_EQ(cryptonote::get_reserve_ratio(circ_amounts, pr.spot), 1.0);

    int64_t tx_amount = 2000 * COIN;
    boost::multiprecision::int128_t tally_stables = -tx_amount;
    boost::multiprecision::int128_t tally_zeph = -(cryptonote::zephusd_to_zeph(tx_amount, pr));
    boost::multiprecision::int128_t tally_reserves = 0;

    EXPECT_FALSE(cryptonote::reserve_ratio_satisfied(circ_amounts, pr, tt::REDEEM_STABLE, tally_zeph, tally_stables, tally_reserves));
}

/*
* MINT_RESERVE
*/
TEST(reserve_ratio_satisfied, mint_reserve_below_800_percent_success)
{
    std::vector<std::pair<std::string, std::string>> circ_amounts;
    oracle::pricing_record pr;
    INIT_PROTOCOL_STATE_600_PERCENT(circ_amounts, pr);

    boost::multiprecision::int128_t tally_zeph = 100 * COIN;
    boost::multiprecision::int128_t tally_stables = 0;
    boost::multiprecision::int128_t tally_reserves = cryptonote::zeph_to_zephrsv((uint64_t)tally_zeph, pr);

    EXPECT_TRUE(cryptonote::reserve_ratio_satisfied(circ_amounts, pr, tt::MINT_RESERVE, tally_zeph, tally_stables, tally_reserves));
}

TEST(reserve_ratio_satisfied, mint_reserve_above_800_percent_fails)
{
    std::vector<std::pair<std::string, std::string>> circ_amounts;
    oracle::pricing_record pr;
    INIT_PROTOCOL_STATE_600_PERCENT(circ_amounts, pr);

    boost::multiprecision::int128_t tally_zeph = 1000 * COIN;
    boost::multiprecision::int128_t tally_stables = 0;
    boost::multiprecision::int128_t tally_reserves = cryptonote::zeph_to_zephrsv((uint64_t)tally_zeph, pr);

    EXPECT_FALSE(cryptonote::reserve_ratio_satisfied(circ_amounts, pr, tt::MINT_RESERVE, tally_zeph, tally_stables, tally_reserves));
}

/*
* REDEEM_RESERVE
*/

TEST(reserve_ratio_satisfied, redeem_reserve_above_400_percent_success)
{
    std::vector<std::pair<std::string, std::string>> circ_amounts;
    oracle::pricing_record pr;
    INIT_PROTOCOL_STATE_600_PERCENT(circ_amounts, pr);

    int64_t tx_amount = 100 * COIN;
    boost::multiprecision::int128_t tally_zeph = -tx_amount;
    boost::multiprecision::int128_t tally_stables = 0;
    boost::multiprecision::int128_t tally_reserves = -(cryptonote::zeph_to_zephrsv((uint64_t)tx_amount, pr));

    EXPECT_TRUE(cryptonote::reserve_ratio_satisfied(circ_amounts, pr, tt::REDEEM_RESERVE, tally_zeph, tally_stables, tally_reserves));
}

TEST(reserve_ratio_satisfied, redeem_reserve_below_400_percent_fails)
{
    std::vector<std::pair<std::string, std::string>> circ_amounts;
    oracle::pricing_record pr;
    INIT_PROTOCOL_STATE_600_PERCENT(circ_amounts, pr);

    int64_t tx_amount = 1000 * COIN;
    boost::multiprecision::int128_t tally_zeph = -tx_amount;
    boost::multiprecision::int128_t tally_stables = 0;
    boost::multiprecision::int128_t tally_reserves = cryptonote::zeph_to_zephrsv((uint64_t)tx_amount, pr);

    EXPECT_FALSE(cryptonote::reserve_ratio_satisfied(circ_amounts, pr, tt::REDEEM_RESERVE, tally_zeph, tally_stables, tally_reserves));
}
