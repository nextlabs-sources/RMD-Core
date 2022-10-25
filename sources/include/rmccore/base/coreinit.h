//
//  coreinit.h
//  rmccore
//
//  Created by Allen Yuen on 6/4/18.
//  Copyright (C) 2017-2018 NextLabs, Inc.  All rights reserved.
//

#pragma once
#ifndef __RMCCORE_BASE_COREINIT_H__
#define __RMCCORE_BASE_COREINIT_H__

namespace RMCCORE {

    void CoreInit(const std::string& appId, const std::string& routerUrl);
    void CoreCleanup(void);

}

#endif /* __RMCCORE_BASE_COREINIT_H__ */
