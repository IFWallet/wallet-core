// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Seele/Address.h"
#include "Seele/RLP.h"
#include "Seele/Signer.h"
#include "HexCoding.h"
#include "proto/Seele.pb.h"
#include "Base64.h"



#include <gtest/gtest.h>

namespace TW::Seele {


TEST(SeeleSigner, Sign) {
    auto input = Proto::SigningInput();

    auto &transaction = *input.mutable_sign_transaction();
    transaction.set_type(0);
    transaction.set_from("0xe95d99fec90954eb8f6f899c188aef5caa20d501");
    transaction.set_to("0x0a57a2714e193b7ac50475ce625f2dcfb483d741");
    transaction.set_amount(0);
    transaction.set_account_nonce(0);
    transaction.set_gas_price(1);
    transaction.set_gas_limit(3000000);
    transaction.set_timestamp(0);
    auto payload = (parse_hex(""));

    transaction.set_payload(payload.data(),payload.size());


    auto key = (parse_hex("0xa417551e1522d88d8b2c1592f9e273f7f8bf68517195418b4b21d40e17cdaa1f"));
    input.set_private_key(key.data(), key.size());


    auto signer = Seele::Signer(std::move(input));
    auto signature = signer.sign();
    auto signatureInBase64 = Base64::encode(signature);

    auto output = signer.build();

    ASSERT_EQ("H8VP/eTJVUohMa08OL5N3tk+qceOnmGTrN3xWbA3HuBmgus1z7mLJg4Gqgv1a8kB6eSIwtAkXX8yRGgK42WY5gE=", signatureInBase64);

}

} // namespace TW::Seele
