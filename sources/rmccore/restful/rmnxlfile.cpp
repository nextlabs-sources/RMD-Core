//
//  rmnxlfile.cpp
//  rmccore
//
//  Created by Haobo Wang on 1/17/18.
//  Copyright © 2018 NextLabs, Inc. All rights reserved.
//

#include "stdafx.h"
#include <stdio.h>
#include <assert.h>
#include "rmccore/utils/time.h"
#include "rmccore/format/localfile.h"
#include "rmccore/format/nxlfile.h"
#include "rmccore/format/nxlpolicy.h"
#include "rmccore/restful/rmnxlfile.h"
#include "rmccore/network/httpClient.h"
#include "rmccore/policy/obligation.h"
#include "rmrestkey.h"


using namespace RMCCORE;
using namespace OBLIGATION;
using namespace CONDITION;
using namespace NXLFMT;
using namespace std;


#define FILENAME_ELEMENT_NAME       "FileName"
#define WFILENAME_ELEMENT_NAME      "wFileName"
#define SOURCEPATH_ELEMENT_NAME		"SourcePath"
#define FILESIZE_ELEMENT_NAME       "FileSize"
#define NXLUPLOAD_ELEMENT_NAME      "IsUploaded"
#define NXLFILEINFO_KEY_NAME        "NXLFileInfo"
#define NXLRECIPIENTS_KEY_NAME		"Recipients"
#define NXLFILE_ELEMENT_NAME        "Name"
#define NXLTOKEN_KEY_NAME			"Token"
#define NXLAGREEMENT_ELEMENT_NAME	"Agreement"
#define NXLAGREEMENT1_ELEMENT_NAME	"Agreement1"
#define NXLOWNERID_ELEMENT_NAME		"OwnerID"
#define NXLLASTMODIFY_ELEMENT_NAME  "LastModify"
#define NXLFILESIZE_ELEMENT_NAME    "Size"
#define NXLPATHID_ELEMENT_NAME      "PathID"
#define NXLDISPLAYPATH_ELEMENT_NAME "DisplayPath"
#define NXLMETADATA_KEY_NAME		"MetaData"
#define NXLMDUPDATETIME_ELEMENT_NAME	"UpdateTime"
#define NXLFILELINE_ELEMENT_NAME	"FileLink"
#define NXLSHAREDFLAG_ELEMENT_NAME	"SharedFlag"
#define NXLDELETEFLAG_ELEMENT_NAME	"DeleteFlag"
#define NXLREVOKEDFLAG_ELEMENT_NAME	"RevokedFlag"
#define NXLVALIDITY_KEY_NAME		"Validity"
#define NXLVALIDITYTYPE_ELEMENT_NAME	"Option"
#define NXLVALIDITYBEGIN_ELEMENT_NAME	"DateBegin"
#define NXLVALIDITYEND_ELEMENT_NAME    "DateEnd"

RMNXLFile::RMNXLFile(const std::string & path, const std::string& source_filepath) : FileBase(path), m_uploaded(false), m_filesize(INVALID_FILE_SIZE), m_nxlname(""), m_nxlduid(""), 
		m_meta_nxllastmodified(0), m_nxlpathid(""),  m_hfile(NULL), m_sourcepath(source_filepath),
		m_meta_lastMDUpdateTime(0), m_meta_nxlfilelink(""), m_meta_nxlshared(false), m_meta_nxldeleted(true), m_meta_nxlrevoked(true), m_expirtion(Expiry(NEVEREXPIRE)), m_meta_expirtion(Expiry(NEVEREXPIRE)),
		m_meta_pathDisplay(""), m_meta_pathId(""), m_meta_bnxlowner(true), m_meta_bnxlfile(true), m_meta_nxlname(""), m_createdby(""), m_modifiedby(""), m_datecreated(0), m_datemodified(0), m_orgfilename(""), m_orgfileext(""),
		m_meta_nxlsharedon(0), m_meta_nxlprotectedon(0), m_meta_protectionType(0)
{
	m_tags = "";
	if (m_sourcepath.length() == 0)//no source path, can't upload
		m_uploaded = true;

    fstream fp;
    fp.open(path, ios::binary|ios::in|ios::ate);
    if(!fp.good()){
        m_lasterror = RETMESSAGE(RMCCORE_ERROR_FILE_NOT_FOUND, "Fail to open file.");
        return;
    }
    m_filesize =  fp.tellg();
    fp.close();
    m_hfile = (HANDLE)new File();
    if(NULL == m_hfile){
        m_lasterror = RETMESSAGE(RMCCORE_ERROR_NOT_ENOUGH_MEMORY, "Fail to allocate memory.");
        return;
    }
    File * pfile = (File *)m_hfile;
    m_nxl = pfile->validate(path);
    if(!m_nxl){
        m_lasterror = RETMESSAGE(RMCCORE_INVALID_NXL_FORMAT, "Not an valid NXL file.");
        return;
    }

    pfile->open(path, true);
    m_opened = pfile->good();
    if(!m_opened){
        m_lasterror = RETMESSAGE(RMCCORE_ERROR_FILE_NOT_FOUND, "Fail to open file.");
        return;
    }
	m_nxlduid = bintohs<char>(pfile->getTokenId().getId());
	m_nxlagreement = bintohs<char>(pfile->getAgreement0());
	m_nxlagreement1 = bintohs<char>(pfile->getAgreement1());
	m_nxlownerid = pfile->getOwnerId();
	m_nxlml = pfile->getTokenId().getLevel();
	m_tenant = pfile->getTenant();
	SetupPolicy();
	pfile->readTagSection(m_tags);
	GetNXLExpiration();
	SetupFileInfo();
}

