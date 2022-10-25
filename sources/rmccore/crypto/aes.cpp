//
//  aes.cpp
//  rmccore
//
//  Created by Gavin Ye on 10/19/17.
//  Copyright © 2017 NextLabs, Inc. All rights reserved.
//

#include "stdafx.h"

#include "rmccore/rmccore.h"
#include "rmccore/crypto/crypto.h"
#include "rmccore/crypto/aes.h"

#include "openssl/aes.h"
#include "openssl/evp.h"
#include "openssl/rand.h"

namespace RMCCORE {
namespace CRYPT {
   
namespace {

FORCEINLINE void initIv(const uint8_t* seed, uint64_t offset, unsigned char* iv)
{
    if (nullptr == seed) {
        memset(iv, 0, 16);
        memcpy(iv, &offset, 8);
    }
    else {
        memcpy(iv, seed, 16);
        if (offset) {
            offset = (offset - 1) * 31;
        }
        ((uint64_t*)iv)[0] ^= offset;
        ((uint64_t*)iv)[1] ^= offset;
    }
}
    
RetValue AesEncryptBlock(EVP_CIPHER_CTX *ctx,
                         const void* key,
                         uint32_t keySize,
                         const void* inBuf,
                         uint32_t inBufLen,
                         const uint8_t* ivSeed,
                         const uint64_t blockOffset,
                         uint32_t blockSize,
                         void* outBuf,
                         uint32_t outBufLen,
                         uint32_t* returnedLen
                         )
{
    RetValue result = RETSUCCEED();
    uint8_t iv[16] = {0};
    int outLen = 0;

	if (blockSize == 0)
		return RETVALUE(-1);
    if(0 != (blockOffset % blockSize))
        return RETVALUE(-1);
    if(0 != (inBufLen % 16))
        return RETVALUE(-1);
    if(inBufLen > blockSize)
        return RETVALUE(-1);
    if(outBufLen < inBufLen)
        return RETVALUE(-1);

    initIv(ivSeed, blockOffset, iv);

    // Init
    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, reinterpret_cast<const uint8_t*>(key), iv))
        return RETVALUE(-1);

    EVP_CIPHER_CTX_set_padding(ctx, 0);

    if(1 != EVP_EncryptUpdate(ctx, reinterpret_cast<uint8_t*>(outBuf), &outLen, reinterpret_cast<const uint8_t*>(inBuf), inBufLen))
        return RETVALUE(-1);

    outBuf = (void*)(((uint8_t*)outBuf) + outLen);
    outBufLen -= outLen;
    *returnedLen += outLen;

    return result;
}

RetValue AesDecryptBlock(EVP_CIPHER_CTX *ctx,
                         const void* key,
                         uint32_t keySize,
                         const void* inBuf,
                         uint32_t inBufLen,
                         const uint8_t* ivSeed,
                         const uint64_t blockOffset,
                         uint32_t blockSize,
                         void* outBuf,
                         uint32_t outBufLen,
                         uint32_t* returnedLen
                         )
{
    RetValue result = RETSUCCEED();
    uint8_t iv[16] = {0};
    int outLen = 0;

	if (blockSize == 0)
		return RETVALUE(-1);
    if(0 != (blockOffset % blockSize))
        return RETVALUE(-1);
    if(0 != (inBufLen % 16))
        return RETVALUE(-1);
    if(inBufLen > blockSize)
        return RETVALUE(-1);
    if(outBufLen < inBufLen)
        return RETVALUE(-1);

    initIv(ivSeed, blockOffset, iv);

    // Init
    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, reinterpret_cast<const uint8_t*>(key), iv))
        return RETVALUE(-1);

    EVP_CIPHER_CTX_set_padding(ctx, 0);

    if(1 != EVP_DecryptUpdate(ctx, reinterpret_cast<uint8_t*>(outBuf), &outLen, reinterpret_cast<const uint8_t*>(inBuf), inBufLen))
        return RETVALUE(-1);

    outBuf = (void*)(((uint8_t*)outBuf) + outLen);
    outBufLen -= outLen;
    *returnedLen += outLen;

    return result;
}
    
}
    
RetValue AesEncrypt(const void* key,
                    uint32_t keySize,
                    const void* inBuf,
                    uint32_t inBufLen,
                    const uint8_t* ivSeed,
                    uint64_t offset,
                    uint32_t blockSize,
                    void* outBuf,
                    uint32_t outBufLen,
                    uint32_t* returnedLen
                    )
{
    RetValue result = RETSUCCEED();
    EVP_CIPHER_CTX *ctx = nullptr;

    *returnedLen = 0;

	if (blockSize == 0)
		return RETVALUE(-1);
    if(0 != (offset % blockSize))
        return RETVALUE(-1);
    if(0 != (inBufLen % 16))
        return RETVALUE(-1);
    if(outBufLen < inBufLen)
        return RETVALUE(-1);

    if(nullptr == (ctx = EVP_CIPHER_CTX_new()))
        return RETVALUE(-1);

    while(inBufLen != 0 && outBufLen != 0) {

        uint32_t outLen = 0;
        uint32_t inLen = (inBufLen > blockSize) ? blockSize : inBufLen;

        result = AesEncryptBlock(ctx, key, keySize, inBuf, inLen, ivSeed, offset, blockSize, outBuf, outBufLen, &outLen);
        if(!result)
            break;

        inBuf = (const void*)((const uint8_t*)inBuf + inLen);
        outBuf = (void*)((uint8_t*)outBuf + outLen);
        inBufLen -= inLen;
        outBufLen -= outLen;
        *returnedLen += outLen;
    }
    if(ctx) {
        EVP_CIPHER_CTX_free(ctx);
        ctx = nullptr;
    }

    return result;
}

RetValue AesDecrypt(const void* key,
                    uint32_t keySize,
                    const void* inBuf,
                    uint32_t inBufLen,
                    const uint8_t* ivSeed,
                    uint64_t offset,
                    uint32_t blockSize,
                    void* outBuf,
                    uint32_t outBufLen,
                    uint32_t* returnedLen
                    )
{
    RetValue result = RETSUCCEED();
    EVP_CIPHER_CTX *ctx = nullptr;

    *returnedLen = 0;

	if (blockSize == 0)
		return RETVALUE(-1);
    if(0 != (offset % blockSize))
        return RETVALUE(-1);
    if(0 != (inBufLen % 16))
        return RETVALUE(-1);
    if(outBufLen < inBufLen)
        return RETVALUE(-1);

    if(nullptr == (ctx = EVP_CIPHER_CTX_new()))
        return RETVALUE(-1);

    while(inBufLen != 0 && outBufLen != 0) {

        uint32_t outLen = 0;
        uint32_t inLen = (inBufLen > blockSize) ? blockSize : inBufLen;

        result = AesDecryptBlock(ctx, key, keySize, inBuf, inLen, ivSeed, offset, blockSize, outBuf, outBufLen, &outLen);
        if(!result)
            break;

        inBuf = (const void*)((const uint8_t*)inBuf + inLen);
        outBuf = (void*)((uint8_t*)outBuf + outLen);
        inBufLen -= inLen;
        outBufLen -= outLen;
        *returnedLen += outLen;
    }
    if(ctx) {
        EVP_CIPHER_CTX_free(ctx);
        ctx = nullptr;
    }

    return result;
}

bool randBytes(uint8_t* buf, uint32_t size)
{
    return (1 == RAND_bytes(buf, size));
}

}
}
