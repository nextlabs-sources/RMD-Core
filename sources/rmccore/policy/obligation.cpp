//
//  obligation.cpp
//  rmccore
//
//  Created by Haobo Wang on 4/3/18.
//  Copyright © 2018 NextLabs, Inc. All rights reserved.
//

#include "stdafx.h"
#include <stdio.h>
#include "rmccore/restful/rmtenant.h"
#include "rmccore/network/httpConst.h"
#include "rmccore/policy/obligation.h"
#include "rmccore/utils/time.h"

#include "../restful/rmrestkey.h"
#include <assert.h>

using namespace RMCCORE;
using namespace RMCCORE::OBLIGATION;

#define WMCONFIG_WATERMARK_KEY_NAME           "Watermark"
#define WMCONFIG_SERIALNO_ELEMENT_NAME  "SerialNo"
#define WMCONFIG_TEXT_ELEMENT_NAME "Text"
#define WMCONFIG_FONTNAME_ELEMENT_NAME "FontName"
#define WMCONFIG_FONTSIZE_ELEMENT_NAME "FontSize"
#define WMCONFIG_FONTCOLOR_ELEMENT_NAME    "FontColor"
#define WMCONFIG_TRANSPARENCY_ELEMENT_NAME    "Transparency"
#define WMCONFIG_ROTATION_ELEMENT_NAME "Rotation"
#define WMCONFIG_REPEAT_ELEMENT_NAME   "Repeat"


Watermark::Watermark()
    : Obligation(OBLIGATION_NAME_WATERMARK), RMCCORE::RestfulBase(), m_initialized(false)
{

}

Watermark::Watermark(const std::string &text, const std::string & fontName, const std::string &fontColor, int fontSize, int transparency, Rotation rotation, bool repeat) : Obligation(OBLIGATION_NAME_WATERMARK), RMCCORE::RestfulBase()
{
    setArg(HEARTBEAT_WATERMARK_TEXT_ELEMENT_NAME, text);
    setArg(HEARTBEAT_WATERMARK_FONTNAME_ELEMENT_NAME, fontName);
    setArg(HEARTBEAT_WATERMARK_FONTCOLOR_ELEMENT_NAME, fontColor);
    setArg(HEARTBEAT_WATERMARK_FONTSIZE_ELEMENT_NAME, std::to_string(fontSize));
    setArg(HEARTBEAT_WATERMARK_TRANSPARENTCY_ELEMENT_NAME, std::to_string(transparency));
    setArg(HEARTBEAT_WATERMARK_ROTATION_ELEMENT_NAME, std::to_string(rotation));
    setArg(HEARTBEAT_WATERMARK_REPEAT_ELEMENT_NAME, repeat ? "true" : "false");
    
    m_initialized = true;
}

Watermark & Watermark::operator= (const Watermark & rhs)
{
    if (this != &rhs)
    {
		m_initialized = rhs.m_initialized;
		if (m_initialized) {
			setArg(HEARTBEAT_WATERMARK_TEXT_ELEMENT_NAME, rhs.getText());
			setArg(HEARTBEAT_WATERMARK_FONTNAME_ELEMENT_NAME, rhs.getFontName());
			setArg(HEARTBEAT_WATERMARK_FONTCOLOR_ELEMENT_NAME, rhs.GetFontColor());
			setArg(HEARTBEAT_WATERMARK_FONTSIZE_ELEMENT_NAME, std::to_string(rhs.getFontSize()));
			setArg(HEARTBEAT_WATERMARK_TRANSPARENTCY_ELEMENT_NAME, std::to_string(rhs.getTransparency()));
			setArg(HEARTBEAT_WATERMARK_ROTATION_ELEMENT_NAME, rhs.getRotationString());
			setArg(HEARTBEAT_WATERMARK_REPEAT_ELEMENT_NAME, rhs.getRepeat() ? "true" : "false");
		}
    }
    
    return *this;
}

