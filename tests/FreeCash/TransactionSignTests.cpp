// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "FreeCash/OutPoint.h"
#include "FreeCash/Script.h"
#include "FreeCash/UnspentSelector.h"
#include "FreeCash/TransactionPlan.h"
#include "FreeCash/TransactionBuilder.h"
#include "proto/FreeCash.pb.h"

#include <TrustWalletCore/TWCoinType.h>

#include <gtest/gtest.h>

using namespace TW;
using namespace TW::FreeCash;

auto const txOutPoint = OutPoint(std::vector<uint8_t>(32), 0);

inline auto sum(const std::vector<Proto::UnspentTransaction>& utxos) {
    int64_t s = 0u;
    for (auto& utxo : utxos) {
        s += utxo.amount();
    }
    return s;
}

inline auto buildUTXO(const OutPoint& outPoint, Amount amount) {
    Proto::UnspentTransaction utxo;
    utxo.set_amount(amount);
    utxo.mutable_out_point()->set_hash(outPoint.hash.data(), outPoint.hash.size());
    utxo.mutable_out_point()->set_index(outPoint.index);
    return utxo;
}

inline auto buildSigningInput(Amount amount, int byteFee, const std::vector<Proto::UnspentTransaction>& utxos, bool useMaxAmount, enum TWCoinType coin) {
    Proto::SigningInput input;
    input.set_amount(amount);
    input.set_byte_fee(byteFee);
    input.set_use_max_amount(useMaxAmount);
    input.set_coin_type(coin);
    *input.mutable_utxo() = { utxos.begin(), utxos.end() };
    return input;
}

TEST(FreeCashTransaction, NonMaxAmount) {
    auto utxos = std::vector<Proto::UnspentTransaction>();
    utxos.push_back(buildUTXO(txOutPoint, 1000000000));

    auto sigingInput = buildSigningInput(10000, 1, utxos, false, TWCoinTypeFreeCash);
    auto txPlan = TransactionBuilder::plan(sigingInput);

    ASSERT_EQ(txPlan.amount, 10000);
    ASSERT_EQ(txPlan.change, 999989774);
}

TEST(FreeCashTransactionPlan, MaxAmount) {
    auto utxos = std::vector<Proto::UnspentTransaction>();
    utxos.push_back(buildUTXO(txOutPoint, 1000000000));

    ASSERT_EQ(sum(utxos), 1000000000);

    auto sigingInput = buildSigningInput(39200, 32, utxos, true, TWCoinTypeBitcoin);
    auto txPlan = TransactionBuilder::plan(sigingInput);

    ASSERT_EQ(txPlan.availableAmount, 30000);
    ASSERT_EQ(txPlan.amount, 19120);
    ASSERT_EQ(txPlan.change, 0);
    ASSERT_EQ(txPlan.fee, 10880);
}

TEST(FreeCashTransactionPlan, MaxAmountDoge) {
    auto utxos = std::vector<Proto::UnspentTransaction>();
    utxos.push_back(buildUTXO(txOutPoint, Amount(100000000)));
    utxos.push_back(buildUTXO(txOutPoint, Amount(2000000000)));
    utxos.push_back(buildUTXO(txOutPoint, Amount(200000000)));

    ASSERT_EQ(sum(utxos), Amount(2300000000));

    auto sigingInput = buildSigningInput(Amount(2300000000), 100, utxos, true, TWCoinTypeFreeCash);
    auto txPlan = TransactionBuilder::plan(sigingInput);

    ASSERT_EQ(txPlan.availableAmount, Amount(2300000000));
    ASSERT_EQ(txPlan.amount, Amount(2299951200));
    ASSERT_EQ(txPlan.change, 0);
    ASSERT_EQ(txPlan.fee, 48800);
}

TEST(FreeCashTransactionPlan, AmountDecred) {
    auto utxos = std::vector<Proto::UnspentTransaction>();
    utxos.push_back(buildUTXO(txOutPoint, Amount(39900000)));

    auto sigingInput = buildSigningInput(Amount(10000000), 10, utxos, false, TWCoinTypeFreeCash);
    auto txPlan = TransactionBuilder::plan(sigingInput);

    ASSERT_EQ(txPlan.availableAmount, Amount(39900000));
    ASSERT_EQ(txPlan.amount, Amount(10000000));
    ASSERT_EQ(txPlan.change, 29897740);
    ASSERT_EQ(txPlan.fee, 2260);
}

