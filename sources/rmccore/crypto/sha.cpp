//
//  sha.cpp
//  rmccore
//
//  Created by Gavin Ye on 10/19/17.
//  Copyright © 2017 NextLabs, Inc. All rights reserved.
//

#include "stdafx.h"

#include "rmccore/rmccore.h"
#include "rmccore/crypto/sha.h"

#include <openssl/evp.h>
#include <openssl/hmac.h>

namespace RMCCORE {
namespace CRYPT {

RetValue CreateSha1(const void* inBuf, uint32_t inLen, unsigned char* outBuf)
{
    EVP_MD_CTX *mdctx = EVP_MD_CTX_create();
    RetValue result = RETSUCCEED();
    uint32_t outLen = 20;

    if(mdctx == NULL)
        RETVALUE(-1);

    do {
        if (1 != EVP_DigestInit_ex(mdctx, EVP_sha1(), NULL)) {
            result = RETVALUE(-1);
            break;
        }

        if(1 != EVP_DigestUpdate(mdctx, inBuf, inLen)) {
            result = RETVALUE(-1);
            break;
        }

        if(1 != EVP_DigestFinal_ex(mdctx, outBuf, &outLen)) {
            result = RETVALUE(-1);
            break;
        }

    } while(false);
    EVP_MD_CTX_destroy(mdctx);

    return result;
}

RetValue CreateSha1Hmac(const void* inBuf,
                        uint32_t inLen,
                        const void* salt,
                        uint32_t saltLen,
                        unsigned char* outBuf)
{
    uint32_t outLen = 20;
    HMAC(EVP_sha1(), salt, saltLen, static_cast<const unsigned char*>(inBuf), inLen, outBuf, &outLen);
    return RETSUCCEED();
}

RetValue CreateSha256(const void* inBuf, uint32_t inLen, unsigned char* outBuf)
{
    EVP_MD_CTX *mdctx = EVP_MD_CTX_create();
    RetValue result = RETSUCCEED();
    uint32_t outLen = 32;

    if(mdctx == NULL)
        RETVALUE(-1);

    do {
        if (1 != EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL)) {
            result = RETVALUE(-1);
            break;
        }

        if(1 != EVP_DigestUpdate(mdctx, inBuf, inLen)) {
            result = RETVALUE(-1);
            break;
        }

        if(1 != EVP_DigestFinal_ex(mdctx, outBuf, &outLen)) {
            result = RETVALUE(-1);
            break;
        }

    } while(false);
    EVP_MD_CTX_destroy(mdctx);

    return result;
}

RetValue CreateSha256Hmac(const void* inBuf,
                          uint32_t inLen,
                          const void* salt,
                          uint32_t saltLen,
                          unsigned char* outBuf)
{
    uint32_t outLen = 32;
    HMAC(EVP_sha256(), salt, saltLen, static_cast<const unsigned char*>(inBuf), inLen, outBuf, &outLen);
    return RETSUCCEED();
}

}
}
