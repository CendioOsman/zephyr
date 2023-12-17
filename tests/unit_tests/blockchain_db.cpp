// Copyright (c) 2014-2023, The Monero Project
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

#include <boost/filesystem.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <cstdio>
#include <iostream>
#include <chrono>
#include <thread>

#include "gtest/gtest.h"

#include "string_tools.h"
#include "blockchain_db/blockchain_db.h"
#include "blockchain_db/lmdb/db_lmdb.h"
#include "cryptonote_basic/cryptonote_format_utils.h"

using namespace cryptonote;
using epee::string_tools::pod_to_hex;

#define ASSERT_HASH_EQ(a,b) ASSERT_EQ(pod_to_hex(a), pod_to_hex(b))

namespace {  // anonymous namespace

const std::vector<std::string> t_blocks =
  {
    "0102b2f19da6061fa1aef306c462fba957c0b0e6232093ec5b45ffda22993751b19a094f4bbf9499aa91c700000000000000000000000000000000000000000000000002c00101ff840102d1b1bae998d90302bae9bcea726d0f5b1dd059a7783ab2eb94cef19c62d19f7c88d06445965ee210045a455048d990d7decaf318023897e7261abf7d104457d5be3844bfc346351aaff7c694255808e3fd2bfd5ac5045a45504878420166caf8c61e5437f0269a1d8dde7f26b2bdbc576f07070cbeede453a72685c6b30175725ad980ce7efc43e0b91692291682ec549b6221b36b6d578a013db28c7b700000000001cd4ae19087057a189958bb5569cc5b06d74cb72e0de6bdaa44ce5ddeb07f836b"
  , "010283f69da6069ec2c4666d5e1d0eed31f0ee428fa5a1e0f820f4a9adbb2ed7fb7035d73ee37c13f6908400000000000000000000000000000000000000000000000002c10101ff8501029acfd48494d9030222db544cfc80232d6ba4d26d540ab9190075569d9f2517b89022df731903e96f045a455048ecfaefaccaf31802a82d8e0146057686592fbc6e427e13e34ce761037d60d14712b05bcd2df162dc045a455048274201216299824b5a97cddec7d145a71a78acf1b2c99afa92f802d21378f46e75e6f801c0a2ab229a21f403e8b18fbe9123657e0959536f96b8027dd4134e0381e592150000000000"
  };

const std::vector<size_t> t_sizes =
  {
    2359
  , 170
  };

const std::vector<difficulty_type> t_diffs =
  {
    333
  , 333
  };

const std::vector<uint64_t> t_coins =
  {
    502241971723972628
  , 502259084934747989
  };

const std::vector<std::vector<std::string>> t_transactions =
  {
    {
      "0200020200045a455048100003020419240608020d020506114c017a1f1699aee820662251e5efd2d16b996dce4d94c67acf17c224effda7c3ce720200045a455048102f08040f0805040a08060103021551015f9a59af94daddff08647c8a052a67e5766fe66926b58ef9597c6e4586447d89020002891fff2089e2e03a543a5d1ed4c994a2e73acd2b8524ca88efb215c826f8fa6a045a4550487600024d8227c6c55681c4ec848d3df297b41c0f0d08a5b638491f5e4a4493815548c0045a455048372c017f6abfa7a00a0bc81f4acc338182e4a283ca3e55264520370619423042e7610f020901b0f0485409c2d01100000006a0bab3dd044b343db7b09c19e907b79c3b242a40c315c3d2285e4bc2b65d947017fd9c8b7bee4d0b9d7d85109581dc3e1749260b677cc089520173e1a4191093548220a595aab2cd509d8bdb83b8cbfa683a3c322001b485bc8d7681c24420efa508f9e640af071c2d895193126ed5a8c029c13ecb176df3790311878b7664735cb155383dd008a7cbd75f96a1fd37ed301af215cc877aa00053c1c401619e7732b42c8c84a26d8488a38d50b1f0b060ce84199a7fc9d848eb220a88ea490678c59fb9d561abfc21d40036a6df3dfe7fea833ea8e40c146894b29b9c61dd6f2a8ae2f0d9115a0df017f1ec8ecd4583eb4758f892050f4abb74ecabaadb15bb11b60afcea4ed3927a5c12d4362b2f5b1002e7e5e1870207c13451245c2c4ace17012c1bdc28af24eaa094d645016c54a9f3aa239864d5a378e0d97852cd68f39c21fe8a95a16095e3811e692aca1f5eab816bddaf74b5da8af40eb80b57aa05bfc1e227a665963fba07f32d7676b97c7e2c3ce7869d20cb41d185dd8f25c22531a8494418956da4a180ce81d04cafac3893f317bfdf6a37afc2d3b0cb957ad9a739f6844b84a240584a93988c03d401b16cf0f41e299e23e69713a3091004dd9bf486762952bbde71acdb99a4bc05276093e88e89ed847a65e8edcea58fb742b8e0681528cd6dc874d1435e3b0fa602ebeeab065f4db56707d07eb5d66bac6cdb67da073209a1bd8899e5561a33b49346c20ff56aaaa2a8cd7bce53744609a7b99c9e6dec6df04a500a0dedda83f65e255c7cdc2601b16766cf910ea5d06e1659d8e7377a1e3ccb81ed67134372556a5e5e78a4e7b40dfccc0b57461be53f8ee59c30f3ccad5b8ee217b5dab4db37e9420131e5ac90ea142cc06c61048bbf50f336c4dbda0b1c9a1bb4ff77af308c27c0b1a5c26ec9250f0ddad5452c249743431974aaf620e74e383f47c3dc13588cb397ae791c1263f1f9bbff1f6fa7817a43f58f9615e84bc66d5382ec297a31b13118efd4237ac5da19830251cd3580f13a23bf01ebb96422e901a093ed8f5636b90fbcae93ccc29303353028f0a4693353a8379c40f661b1d96fce9d05cf1410f0e5b4ff136c90cc0e3381bffba9936e7b5b051007a2e5596c40140e45c398f871b9070993bea4a4067a194e29bdef687321b9833f614d959de364cde9dea50303c6dfc5ee8cbcf80985bb269462435db6a7b68708959525e5dff6cf1a651bff9504411b1395b8810ba260bf1efdc126cc86bed940ead9d6f5c1b74f62397777865639181cff54030637356a5aa3675c84aeda55d2b9287a4aa418c2bf47b37814bc44594948637c0c281d3d7458f3d3e6a87640006760b7737c7c7e9d5ebf3b553fdaf18615a9420c8b56bdb37a929af4360b97bd77bb028538cd94cc3b750d06eb482aff7e5ee80bacab1ad9278d08c3f4666d51e39d39f38e039754f89413c0afd6213d71409104cd65bf59e323bb2aea27bb777076f67fa03fdd84430462aba4602a60ea74640442a0ab638fef6d3f6defbd4b400eb0da3360555fc035a2297334bfb5899e970361645b49f478041d9d2dd14d0a823e987a060e56a087e10e6a12da238f42140fc8429c71bfc73ee57f0822c080af1b31f1b8324eab7ade893bee01d170a71b02034055b306b588ccca0f7c281a25f6b0d1a89576c4e5e49231efdc9dc29d1c008d1086731347d2d299fafef07017370aa5b133bac2260f3335dfa6d8007e7d036aeec73a9fa9d40ec353cb6ab175e2088b33bd806afe74158e86778ef7f59009726c7f0793bf576a1bc3363cb4c2ebff54ce4581dc925bf4af97a63756b0c674f5cd1cfb762c47f672108bb60f9a72720dcc32cffef100838a5a45dbf89d640575d8e644f544fdfa1f8cb018885cb11846a2fd2ff1e2f87f6dfa31e36b9eaf02e2061efd289b2a10c50b57aa70a39fcc87999e61b11118554a525c736d1a3a0cfd7de9e941cba7083a822ca0296e21fc066d257db1f1644478b74358642c1102378373eb75f56c4022ec15656d64d6cf0458c2c2d669926357a97610a3ab580e75673a4ab27da1bf715afb4012cc2df1fe3edd57277971b2e602722a47d5a908bc7208e0e214f141c909b98249da5faf24595bcb12e9d0dbac35bc9154b27203c54d0437ffe5bd8cb43da48e93ac4dc0f1fe7213fe7c6800addc2bded7e669096f188c6eda58cd12e280df5451651979dff148ef953914763af09bff01f6ed02a22249b7c3e73de86bce52e66f630acd2bb71fc051f26a247b3909bab20313045fedeff70ea1c37b04eafa963167c3c28403d8795b897ffd40c2da89752f8709ced0fc804d1cecd3b1ee39f70aee0ea16583826cdfdb69c2ad9d60e539196c057f86485d71fc05c672bb0e7cb2ff27c0ba506861177cfe5659e839464435ec082839d4a6da97fbf79afbff935ff645e4bdd96410d840dd17050930df92e40402f9074a040c365399a0ffa93c4c029ee656f235e2e432124f996a96ff8cded50a47335455af231e1234595d1b611e6c4ff8a9c10fe1e5cc2cb05febe563d6410ce09e634665eb8620c2e7ab980b2c84c52d24d51050ec6adb625dcd6ca6d8c3087243a29e1e5eff92f18dde2a6404433ee3caffbfa5d26c4f8bb9536e966734b59f07ad66244c7887bcd5816a4aee2d60efac0acafccc7cd78c0ff1d8408a837994a7eeafff544bfd45a8157b0407d2c5946b41ef72f48ef00365e7bbb6374f62"
    }
  , {
    }
  };

// if the return type (blobdata for now) of block_to_blob ever changes
// from std::string, this might break.
bool compare_blocks(const block& a, const block& b)
{
  auto hash_a = pod_to_hex(get_block_hash(a));
  auto hash_b = pod_to_hex(get_block_hash(b));

  return hash_a == hash_b;
}

/*
void print_block(const block& blk, const std::string& prefix = "")
{
  std::cerr << prefix << ": " << std::endl
            << "\thash - " << pod_to_hex(get_block_hash(blk)) << std::endl
            << "\tparent - " << pod_to_hex(blk.prev_id) << std::endl
            << "\ttimestamp - " << blk.timestamp << std::endl
  ;
}

// if the return type (blobdata for now) of tx_to_blob ever changes
// from std::string, this might break.
bool compare_txs(const transaction& a, const transaction& b)
{
  auto ab = tx_to_blob(a);
  auto bb = tx_to_blob(b);

  return ab == bb;
}
*/

// convert hex string to string that has values based on that hex
// thankfully should automatically ignore null-terminator.
std::string h2b(const std::string& s)
{
  bool upper = true;
  std::string result;
  unsigned char val = 0;
  for (char c : s)
  {
    if (upper)
    {
      val = 0;
      if (c <= 'f' && c >= 'a')
      {
        val = ((c - 'a') + 10) << 4;
      }
      else
      {
        val = (c - '0') << 4;
      }
    }
    else
    {
      if (c <= 'f' && c >= 'a')
      {
        val |= (c - 'a') + 10;
      }
      else
      {
        val |= c - '0';
      }
      result += (char)val;
    }
    upper = !upper;
  }
  return result;
}

template <typename T>
class BlockchainDBTest : public testing::Test
{
protected:
  BlockchainDBTest() : m_db(new T()), m_hardfork(*m_db, 1, 0)
  {
    for (auto& i : t_blocks)
    {
      block bl;
      blobdata bd = h2b(i);
      CHECK_AND_ASSERT_THROW_MES(parse_and_validate_block_from_blob(bd, bl), "Invalid block");
      m_blocks.push_back(std::make_pair(bl, bd));
    }
    for (auto& i : t_transactions)
    {
      std::vector<std::pair<transaction, blobdata>> txs;
      for (auto& j : i)
      {
        transaction tx;
        blobdata bd = h2b(j);
        CHECK_AND_ASSERT_THROW_MES(parse_and_validate_tx_from_blob(bd, tx), "Invalid transaction");
        txs.push_back(std::make_pair(tx, bd));
      }
      m_txs.push_back(txs);
    }
  }