Watermark & Watermark::operator= (const Obligation & rhs)
{
	m_initialized = true;
	setArg(HEARTBEAT_WATERMARK_TEXT_ELEMENT_NAME, rhs.getArg(HEARTBEAT_WATERMARK_TEXT_ELEMENT_NAME));
	setArg(HEARTBEAT_WATERMARK_FONTNAME_ELEMENT_NAME, rhs.getArg(HEARTBEAT_WATERMARK_FONTNAME_ELEMENT_NAME));
	setArg(HEARTBEAT_WATERMARK_FONTCOLOR_ELEMENT_NAME, rhs.getArg(HEARTBEAT_WATERMARK_FONTCOLOR_ELEMENT_NAME));
	setArg(HEARTBEAT_WATERMARK_FONTSIZE_ELEMENT_NAME, rhs.getArg(HEARTBEAT_WATERMARK_FONTSIZE_ELEMENT_NAME));
	setArg(HEARTBEAT_WATERMARK_TRANSPARENTCY_ELEMENT_NAME, rhs.getArg(HEARTBEAT_WATERMARK_TRANSPARENTCY_ELEMENT_NAME));
	setArg(HEARTBEAT_WATERMARK_ROTATION_ELEMENT_NAME, rhs.getArg(HEARTBEAT_WATERMARK_ROTATION_ELEMENT_NAME));
	setArg(HEARTBEAT_WATERMARK_REPEAT_ELEMENT_NAME, rhs.getArg(HEARTBEAT_WATERMARK_REPEAT_ELEMENT_NAME));

	return *this;
}

bool Watermark::operator== (const Watermark &rhs) const
{
	if (this == &rhs)
		return true;

	if (m_initialized != rhs.m_initialized)
		return false;

	if (Obligation::operator==(rhs))
		return true;

	return false;
}

std::string Watermark::getText() const
{
    return getArg(HEARTBEAT_WATERMARK_TEXT_ELEMENT_NAME);
}

std::string Watermark::getFontName() const
{
    return getArg(HEARTBEAT_WATERMARK_FONTNAME_ELEMENT_NAME);
}

std::string Watermark::GetFontColor() const
{
    return getArg(HEARTBEAT_WATERMARK_FONTCOLOR_ELEMENT_NAME);
}

int Watermark::getFontSize() const
{
    const std::string& s = getArg(HEARTBEAT_WATERMARK_FONTSIZE_ELEMENT_NAME);
    if (s.empty())
        return -1;
    return std::stoi(s);
}

int Watermark::getTransparency() const
{
    const std::string& s = getArg(HEARTBEAT_WATERMARK_TRANSPARENTCY_ELEMENT_NAME);
    if (s.empty())
        return -1;
    return std::stoi(s);
}

std::string Watermark::getRotationString() const
{
    return getArg(HEARTBEAT_WATERMARK_ROTATION_ELEMENT_NAME);
}

Watermark::Rotation Watermark::getRotation() const
{
    const std::string& s = getArg(HEARTBEAT_WATERMARK_ROTATION_ELEMENT_NAME);
	if (s.empty())
		return NOROTATION;

    static std::map<std::string, int> mapRotation = { { "clockwise", 1 },{ "anticlockwise", 2 } };
    int rotation = 0;

    std::string strKey = s;
    std::transform(strKey.begin(), strKey.end(), strKey.begin(), ::tolower);
    strKey.erase(strKey.find_last_not_of(" \t") + 1);
    strKey.erase(0, strKey.find_first_not_of(" \t"));

    auto it = mapRotation.find(strKey);
    if (mapRotation.end() != it)
    {
        rotation = it->second;
    }
    else
    {
        rotation = std::stoi(s);
    }

    if (CLOCKWISE == rotation)
        return CLOCKWISE;
    else if (ANTICLOCKWISE == rotation)
        return ANTICLOCKWISE;
    else
        return NOROTATION;
}

bool Watermark::getRepeat() const
{
    const std::string& s = getArg(HEARTBEAT_WATERMARK_REPEAT_ELEMENT_NAME);
    return (s == "true");
}

