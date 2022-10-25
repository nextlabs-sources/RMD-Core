//
//  coreinit.cpp
//  rmccore
//
//  Created by Gavin Ye on 10/21/17.
//  Copyright © 2017 NextLabs, Inc. All rights reserved.
//

#include "stdafx.h"
#include "openssl/crypto.h"
#include "openssl/err.h"

#include "nxversion.h"
#include "common/celog2/celog.h"
#include "rmccore/base/coreinit.h"

#define CELOG_CUR_MODULE "rmccore"
#define CELOG_CUR_FILE CELOG_FILEPATH_SOURCES_RMCCORE_BASE_COREINIT_CPP

std::string gCoreAppId;
std::string gCoreRouterUrl;

namespace RMCCORE {

void CoreInit(const std::string& appId, const std::string& routerUrl)
{
    bool result = CELog_Init();
    if (!result) {
#ifdef WIN32
        OutputDebugString(__FUNCTION__ ": ERROR: CELog_Init() failed!");
#else
#endif
    }

    CELOG_LOG(CELOG_CRITICAL, "\n");
    CELOG_LOG(CELOG_CRITICAL, "===============================================================\n");
    CELOG_LOG(CELOG_CRITICAL, "*                 SkyDRM Client Core Library                  *\n");
    CELOG_LOG(CELOG_CRITICAL, "*                     Version %02d.%02d.%05d                     *\n", VERSION_MAJOR, VERSION_MINOR, BUILD_NUMBER);
    CELOG_LOG(CELOG_CRITICAL, "===============================================================\n");
    CELOG_LOG(CELOG_CRITICAL, "\n");

    gCoreAppId = appId;
    gCoreRouterUrl = routerUrl;

    // Tell OpenSSL to Enter FIPS mode.
    //
    // We only tell OpenSSL to enter FIPS mode only if it is not already in
    // FIPS mode.  Otherwise, if we tell it to enter FIPS mode while it is
    // already in FIPS mode, not only will FIPS_mode_set() return error, but
    // also many crypto functions will fail afterwards.
    //
    // We also don't re-initialize OpenSSL here by telling it to exit and
    // re-enter FIPS mode if it is already in FIPS mode.  This is in case some
    // other module in the current process besides RMD-Core is also using the
    // OpenSSL library.
    if (FIPS_mode() != 0) {
        CELOG_LOG(CELOG_INFO, "Already in FIPS mode\n");
    } else {
        if (FIPS_mode_set(1) == 0) {
            // Log the error as WARNING level instead of ERROR level.  This is
            // because the error may be caused by a non-FIPS-capable
            // libeay32.dll, which is currently needed for RMX for MS Office.
            CELOG_LOG(CELOG_WARNING, "FIPS_mode_set() returns 0\n");
            unsigned long err;
            while ((err = ERR_get_error()) != 0) {
                CELOG_LOG(CELOG_WARNING, "\tmsg = %lu, %s\n", err, ERR_error_string(err, NULL));
            }
        }
    }
}

void CoreCleanup(void)
{
    gCoreAppId.clear();
    gCoreRouterUrl.clear();

    CELOG_LOG(CELOG_CRITICAL, "SkyDRM Client Core Library cleaning up\n");
    CELog_Destroy();
}
    
}