RMNXLFile::RMNXLFile(const RMNXLFile & rhs) : FileBase(""), m_uploaded(false), m_filesize(INVALID_FILE_SIZE), m_nxlname(""), m_nxlduid(""),
m_meta_nxllastmodified(0), m_nxlpathid(""), m_hfile(NULL), m_sourcepath(""), m_meta_lastMDUpdateTime(0), m_meta_nxlfilelink(""), m_meta_nxlshared(false),
m_meta_nxldeleted(true), m_meta_nxlrevoked(true), m_expirtion(Expiry(NEVEREXPIRE)), m_meta_expirtion(Expiry(NEVEREXPIRE)),
m_meta_pathDisplay(""), m_meta_pathId(""), m_meta_bnxlowner(true), m_meta_bnxlfile(true), m_meta_nxlname(""), m_createdby(""), m_modifiedby(""), m_datecreated(0), m_datemodified(0), m_orgfilename(""), m_orgfileext(""), 
m_meta_nxlsharedon(0), m_meta_nxlprotectedon(0), m_meta_protectionType(0)
{
	*this = rhs;
}

#ifdef _WIN32
RMNXLFile::RMNXLFile(const std::wstring & path, const std::wstring & source_filepath) : FileBase(path), m_uploaded(false), m_filesize(INVALID_FILE_SIZE), m_nxlname(""), m_nxlduid(""),
		m_meta_nxllastmodified(0), m_nxlpathid(""), m_hfile(NULL), m_sourcepath(toUtf8(source_filepath)),
		m_meta_lastMDUpdateTime(0), m_meta_nxlfilelink(""), m_meta_nxlshared(false), m_meta_nxldeleted(true), m_meta_nxlrevoked(true), m_expirtion(Expiry(NEVEREXPIRE)), m_meta_expirtion(Expiry(NEVEREXPIRE)),
		m_meta_pathDisplay(""), m_meta_pathId(""), m_meta_bnxlowner(true), m_meta_bnxlfile(true), m_meta_nxlname(""), m_createdby(""), m_modifiedby(""), m_datecreated(0), m_datemodified(0), m_orgfilename(""), m_orgfileext(""), 
		m_meta_nxlsharedon(0), m_meta_nxlprotectedon(0), m_meta_protectionType(0)
{
	if (m_sourcepath.length() == 0)//no source path, can't upload
		m_uploaded = true;

	fstream fp;
    fp.open(path, ios::binary | ios::in | ios::ate);
    if(!fp.good()){
        m_lasterror = RETMESSAGE(RMCCORE_ERROR_FILE_NOT_FOUND, "Fail to open file.");
        return;
    }
    m_filesize =  fp.tellg();
    fp.close();
    m_hfile = (HANDLE)new File();
    if(NULL == m_hfile){
        m_lasterror = RETMESSAGE(RMCCORE_ERROR_NOT_ENOUGH_MEMORY, "Fail to allocate memory.");
        return;
    }
    File * pfile = (File *)m_hfile;
    m_nxl = pfile->validate(path);
    if(!m_nxl){
        m_lasterror = RETMESSAGE(RMCCORE_INVALID_NXL_FORMAT, "Not an valid NXL file.");
        return;
    }
    
    pfile->open(path, true);
    m_opened = pfile->good();
    if(!m_opened){
        m_lasterror = RETMESSAGE(RMCCORE_ERROR_FILE_NOT_FOUND, "Fail to open file.");
        return;
    }
	m_nxlduid = bintohs<char>(pfile->getTokenId().getId());
	m_nxlagreement = bintohs<char>(pfile->getAgreement0());
	m_nxlagreement1 = bintohs<char>(pfile->getAgreement1());
	m_nxlownerid = pfile->getOwnerId();
	m_nxlml = pfile->getTokenId().getLevel();
	m_tenant = pfile->getTenant();
	SetupPolicy();
	pfile->readTagSection(m_tags);
	GetNXLExpiration();
	SetupFileInfo();
}
#endif

RMNXLFile::~RMNXLFile()
{
    if(m_hfile){
        delete (File *)m_hfile;
		m_hfile = NULL;
    }
}

