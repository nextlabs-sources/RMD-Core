#include "stdafx.h"

#include "rmccore/crypto/aes.h"
#include <boost/test/unit_test.hpp>

using namespace RMCCORE;
using namespace RMCCORE::CRYPT;

static const uint8_t kAesKey[32] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
};

BOOST_AUTO_TEST_SUITE(RmcCoreCryptoTest)


BOOST_AUTO_TEST_CASE(TestAes)
{
    const uint8_t data[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF };
    uint8_t cipher[16] = {0};
    uint8_t plain[16] = {0};
    uint32_t cipherLen = 0;
    uint32_t plainLen = 0;
	uint8_t ivseed[16] = { 0 };

    RetValue result = AesEncrypt(kAesKey, 32, data, sizeof(data), ivseed, 0, 0x200, cipher, 64, &cipherLen);
    BOOST_CHECK(result);
    BOOST_CHECK(16 == cipherLen);

    result = AesDecrypt(kAesKey, 32, cipher, cipherLen, ivseed, 0, 0x200, plain, 64, &plainLen);
    BOOST_CHECK(result);
    BOOST_CHECK(16 == plainLen);
    BOOST_CHECK(0 == memcmp(plain, data, 16));
}

BOOST_AUTO_TEST_CASE(TestRand)
{
    const std::vector<uint8_t> kZeroRand(32, 0);
    std::vector<uint8_t> rand16(16, 0);
    std::vector<uint8_t> rand32(32, 0);

    BOOST_CHECK(CRYPT::randBytes(rand16.data(), 16));
    BOOST_CHECK(0 != memcmp(rand16.data(), kZeroRand.data(), 16));
    BOOST_CHECK(CRYPT::randBytes(rand32.data(), 32));
    BOOST_CHECK(0 != memcmp(rand32.data(), kZeroRand.data(), 32));
}

BOOST_AUTO_TEST_SUITE_END()
