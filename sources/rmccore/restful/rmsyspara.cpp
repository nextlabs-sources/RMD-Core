//
//  rmsyspara.cpp
//  rmccore
//
//  Created by Haobo Wang on 2/1/18.
//  Copyright © 2018 NextLabs, Inc. All rights reserved.
//

#include "stdafx.h"
#include <stdio.h>
#include <cstdlib>
#include <ctime>
#include "rmccore/restful/rmsyspara.h"
#include "rmccore/utils/time.h"
#include "rmccore/network/httpConst.h"
#include "rmrestkey.h"

#include "rmrestkey.h"


using namespace RMCCORE;


#define CLIENTID_ELEMENT_NAME     "ClientGUID"
#define DEVICEID_ELEMENT_NAME     "DeviceName"
#define PRODUCT_KEY_NAME            "ProductInfo"
#define PROCESSOR_ELEMENT_NAME      "ProcessorArch"
#define PLATFORMID_ELEMENT_NAME   "PlatformID"

#define WINDOWS_DESKTOP_DEVICETYPE_STR  "Windows Desktop"
#define WINDOWS_SERVER_DEVICETYPE_STR   "Windows Server"
#define LINUX_UNIX_DEVICETYPE_STR       "Linux/Unix"
#define MAC_OS_DEVICETYPE_STR           "Mac OS"
#define WINDOWS_PHONE_DEVICETYPE_STR    "Windows Phone"
#define WINDOWS_TABLET_DEVICETYPE_STR   "Windows Tablet"
#define IPHONE_DEVICETYPE_STR           "iPhone"
#define IPAD_DEVICETYPE_STR             "iPad"
#define ANDROID_PHONE_DEVICETYPE_STR    "Android Phone"
#define ANDROID_TABLET_DEVICETYPE_STR   "Android Tablet"
#define WEB_DEVICETYPE_STR              "WebApp"
#define RMX_DEVICETYPE_STR              "RMX"

RMSystemPara::RMSystemPara() : RestfulBase(),m_deviceid(""),m_processor(RPAUnknown), m_product("RMCCore", 0, 0, 0)
{
#ifdef _WIN32
    m_platformid = RPWindowsDesktop;
#ifdef _X64
    m_processor = RPAWindowsX64;
#else
    m_processor = RPAWindowsX86;
#endif
#else
    m_platformid = RPMac;
#endif // _WIN32

    GenerateNewClientID();
}


RMSystemPara::~RMSystemPara()
{
}

void RMSystemPara::SetDeviceID(const std::string deviceid)
{
    if(deviceid.length() > 0)
        m_deviceid = deviceid;
}

bool RMSystemPara::SetClientID(const std::string& clientid)
{
	if (clientid.length() != 32)
		return false;
	if (!isalphanumstring(clientid))
		return false;

	m_clientid = clientid;

	return true;
}

void RMSystemPara::GenerateNewClientID()
{
    m_clientid.clear();
    std::srand((unsigned int)NXTime::nowTime());
    for (int i = 0; i < 16; i++) {
        unsigned char r = (unsigned char)std::rand();
        std::string tmp = utohs<char>(r);
        m_clientid += tmp;
    }
}

// The following platform ID to device type map must be sorted by platform ID.
static const struct {RMPlatformID id; const char *type;} idToTypeMap[] = {
    {RPWindowsDesktop,  WINDOWS_DESKTOP_DEVICETYPE_STR},
    {RPWindowsServer,   WINDOWS_SERVER_DEVICETYPE_STR},
    {RPLinux,           LINUX_UNIX_DEVICETYPE_STR},
    {RPMac,             MAC_OS_DEVICETYPE_STR},
    {RPWindowsPhone,    WINDOWS_PHONE_DEVICETYPE_STR},
    {RPWindowsTablet,   WINDOWS_TABLET_DEVICETYPE_STR},
    {RPIPhone,          IPHONE_DEVICETYPE_STR},
    {RPIPad,            IPAD_DEVICETYPE_STR},
    {RPAndroidPhone,    ANDROID_PHONE_DEVICETYPE_STR},
    {RPAndroidTablet,   ANDROID_TABLET_DEVICETYPE_STR},
    {RPWebApp,          WEB_DEVICETYPE_STR},
    {RPRMX,             RMX_DEVICETYPE_STR},
};

std::string RMSystemPara::PlatformIDToDeviceType(RMPlatformID id)
{
    for (int i = _countof(idToTypeMap) - 1; i >= 0; i--) {
        if (id >= idToTypeMap[i].id) {
            return idToTypeMap[i].type;
        }
    }

    return "Unknown";
}

std::string RMSystemPara::GetDeviceType(void)
{
    return PlatformIDToDeviceType(m_platformid);
}

RMSystemPara & RMSystemPara::operator= (const RMSystemPara & rhs)
{
    if (this != &rhs)
    {
        m_clientid = rhs.m_clientid;
        m_product = rhs.m_product;
        m_platformid = rhs.m_platformid;
        m_processor = rhs.m_processor;
        m_deviceid = rhs.m_deviceid;
    }
    
    return *this;
}

RetValue RMSystemPara::ImportFromJson(const nlohmann::json& root)
{
    RetValue res = RETSUCCEED();
    try {
        m_clientid = root.at(CLIENTID_ELEMENT_NAME).get<std::string>();
        m_deviceid = root.at(DEVICEID_ELEMENT_NAME).get<std::string>();
        m_platformid = root.at(PLATFORMID_ELEMENT_NAME).get<RMPlatformID>();
        m_processor = root.at(PROCESSOR_ELEMENT_NAME).get<RMProcessorArch>();

        res = m_product.ImportFromJson(root.at(PRODUCT_KEY_NAME));
    }
    catch (...) {
        // The JSON data is NOT correct
        res = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "JSON value is not correct");
    }

    return res;
}

nlohmann::json RMSystemPara::ExportToJson()
{
    nlohmann::json root = nlohmann::json::object();
    try {
        root[CLIENTID_ELEMENT_NAME] = m_clientid;
        root[DEVICEID_ELEMENT_NAME] = m_deviceid;
        root[PLATFORMID_ELEMENT_NAME] = m_platformid;
        root[PROCESSOR_ELEMENT_NAME] = m_processor;

        root[PRODUCT_KEY_NAME] = m_product.ExportToJson();
    }
    catch (std::exception &e) {
        std::string strError = "JSON response is not correct, error : " + std::string(e.what());
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "Export Tenant Json error");
    }

    return root;
}