RMNXLFile & RMNXLFile::operator= (const RMNXLFile &rhs)
{
	if (this != &rhs)
	{
		m_filepath = rhs.m_filepath;
		m_wfilepath = rhs.m_wfilepath;
		m_sourcepath = rhs.m_sourcepath;
		m_nxl = false;

		m_opened = false;
		m_lasterror = RETSUCCEED();
		m_token = rhs.m_token;

		if (m_hfile) {
			delete (FILE *)m_hfile;
			m_hfile = NULL;
		}
		if (NULL == m_hfile) {
			m_hfile = (HANDLE)new File();
			File * pfile = (File *)m_hfile;
#ifdef _WIN32
            if(m_wfilepath.length() > 0){
                m_nxl = pfile->validate(m_wfilepath);
                pfile->open(m_wfilepath, true);
            }
#endif
            if (m_filepath.length() > 0) {
				m_nxl = pfile->validate(m_filepath);
				pfile->open(m_filepath, true);
			}

			m_opened = pfile->opened();
			if(m_nxl)
				pfile->setTokenKey(hstobin(m_token.GetKey()));
		}

		m_uploaded = rhs.m_uploaded;
		m_filesize = rhs.m_filesize;
		m_nxlduid = rhs.m_nxlduid;
		m_nxlownerid = rhs.m_nxlownerid;
		m_nxlagreement = rhs.m_nxlagreement;
		m_nxlagreement1 = rhs.m_nxlagreement1;
		m_nxlml = rhs.m_nxlml;
		m_tenant = rhs.m_tenant;

		m_recipients = rhs.m_recipients;
		m_nxlname = rhs.m_nxlname;
		m_meta_nxlname = rhs.m_meta_nxlname;
		m_meta_nxllastmodified = rhs.m_meta_nxllastmodified;
		m_nxlpathid = rhs.m_nxlpathid;

		m_meta_lastMDUpdateTime = rhs.m_meta_lastMDUpdateTime;
		m_meta_nxlfilelink = rhs.m_meta_nxlfilelink;
		m_meta_nxlshared = rhs.m_meta_nxlshared;
		m_meta_nxldeleted = rhs.m_meta_nxldeleted;
		m_meta_nxlrevoked = rhs.m_meta_nxlrevoked;
		m_meta_bnxlfile = rhs.m_meta_bnxlfile;
		m_meta_bnxlowner = rhs.m_meta_bnxlowner;
		m_meta_pathDisplay = rhs.m_meta_pathDisplay;
		m_meta_pathId = rhs.m_meta_pathId;
		m_meta_recipients = rhs.m_meta_recipients;
		m_meta_expirtion = rhs.m_meta_expirtion;
		m_meta_nxlsharedon = rhs.m_meta_nxlsharedon;
		m_meta_nxlprotectedon = rhs.m_meta_nxlprotectedon;
		m_meta_protectionType = rhs.m_meta_protectionType;

		m_expirtion = rhs.m_expirtion;

		m_createdby = rhs.m_createdby;
		m_modifiedby = rhs.m_modifiedby;
		m_datecreated = rhs.m_datecreated;
		m_datemodified = rhs.m_datemodified;
		m_orgfilename = rhs.m_orgfilename;
		m_orgfileext = rhs.m_orgfileext;
	}
	return *this;
}

bool RMNXLFile::Open()
{
	if (NULL == m_hfile) {
		m_hfile = (HANDLE)new File();
		if (NULL == m_hfile) {
			m_lasterror = RETMESSAGE(RMCCORE_ERROR_NOT_ENOUGH_MEMORY, "Fail to allocate memory.");
			return false;
		}
	}
	if (m_token.GetKey().size() == 0)
	{
		m_lasterror = RETMESSAGE(RMCCORE_ERROR_NOT_READY, "No token key.");
		return false;
	}
    if(m_hfile){
        File * pfile = (File *)m_hfile;
		if (pfile->opened())
		{
			pfile->setTokenKey(hstobin(m_token.GetKey()));
			return true;
		}
#ifdef _WIN32
        if(m_wfilepath.length() > 0){
            pfile->open(m_wfilepath, true);
			m_opened = pfile->opened();
			pfile->setTokenKey(hstobin(m_token.GetKey()));
			return m_opened;
        }
#endif
        if(m_filepath.length() > 0){
            pfile->open(m_filepath, true);
			m_opened = pfile->opened();
			pfile->setTokenKey(hstobin(m_token.GetKey()));
			return m_opened;
        }
		m_lasterror = RETMESSAGE(RMCCORE_ERROR_FILE_NOT_FOUND, "Can't open file.");
    }else{
        m_lasterror = RETMESSAGE(RMCCORE_ERROR_NOT_READY, "Fail to initialize NXLFile.");
    }
    return false;
}

