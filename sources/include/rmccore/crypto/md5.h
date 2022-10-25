//
//  md5.h
//  rmccore
//
//  Created by Gavin Ye on 10/19/17.
//  Copyright © 2017 NextLabs, Inc. All rights reserved.
//

#pragma once
#ifndef __RMCCORE_CRYPTO_MD5_H__
#define __RMCCORE_CRYPTO_MD5_H__

#include "rmccore/rmccore.h"

namespace RMCCORE {
namespace CRYPT {
        
RetValue CreateMd5();
RetValue CreateMd5Hmac();
        
}
}

#endif /* __RMCCORE_CRYPTO_MD5_H__ */
