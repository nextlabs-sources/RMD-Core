//
//  crc.h
//  rmccore
//
//  Created by Gavin Ye on 10/19/17.
//  Copyright © 2017 NextLabs, Inc. All rights reserved.
//

#pragma once
#ifndef __RMCCORE_UTILS_CRC_H__
#define __RMCCORE_UTILS_CRC_H__

namespace RMSDK {
   
uint32_t crc32(const unsigned char* data, uint32_t size);
uint64_t crc64(const unsigned char* data, uint32_t size);
    
}

#endif /* __RMCCORE_UTILS_CRC_H__ */