std::string RMNXLFile::GetSourceFileName() const
{
    size_t pos;

#ifdef _WIN32
	pos = m_sourcepath.rfind('\\');    
#else  //#ifdef __APPLE__
	pos = m_sourcepath.rfind('/');
#endif
	return (pos == std::string::npos) ? m_sourcepath : m_sourcepath.substr(pos + 1);
}

std::string RMNXLFile::GetSourceFileExt() const
{
	const std::string& name = GetSourceFileName();
	auto pos = name.rfind('.');
	return (pos == std::string::npos) ? std::string() : name.substr(pos + 1);
}

bool RMNXLFile::Open(const RMToken & token)
{
	if(m_token.IsInitialized()){
		m_lasterror = RETMESSAGE(RMCCORE_ERROR_ALREADY_OPENED, "File is assigned with other token");
		return false;
	}
	m_token = token;
	m_opened = Open();

	if (m_nxlduid.compare(token.GetDUID().c_str()) != 0) {//invalid token
		m_lasterror = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "Token doesn't match with file");
		return false;
	}

	File * pfile = (File *)m_hfile;
	pfile->setTokenKey(hstobin(m_token.GetKey()));

	m_nxlduid = m_token.GetDUID(); //set token DUID based on the passed token 

	return m_opened;
}
void RMNXLFile::Close()
{
    if(m_hfile){
        File * pfile = (File *)m_hfile;
        pfile->close();
		m_opened = false;
		delete pfile;
		m_hfile = NULL;
    }
}

void RMNXLFile::RemoveToken()
{
	RMToken token;
	m_token = token;
}

bool RMNXLFile::SetupPolicy()
{
	if (NULL == m_hfile) {
		return false;
	}
	File * pfile = (File *)m_hfile;
	if (!pfile->good()) 
		return false;
	
	std::string policystr;
	pfile->readPolicySection(m_policy);
	bool ret = true;

	return ret;
}

uint64_t RMNXLFile::GetNXLRights() const
{
	if (NULL == m_hfile) {
		return 0;
	}
	File * pfile = (File *)m_hfile;
	if (!pfile->good()) {
		return 0;
	}
	std::string policystr;
	pfile->readPolicySection(policystr);
	uint64_t ret = 0;
	try {
		File::AdhocPolicy policy(File::AdhocPolicy::deserialize(policystr));

		ret = policy.getRights();
	}
	catch (...) {
		ret = false;
	}

	return ret;
}

uint64_t RMNXLFile::GetIssueTime() const
{
	if (NULL == m_hfile) {
		return 0;
	}
	File * pfile = (File *)m_hfile;
	if (!pfile->good()) {
		return 0;
	}
	std::string policystr;
	pfile->readPolicySection(policystr);
	uint64_t ret = 0;
	try {
		File::AdhocPolicy policy(File::AdhocPolicy::deserialize(policystr));

		ret = policy.getIssueTime();
	}
	catch (...) {
		ret = false;
	}

	return ret;
}

bool RMNXLFile::IsAdhoc()
{
	uint64_t rights = GetNXLRights();
	return ((rights > 0) ? true : false);
}

std::string	RMNXLFile::GetTags()
{
	std::string tags;
	if (NULL == m_hfile) {
		return tags;
	}
	File * pfile = (File *)m_hfile;
	if (!pfile->good()) {
		return tags;
	}
	
	pfile->readTagSection(tags);
	m_tags = tags;

	return tags;

}

bool RMNXLFile::getWatermark(OBLIGATION::Watermark& watermark)
{
	Obligations obs = getObligations();
	for (Obligation ob : obs)
	{
		if (ob.getName().compare(OBLIGATION_NAME_WATERMARK) == 0)
		{
			watermark = ob;
			return true;
		}
	}
	return false;
}

const Obligations RMNXLFile::getObligations()
{
	Obligations obs;
	if (NULL == m_hfile) {
		return obs;
	}
	File * pfile = (File *)m_hfile;
	if (!pfile->good()) {
		return obs;
	}
	std::string policystr;
	pfile->readPolicySection(policystr);
	
	try {
		File::AdhocPolicy policy(File::AdhocPolicy::deserialize(policystr));
		obs = policy.getObligations();
	}
	catch (...) {
		// add log
	}
	return obs;
}

bool RMNXLFile::GetNXLExpiration()
{
	if (NULL == m_hfile) {
		return false;
	}
	File * pfile = (File *)m_hfile;
	if (!pfile->good()) {
		return false;
	}
	std::string policystr;
	pfile->readPolicySection(policystr);
	bool ret = true;
	try {

		File::AdhocPolicy policy(File::AdhocPolicy::deserialize(policystr));

		m_expirtion = policy.getExpireTime();
	}
	catch (...) {
		ret = false;
	}

	return ret;
}

