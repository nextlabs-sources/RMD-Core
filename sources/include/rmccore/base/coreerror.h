//
//  coreerror.h
//  rmccore
//
//  Created by NextLabs Inc on 11/21/17.
//  Copyright © 2017 NextLabs, Inc. All rights reserved.
//

#pragma once
#ifndef __RMCCORE_BASE_COREERROR_H__
#define __RMCCORE_BASE_COREERROR_H__

#define RMCCORE_ERROR_SUCCESS               0l
#define RMCCORE_ERROR_FILE_NOT_FOUND        2l      //ERROR_FILE_NOT_FOUND
#define RMCCORE_ERROR_ACCESS_DENIED         5l      //ERROR_ACCESS_DENIED
#define RMCCORE_ERROR_NOT_ENOUGH_MEMORY     8l      //ERROR_NOT_ENOUGH_MEMORY
#define RMCCORE_INVALID_JSON_FORMAT         11l     //ERROR_BAD_FORMAT
#define RMCCORE_INVALID_NXL_FORMAT          11l     //ERROR_BAD_FORMAT
#define RMCCORE_ERROR_INVALID_DATA          13l     //ERROR_INVALID_DATA
#define RMCCORE_ERROR_NOT_READY             21l     //ERROR_NOT_READY
#define RMCCORE_ERROR_END_OF_FILE           38l     //ERROR_HANDLE_EOF
#define RMCCORE_ERROR_NOT_SUPPORTED         50l     //ERROR_NOT_SUPPORTED
#define RMCCORE_ERROR_CREATE_NXL            82l     //ERROR_CANNOT_MAKE
#define RMCCORE_ERROR_ALREADY_OPENED        183l    //ERROR_ALREADY_EXISTS
#define RMCCORE_ERROR_NOT_FOUND             1168l   //ERROR_NOT_FOUND

/************************************************************************/
/* RMS Error Code base starts from 0xF000                               */
/* for API return code from RMS, use the error code minus Error code    */
/*   base to get the result.                                            */
/*   For example, error code 61940 means 500 (61940-0xF000)             */
/************************************************************************/
#define RMCCORE_RMS_ERRORCODE_BASE          0xF000

#endif /* __RMCCORE_BASE_COREERROR_H__ */
