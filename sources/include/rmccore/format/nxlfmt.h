//
//  nxlfmt.h
//  rmccore
//
//  Created by Gavin Ye on 10/19/17.
//  Copyright © 2017 NextLabs, Inc. All rights reserved.
//

#pragma once
#ifndef __RMCCORE_FORMAT_NXLFMT_H__
#define __RMCCORE_FORMAT_NXLFMT_H__

//
//  Define Versions
//
#define NXLFMT_VERSION_1            0x0001
#define NXLFMT_VERSION_2            0x0002

#define NXL_MAGIC_NAME_1            "NXLFMT!"
#define NXL_MAGIC_CODE_1            0x0021544D464C584EULL
        
#define NXL_MAGIC_NAME_2            "NXLFMT@"
#define NXL_MAGIC_CODE_2            0x0040544D464C584EULL

// Key Header Flags
#define KF_RECOVERY_KEY_ENABLED     0x000001

// Secure Mode
#define KF_CLIENT_SECURE_MODE       0x01
#define KF_SERVER_SECURE_MODE       0x02
#define KF_SPLIT_SECURE_MODE        0x03
        
//
//  Struct Alignment: 4
//
#pragma pack(push)
#pragma pack(4)
        
// Signature Header
typedef union _NXL_MAGIC {
    long long code;
    unsigned char name[8];
} NXL_MAGIC;

typedef struct _NXL_SIGNATURE1 {
    NXL_MAGIC magic;
    wchar_t message[68];
} NXL_SIGNATURE1;

typedef struct _NXL_SIGNATURE_LITE {
    NXL_MAGIC magic;
    unsigned int version;
} NXL_SIGNATURE_LITE;

typedef struct _NXL_SIGNATURE2 {
    NXL_MAGIC magic;
    unsigned int version;
    char message[244];
} NXL_SIGNATURE2;

// File Info Header

typedef struct _NXL_FILEINFO1 {
    unsigned char thumbPrint[16];
    unsigned int version;
    unsigned int flags;
    unsigned int alignment;
    unsigned int contentOffset;
} NXL_FILEINFO1;

typedef struct _NXL_FILEINFO2 {
    unsigned char duid[16];
    unsigned int flags;
    unsigned int alignment;
    unsigned int algorithm;
    unsigned int blockSize;
    unsigned int contentOffset;
    char ownerId[256];
    unsigned int extendedDataOffset;
} NXL_FILEINFO2;


// Crypto/Key Header

typedef struct _NXL_KEK_BLOB {
    unsigned short keyAlgorithm;
    unsigned short keyIdSize;
    unsigned char keyId[60];
    unsigned char cek[256];
} NXL_KEK_BLOB;

typedef struct _NXL_CRYPTO {
    unsigned int algorithm;
    unsigned int cbcSize;
    NXL_KEK_BLOB primaryKey;
    NXL_KEK_BLOB recoveryKey;
    long long contentSize;
    long long allocationSize;
    unsigned char reserved[32];
} NXL_CRYPTO;

typedef struct _NXL_KEYS {
    unsigned int modeFlags;
    unsigned char ivSeed[16];
    unsigned char tokenCek[32];
    unsigned char tokenCekChecksum[32];
    unsigned char recoveryCek[32];
    unsigned char recoveryCekChecksum[32];
    unsigned char publicKey1[256];
    unsigned char publicKey2[256];
    unsigned int tokenLevel;
    unsigned int extendedDataOffset;
} NXL_KEYS;


// Section Header

typedef struct _NXL_SECTION1 {
    char name[8];
    unsigned int size;
    unsigned int checksum;
} NXL_SECTION1;

typedef struct _NXL_SECTION2 {
    char name[16];
    unsigned int flags;
    unsigned int offset;
    unsigned int size;
    unsigned short dataSize;
    unsigned short compressedSize;
    unsigned char checksum[32];   // HMAC_SHA256, encrypted by token
} NXL_SECTION2;

typedef struct _NXL_SECTION_HEADER1 {
    unsigned char checksum[16];
    unsigned int count;
    unsigned int reserved;
    NXL_SECTION1 record[75];
} NXL_SECTION_HEADER1;

typedef struct _NXL_SECTION_HEADER2 {
    unsigned int section_map;
    NXL_SECTION2 record[32];
} NXL_SECTION_HEADER2;

// Fixed & Dynamic Header