bool RMNXLFile::SetupFileInfo()
{
	bool ret = false;
	std::string fileinfo;
	if (NULL == m_hfile) {
		return ret;
	}
	File * pfile = (File *)m_hfile;
	if (!pfile->good()) {
		return ret;
	}

	if (pfile->readInfoSection(fileinfo) != FileError::FeSuccess)
		return ret;

    try
    {
        nlohmann::json root = nlohmann::json::parse(fileinfo);
        if (!root.is_object())
            return ret;

        m_orgfilename = root.at(NXLFMT_FILEINFO_FILENAME).get<std::string>();
        m_orgfileext = root.at(NXLFMT_FILEINFO_FILEEXTENSION).get<std::string>();
        m_createdby = root.at(NXLFMT_FILEINFO_CREATEDBY).get<std::string>();
        m_datecreated = root.at(NXLFMT_FILEINFO_DATECREATED).get<uint64_t>();
        m_modifiedby = root.at(NXLFMT_FILEINFO_MODIFIEDBY).get<std::string>();
        m_datemodified = root.at(NXLFMT_FILEINFO_DATEMODIFIED).get<uint64_t>();

        m_fileinfo = fileinfo;
        ret = true;
    }
    catch (...) {
        /*res = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "JSON membership value is not correct");*/
    }

	return ret;
}

const std::string RMNXLFile::GetOrgFileName()
{
	if (m_orgfilename.size() > 0)
		return m_orgfilename;

	SetupFileInfo();

	return m_orgfilename;
}

const std::string RMNXLFile::GetOrgFileExtension()
{
	if (m_orgfileext.size() > 0)
		return m_orgfileext;

	SetupFileInfo();

	return m_orgfileext;
}

const std::string	RMNXLFile::GetCreatedBy()
{
	if (m_createdby.size() > 0)
		return m_createdby;

	SetupFileInfo();

	return m_createdby;
}

const std::string	RMNXLFile::GetModifiedBy()
{
	if (m_modifiedby.size() > 0)
		return m_modifiedby;

	SetupFileInfo();

	return m_modifiedby;
}

const uint64_t RMNXLFile::GetDateCreated()
{
	if (m_datecreated > 0)
		return m_datecreated;

	SetupFileInfo();

	return m_datecreated;
}

const uint64_t RMNXLFile::GetDateModified()
{
	if (m_datemodified > 0)
		return m_datemodified;

	SetupFileInfo();

	return m_datemodified;
}

const std::string	RMNXLFile::GetFileInfo()
{
	if (m_fileinfo.size() > 0)
		return m_fileinfo;

	SetupFileInfo();

	return m_fileinfo;
}

uint32_t RMNXLFile::read(uint64_t offset, uint8_t *buf, uint32_t bytesToRead)
{
    if(NULL == m_hfile){
		if (!Open()) {
			m_lasterror = RETMESSAGE(RMCCORE_ERROR_NOT_READY, "Fail to initialize NXLFile.");
			return 0;
		}
    }
    File * pfile = (File *)m_hfile;
    if(!pfile->good()){
        m_lasterror = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "Invalid NXL file.");
        return 0;
    }
    
	uint32_t readbytes = pfile->read(offset, buf, bytesToRead);
	
	return readbytes;
}

bool RMNXLFile::SetRecipientsList(const std::vector<std::string>& recipients)
{
    RMRecipients rp(recipients);
    
	m_recipients = rp;

	return true;
}

RetValue RMNXLFile::ImportFromRMSJson(const nlohmann::json& root)
{
    try {
        int status = root.at(STATUS_ELEMENT_NAME).get<int>();
        std::string message = root.at(MESSAGE_ELEMENT_NAME).get<std::string>();
        if (status != http::status_codes::OK.id) {
            return RETMESSAGE((RMCCORE_RMS_ERRORCODE_BASE + status), message);
        }

        const nlohmann::json& result = root.at(RESULT_KEY_NAME);
        if (result.end() != result.find(MYVAULT_NXL_FILENAME_ELEMENT_NAME))
        {
            m_nxlname = result.at(MYVAULT_NXL_FILENAME_ELEMENT_NAME).get<std::string>();
            m_nxlduid = result.at(MYVAULT_NXL_DUID_ELEMENT_NAME).get<std::string>();
            m_nxlpathid = result.at(MYVAULT_NXL_PATHID_ELEMENT_NAME).get<std::string>();
        }
        else if (result.end() != result.find("entry"))
        {
            const nlohmann::json& entry = result["entry"];
            m_nxlname = entry.at("name").get<std::string>();
            m_nxlpathid = entry.at("pathId").get<std::string>();
        }
        else
        {
            m_nxlname = result.at(SHARE_FILENAME_ELEMENT_NAME).get<std::string>();
            m_nxlduid = result.at(SHARE_DUID_ELEMENT_NAME).get<std::string>();
            m_nxlpathid = result.at(SHARE_PATHID_ELEMENT_NAME).get<std::string>();
        }

        m_recipients.UpdateComments("");//clear comments after upload
        m_uploaded = true;
    }
    catch (std::exception &e) {
        std::string strError = "RMS JSON data is not correct : " + std::string(e.what());
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "RMS JSON data is not correct");
    }

    return RETSUCCEED();
}

