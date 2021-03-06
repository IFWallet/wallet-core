// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "../FreeCash/Amount.h"
#include "../FreeCash/Script.h"

#include <memory>

namespace TW::FreeCash {

/// FreeCash transaction output.
struct TransactionOutput {
    /// Transaction amount.
    Amount value;

    /// Usually contains the public key as a FreeCash script setting up
    /// conditions to claim this output.
    Script script;

    /// Initializes an empty transaction output.
    TransactionOutput() = default;

    /// Initializes a transaction output with a value and a script.
    TransactionOutput(Amount value, Script script) : value(value), script(std::move(script)) {}

    /// Encodes the output into the provided buffer.
    void encode(std::vector<uint8_t>& data) const;
};

} // namespace TW::FreeCash

/// Wrapper for C interface.
struct TWFreeCashTransactionOutput {
    TW::FreeCash::TransactionOutput impl;
};
