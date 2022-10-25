//
//  crypto.h
//  rmccore
//
//  Created by Gavin Ye on 10/19/17.
//  Copyright © 2017 NextLabs, Inc. All rights reserved.
//

#pragma once
#ifndef __RMCCORE_CRYPTO_CRYPTO_H__
#define __RMCCORE_CRYPTO_CRYPTO_H__

namespace RMCCORE {

class CryptoSession
{
public:
    ~CryptoSession();
    
    static CryptoSession& getCryptoSession();
    
protected:
    CryptoSession();
};
    
}

#endif /* __RMCCORE_CRYPTO_CRYPTO_H__ */