RetValue RMNXLFile::ImportFromJson(const nlohmann::json& root)
{
    RetValue res = RETSUCCEED();
    std::string strtmp;
    try {
        m_filepath = root.at(FILENAME_ELEMENT_NAME).get<std::string>();
        m_wfilepath = toUtf16(root.at(WFILENAME_ELEMENT_NAME).get<std::string>());
        m_sourcepath = root.at(SOURCEPATH_ELEMENT_NAME).get<std::string>();
        m_filesize = root.at(FILESIZE_ELEMENT_NAME).get<uint64_t>();
        m_uploaded = root.at(NXLUPLOAD_ELEMENT_NAME).get<bool>();
        m_nxlpathid = root.at(NXLPATHID_ELEMENT_NAME).get<std::string>();

        //NXLFileInfo
        const nlohmann::json& nxlFileInfo = root.at(NXLFILEINFO_KEY_NAME);
        m_nxlname = nxlFileInfo.at(NXLFILE_ELEMENT_NAME).get<std::string>();
        m_nxlagreement = nxlFileInfo.at(NXLAGREEMENT_ELEMENT_NAME).get<std::string>();
        m_nxlagreement1 = nxlFileInfo.at(NXLAGREEMENT1_ELEMENT_NAME).get<std::string>();
        m_nxlownerid = nxlFileInfo.at(NXLOWNERID_ELEMENT_NAME).get<std::string>();

        if (m_token.GetDUID().size() > 0)
        {
            m_nxlduid = m_token.GetDUID();
        }
        m_nxlml = m_token.GetMaintainLevel();

        const nlohmann::json& token = nxlFileInfo.at(NXLTOKEN_KEY_NAME);
        m_token.ImportFromJson(token);

        //Recipients
        const nlohmann::json& recipients = root.at(NXLRECIPIENTS_KEY_NAME);
        m_recipients.ImportFromJson(recipients);

        //MetaData
        const nlohmann::json& metaData = root.at(NXLMETADATA_KEY_NAME);

        m_meta_lastMDUpdateTime = metaData.at(NXLMDUPDATETIME_ELEMENT_NAME).get<uint32_t>();
        m_meta_nxllastmodified = metaData.at(NXLLASTMODIFY_ELEMENT_NAME).get<uint32_t>();
        m_meta_nxlshared = metaData.at(NXLSHAREDFLAG_ELEMENT_NAME).get<bool>();
        m_meta_nxldeleted = metaData.at(NXLDELETEFLAG_ELEMENT_NAME).get<bool>();
        m_meta_nxlrevoked = metaData.at(NXLREVOKEDFLAG_ELEMENT_NAME).get<bool>();

        m_meta_nxlname = metaData.at("meta_name").get<std::string>();
        m_meta_nxlfilelink = metaData.at("meta_filelink").get<std::string>();
        m_meta_nxlsharedon = metaData.at("meta_sharedon").get<uint32_t>();
        m_meta_nxlprotectedon = metaData.at("meta_protectedon").get<uint32_t>();
        m_meta_protectionType = metaData.at("meta_protectiontype").get<uint32_t>();

        m_meta_bnxlowner = metaData.at("meta_owner").get<bool>();
        m_meta_bnxlfile = metaData.at("meta_nxl").get<bool>();
        m_meta_pathDisplay = metaData.at("meta_pathdisplay").get<std::string>();
        m_meta_pathId = metaData.at("meta_pathid").get<std::string>();

        const nlohmann::json& metaRecipients = metaData.at("meta_recipients");
        m_meta_recipients.ImportFromJson(metaRecipients);

        //Validity
        const nlohmann::json& validity = metaData.at(NXLVALIDITY_KEY_NAME);
        ExpiryType type = validity.at(NXLVALIDITYTYPE_ELEMENT_NAME).get<ExpiryType>();
        uint64_t begin = validity.at(NXLVALIDITYBEGIN_ELEMENT_NAME).get<uint64_t>();
        uint64_t end = validity.at(NXLVALIDITYEND_ELEMENT_NAME).get<uint64_t>();
        m_meta_expirtion = Expiry(type, end, begin);
    }
    catch (std::exception &e) {
        std::string strError = "JSON NXLFile value is not correct, error : " + std::string(e.what());
        res = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        res = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "JSON NXLFile value is not correct");
    }

    return res;
}

