//
//  nxlrights.h
//  rmccore
//
//  Created by Gavin Ye on 10/20/17.
//  Copyright � 2017 NextLabs, Inc. All rights reserved.
//

#pragma once
#ifndef __RMCCORE_FORMAT_NXLRIGHTS_H__
#define __RMCCORE_FORMAT_NXLRIGHTS_H__

#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif


//
//  Define Built-in Rights
//
#define BUILTIN_RIGHT_VIEW                      0x0000000000000001ULL
#define BUILTIN_RIGHT_EDIT                      0x0000000000000002ULL
#define BUILTIN_RIGHT_PRINT                     0x0000000000000004ULL
#define BUILTIN_RIGHT_CLIPBOARD                 0x0000000000000008ULL
#define BUILTIN_RIGHT_SAVEAS                    0x0000000000000010ULL
#define BUILTIN_RIGHT_DECRYPT                   0x0000000000000020ULL
#define BUILTIN_RIGHT_SCREENCAP                 0x0000000000000040ULL
#define BUILTIN_RIGHT_SEND                      0x0000000000000080ULL
#define BUILTIN_RIGHT_CLASSIFY                  0x0000000000000100ULL
#define BUILTIN_RIGHT_SHARE                     0x0000000000000200ULL
#define BUILTIN_RIGHT_DOWNLOAD                  0x0000000000000400ULL

#define BUILTIN_OBRIGHT_WATERMARK               0x0000000040000000ULL


#define BUILTIN_RIGHT_ALL                       (BUILTIN_RIGHT_VIEW \
                                                 | BUILTIN_RIGHT_EDIT \
                                                 | BUILTIN_RIGHT_PRINT \
                                                 | BUILTIN_RIGHT_CLIPBOARD \
                                                 | BUILTIN_RIGHT_SAVEAS \
                                                 | BUILTIN_RIGHT_DECRYPT \
                                                 | BUILTIN_RIGHT_SCREENCAP \
                                                 | BUILTIN_RIGHT_SEND \
                                                 | BUILTIN_RIGHT_CLASSIFY \
												 | BUILTIN_RIGHT_SHARE \
												 | BUILTIN_RIGHT_DOWNLOAD)
    
#define RIGHT_ACTION_VIEW                       "VIEW"
#define RIGHT_ACTION_EDIT                       "EDIT"
#define RIGHT_ACTION_PRINT                      "PRINT"
#define RIGHT_ACTION_CLIPBOARD                  "CLIPBOARD"
#define RIGHT_ACTION_SAVEAS                     "SAVEAS"
#define RIGHT_ACTION_DECRYPT                    "DECRYPT"
#define RIGHT_ACTION_SCREENCAP                  "SCREENCAP"
#define RIGHT_ACTION_SEND                       "SEND"
#define RIGHT_ACTION_CLASSIFY                   "CLASSIFY"
#define RIGHT_ACTION_SHARE                      "SHARE"
#define RIGHT_ACTION_DOWNLOAD                   "DOWNLOAD"

#define OBLIGATION_NAME_WATERMARK               "WATERMARK"

#define RIGHT_DISP_VIEW                         "View"
#define RIGHT_DISP_EDIT                         "Edit"
#define RIGHT_DISP_PRINT                        "Print"
#define RIGHT_DISP_CLIPBOARD                    "Access Clipboard"
#define RIGHT_DISP_SAVEAS                       "Save As"
#define RIGHT_DISP_DECRYPT                      "Decrypt"
#define RIGHT_DISP_SCREENCAP                    "Capture Screen"
#define RIGHT_DISP_SEND                         "Send"
#define RIGHT_DISP_CLASSIFY                     "Classify"
#define RIGHT_DISP_SHARE                        "Share"
#define RIGHT_DISP_DOWNLOAD                     "Download"


#define ACTIVITY_OPERATION_ID_PROTECT           1
#define ACTIVITY_OPERATION_ID_SHARE             2
#define ACTIVITY_OPERATION_ID_REMOVEUSER        3
#define ACTIVITY_OPERATION_ID_VIEW              4
#define ACTIVITY_OPERATION_ID_PRINT             5
#define ACTIVITY_OPERATION_ID_DOWNLOAD          6
#define ACTIVITY_OPERATION_ID_EDIT              7
#define ACTIVITY_OPERATION_ID_REVOKE            8
#define ACTIVITY_OPERATION_ID_DECRYPT           9
#define ACTIVITY_OPERATION_ID_COPYCONTENT       10
#define ACTIVITY_OPERATION_ID_SCREENCAP         11
#define ACTIVITY_OPERATION_ID_CLASSIFY          12
#define ACTIVITY_OPERATION_ID_RESHARE           13
#define ACTIVITY_OPERATION_ID_DELETE            14

#define ACTIVITY_DISP_PROTECT                   "Protect"
#define ACTIVITY_DISP_SHARE                     "Share"
#define ACTIVITY_DISP_REMOVEUSER                "Remove User"
#define ACTIVITY_DISP_VIEW                      "View"
#define ACTIVITY_DISP_PRINT                     "Print"
#define ACTIVITY_DISP_DOWNLOAD                  "Download"
#define ACTIVITY_DISP_EDIT                      "Edit/Save"
#define ACTIVITY_DISP_REVOKE                    "Revoke"
#define ACTIVITY_DISP_DECRYPT                   "Decrypt"
#define ACTIVITY_DISP_COPYCONTENT               "Copy Content"
#define ACTIVITY_DISP_SCREENCAP                 "Capture Screen"
#define ACTIVITY_DISP_CLASSIFY                  "Classify"

