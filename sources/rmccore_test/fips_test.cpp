#include "stdafx.h"

#include "openssl/evp.h"
#include "openssl/crypto.h"
#include "rmccore/crypto/aes.h"
#include "rmccore/crypto/sha.h"
#include <boost/test/unit_test.hpp>

using namespace RMCCORE;
using namespace RMCCORE::CRYPT;

BOOST_AUTO_TEST_SUITE(RmcCoreCryptoTest)

BOOST_AUTO_TEST_CASE(TestFipsApprovedAlgs)
{
    int origMode;
    int otherMode;
    int ret;
    const uint8_t data[16] = "The quick brown";

    // For AES-256 CBC
    const uint8_t kAesKey[32] = " fox jumps over the lazy dog. t";
    const uint8_t ivseed[16] = "HE QUICK BROWN ";

    uint8_t cipher1[16] = {0x01};
    uint8_t cipher2[16] = {0x02};
    uint8_t cipher3[16] = {0x03};
    uint8_t plain1[16] = {0xF1};
    uint8_t plain2[16] = {0xF2};
    uint8_t plain3[16] = {0xF3};
    uint32_t cipherLen1 = 1;
    uint32_t cipherLen2 = 2;
    uint32_t cipherLen3 = 3;
    uint32_t plainLen1 = 11;
    uint32_t plainLen2 = 12;
    uint32_t plainLen3 = 13;

    // For SHA-1
    uint8_t sha1Hash1[20] = {0x01};
    uint8_t sha1Hash2[20] = {0x02};
    uint8_t sha1Hash3[20] = {0x03};

    // For SHA-256
    uint8_t sha256Hash1[32] = {0xF1};
    uint8_t sha256Hash2[32] = {0xF2};
    uint8_t sha256Hash3[32] = {0xF3};

    // For SHA-1 HMAC
    const uint8_t kSalt[32] = "FOX JUMPS OVER THE LAZY DOG. Th";
    uint8_t sha1Code1[20] = {0x04};
    uint8_t sha1Code2[20] = {0x05};
    uint8_t sha1Code3[20] = {0x06};

    // For SHA-256 HMAC
    uint8_t sha256Code1[32] = {0xF4};
    uint8_t sha256Code2[32] = {0xF5};
    uint8_t sha256Code3[32] = {0xF6};

    //
    // Test the original mode.
    //
    origMode = FIPS_mode();

    // AES-256 CBC
    RetValue result = AesEncrypt(kAesKey, 32, data, sizeof data, ivseed, 0, 0x200, cipher1, 64, &cipherLen1);
    BOOST_CHECK(result);
    BOOST_CHECK(16 == cipherLen1);
    result = AesDecrypt(kAesKey, 32, cipher1, cipherLen1, ivseed, 0, 0x200, plain1, 64, &plainLen1);
    BOOST_CHECK(result);
    BOOST_CHECK(16 == plainLen1);
    BOOST_CHECK(0 == memcmp(plain1, data, 16));

    // SHA-1
    result = CreateSha1(data, sizeof data, sha1Hash1);
    BOOST_CHECK(result);

    // SHA-256
    result = CreateSha256(data, sizeof data, sha256Hash1);
    BOOST_CHECK(result);

    // SHA-1 HMAC
    result = CreateSha1Hmac(data, sizeof data, kSalt, 32, sha1Code1);
    BOOST_CHECK(result);

    // SHA-256 HMAC
    result = CreateSha256Hmac(data, sizeof data, kSalt, 32, sha256Code1);
    BOOST_CHECK(result);

    //
    // Test the other mode.
    //
    otherMode = (origMode == 0 ? 1 : 0);
    ret = FIPS_mode_set(otherMode);
    BOOST_CHECK(1 == ret);
    BOOST_CHECK(otherMode == FIPS_mode());

    result = AesEncrypt(kAesKey, 32, data, sizeof data, ivseed, 0, 0x200, cipher2, 64, &cipherLen2);
    BOOST_CHECK(result);
    BOOST_CHECK(16 == cipherLen2);
    BOOST_CHECK(0 == memcmp(cipher2, cipher1, 16));

    result = AesDecrypt(kAesKey, 32, cipher2, cipherLen2, ivseed, 0, 0x200, plain2, 64, &plainLen2);
    BOOST_CHECK(result);
    BOOST_CHECK(16 == plainLen2);
    BOOST_CHECK(0 == memcmp(plain2, data, 16));

    // SHA-1
    result = CreateSha1(data, sizeof data, sha1Hash2);
    BOOST_CHECK(result);
    BOOST_CHECK(0 == memcmp(sha1Hash2, sha1Hash1, sizeof sha1Hash1));

    // SHA-256
    result = CreateSha256(data, sizeof data, sha256Hash2);
    BOOST_CHECK(result);
    BOOST_CHECK(0 == memcmp(sha256Hash2, sha256Hash1, sizeof sha256Hash1));

    // SHA-1 HMAC
    result = CreateSha1Hmac(data, sizeof data, kSalt, 32, sha1Code2);
    BOOST_CHECK(result);
    BOOST_CHECK(0 == memcmp(sha1Code2, sha1Code1, sizeof sha1Code1));

    // SHA-256 HMAC
    result = CreateSha256Hmac(data, sizeof data, kSalt, 32, sha256Code2);
    BOOST_CHECK(result);
    BOOST_CHECK(0 == memcmp(sha256Code2, sha256Code1, sizeof sha256Code1));

    //
    // Test the original mode again.
    //
    ret = FIPS_mode_set(origMode);
    BOOST_CHECK(1 == ret);
    BOOST_CHECK(origMode == FIPS_mode());

    // AES-256 CBC
    result = AesEncrypt(kAesKey, 32, data, sizeof data, ivseed, 0, 0x200, cipher3, 64, &cipherLen3);
    BOOST_CHECK(result);
    BOOST_CHECK(16 == cipherLen3);
    BOOST_CHECK(0 == memcmp(cipher3, cipher1, 16));

    result = AesDecrypt(kAesKey, 32, cipher3, cipherLen3, ivseed, 0, 0x200, plain3, 64, &plainLen3);
    BOOST_CHECK(result);
    BOOST_CHECK(16 == plainLen3);
    BOOST_CHECK(0 == memcmp(plain3, data, 16));

    // SHA-1
    result = CreateSha1(data, sizeof data, sha1Hash3);
    BOOST_CHECK(result);
    BOOST_CHECK(0 == memcmp(sha1Hash3, sha1Hash1, sizeof sha1Hash1));

    // SHA-256
    result = CreateSha256(data, sizeof data, sha256Hash3);
    BOOST_CHECK(result);
    BOOST_CHECK(0 == memcmp(sha256Hash3, sha256Hash1, sizeof sha256Hash1));

    // SHA-1 HMAC
    result = CreateSha1Hmac(data, sizeof data, kSalt, 32, sha1Code3);
    BOOST_CHECK(result);
    BOOST_CHECK(0 == memcmp(sha1Code3, sha1Code1, sizeof sha1Code1));

    // SHA-256 HMAC
    result = CreateSha256Hmac(data, sizeof data, kSalt, 32, sha256Code3);
    BOOST_CHECK(result);
    BOOST_CHECK(0 == memcmp(sha256Code3, sha256Code1, sizeof sha256Code1));
}

