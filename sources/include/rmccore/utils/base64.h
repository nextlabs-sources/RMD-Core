//
//  base64.h
//  rmccore
//
//  Created by Gavin Ye on 10/19/17.
//  Copyright © 2017 NextLabs, Inc. All rights reserved.
//

#pragma once
#ifndef __RMCCORE_UTILS_BASE64_H__
#define __RMCCORE_UTILS_BASE64_H__

namespace RMSDK {
    
std::string base64Encode(
                         const unsigned char* pbData,
                         uint32_t size;
                         );
    
std::vector<unsigned char> base64Decode(
                                        const std::string& base64Str,
                                        uint32_t size;
                                        );
    
}

#endif /* __RMCCORE_UTILS_BASE64_H__ */