RetValue Watermark::ImportFromJson(const nlohmann::json& root)
{
    RetValue res = RETSUCCEED();
    try {
        m_initialized = false;
        if (0 >= root.size())
        {
            return res;
        }

        if (root.end() != root.find(HEARTBEAT_WATERMARK_TEXT_ELEMENT_NAME))
        {//From RMS
            std::string text = root.at(HEARTBEAT_WATERMARK_TEXT_ELEMENT_NAME).get<std::string>();
            setArg(HEARTBEAT_WATERMARK_TEXT_ELEMENT_NAME, text);

            std::string fontname = root.at(HEARTBEAT_WATERMARK_FONTNAME_ELEMENT_NAME).get<std::string>();
            setArg(HEARTBEAT_WATERMARK_FONTNAME_ELEMENT_NAME, fontname);

            std::string fontcolor = root.at(HEARTBEAT_WATERMARK_FONTCOLOR_ELEMENT_NAME).get<std::string>();
            setArg(HEARTBEAT_WATERMARK_FONTCOLOR_ELEMENT_NAME, fontcolor);

            uint32_t fontsize = root.at(HEARTBEAT_WATERMARK_FONTSIZE_ELEMENT_NAME).get<uint32_t>();
            setArg(HEARTBEAT_WATERMARK_FONTSIZE_ELEMENT_NAME, std::to_string(fontsize));

            uint32_t transparency = root.at(HEARTBEAT_WATERMARK_TRANSPARENTCY_ELEMENT_NAME).get<uint32_t>();
            setArg(HEARTBEAT_WATERMARK_TRANSPARENTCY_ELEMENT_NAME, std::to_string(transparency));

            std::string rotation = root.at(HEARTBEAT_WATERMARK_ROTATION_ELEMENT_NAME).get<std::string>();
            setArg(HEARTBEAT_WATERMARK_ROTATION_ELEMENT_NAME, rotation);

            bool repeat = root.at(HEARTBEAT_WATERMARK_REPEAT_ELEMENT_NAME).get<bool>();
            setArg(HEARTBEAT_WATERMARK_REPEAT_ELEMENT_NAME, repeat ? "true" : "false");
        }
        else if (root.end() != root.find(WMCONFIG_TEXT_ELEMENT_NAME))
        {
            std::string text = root.at(WMCONFIG_TEXT_ELEMENT_NAME).get<std::string>();
            setArg(HEARTBEAT_WATERMARK_TEXT_ELEMENT_NAME, text);

            std::string fontname = root.at(WMCONFIG_FONTNAME_ELEMENT_NAME).get<std::string>();
            setArg(HEARTBEAT_WATERMARK_FONTNAME_ELEMENT_NAME, fontname);

            std::string fontcolor = root.at(WMCONFIG_FONTCOLOR_ELEMENT_NAME).get<std::string>();
            setArg(HEARTBEAT_WATERMARK_FONTCOLOR_ELEMENT_NAME, fontcolor);

            uint32_t fontsize = root.at(WMCONFIG_FONTSIZE_ELEMENT_NAME).get<uint32_t>();
            setArg(HEARTBEAT_WATERMARK_FONTSIZE_ELEMENT_NAME, std::to_string(fontsize));

            uint32_t transparency = root.at(WMCONFIG_TRANSPARENCY_ELEMENT_NAME).get<uint32_t>();
            setArg(HEARTBEAT_WATERMARK_TRANSPARENTCY_ELEMENT_NAME, std::to_string(transparency));

            std::string rotation = root.at(WMCONFIG_ROTATION_ELEMENT_NAME).get<std::string>();
            setArg(HEARTBEAT_WATERMARK_ROTATION_ELEMENT_NAME, rotation);

            bool repeat = root.at(WMCONFIG_REPEAT_ELEMENT_NAME).get<bool>();
            setArg(HEARTBEAT_WATERMARK_REPEAT_ELEMENT_NAME, repeat ? "true" : "false");
        }

        m_initialized = true;
    }
    catch (std::exception &e) {
        std::string strError = "JSON response is not correct, error : " + std::string(e.what());
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        m_initialized = false;
        // The JSON data is NOT correct
        res = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "Watermark JSON value is not correct");
    }

    return res;
}

