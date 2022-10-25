//
//  rmremoteviewer.h
//  rmccore
//
//  Created by Allen Yuen on 5/23/19.
//  Copyright © 2018 NextLabs, Inc. All rights reserved.
//

#pragma once
#ifndef __RMCCORE_RESTFUL_RMREMOTEVIEWER_H__
#define __RMCCORE_RESTFUL_RMREMOTEVIEWER_H__

//
// Remote Viewer supported operations.
//
// The following values can be OR'ed together.
// Actual values must match RMS RESTful API spec.
#define REMOTEVIEWER_OPERATION_VIEWFILEINFO     0x0000000000000001ULL
#define REMOTEVIEWER_OPERATION_PRINT            0x0000000000000002ULL
#define REMOTEVIEWER_OPERATION_PROTECT          0x0000000000000004ULL
#define REMOTEVIEWER_OPERATION_SHARE            0x0000000000000008ULL
#define REMOTEVIEWER_OPERATION_DOWNLOAD         0x0000000000000010ULL

#endif /* __RMCCORE_RESTFUL_RMREMOTEVIEWER_H__ */
