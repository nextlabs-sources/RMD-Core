#include "stdafx.h"

#include "rmccore/crypto/sha.h"
#include <boost/test/unit_test.hpp>

using namespace RMCCORE;
using namespace RMCCORE::CRYPT;

static const uint8_t kSalt[32] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
};

BOOST_AUTO_TEST_SUITE(RmcCoreCryptoTest)

BOOST_AUTO_TEST_CASE(TestSha1)
{
    const uint8_t data[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF };
    uint8_t hash0[20] = {0};
    uint8_t hash1[20] = {0};
    uint32_t cipherLen = 0;
    uint32_t plainLen = 0;

    RetValue result = CreateSha1(data, sizeof(data), hash0);
    BOOST_CHECK(result);
    result = CreateSha1(data, sizeof(data), hash1);
    BOOST_CHECK(result);
    BOOST_CHECK(0 == memcmp(hash0, hash1, 20));

    result = CreateSha1Hmac(data, sizeof(data), kSalt, 32, hash0);
    BOOST_CHECK(result);
    result = CreateSha1Hmac(data, sizeof(data), kSalt, 32, hash1);
    BOOST_CHECK(result);
    BOOST_CHECK(0 == memcmp(hash0, hash1, 20));
}


BOOST_AUTO_TEST_CASE(TestSha256)
{
    const uint8_t data[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF };
    uint8_t hash0[32] = {0};
    uint8_t hash1[32] = {0};
    uint32_t cipherLen = 0;
    uint32_t plainLen = 0;

    RetValue result = CreateSha256(data, sizeof(data), hash0);
    BOOST_CHECK(result);
    result = CreateSha256(data, sizeof(data), hash1);
    BOOST_CHECK(result);
    BOOST_CHECK(0 == memcmp(hash0, hash1, 32));

    result = CreateSha256Hmac(data, sizeof(data), kSalt, 32, hash0);
    BOOST_CHECK(result);
    result = CreateSha256Hmac(data, sizeof(data), kSalt, 32, hash1);
    BOOST_CHECK(result);
    BOOST_CHECK(0 == memcmp(hash0, hash1, 32));
}

BOOST_AUTO_TEST_SUITE_END()