nlohmann::json Watermark::ExportToJson()
{
    nlohmann::json root = nlohmann::json::object();
    try {
        if (empty())
        {
            return root;
        }

        root[WMCONFIG_TEXT_ELEMENT_NAME] = getText();
        root[WMCONFIG_FONTNAME_ELEMENT_NAME] = getFontName();
        root[WMCONFIG_FONTCOLOR_ELEMENT_NAME] = GetFontColor();
        root[WMCONFIG_FONTSIZE_ELEMENT_NAME] = getFontSize();
        root[WMCONFIG_TRANSPARENCY_ELEMENT_NAME] = getTransparency();
        root[WMCONFIG_ROTATION_ELEMENT_NAME] = getRotationString();
        root[WMCONFIG_REPEAT_ELEMENT_NAME] = getRepeat();
    }
    catch (std::exception &e) {
        std::string strError = "ExportToJson error, error : " + std::string(e.what());
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "ExportToJson error");
    }

    return root;
}

WatermarkConfig::WatermarkConfig() 
{
}

bool WatermarkConfig::operator== (const WatermarkConfig &rhs) const
{
	if (this == &rhs)
		return true;

	if (0 != m_serialno.compare(rhs.m_serialno))
		return false;

	if (m_watermark == rhs.m_watermark)
		return true;

	return false;
}

/*
"watermarkConfig": {
    "name": "watermarkConfig",
        "serialNumber" : "",
        "content" : "$(User)$(Break)$(Date) $(Time)"
}

"watermarkConfig": {
    "serialNumber": "edbd53a",
        "content" : "{\n    \"text\": \"$(User)\\n$(Date) $(Time)\",\n    \"transparentRatio\": 70,\n    \"fontName\": \"Sitka Text\",\n    \"fontSize\": 26,\n    \"fontColor\": \"#008000\",\n    \"rotation\": \"Anticlockwise\",\n    \"repeat\": true,\n    \"density\": \"dense\"\n}\n"
}
*/

RetValue WatermarkConfig::ImportFromJson(const nlohmann::json& root)
{
    RetValue res = RETSUCCEED();
    try {
        if (root.end() != root.find(HEARTBEAT_WATERMARK_SERIAL_ELEMENT_NAME))
        {//From RMS
            m_serialno = root.at(HEARTBEAT_WATERMARK_SERIAL_ELEMENT_NAME).get<std::string>();
            std::string strContent = root.at(HEARTBEAT_WATERMARK_CONTENT_ELEMENT_NAME).get<std::string>();
            if (root.end() == root.find("name"))
            {
                auto content = nlohmann::json::parse(strContent);
                res = m_watermark.ImportFromJson(content);
            }
        }
        else if (root.end() != root.find(WMCONFIG_SERIALNO_ELEMENT_NAME))
        {
            m_serialno = root.at(WMCONFIG_SERIALNO_ELEMENT_NAME).get<std::string>();
            const nlohmann::json& watermark = root.at(WMCONFIG_WATERMARK_KEY_NAME);
            res = m_watermark.ImportFromJson(watermark);
        }
    }
    catch (std::exception &e) {
        std::string strError = "JSON response is not correct, error : " + std::string(e.what());
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        res = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "JSON value is not correct");
    }

    return res;
}

nlohmann::json WatermarkConfig::ExportToJson()
{
    nlohmann::json root = nlohmann::json::object();
    try
    {
        root[WMCONFIG_SERIALNO_ELEMENT_NAME] = m_serialno;
        root[WMCONFIG_WATERMARK_KEY_NAME] = m_watermark.ExportToJson();
    }
    catch (std::exception &e) {
        std::string strError = "ExportToJson error, error : " + std::string(e.what());
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "ExportToJson error");
    }

    return root;
}

