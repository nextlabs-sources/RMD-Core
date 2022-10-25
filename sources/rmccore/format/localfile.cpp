//
//  localfile.cpp
//  rmccore
//
//  Created by Haobo Wang on 1/12/18.
//  Copyright © 2018 NextLabs, Inc. All rights reserved.
//

#include "stdafx.h"
#include <stdio.h>
#include "rmccore/format/localfile.h"
#include "rmccore/format/nxlfile.h"

#include "rmccore/base/coreerror.h"
#include "rmccore/utils/string.h"


using namespace RMCCORE;
using namespace std;

std::string FileBase::GetFileName() const
{
	size_t pos;
#ifdef _WIN32
	if (m_wfilepath.length() > 0) {
		auto pos = m_wfilepath.rfind(L'\\');
		std::wstring strtemp = (pos == std::wstring::npos) ? m_wfilepath : m_wfilepath.substr(pos + 1);
		std::string ret = toUtf8(strtemp);
		return ret;
	}
	// should not happen in Windows
	pos = m_filepath.rfind('\\');
	return (pos == std::string::npos) ? m_filepath : m_filepath.substr(pos + 1);
#else 	
	pos = m_filepath.rfind('/');
	return (pos == std::string::npos) ? m_filepath : m_filepath.substr(pos + 1);
#endif
	
}

std::string FileBase::GetFileExtension() const
{
	const std::string& name = GetFileName();
	auto pos = name.rfind('.');
	return (pos == std::string::npos) ? std::string() : name.substr(pos + 1);
}

std::string FileBase::GetFilePath() const
{
#ifdef _WIN32
	if (m_wfilepath.length() > 0) {
		return toUtf8(m_wfilepath);
	}
#endif
	return m_filepath;
}
RMLocalFile::RMLocalFile(const std::string &path) : FileBase(path)
{
    m_filesize = INVALID_FILE_SIZE64;
	m_lasterror = RETSUCCEED();
    m_fp.open(path, ios::binary | ios::in | ios::ate);
    if(!m_fp.good()){
        m_lasterror = RETMESSAGE(RMCCORE_ERROR_FILE_NOT_FOUND, "Fail to open file.");
    }
    m_filesize =  m_fp.tellg();
    m_fp.close();
}

#ifdef _WIN32
RMLocalFile::RMLocalFile(const std::wstring &path) : FileBase(path)
{
    m_filesize = INVALID_FILE_SIZE64;
	m_lasterror = RETSUCCEED();
	
	m_fp.open(path, ios::binary | ios::in | ios::ate);
    if(!m_fp.good()){
        m_lasterror = RETMESSAGE(RMCCORE_ERROR_FILE_NOT_FOUND, "Fail to open file.");
        return;
    }
    m_filesize =  m_fp.tellg();
    m_fp.close();
}
#endif

RMLocalFile::~RMLocalFile()
{
    Close();
}

void RMLocalFile::Close()
{
    m_fp.close();
}
bool RMLocalFile::Open()
{
    if(m_opened) return true;
    if(m_filesize == INVALID_FILE_SIZE64 && m_lasterror){
        return false;
    }
    NXLFMT::File file;
    if(m_filepath.length() > 0)
        file.open(m_filepath, true);
    else{
#ifdef _WIN32
        file.open(m_wfilepath, true);
#else
        m_lasterror = RETMESSAGE(RMCCORE_ERROR_FILE_NOT_FOUND, "Path is not valid");
        return false;
#endif
    }
    if(!file.good()){
        if(file.getError() == NXLFMT::FileError::FeIoError){
            m_lasterror = RETMESSAGE(RMCCORE_ERROR_FILE_NOT_FOUND, "Fail to open file error.");
            return false;
        }
        else
            m_nxl = false;
    }else{
        m_nxl = true;
    }
    file.close();
    if(m_filepath.length() > 0)
        m_fp.open(m_filepath, ios::binary | ios::in | ios::ate);
    else{
#ifdef _WIN32
        m_fp.open(m_wfilepath, ios::binary | ios::in | ios::ate);
#else
        m_lasterror = RETMESSAGE(RMCCORE_ERROR_FILE_NOT_FOUND, "Path is not valid");
        return false;
#endif
    }
    m_opened = file.good();
    
    return m_opened;
}

uint32_t RMLocalFile::read(uint64_t offset, uint8_t *buf, uint32_t bytesToRead)
{
    if(!m_opened){
        m_lasterror = RETMESSAGE(RMCCORE_ERROR_NOT_READY, "File is not opened.");
        return 0;
    }
    m_fp.seekg(offset,  std::ios_base::beg);
    if (!m_fp.good()) {
        m_lasterror = RETMESSAGE(RMCCORE_ERROR_END_OF_FILE, "End of file is reached.");
        return 0;
    }
    m_fp.read((char*)buf, bytesToRead);
    if(m_filesize < bytesToRead + offset)
        return (uint32_t) (m_filesize - offset);
    
    return bytesToRead;
}


