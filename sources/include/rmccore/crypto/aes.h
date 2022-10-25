//
//  aes.h
//  rmccore
//
//  Created by Gavin Ye on 10/19/17.
//  Copyright © 2017 NextLabs, Inc. All rights reserved.
//

#pragma once
#ifndef __RMCCORE_CRYPTO_AES_H__
#define __RMCCORE_CRYPTO_AES_H__

#include "rmccore/rmccore.h"

namespace RMCCORE {
namespace CRYPT {

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
                    );

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
                    );

bool randBytes(uint8_t* buf, uint32_t size);
}
}

#endif /* __RMCCORE_CRYPTO_AES_H__ */
