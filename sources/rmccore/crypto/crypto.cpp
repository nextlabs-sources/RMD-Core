//
//  crypto.cpp
//  rmccore
//
//  Created by Gavin Ye on 10/20/17.
//  Copyright © 2017 NextLabs, Inc. All rights reserved.
//

#include "stdafx.h"
#include "rmccore/format/nxlfile.h"
#include "rmccore/utils/string.h"
#include "rmccore/crypto/crypto.h"

#include "openssl/aes.h"
#include "openssl/evp.h"

namespace RMCCORE {

CryptoSession::CryptoSession()
{
}
    
CryptoSession::~CryptoSession()
{
}
    
CryptoSession& CryptoSession::getCryptoSession()
{
    static CryptoSession cryptoSession;
    return cryptoSession;
}
    
}