// Make sure that non-FIPS approved algorithms succeed if-and-only-if in
// non-FIPS mode.  The purpose is to make sure that FIPS mode is actually
// turned on/off when we turn it on/off.
BOOST_AUTO_TEST_CASE(TestFipsNonApprovedAlgs)
{
    EVP_CIPHER_CTX cipherCtx;
    const uint8_t key[32] = {};
    const uint8_t iv[16] = {};
    EVP_MD_CTX mdCtx;
    int origMode;
    int otherMode;
    int ret;

    //
    // Test the original mode.
    //
    origMode = FIPS_mode();

    // Blowfish
    ret = EVP_EncryptInit(&cipherCtx, EVP_bf_cbc(), key, iv);
    BOOST_CHECK((0 == origMode) == (1 == ret));

    // MD5
    ret = EVP_DigestInit(&mdCtx, EVP_md5());
    BOOST_CHECK((0 == origMode) == (1 == ret));

    //
    // Test the other mode.
    //
    otherMode = (origMode == 0 ? 1 : 0);
    ret = FIPS_mode_set(otherMode);
    BOOST_CHECK(1 == ret);
    BOOST_CHECK(otherMode == FIPS_mode());

    // Blowfish
    ret = EVP_EncryptInit(&cipherCtx, EVP_bf_cbc(), key, iv);
    BOOST_CHECK((0 == otherMode) == (1 == ret));

    // MD5
    ret = EVP_DigestInit(&mdCtx, EVP_md5());
    BOOST_CHECK((0 == otherMode) == (1 == ret));

    //
    // Test the original mode again.
    //
    ret = FIPS_mode_set(origMode);
    BOOST_CHECK(1 == ret);
    BOOST_CHECK(origMode == FIPS_mode());

    // Blowfish
    ret = EVP_EncryptInit(&cipherCtx, EVP_bf_cbc(), key, iv);
    BOOST_CHECK((0 == origMode) == (1 == ret));

    // MD5
    ret = EVP_DigestInit(&mdCtx, EVP_md5());
    BOOST_CHECK((0 == origMode) == (1 == ret));
}

BOOST_AUTO_TEST_SUITE_END()