  ~BlockchainDBTest() {
    delete m_db;
    remove_files();
  }

  BlockchainDB* m_db;
  HardFork m_hardfork;
  std::string m_prefix;
  std::vector<std::pair<block, blobdata>> m_blocks;
  std::vector<std::vector<std::pair<transaction, blobdata>>> m_txs;
  std::vector<std::string> m_filenames;

  void init_hard_fork()
  {
    m_hardfork.init();
    m_db->set_hard_fork(&m_hardfork);
  }

  void get_filenames()
  {
    m_filenames = m_db->get_filenames();
    for (auto& f : m_filenames)
    {
      std::cerr << "File created by test: " << f << std::endl;
    }
  }

  void remove_files()
  {
    // remove each file the db created, making sure it starts with fname.
    for (auto& f : m_filenames)
    {
      if (boost::starts_with(f, m_prefix))
      {
        boost::filesystem::remove(f);
      }
      else
      {
        std::cerr << "File created by test not to be removed (for safety): " << f << std::endl;
      }
    }

    // remove directory if it still exists
    boost::filesystem::remove_all(m_prefix);
  }

  void set_prefix(const std::string& prefix)
  {
    m_prefix = prefix;
  }
};

using testing::Types;

typedef Types<BlockchainLMDB> implementations;

TYPED_TEST_CASE(BlockchainDBTest, implementations);

TYPED_TEST(BlockchainDBTest, OpenAndClose)
{
  boost::filesystem::path tempPath = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();
  std::string dirPath = tempPath.string();

  this->set_prefix(dirPath);

  // make sure open does not throw
  ASSERT_NO_THROW(this->m_db->open(dirPath));
  this->get_filenames();

  // make sure open when already open DOES throw
  ASSERT_THROW(this->m_db->open(dirPath), DB_OPEN_FAILURE);

  ASSERT_NO_THROW(this->m_db->close());
}

TYPED_TEST(BlockchainDBTest, AddBlock)
{

  boost::filesystem::path tempPath = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();
  std::string dirPath = tempPath.string();

  this->set_prefix(dirPath);

  // make sure open does not throw
  ASSERT_NO_THROW(this->m_db->open(dirPath));
  this->get_filenames();
  this->init_hard_fork();

  db_wtxn_guard guard(this->m_db);

  // adding a block with no parent in the blockchain should throw.
  // note: this shouldn't be possible, but is a good (and cheap) failsafe.
  //
  // TODO: need at least one more block to make this reasonable, as the
  // BlockchainDB implementation should not check for parent if
  // no blocks have been added yet (because genesis has no parent).
  //ASSERT_THROW(this->m_db->add_block(this->m_blocks[1], t_sizes[1], t_sizes[1], t_diffs[1], t_coins[1], this->m_txs[1]), BLOCK_PARENT_DNE);

  ASSERT_NO_THROW(this->m_db->add_block(this->m_blocks[0], t_sizes[0], t_sizes[0], t_diffs[0], t_coins[0], 0, this->m_txs[0]));
  ASSERT_NO_THROW(this->m_db->add_block(this->m_blocks[1], t_sizes[1], t_sizes[1], t_diffs[1], t_coins[1], 0, this->m_txs[1]));

  block b;
  ASSERT_TRUE(this->m_db->block_exists(get_block_hash(this->m_blocks[0].first)));
  ASSERT_NO_THROW(b = this->m_db->get_block(get_block_hash(this->m_blocks[0].first)));

  ASSERT_TRUE(compare_blocks(this->m_blocks[0].first, b));

  ASSERT_NO_THROW(b = this->m_db->get_block_from_height(0));

  ASSERT_TRUE(compare_blocks(this->m_blocks[0].first, b));

  // assert that we can't add the same block twice
  ASSERT_THROW(this->m_db->add_block(this->m_blocks[0], t_sizes[0], t_sizes[0], t_diffs[0], t_coins[0], 0, this->m_txs[0]), TX_EXISTS);

  for (auto& h : this->m_blocks[0].first.tx_hashes)
  {
    transaction tx;
    ASSERT_TRUE(this->m_db->tx_exists(h));
    ASSERT_NO_THROW(tx = this->m_db->get_tx(h));

    ASSERT_HASH_EQ(h, get_transaction_hash(tx));
  }
}

TYPED_TEST(BlockchainDBTest, RetrieveBlockData)
{
  boost::filesystem::path tempPath = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();
  std::string dirPath = tempPath.string();

  this->set_prefix(dirPath);

  // make sure open does not throw
  ASSERT_NO_THROW(this->m_db->open(dirPath));
  this->get_filenames();
  this->init_hard_fork();

  db_wtxn_guard guard(this->m_db);

  ASSERT_NO_THROW(this->m_db->add_block(this->m_blocks[0], t_sizes[0], t_sizes[0],  t_diffs[0], t_coins[0], 0, this->m_txs[0]));

  ASSERT_EQ(t_sizes[0], this->m_db->get_block_weight(0));
  ASSERT_EQ(t_diffs[0], this->m_db->get_block_cumulative_difficulty(0));
  ASSERT_EQ(t_diffs[0], this->m_db->get_block_difficulty(0));
  ASSERT_EQ(t_coins[0], this->m_db->get_block_already_generated_coins(0));

  ASSERT_NO_THROW(this->m_db->add_block(this->m_blocks[1], t_sizes[1], t_sizes[1], t_diffs[1], t_coins[1], 0, this->m_txs[1]));
  ASSERT_EQ(t_diffs[1] - t_diffs[0], this->m_db->get_block_difficulty(1));

  ASSERT_HASH_EQ(get_block_hash(this->m_blocks[0].first), this->m_db->get_block_hash_from_height(0));

  std::vector<block> blks;
  ASSERT_NO_THROW(blks = this->m_db->get_blocks_range(0, 1));
  ASSERT_EQ(2, blks.size());
  
  ASSERT_HASH_EQ(get_block_hash(this->m_blocks[0].first), get_block_hash(blks[0]));
  ASSERT_HASH_EQ(get_block_hash(this->m_blocks[1].first), get_block_hash(blks[1]));

  std::vector<crypto::hash> hashes;
  ASSERT_NO_THROW(hashes = this->m_db->get_hashes_range(0, 1));
  ASSERT_EQ(2, hashes.size());

  ASSERT_HASH_EQ(get_block_hash(this->m_blocks[0].first), hashes[0]);
  ASSERT_HASH_EQ(get_block_hash(this->m_blocks[1].first), hashes[1]);
}

}  // anonymous namespace
