//
//  time.h
//  rmccore
//
//  Created by Haobo Wang on 3/28/18.
//  Copyright © 2018 NextLabs, Inc. All rights reserved.
//

#pragma once
#ifndef __RMCCORE_UTILS_TIME_H__
#define __RMCCORE_UTILS_TIME_H__

#include "rmccore/base/coreerror.h"
#include "rmccore/base/coredefs.h"
#include "rmccore/utils/string.h"

namespace RMCCORE {
    class NXTime{
    public:
        // static const
        static FORCEINLINE uint64_t nowTime()
        {
            return (uint64_t)std::time(nullptr);
        }
        
        static uint64_t stringToTime(const std::string& s);
        static std::string timeToString(uint64_t t);
        static int getTimeZoneOffset(void);
    }; 
}

#endif /* __RMCCORE_UTILS_TIME_H__ */