nlohmann::json RMNXLFile::ExportToJson()
{
    nlohmann::json root = nlohmann::json::object();
    try {
        root[FILENAME_ELEMENT_NAME] = m_filepath;
        root[WFILENAME_ELEMENT_NAME] = toUtf8(m_wfilepath);
        root[SOURCEPATH_ELEMENT_NAME] = m_sourcepath;
        root[FILESIZE_ELEMENT_NAME] = m_filesize;
        root[NXLUPLOAD_ELEMENT_NAME] = m_uploaded;
        root[NXLPATHID_ELEMENT_NAME] = m_nxlpathid;

        //NXLFileInfo
        root[NXLFILEINFO_KEY_NAME] = nlohmann::json::object();
        nlohmann::json& nxlFileInfo = root[NXLFILEINFO_KEY_NAME];

        nxlFileInfo[NXLFILE_ELEMENT_NAME] = m_nxlname;
        nxlFileInfo[NXLTOKEN_KEY_NAME] = m_token.ExportToJson();
        nxlFileInfo[NXLAGREEMENT_ELEMENT_NAME] = m_nxlagreement;
        nxlFileInfo[NXLAGREEMENT1_ELEMENT_NAME] = m_nxlagreement1;
        nxlFileInfo[NXLOWNERID_ELEMENT_NAME] = m_nxlownerid;

        //Recipients
        root[NXLRECIPIENTS_KEY_NAME] = m_recipients.ExportToJson();

        //MetaData
        root[NXLMETADATA_KEY_NAME] = nlohmann::json::object();
        nlohmann::json& metadata = root[NXLMETADATA_KEY_NAME];

        metadata["meta_name"] = m_meta_nxlname;
        metadata["meta_filelink"] = m_meta_nxlfilelink;
        metadata[NXLMDUPDATETIME_ELEMENT_NAME] = m_meta_lastMDUpdateTime;
        metadata["meta_sharedon"] = m_meta_nxlsharedon;
        metadata["meta_protectedon"] = m_meta_nxlprotectedon;

        metadata["meta_protectiontype"] = m_meta_protectionType;
        metadata[NXLLASTMODIFY_ELEMENT_NAME] = m_meta_nxllastmodified;
        metadata[NXLSHAREDFLAG_ELEMENT_NAME] = m_meta_nxlshared;
        metadata[NXLDELETEFLAG_ELEMENT_NAME] = m_meta_nxldeleted;
        metadata[NXLREVOKEDFLAG_ELEMENT_NAME] = m_meta_nxlrevoked;

        metadata["meta_owner"] = m_meta_bnxlowner;
        metadata["meta_nxl"] = m_meta_bnxlfile;
        metadata["meta_pathdisplay"] = m_meta_pathDisplay;
        metadata["meta_pathid"] = m_meta_pathId;
        metadata["meta_recipients"] = m_meta_recipients.ExportToJson();

        //Validity
        metadata[NXLVALIDITY_KEY_NAME] = nlohmann::json::object();
        nlohmann::json& validity = metadata[NXLVALIDITY_KEY_NAME];

        validity[NXLVALIDITYTYPE_ELEMENT_NAME] = m_meta_expirtion.getType();
        validity[NXLVALIDITYBEGIN_ELEMENT_NAME] = m_meta_expirtion.getStartDate();
        validity[NXLVALIDITYEND_ELEMENT_NAME] = m_meta_expirtion.getEndDate();
    }
    catch (std::exception &e) {
        std::string strError = "Export NXL File Info Json error : " + std::string(e.what());
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "Export NXL File Info Json error");
    }

    return root;
}

