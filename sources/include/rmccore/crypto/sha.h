//
//  sha.h
//  rmccore
//
//  Created by Gavin Ye on 10/19/17.
//  Copyright © 2017 NextLabs, Inc. All rights reserved.
//

#pragma once
#ifndef __RMCCORE_CRYPTO_SHA_H__
#define __RMCCORE_CRYPTO_SHA_H__

#include "rmccore/rmccore.h"

namespace RMCCORE {
namespace CRYPT {

RetValue CreateSha1(const void* inBuf, uint32_t inLen, unsigned char* outBuf);
RetValue CreateSha1Hmac(const void* inBuf,
                        uint32_t inLen,
                        const void* salt,
                        uint32_t saltLen,
                        unsigned char* outBuf);
RetValue CreateSha256(const void* inBuf, uint32_t inLen, unsigned char* outBuf);
RetValue CreateSha256Hmac(const void* inBuf,
                          uint32_t inLen,
                          const void* salt,
                          uint32_t saltLen,
                          unsigned char* outBuf);
        
}
}

#endif /* __RMCCORE_CRYPTO_SHA_H__ */
