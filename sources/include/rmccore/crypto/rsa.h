//
//  rsa.h
//  rmccore
//
//  Created by Gavin Ye on 10/19/17.
//  Copyright © 2017 NextLabs, Inc. All rights reserved.
//

#pragma once
#ifndef __RMCCORE_CRYPTO_RSA_H__
#define __RMCCORE_CRYPTO_RSA_H__

#include "rmccore/rmccore.h"

namespace RMCCORE {
namespace CRYPT {
        
RetValue RsaEncrypt(const void* keyBlob,
                    uint32_t keyBlobLen,
                    const void* inBuf,
                    uint32_t inBufLen,
                    void* outBuf,
                    uint32_t outBufLen,
                    uint32_t* returnedLen);

RetValue RsaDecrypt(const void* keyBlob,
                    uint32_t keyBlobLen,
                    const void* inBuf,
                    uint32_t inBufLen,
                    void* outBuf,
                    uint32_t outBufLen,
                    uint32_t* returnedLen);
    
RetValue RsaSign(const void* keyBlob,
                 uint32_t keyBlobLen,
                 const void* inBuf,
                 uint32_t inBufLen,
                 void* outBuf,
                 uint32_t outBufLen,
                 uint32_t* returnedLen);
    
RetValue RsaVerify(const void* keyBlob,
                   uint32_t keyBlobLen,
                   const void* inBuf,
                   uint32_t inBufLen,
                   const void* hashValue,
                   uint32_t hashValueLen);
        
}
}

#endif /* __RMCCORE_CRYPTO_RSA_H__ */