RetValue RMNXLFile::ImportMetadataQueryFromRMSResponse(const std::string& jsonstr)
{
    RetValue res = RETSUCCEED();

    try {
        nlohmann::json root = nlohmann::json::parse(jsonstr);
        if (!root.is_object())
        {
            return RETMESSAGE(RMCCORE_INVALID_JSON_FORMAT, "RMS Json data error!");
        }

        int status = root.at(STATUS_ELEMENT_NAME).get<int>();
        std::string message = root.at(MESSAGE_ELEMENT_NAME).get<std::string>();
        if (status != http::status_codes::OK.id)
        {
            return RETMESSAGE((RMCCORE_RMS_ERRORCODE_BASE + status), message);
        }

        const nlohmann::json& results = root.at(RESULT_KEY_NAME);
        const nlohmann::json& detail = results.at(MYVALUT_NXL_DETAIL_KEY_NAME);

        m_meta_nxlname = detail.at(MYVAULT_NXL_FILENAME_ELEMENT_NAME).get<std::string>();

        if (detail.end() != detail.find(MYVALUT_NXL_FILELINK_ELEMENT_NAME))
        {//Fix bug 62032
            m_meta_nxlfilelink = detail[MYVALUT_NXL_FILELINK_ELEMENT_NAME].get<std::string>();
        }

        if (detail.end() != detail.find(MYVALUT_NXL_SHAREDTIME_ELEMENT_NAME))
        {//Fix bug 62032, if file not be shared, will not have this field
            m_meta_nxlsharedon = detail[MYVALUT_NXL_SHAREDTIME_ELEMENT_NAME].get<uint64_t>();
        }

        m_meta_nxlprotectedon = detail.at(MYVALUT_NXL_PROTECTTIME_ELEMENT_NAME).get<uint64_t>();

        if (detail.end() != detail.find(MYVALUT_NXL_RECIPIENTS_KEY_NAME))
        {
            RMRecipients recipients(detail[MYVALUT_NXL_RECIPIENTS_KEY_NAME].get<RMRecipientList>());
            m_meta_recipients = recipients;
        }

        m_meta_nxllastmodified = m_meta_nxlprotectedon; // ignore the shared time

        m_meta_nxlshared = detail.at(MYVALUT_NXL_SHAREDFLAG_ELEMENT_NAME).get<bool>();
        m_meta_nxldeleted = detail.at(MYVALUT_NXL_DELETEDFLAG_ELEMENT_NAME).get<bool>();
        m_meta_nxlrevoked = detail.at(MYVALUT_NXL_REVOKEDFLAG_ELEMENT_NAME).get<bool>();
        m_meta_protectionType = detail.at(MYVALUT_NXL_PROTECTION_TYPE).get<uint32_t>();

        m_meta_expirtion = Expiry(NEVEREXPIRE);
        if (detail.end() != detail.find(MYVALUT_NXL_VALIDITY_KEY_NAME))
        {
            const nlohmann::json& validaty = detail[MYVALUT_NXL_VALIDITY_KEY_NAME];
            if (validaty.end() != validaty.find(MYVALUT_NXL_VENDDATE_ELEMENT_NAME))
            {
                uint64_t vbegin, vend;
                vend = validaty[MYVALUT_NXL_VENDDATE_ELEMENT_NAME].get<uint64_t>();
                if (validaty.end() != validaty.find(MYVALUT_NXL_VSTARTDATE_ELEMENT_NAME))
                {
                    vbegin = validaty[MYVALUT_NXL_VSTARTDATE_ELEMENT_NAME].get<uint64_t>();
                    m_meta_expirtion = Expiry(RANGEEXPIRE, vend, vbegin);
                }
                else
                {
                    m_meta_expirtion = Expiry(ABSOLUTEEXPIRE, vend);
                }
            }
        }

        m_meta_lastMDUpdateTime = NXTime::nowTime();
    }
    catch (std::exception &e) {
        std::string strError = "JSON data is not correct : " + std::string(e.what());
        res = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        res = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "JSON membership value is not correct");
    }

	return res;
}

RetValue RMNXLFile::ImportProjectFileMetadataQueryFromRMSResponse(const std::string& jsonstr)
{
    RetValue res = RETSUCCEED();

    try
    {
        nlohmann::json root = nlohmann::json::parse(jsonstr);
        if (!root.is_object())
        {
            return RETMESSAGE(RMCCORE_INVALID_JSON_FORMAT, "RMS Json data error!");
        }

        int status = root.at(STATUS_ELEMENT_NAME).get<int>();
        std::string message = root.at(MESSAGE_ELEMENT_NAME).get<std::string>();
        if (status != http::status_codes::OK.id)
        {
            return RETMESSAGE((RMCCORE_RMS_ERRORCODE_BASE + status), message);
        }

        const nlohmann::json& result = root.at(RESULT_KEY_NAME);
        const nlohmann::json& detail = result.at("fileInfo");

        m_meta_nxlname = detail.at(MYVAULT_NXL_FILENAME_ELEMENT_NAME).get<std::string>();
        m_meta_pathDisplay = detail.at("pathDisplay").get<std::string>();
        m_meta_pathId = detail.at("pathId").get<std::string>();
        m_meta_nxllastmodified = detail.at("lastModified").get<uint64_t>();
        m_meta_protectionType = detail.at("protectionType").get<uint32_t>();
        m_meta_bnxlowner = detail.at("owner").get<bool>();
        m_meta_bnxlfile = detail.at("nxl").get<bool>();

        m_meta_expirtion = Expiry(NEVEREXPIRE);
        if (detail.end() != detail.find(SHARE_EXPIRY_KEY_NAME))
        {
            const nlohmann::json& expiry = detail[SHARE_EXPIRY_KEY_NAME];
            if (expiry.end() != expiry.find(MYVALUT_NXL_VENDDATE_ELEMENT_NAME))
            {
                uint64_t vbegin, vend;
                vend = expiry[MYVALUT_NXL_VENDDATE_ELEMENT_NAME].get<uint64_t>();
                if (expiry.end() != expiry.find(MYVALUT_NXL_VSTARTDATE_ELEMENT_NAME))
                {
                    vbegin = expiry[MYVALUT_NXL_VSTARTDATE_ELEMENT_NAME].get<uint64_t>();
                    m_meta_expirtion = Expiry(RANGEEXPIRE, vend, vbegin);
                }
                else
                {
                    m_meta_expirtion = Expiry(ABSOLUTEEXPIRE, vend);
                }
            }
        }

        m_meta_lastMDUpdateTime = NXTime::nowTime();
    }
    catch (std::exception &e) {
        std::string strError = "JSON data is not correct : " + std::string(e.what());
        res = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        res = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "JSON membership value is not correct");
    }

	return res;
}