#ifdef _MSC_VER
__forceinline
#else
inline __attribute__((always_inline))
#endif
uint64_t ActionToRights(const char* action)
{
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#endif
    if(0 == strcmp(RIGHT_ACTION_VIEW, action)) {
        return BUILTIN_RIGHT_VIEW;
    }
    else if(0 == strcmp(RIGHT_ACTION_EDIT, action)) {
        return BUILTIN_RIGHT_EDIT;
    }
    else if(0 == strcmp(RIGHT_ACTION_PRINT, action)) {
        return BUILTIN_RIGHT_PRINT;
    }
    else if(0 == strcmp(RIGHT_ACTION_CLIPBOARD, action)) {
        return BUILTIN_RIGHT_CLIPBOARD;
    }
    else if(0 == strcmp(RIGHT_ACTION_SAVEAS, action)) {
        return BUILTIN_RIGHT_SAVEAS;
    }
    else if(0 == strcmp(RIGHT_ACTION_DECRYPT, action)) {
        return BUILTIN_RIGHT_DECRYPT;
    }
    else if(0 == strcmp(RIGHT_ACTION_SCREENCAP, action)) {
        return BUILTIN_RIGHT_SCREENCAP;
    }
    else if(0 == strcmp(RIGHT_ACTION_SEND, action)) {
        return BUILTIN_RIGHT_SEND;
    }
    else if(0 == strcmp(RIGHT_ACTION_CLASSIFY, action)) {
        return BUILTIN_RIGHT_CLASSIFY;
    }
	else if (0 == strcmp(RIGHT_ACTION_SHARE, action)) {
		return BUILTIN_RIGHT_SHARE;
	}
	else if (0 == strcmp(RIGHT_ACTION_DOWNLOAD, action)) {
		return BUILTIN_RIGHT_DOWNLOAD;
	}
    else {
        return 0ULL;
    }
#ifdef _MSC_VER
#pragma warning(pop)
#endif
}

#ifdef _MSC_VER
__forceinline
#else
inline __attribute__((always_inline))
#endif
const char* RightToDisplayName(const uint64_t right)
{
    if(BUILTIN_RIGHT_VIEW == right) {
        return RIGHT_DISP_VIEW;
    }
    else if(BUILTIN_RIGHT_EDIT == right) {
        return RIGHT_DISP_EDIT;
    }
    else if(BUILTIN_RIGHT_PRINT == right) {
        return RIGHT_DISP_PRINT;
    }
    else if(BUILTIN_RIGHT_CLIPBOARD == right) {
        return RIGHT_DISP_CLIPBOARD;
    }
    else if(BUILTIN_RIGHT_SAVEAS == right) {
        return RIGHT_DISP_SAVEAS;
    }
    else if(BUILTIN_RIGHT_DECRYPT == right) {
        return RIGHT_DISP_DECRYPT;
    }
    else if(BUILTIN_RIGHT_SCREENCAP == right) {
        return RIGHT_DISP_SCREENCAP;
    }
    else if(BUILTIN_RIGHT_SEND == right) {
        return RIGHT_DISP_SEND;
    }
    else if(BUILTIN_RIGHT_CLASSIFY == right) {
        return RIGHT_DISP_CLASSIFY;
    }
	else if (BUILTIN_RIGHT_SHARE == right) {
		return RIGHT_DISP_SHARE;
	}
    else {
        return "";
    }
}

#ifdef _MSC_VER
__forceinline
#else
inline __attribute__((always_inline))
#endif
const char* ActivityToDisplayName(const uint64_t op)
{
    if(ACTIVITY_OPERATION_ID_PROTECT == op) {
        return ACTIVITY_DISP_PROTECT;
    }
    else if(ACTIVITY_OPERATION_ID_SHARE == op) {
        return ACTIVITY_DISP_SHARE;
    }
    else if(ACTIVITY_OPERATION_ID_REMOVEUSER == op) {
        return ACTIVITY_DISP_REMOVEUSER;
    }
    else if(ACTIVITY_OPERATION_ID_VIEW == op) {
        return ACTIVITY_DISP_VIEW;
    }
    else if(ACTIVITY_OPERATION_ID_PRINT == op) {
        return ACTIVITY_DISP_PRINT;
    }
    else if(ACTIVITY_OPERATION_ID_DOWNLOAD == op) {
        return ACTIVITY_DISP_DOWNLOAD;
    }
    else if(ACTIVITY_OPERATION_ID_EDIT == op) {
        return ACTIVITY_DISP_EDIT;
    }
    else if(ACTIVITY_OPERATION_ID_REVOKE == op) {
        return ACTIVITY_DISP_REVOKE;
    }
    else if(ACTIVITY_OPERATION_ID_DECRYPT == op) {
        return ACTIVITY_DISP_DECRYPT;
    }
	else if (ACTIVITY_OPERATION_ID_COPYCONTENT == op) {
		return ACTIVITY_DISP_COPYCONTENT;
	}
	else if (ACTIVITY_OPERATION_ID_SCREENCAP == op) {
		return ACTIVITY_DISP_SCREENCAP;
	}
	else if (ACTIVITY_OPERATION_ID_CLASSIFY == op) {
		return ACTIVITY_DISP_CLASSIFY;
	}
    else {
        return "Unknown";
    }
}

#ifdef __cplusplus
}
#endif

#endif /* __RMCCORE_FORMAT_NXLRIGHTS_H__ */
