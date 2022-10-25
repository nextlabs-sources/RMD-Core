//
//  localfile.h
//  rmccore
//
//  Created by Haobo Wang on 1/12/18.
//  Copyright © 2018 NextLabs, Inc. All rights reserved.
//

#pragma once
#ifndef __RMCCORE_FORMAT_LOCALFILE_H__
#define __RMCCORE_FORMAT_LOCALFILE_H__

#include <string>
#include <fstream>
#include "rmccore/base/coreretval.h"


#ifndef INVALID_FILE_SIZE
#define INVALID_FILE_SIZE ((DWORD)0xFFFFFFFF)
#endif

#define INVALID_FILE_SIZE64 ((ULONGLONG)(-1))

namespace RMCCORE {
    class FileBase
    {
    public:
        FileBase(const std::string& path) : m_filepath(path), m_wfilepath(L""), m_opened(false), m_nxl(false), m_lasterror(RETSUCCEED()){};
		virtual ~FileBase() {};
#ifdef _WIN32
        FileBase(const std::wstring &path): m_wfilepath(path), m_filepath(""), m_opened(false), m_nxl(false), m_lasterror(RETSUCCEED()){};
#endif
        virtual bool IsOpened(){return m_opened;}
        virtual bool Open() = 0;
        virtual void Close() = 0;
        bool IsNXL() {return m_nxl;}

        RetValue GetLastError() {return m_lasterror;}
	public:
		std::string GetFilePath(void) const;
		std::string GetFileName(void) const;
		std::string GetFileExtension(void) const;
    public:
        virtual uint32_t read(uint64_t offset, uint8_t* buf, uint32_t bytesToRead) = 0;
    public:
        std::string     m_filepath;
        std::wstring    m_wfilepath;
    protected:
        bool            m_opened;
        bool            m_nxl;
        RetValue        m_lasterror;
    };
    
    class RMLocalFile : public FileBase
    {
    public:
        RMLocalFile(const std::string& path);
#ifdef _WIN32
        RMLocalFile(const std::wstring &path);
#endif
        ~RMLocalFile();
    
    public:
        bool Open();
        void Close();
        uint64_t size(){return m_filesize;}
        
    public:
        uint32_t read(uint64_t offset, uint8_t* buf, uint32_t bytesToRead);
    protected:
        std::fstream    m_fp;
        uint64_t        m_filesize;
    };
}

#endif /* __RMCCORE_FORMAT_LOCALFILE_H__ */