typedef struct _NXL_FIXED_HEADER {
    NXL_SIGNATURE2 signature;
    NXL_FILEINFO2 fileInfo;
    NXL_KEYS keys;
    NXL_SECTION_HEADER2 sections;
    unsigned char extendData[784];
} NXL_FIXED_HEADER;

typedef struct _NXL_DYNAMIC_HEADER {
    unsigned char fixedHeaderHash[32];
    long long contentLength;
} NXL_DYNAMIC_HEADER;

// Header
typedef struct _NXL_HEADER1 {
    NXL_SIGNATURE1 signature;
    NXL_FILEINFO1 fileInfo;
    NXL_CRYPTO crypto;
    NXL_SECTION_HEADER1 sections;
} NXL_HEADER1;

typedef struct _NXL_HEADER2 {
    NXL_FIXED_HEADER fixed;
    NXL_DYNAMIC_HEADER dynamic;
} NXL_HEADER2;

typedef struct _NXL_TOKEN {
    unsigned char Token[32];
    unsigned int TokenSecureMode;
    unsigned int TokenLevel;
} NXL_TOKEN;

typedef struct _NXL_CEK {
    unsigned char cek[32];
} NXL_CEK;

typedef struct _NXL_DUID {
    unsigned char duid[16];
} NXL_DUID;
        
#pragma pack(pop)   // End of Struct Alignment: 4
   
//
//  Define default Version & Revision (always up-to-date)
//
#ifndef NXL_VERSION
#define NXL_VERSION     2
#endif
        
#ifndef NXL_REVISION
#define NXL_REVISION    0
#endif

//
//  Define magic code
//
#if (NXL_VERSION == 1)
        
#define NXL_MAGIC_NAME  NXL_MAGIC_NAME1
#define NXL_MAGIC_CODE  NXL_MAGIC_CODE1
#define NXL_HEADER      NXL_HEADER1
#define PNXL_HEADER     NXL_HEADER1*
#define PCNXL_HEADER    const NXL_HEADER1*
        
#else   // #if (NXL_VERSION == 1)
        
#define NXL_MAGIC_NAME  NXL_MAGIC_NAME2
#define NXL_MAGIC_CODE  NXL_MAGIC_CODE2
#define NXL_HEADER      NXL_HEADER2
#define PNXL_HEADER     NXL_HEADER2*
#define PCNXL_HEADER    const NXL_HEADER2*
        
#endif   // #if (NXL_VERSION == 1)
        
#define NXL_PAGE_SIZE           0x1000      /**< NXL Format Page Size */
#define NXL_BLOCK_SIZE          0x200       /**< NXL Format CBC Block Size */
#define NXL_MIN_SIZE            0x4000      /**< NXL Format Minimum File Size */
#define NXL_ALIGNMENT_SIZE      0x1000       /**< NXL Format CBC Alignment Size */

#define NXL_MAX_SECTION_DATA_LENGTH 0xFFFF  // The original data length must not exceed 64K bytes
        
#define NXL_VERSION_10          0x00010000  /**< NXL Format Version 1.0 */
#define NXL_VERSION_30          0x00030000  /**< NXL Format Version 3.0 */
        
enum _NXLALGORITHM {
    NXL_ALGORITHM_NONE = 0,     /**< No algorithm (No encrypted) */
    NXL_ALGORITHM_AES128 = 1,   /**< AES 128 bits */
    NXL_ALGORITHM_AES256 = 2,   /**< AES 256 bits (Default content encryption algorithm) */
    NXL_ALGORITHM_RSA1024 = 3,  /**< RSA 1024 bits */
    NXL_ALGORITHM_RSA2048 = 4,  /**< RSA 2048 bits */
    NXL_ALGORITHM_SHA1 = 5,     /**< SHA1 */
    NXL_ALGORITHM_SHA256 = 6,   /**< SHA256 (Default hash algorithm) */
    NXL_ALGORITHM_MD5 = 7       /**< MD5 */
};
        
#define NXL_DEFAULT_MSG                 "Protected by SkyDRM.com"   /**< NXL Format default message */
        
#define NXL_SECTION_FLAG_ENCRYPTED      (0x00000001)
#define NXL_SECTION_FLAG_COMPRESSED     (0x00000002)
        
#define NXL_SECTION_NAME_FILEINFO       ".FileInfo"
#define NXL_SECTION_NAME_FILEPOLICY     ".FilePolicy"
#define NXL_SECTION_NAME_FILETAG        ".FileTag"

#endif /* __RMCCORE_FORMAT_NXLFMT_H__ */
