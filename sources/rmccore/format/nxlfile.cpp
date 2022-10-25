//
//  nxlfile.cpp
//  rmccore
//
//  Created by Gavin Ye on 10/20/17.
//  Copyright © 2017 NextLabs, Inc. All rights reserved.
//

#include "stdafx.h"
#include "common/celog2/celog.h"
#include "rmccore/format/nxlfile.h"
#include "rmccore/format/nxlrights.h"
#include "rmccore/utils/string.h"

#include "rmccore/crypto/aes.h"
#include "rmccore/crypto/sha.h"
#include "rmccore/utils/time.h"
#include "nlohmann/json.hpp"

#include <memory>
#include <iomanip>

#define CELOG_CUR_MODULE "rmccore"
#define CELOG_CUR_FILE CELOG_FILEPATH_SOURCES_RMCCORE_FORMAT_NXLFILE_CPP

namespace RMCCORE {
namespace NXLFMT {

static const std::vector<uint8_t> kZeroData(256, 0);

namespace {
void createChecksum(const uint8_t* data, uint32_t size, const uint8_t* salt, uint32_t saltSize, uint8_t* checksum)
{
    std::vector<uint8_t> buf(size + 4);
    memcpy(buf.data(), &size, 4);
    if(0 != size)
        memcpy(buf.data() + 4, data, size);
    CRYPT::CreateSha256Hmac(buf.data(), (uint32_t)buf.size(), salt, saltSize, checksum);
}

bool verifyChecksum(const uint8_t* data, uint32_t size, const uint8_t* salt, uint32_t saltSize, const uint8_t* checksum)
{
    uint8_t checksum2[32] = {0};
    createChecksum(data, size, salt, saltSize, checksum2);
    return (0 == memcmp(checksum, checksum2, 32));
}
}

Header::Header()
{
    memset(this, 0, sizeof(NXL_HEADER2));
}

Header::~Header()
{
}

FileError Header::validate() const
{
    CELOG_ENUM_ENTER(FileError);
    // Check signature
    if(fixed.signature.magic.code != NXL_MAGIC_CODE_2)
        CELOG_ENUM_RETURN_VAL(FeInvalidMagic);
    const uint16_t majorVer = (uint16_t)(fixed.signature.version >> 16);
    if(majorVer != 3)
        CELOG_ENUM_RETURN_VAL(FeInvalidVersion);
    if(majorVer < 3)
        CELOG_ENUM_RETURN_VAL(FeVersionTooOld);

    // Check file info
    if(0 == memcmp(fixed.fileInfo.duid, kZeroData.data(), 16))
        CELOG_ENUM_RETURN_VAL(FeInvalidDuid);
    if(0 != (fixed.fileInfo.alignment % NXL_ALIGNMENT_SIZE))
        CELOG_ENUM_RETURN_VAL(FeInvalidAlignment);
    if(fixed.fileInfo.algorithm != NXL_ALGORITHM_AES256)
        CELOG_ENUM_RETURN_VAL(FeInvalidAlgorithm);
    if(fixed.fileInfo.blockSize != NXL_BLOCK_SIZE)
        CELOG_ENUM_RETURN_VAL(FeInvalidBlockSize);
    if(0 != (fixed.fileInfo.contentOffset % NXL_PAGE_SIZE))
        CELOG_ENUM_RETURN_VAL(FeInvalidHeaderSize);
    if(0 == memcmp(fixed.fileInfo.ownerId, kZeroData.data(), 16))
        CELOG_ENUM_RETURN_VAL(FeInvalidOwnerId);

    // Check keys
    if(0 == memcmp(fixed.keys.ivSeed, kZeroData.data(), 16))
        CELOG_ENUM_RETURN_VAL(FeInvalidCryptIv);
    if(0 == memcmp(fixed.keys.tokenCek, kZeroData.data(), 32) &&
       0 == memcmp(fixed.keys.tokenCekChecksum, kZeroData.data(), 32))
        CELOG_ENUM_RETURN_VAL(FeInvalidCipherCek);
    if(0 == memcmp(fixed.keys.publicKey1, kZeroData.data(), 64))
        CELOG_ENUM_RETURN_VAL(FeNoAgreement);

    // Check sections
    //  -> Section 0:
    if(std::string(fixed.sections.record[0].name) != NXL_SECTION_NAME_FILEINFO)
        CELOG_ENUM_RETURN_VAL(FeNoSectionFileInfo);
    //  -> Section 1: 
    if(std::string(fixed.sections.record[1].name) != NXL_SECTION_NAME_FILEPOLICY)
        CELOG_ENUM_RETURN_VAL(FeNoSectionFilePolicy);
    //  -> Section 2: 
    if(std::string(fixed.sections.record[2].name) != NXL_SECTION_NAME_FILETAG)
        CELOG_ENUM_RETURN_VAL(FeNoSectionFileTag);

    CELOG_ENUM_RETURN_VAL(FeSuccess);
}

FileError Header::init(const Token& token,
                       const std::string& ownerId,
                       const std::vector<uint8_t>& agreement0,
                       const std::vector<uint8_t>& agreement1,
                       const std::string& info,
                       const std::string& policy,
                       const std::string& tags,
                       const std::vector<uint8_t>& recoveryKey)
{
    static const std::string emptySection("{}");
    uint32_t len = 0;

    memset(this, 0, sizeof(NXL_HEADER2));

    if(token.empty() || token.getId().size() != 16 || token.getKey().size() != 32)
        return FeInvalidToken;
    if(ownerId.empty())
        return FeInvalidOwnerId;
    if(agreement0.size() != 256)
        return FeInvalidAgreement;
	if (agreement1.size() != 256)
		return FeInvalidAgreement;
    //
    //  Fixed Header
    //

    //  # Signature
    fixed.signature.magic.code = NXL_MAGIC_CODE_2;
    fixed.signature.version = NXL_VERSION_30;
    memcpy(fixed.signature.message, NXL_DEFAULT_MSG, sizeof(NXL_DEFAULT_MSG));

    // # FileInfo
    memcpy(fixed.fileInfo.duid, token.getId().data(), 16);
    fixed.fileInfo.flags = 0;
	fixed.fileInfo.alignment = NXL_ALIGNMENT_SIZE;
    fixed.fileInfo.algorithm = NXL_ALGORITHM_AES256;
    fixed.fileInfo.blockSize = NXL_BLOCK_SIZE;
    fixed.fileInfo.contentOffset = NXL_MIN_SIZE;
    memcpy(fixed.fileInfo.ownerId, ownerId.c_str(), ownerId.length());
    fixed.fileInfo.extendedDataOffset = 0;

    // # Keys
    fixed.keys.modeFlags = ((uint32_t)KF_SERVER_SECURE_MODE) << 24;
    CRYPT::randBytes(fixed.keys.ivSeed, 16);
    CRYPT::randBytes(fixed.keys.tokenCek, 32);
    createChecksum(fixed.keys.tokenCek, 32, token.getKey().data(), 32, fixed.keys.tokenCekChecksum);
	if (fixed.fileInfo.blockSize == 0)
		return FeInvalidBlockSize;
    if(!CRYPT::AesEncrypt(token.getKey().data(), 32, fixed.keys.tokenCek, 32, fixed.keys.ivSeed, 0, fixed.fileInfo.blockSize, fixed.keys.tokenCek, 32, &len))
        return FeCryptoError;
    if (recoveryKey.size() == 32)
    {
        memcpy(fixed.keys.recoveryCek, fixed.keys.tokenCek, 32);
        createChecksum(fixed.keys.recoveryCek, 32, recoveryKey.data(), 32, fixed.keys.recoveryCekChecksum);
        if(!CRYPT::AesEncrypt(token.getKey().data(), 32, fixed.keys.recoveryCek, 32, fixed.keys.ivSeed, 0, fixed.fileInfo.blockSize, fixed.keys.recoveryCek, 32, &len))
            return FeCryptoError;
    }
    memcpy(fixed.keys.publicKey1, agreement0.data(), 256);
    if(256 == agreement1.size())
        memcpy(fixed.keys.publicKey2, agreement1.data(), 256);
    fixed.keys.tokenLevel = token.getLevel();
    fixed.keys.extendedDataOffset = token.getLevel();

    // # Sections
    fixed.sections.section_map = 7;
    memcpy(fixed.sections.record[0].name, NXL_SECTION_NAME_FILEINFO, sizeof(NXL_SECTION_NAME_FILEINFO));
    fixed.sections.record[0].flags = 0;
    fixed.sections.record[0].offset= (ULONG)sizeof(NXL_HEADER2);
    fixed.sections.record[0].size  = NXL_PAGE_SIZE;
    fixed.sections.record[0].dataSize = (uint16_t)(info.empty() ? emptySection.length() : info.length());
    fixed.sections.record[0].compressedSize = 0;
    fixed.sections.record[0].flags = 0;
    createChecksum((const uint8_t*)(info.empty() ? emptySection.c_str() : info.c_str()), fixed.sections.record[0].dataSize, token.getKey().data(), 32, fixed.sections.record[0].checksum);
    memcpy(fixed.sections.record[1].name, NXL_SECTION_NAME_FILEPOLICY, strlen(NXL_SECTION_NAME_FILEPOLICY));
    fixed.sections.record[1].flags = 0;
    fixed.sections.record[1].offset= fixed.sections.record[0].offset + fixed.sections.record[0].size;
    fixed.sections.record[1].size  = NXL_PAGE_SIZE;
    fixed.sections.record[1].dataSize = (uint16_t)(policy.empty() ? emptySection.length() : policy.length());
    fixed.sections.record[1].compressedSize = 0;
    fixed.sections.record[1].flags = 0;
    createChecksum((const uint8_t*)(policy.empty() ? emptySection.c_str() : policy.c_str()), fixed.sections.record[1].dataSize, token.getKey().data(), 32, fixed.sections.record[1].checksum);
    memcpy(fixed.sections.record[2].name, NXL_SECTION_NAME_FILETAG, strlen(NXL_SECTION_NAME_FILETAG));
    fixed.sections.record[2].flags = 0;
    fixed.sections.record[2].offset= fixed.sections.record[1].offset + fixed.sections.record[1].size;
    fixed.sections.record[2].size = NXL_PAGE_SIZE;
    fixed.sections.record[2].dataSize = (uint16_t)(tags.empty() ? emptySection.length() : tags.length());
    fixed.sections.record[2].compressedSize = 0;
    fixed.sections.record[2].flags = 0;
    createChecksum((const uint8_t*)(tags.empty() ? emptySection.c_str() : tags.c_str()), fixed.sections.record[2].dataSize, token.getKey().data(), 32, fixed.sections.record[2].checksum);

    //
    // Dynamic Header
    //
    dynamic.contentLength = 0;
    createChecksum(reinterpret_cast<const uint8_t*>(this), sizeof(NXL_FIXED_HEADER), token.getKey().data(), 32, dynamic.fixedHeaderHash);

    return FeSuccess;
}

NXL_SECTION2* Header::findSection(const std::string& name)
{
    for (int i = 0; i < 32; i++) {
        if(name == fixed.sections.record[i].name)
            return &fixed.sections.record[i];
    }
    return nullptr;
}

std::vector<uint8_t> Header::getAgreement0() const
{
	std::vector<uint8_t> agreement(fixed.keys.publicKey1, fixed.keys.publicKey1 + 256);
	if (0 == memcmp(kZeroData.data(), agreement.data(), 256))
		agreement.clear();
	return agreement;
}

std::vector<uint8_t> Header::getAgreement1() const
{
	std::vector<uint8_t> agreement(fixed.keys.publicKey2, fixed.keys.publicKey2 + 256);
	if (0 == memcmp(kZeroData.data(), agreement.data(), 256))
		agreement.clear();
	return agreement;
}

std::vector<uint8_t> Header::getIvSeed() const
{
	return std::vector<uint8_t>(fixed.keys.ivSeed, fixed.keys.ivSeed + 16);
}

std::string Header::getOwnerId() const
{
	return std::string(fixed.fileInfo.ownerId);
}

uint64_t Header::getContentLength() const
{
	return dynamic.contentLength;
}

std::string Header::getTenant() const
{
	std::string ownerid = std::string(fixed.fileInfo.ownerId);
	auto pos = ownerid.rfind('@');
	std::string tenant = ownerid.substr(pos + 1);
	return (pos == std::string::npos) ? std::string() : ownerid.substr(pos + 1);
}

uint32_t Header::getBlockSize() const
{
	return fixed.fileInfo.blockSize;
}

TokenId Header::getTokenId() const
{
	return TokenId(std::vector<uint8_t>(fixed.fileInfo.duid, fixed.fileInfo.duid + 16), fixed.keys.tokenLevel);
}

std::string Header::getDuid() const
{
	return bintohs<char>(getTokenId().getId());
}

File::File() : ferror(FeSuccess), readOnly(true)
{
}

File::~File()
{
}

bool File::validate(const std::string& file)
{
    CELOG_ENTER;
    CELOG_LOG(CELOG_INFO, "file = %s\n", file.c_str());
    File f;
    CELOG_RETURN_VAL(f.open(file, true));
}

#ifdef _WIN32
bool File::validate(const std::wstring& file)
{
    CELOG_ENTER;
    CELOG_LOGW(CELOG_INFO, L"file = %s\n", file.c_str());
    File f;
    CELOG_RETURN_VAL(f.open(file, true));
}
#endif

bool File::create(const std::string& file,
                  const Token& token,
                  const std::string& ownerId,
                  const std::vector<uint8_t>& agreement0,
                  const std::vector<uint8_t>& agreement1,
                  const std::string& info,
                  const std::string& policy,
                  const std::string& tags,
                  const std::vector<uint8_t>& recoveryKey)
{
    fp.open(file, fp.out | fp.binary | fp.trunc);
	if (!fp.is_open()) {
		ferror = FeIoError;
		return false;
	}
	ferror = header.init(token, ownerId, agreement0, agreement1, info, policy, tags, recoveryKey);
	if (ferror != FeSuccess)
		return false;
    fp.seekp(NXL_MIN_SIZE - 1);
    fp.write("", 1);
    fp.seekg(header.fixed.sections.record[0].offset, fp.beg);
	if (info.length() > 0) {
		fp.write(info.data(), info.length());
	}
	else {
		fp.write("{}", 2);
	}
    fp.seekg(header.fixed.sections.record[1].offset, fp.beg);
	if (policy.length() > 0) {
		fp.write(policy.data(), policy.length());
	}
	else {
		fp.write("{}", 2);
	}
    fp.seekg(header.fixed.sections.record[2].offset, fp.beg);
	if (tags.length() > 0) {
		fp.write(tags.data(), tags.length());
	}
	else {
		fp.write("{}", 2);
	}
    flushHeader();
    return true;
}

#ifdef _WIN32
bool File::create(const std::wstring& file,
                  const Token& token,
                  const std::string& ownerId,
                  const std::vector<uint8_t>& agreement0,
                  const std::vector<uint8_t>& agreement1,
                  const std::string& info,
                  const std::string& policy,
                  const std::string& tags,
                  const std::vector<uint8_t>& recoveryKey)
{
    fp.open(file, fp.out | fp.binary | fp.trunc);
	if (!fp.is_open()) {
		ferror = FeIoError;
		return false;
	}
	ferror = header.init(token, ownerId, agreement0, agreement1, info, policy, tags, recoveryKey);
	if (ferror != FeSuccess)
		return false;
    fp.seekp(NXL_MIN_SIZE - 1);
    fp.write("", 1);
	fp.seekg(header.fixed.sections.record[0].offset, fp.beg);
	if (info.length() > 0) {
		fp.write(info.data(), info.length());
	}
	else {
		fp.write("{}", 2);
	}
	fp.seekg(header.fixed.sections.record[1].offset, fp.beg);
	if (policy.length() > 0) {
		fp.write(policy.data(), policy.length());
	}
	else {
		fp.write("{}", 2);
	}
	fp.seekg(header.fixed.sections.record[2].offset, fp.beg);
	if (tags.length() > 0) {
		fp.write(tags.data(), tags.length());
	}
	else {
		fp.write("{}", 2);
	}
	flushHeader();
    return true;
}
#endif
 
#ifdef _WIN32
bool File::open(const std::wstring& path, bool readOnly)
{
    // ensure close state
    close();

    fp.open(path, readOnly ? (std::fstream::in | std::fstream::binary) : (std::fstream::in | std::fstream::out | std::fstream::binary));
    if(!fp.is_open()) {
        ferror = FeIoError;
        return false;
    }

    if (!load()) {
        fp.close();
        return false;
    }

    return true;
}
#endif

bool File::open(const std::string& path, bool readOnly)
{
    // ensure close state
    close();

    fp.open(path, readOnly ? (std::fstream::in | std::fstream::binary) : (std::fstream::in | std::fstream::out | std::fstream::binary));
    if(!fp.is_open()) {
        ferror = FeIoError;
        return false;
    }

    if (!load()) {
        fp.close();
        return false;
    }

    return true;
}

bool File::load()
{
    do {

        fp.seekg(0, std::ios_base::end);
        size_t fsize = (size_t)fp.tellg();

        if(fsize < NXL_MIN_SIZE  || 0 != (fsize % 512)) {
            ferror = FeInvalidSize;
            break;
        }

        fp.seekg(0, std::ios_base::beg);
        fp.read((char*)&header, sizeof(NXL_HEADER2));
        if(!fp.good()) {
            ferror = FeIoError;
            break;
        }

        ferror = header.validate();
        if(!good())
            break;

    } while (false);

    return good();
}

void File::close()
{
    if(fp.is_open())
        fp.close();
    ferror = FeSuccess;
}

bool File::setTokenKey(const std::vector<uint8_t>& key)
{
    uint8_t buf[32] = {0};
    uint8_t checksum[32] = {0};
    uint32_t len = 0;

    if(key.size() != 32)
        return false;
    memcpy(buf, header.fixed.keys.tokenCek, 32);
    if(!CRYPT::AesDecrypt(key.data(), 32, buf, 32, header.fixed.keys.ivSeed, 0, header.fixed.fileInfo.blockSize, buf, 32, &len))
        return false;
    createChecksum(buf, 32, key.data(), 32, checksum);
    if(0 != memcmp(checksum, header.fixed.keys.tokenCekChecksum, 32))
        return false;

    kek = key;
    cek = std::vector<uint8_t>(buf, buf+32);
    return true;
}

bool File::unpack(const std::string& plainFile, bool replaceExisting)
{
    if(kek.size() != 32 || cek.size() != 32)
        return false;

    std::ofstream ofs;
    if (replaceExisting) {
        ofs.open(plainFile, std::ios::binary | std::ios::out | std::ios::trunc);
    }
    else {
        std::ifstream ifs;
        ifs.open(plainFile, std::ios::binary);
        if(ifs.is_open())
            return false;
        ofs.open(plainFile, std::ios::binary | std::ios::out | std::ios::trunc);
    }
    if(!ofs.is_open())
        return false;

    // Now decrypt file
    fp.seekg(0, fp.end);
    const uint64_t fileSize = (uint64_t)fp.tellg();
    if(fileSize < header.fixed.fileInfo.contentOffset)
        return false;
    uint64_t dataSize = header.dynamic.contentLength;
    uint64_t cipherDataSize = fileSize - header.fixed.fileInfo.contentOffset;
    if(cipherDataSize < dataSize)
        dataSize = cipherDataSize;

    std::vector<uint8_t> buf(header.fixed.fileInfo.blockSize, 0);
    uint64_t offset = 0;
    fp.seekg(header.fixed.fileInfo.contentOffset, fp.beg);
    while (dataSize != 0) {
        uint32_t validBytes = (uint32_t)(dataSize < header.fixed.fileInfo.blockSize ? dataSize : header.fixed.fileInfo.blockSize);
        uint32_t bytesToRead = ((validBytes + 15) / 16) * 16;
        uint32_t bytesRead = 0;
        fp.read((char*)buf.data(), bytesToRead);
        if(!fp.good())
            return false;

        if (!CRYPT::AesDecrypt(cek.data(), (uint32_t)cek.size(), buf.data(), bytesToRead, header.fixed.keys.ivSeed, offset, getBlockSize(), buf.data(), bytesToRead, &bytesRead))
            return false;

        ofs.write((const char*)buf.data(), bytesRead);
        if(!ofs.good())
            return false;

        offset += bytesRead;
        dataSize -= validBytes;
    }

    return true;
}

bool File::changeToken(const Token& newToken)
{
    uint32_t len = 0;
    if(newToken.getId().size() != 16 || newToken.getKey().size() != 32)
        return false;
    if(kek.size() != 32 || cek.size() != 32)
        return false;

    // Readonly not use.
    /*if(!fp.is_open() || readOnly)
        return false;*/
    if (!fp.is_open())
        return false;

    // When token is changed, following stuff should also be changed
    //      1. this->header.fixed.fileInfo.duid (tokenId)
    //      2. this->header.fixed.keys.tokenCek (encrypted cek)
    //      3. this->header.fixed.keys.tokenCekChecksum (HMAC-SHA256 checksum)
    //      4. header.fixed.keys.tokenLevel
    //      5. this->header.fixed.sections.record[i].checksum
    //      6. this->header.dynamic.fixedHeaderHash
    //      7. this->kek
    memcpy(header.fixed.fileInfo.duid, newToken.getId().data(), 16);
    memset(header.fixed.keys.tokenCek, 0, 32);
    memset(header.fixed.keys.tokenCekChecksum, 0, 32);
    memcpy(header.fixed.keys.tokenCek, cek.data(), 32);
    createChecksum(header.fixed.keys.tokenCek, 32, newToken.getKey().data(), 32, header.fixed.keys.tokenCekChecksum);
    if(!CRYPT::AesEncrypt(newToken.getKey().data(), 32, header.fixed.keys.tokenCek, 32, header.fixed.keys.ivSeed, 0, header.fixed.fileInfo.blockSize, header.fixed.keys.tokenCek, 32, &len))
        return false;
    header.fixed.keys.tokenLevel = newToken.getLevel();
    for (int i = 0; i < 32; i++)
    {
        if (header.fixed.sections.record[i].name[0] == 0)
            break;
        fp.seekg(header.fixed.sections.record[i].offset, fp.beg);
        std::vector<uint8_t> data(header.fixed.sections.record[i].dataSize);
        fp.read((char*)data.data(), header.fixed.sections.record[i].dataSize);
        if(!fp.good())
            return false;
        createChecksum(data.data(), header.fixed.sections.record[i].dataSize, newToken.getKey().data(), 32, header.fixed.sections.record[i].checksum);
    }
    createChecksum((const uint8_t*)&header.fixed, (uint32_t)sizeof(NXL_FIXED_HEADER), newToken.getKey().data(), 32, header.dynamic.fixedHeaderHash);
    memcpy(kek.data(), newToken.getKey().data(), 32);
    // Write header into file
    fp.seekg(0, fp.beg);
    fp.write((const char*)&header, sizeof(NXL_HEADER2));
    return true;
}

TokenId File::getTokenId() const
{
    return TokenId(std::vector<uint8_t>(header.fixed.fileInfo.duid, header.fixed.fileInfo.duid + 16), header.fixed.keys.tokenLevel);
}

std::string File::getOwnerId() const
{
    return std::string(header.fixed.fileInfo.ownerId);
}

bool File::changeOwnerId(const std::string& ownerId)
{
#ifdef _WIN32
    // On Windows, the MS Debug version of strcpy_s() first fills the buffer
    // with 0xFE before copying the string.  As a result, the bytes in the
    // ownerId field after the end of string all contain 0xFE.  Since the
    // checksum of the header is calculated based on the whole ownerId field
    // instead of only the used part, we need to fill the unused part with
    // 0x00 *after* calling strcpy_s() in order to ensure checksum
    // consistency.
    strcpy_s(header.fixed.fileInfo.ownerId, ownerId.c_str());
#else
    strcpy(header.fixed.fileInfo.ownerId, ownerId.c_str());
#endif
    if (sizeof(header.fixed.fileInfo.ownerId) > ownerId.size() + 1) {
        memset(header.fixed.fileInfo.ownerId + (ownerId.size() + 1), 0, sizeof(header.fixed.fileInfo.ownerId) - (ownerId.size() + 1));
    }

    createChecksum(reinterpret_cast<const uint8_t*>(&header), sizeof(NXL_FIXED_HEADER), kek.data(), 32, header.dynamic.fixedHeaderHash);
    flushHeader();

    return true;
}

std::string File::getTenant() const
{
	std::string ownerid = std::string(header.fixed.fileInfo.ownerId);
	auto pos = ownerid.rfind('@');
	std::string tenant = ownerid.substr(pos + 1);
	return (pos == std::string::npos) ? std::string() : ownerid.substr(pos + 1);
}

uint32_t File::getFileFlags() const
{
    return header.fixed.fileInfo.flags;
}

uint32_t File::getFileAlignment() const
{
    return header.fixed.fileInfo.alignment;
}

uint32_t File::getAlgorithm() const
{
    return header.fixed.fileInfo.algorithm;
}

uint32_t File::getBlockSize() const
{
    return header.fixed.fileInfo.blockSize;
}

uint32_t File::getContentOffset() const
{
    return header.fixed.fileInfo.contentOffset;
}

uint32_t File::getKeyMode() const
{
    return ((header.fixed.keys.modeFlags & 0xFF000000) >> 24);
}

uint32_t File::getKeyFlags() const
{
    return header.fixed.keys.modeFlags & 0x00FFFFFF;
}

std::vector<uint8_t> File::getIvSeed() const
{
    return std::vector<uint8_t>(header.fixed.keys.ivSeed, header.fixed.keys.ivSeed + 16);
}

std::vector<uint8_t> File::getAgreement0() const
{
    std::vector<uint8_t> agreement(header.fixed.keys.publicKey1, header.fixed.keys.publicKey1 + 256);
    if(0 == memcmp(kZeroData.data(), agreement.data(), 256))
        agreement.clear();
    return agreement;
}

std::vector<uint8_t> File::getAgreement1() const
{
    std::vector<uint8_t> agreement(header.fixed.keys.publicKey2, header.fixed.keys.publicKey2 + 256);
    if(0 == memcmp(kZeroData.data(), agreement.data(), 256))
        agreement.clear();
    return agreement;
}

uint64_t File::getContentLength() const
{
    return header.dynamic.contentLength;
}

uint32_t File::read(uint64_t offset, uint8_t* buf, uint32_t bytesToRead)
{
    if(cek.empty()) {
        ferror = FeNoCek;
        return 0;
    }
	if (getBlockSize() == 0)
		return 0;
    if(0 != (offset%getBlockSize())) {
        ferror = FeInvalidAlignment;
        return 0;
    }
    if(0 != (bytesToRead%16)) {
        ferror = FeInvalidAlignment;
        return 0;
    }
    if(offset >= (uint64_t)header.dynamic.contentLength)
        return 0;
    fp.seekg(getContentOffset() + offset,  std::ios_base::beg);
    if (!fp.good()) {
        ferror = FeIoError;
        return 0;
    }

	if ((offset + bytesToRead) > (uint64_t)header.dynamic.contentLength) {//calculate how many blocks left
		int nblockleft = (int)(((uint64_t)header.dynamic.contentLength - offset - 1) /getBlockSize()) + 1;
		if (bytesToRead > nblockleft * getBlockSize())
			bytesToRead = nblockleft * getBlockSize();
	}


    fp.read((char*)buf, bytesToRead);
    if (!fp.good()) {
        ferror = FeIoError;
        return 0;
    }
    if (!CRYPT::AesDecrypt(cek.data(), (uint32_t)cek.size(), buf, bytesToRead, header.fixed.keys.ivSeed, offset, getBlockSize(), buf, bytesToRead, &bytesToRead)) {
        ferror = FeCryptoError;
        return 0;
    }
    if((offset + bytesToRead) > (uint64_t)header.dynamic.contentLength)
        bytesToRead = (uint32_t)(header.dynamic.contentLength - offset);
    return bytesToRead;
}

uint32_t File::write(uint64_t offset, const uint8_t* buf, uint32_t bytesToWrite, uint32_t bytesValid)
{
    if(cek.empty()) {
        ferror = FeNoCek;
        return 0;
    }
	if (getBlockSize() == 0)
		return 0;
    if(0 != (offset%getBlockSize())) {
        ferror = FeInvalidAlignment;
        return 0;
    }
    if(0 != (bytesToWrite%getBlockSize())) {
        ferror = FeInvalidAlignment;
        return 0;
    }
    if(offset > (uint64_t)header.dynamic.contentLength) {
        ferror = FeInvalidAlignment;
        return 0;
    }
    if(bytesValid > bytesToWrite) {
        ferror = FeInvalidParameter;
        return 0;
    }
    fp.seekg(getContentOffset() + offset,  std::ios_base::beg);
    if (!fp.good()) {
        ferror = FeIoError;
        return 0;
    }
    std::vector<uint8_t> cipher(buf, buf + bytesToWrite);
    if (!CRYPT::AesEncrypt(cek.data(), (uint32_t)cek.size(), cipher.data(), bytesToWrite, header.fixed.keys.ivSeed, offset, getBlockSize(), cipher.data(), bytesToWrite, &bytesToWrite)) {
        ferror = FeCryptoError;
        return 0;
    }
    fp.write((const char*)cipher.data(), bytesToWrite);
    if(!fp.good())
        return FeIoError;

    if((offset + bytesValid) > (uint64_t)header.dynamic.contentLength) {
        header.dynamic.contentLength = offset + bytesValid;
        updateContentSize();
    }

    return bytesToWrite;
}

FileError File::readSection(const std::string& name, std::vector<uint8_t>& data, bool verify)
{
    if(!fp.is_open())
        return FeIoError;
	if (verify)
	{
		if (kek.size() != 32)
			return FeInvalidToken;
	}
    const NXL_SECTION2* section = header.findSection(name);
    if(section == nullptr)
        return FeNoSectionNotFound;
    data.resize(section->dataSize, 0);
    fp.seekg(section->offset, fp.beg);
    fp.read((char*)data.data(), data.size());
    if(!fp.good())
        return FeIoError;

	if (verify)
	{
		if (!verifyChecksum(data.data(), (uint32_t)data.size(), kek.data(), (uint32_t)kek.size(), section->checksum))
			return FeChecksumMismatch;
	}

    return FeSuccess;
}

FileError File::writeSection(const std::string& name, const uint8_t* data, uint32_t size)
{
    if(!fp.is_open())
        return FeIoError;
    if(kek.size() != 32)
        return FeInvalidToken;
    NXL_SECTION2* section = header.findSection(name);
    if(section == nullptr)
        return FeNoSectionNotFound;
    if(size > section->size)
        return FeDataOverflow;
    std::vector<uint8_t> buf(section->size, 0);
    if(data && 0 != size)
        memcpy(buf.data(), data, size);
    else
        memcpy(buf.data(), "{}", 2);
    fp.seekg(section->offset, fp.beg);
    fp.write((const char*)buf.data(), buf.size());
    if(!fp.good())
        return FeIoError;
    section->dataSize = size;
    createChecksum(data ? data : reinterpret_cast<const uint8_t*>("{}"), size ? size : 2, kek.data(), (uint32_t)kek.size(), section->checksum);
    createChecksum(reinterpret_cast<const uint8_t*>(&header), sizeof(NXL_FIXED_HEADER), kek.data(), 32, header.dynamic.fixedHeaderHash);
    flushHeader();
    return FeSuccess;
}

FileError File::flushHeader()
{
    fp.seekg(0, fp.beg);
    fp.write((const char*)&header, sizeof(header));
    return FeSuccess;
}

FileError File::updateContentSize()
{
    fp.seekg(sizeof(NXL_FIXED_HEADER) + 32, fp.beg);
    fp.write((const char*)&header.dynamic.contentLength, sizeof(long long));
    return FeSuccess;
}

FileError File::readInfoSection(std::string& s)
{
    std::vector<uint8_t> buf;
    FileError e = readSection(NXL_SECTION_NAME_FILEINFO, buf);
    if (0 == e) {
        s = std::string(buf.begin(), buf.end());
    }
    return e;
}

FileError File::readPolicySection(std::string& s)
{
    std::vector<uint8_t> buf;
    FileError e = readSection(NXL_SECTION_NAME_FILEPOLICY, buf);
    if (0 == e) {
        s = std::string(buf.begin(), buf.end());
    }
    return e;
}

FileError File::readTagSection(std::string& s)
{
    std::vector<uint8_t> buf;
    FileError e = readSection(NXL_SECTION_NAME_FILETAG, buf);
    if (0 == e) {
        s = std::string(buf.begin(), buf.end());
    }
    return e;
}

FileError File::writeInfoSection(const std::string& s)
{
    return writeSection(NXL_SECTION_NAME_FILEINFO, (const uint8_t*)(s.empty() ? "{}" : s.data()), (uint32_t)(s.empty() ? 2 : s.length()));
}

FileError File::writePolicySection(const std::string& s)
{
    return writeSection(NXL_SECTION_NAME_FILEPOLICY, (const uint8_t*)(s.empty() ? "{}" : s.data()), (uint32_t)(s.empty() ? 2 : s.length()));
}

FileError File::writeTagSection(const std::string& s)
{
    return writeSection(NXL_SECTION_NAME_FILETAG, (const uint8_t*)(s.empty() ? "{}" : s.data()), (uint32_t)(s.empty() ? 2 : s.length()));
}

File::Attributes::Attributes()
{
}

File::Attributes::Attributes(const Attributes& rhs)
    : data(rhs.data)
{
}

File::Attributes::~Attributes()
{
}

void File::Attributes::clear()
{
    data.clear();
}

void File::Attributes::set(const std::string& key, const std::string& value)
{
    if(!key.empty())
        data[toLower<char>(key)] = value;
}

std::string File::Attributes::get(const std::string& key) const
{
    if(key.empty())
        return "";

    auto pos = data.find(toLower<char>(key));
    return (pos == data.end()) ? "" : (*pos).second;
}

void File::Attributes::remove(const std::string& key)
{
    if(!key.empty())
        data.erase(toLower<char>(key));
}

void File::Attributes::remove_if(const std::function<bool(const std::pair<std::string, std::string>& item)>& func)
{
    while (!data.empty())
    {
        auto it = std::find_if(data.begin(), data.end(), func);
        if(it == data.end())
            break;
        data.erase(it);
    }
}

std::string File::Attributes::serialize() const
{
    std::string s;

    try
    {
        nlohmann::json root = nlohmann::json::object();
        for (auto& item : data)
        {
            if (!item.first.empty())
            {
                root[item.first] = item.second;
            }
        }

        s = root.dump();
    }
    catch (...)
    {
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "File::Attributes::serialize Json error");
    }
    return s;
}

File::Attributes File::Attributes::deserialize(const std::string& s)
{
    File::Attributes attrs;
    if (s.empty())
    {
        return attrs;
    }

    try
    {
        nlohmann::json root = nlohmann::json::parse(s);
        if (!root.is_object())
        {
            return attrs;
        }

        for (auto& item : root.items())
        {
            std::string strKey = item.key();
            auto& value = item.value();
            if (value.is_string())
            {
                attrs.set(strKey, value.get<std::string>());
            }
        }
    }
    catch (...)
    {
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "File::Attributes::deserialize Json error");
    }

    return attrs;
}


File::AdhocPolicy::AdhocPolicy(const std::string& ownerId, const Expiration& expireTime, uint64_t issueTime)
    : ownerId(ownerId)
    , expireTime(expireTime)
    , issueTime(issueTime ? issueTime : RMCCORE::NXTime::nowTime())
{
}

File::AdhocPolicy::AdhocPolicy(const File::AdhocPolicy& rhs)
    : ownerId(rhs.ownerId)
    , rights(rhs.rights)
    , obligations(rhs.obligations)
    , issueTime(rhs.issueTime)
    , expireTime(rhs.expireTime)
{
}

File::AdhocPolicy::~AdhocPolicy()
{
}

uint64_t File::AdhocPolicy::getRights() const
{
    uint64_t result = 0;
    for (auto name : rights)
    {
        if(name == RIGHT_ACTION_VIEW)
            result |= BUILTIN_RIGHT_VIEW;
        if(name == RIGHT_ACTION_EDIT)
            result |= BUILTIN_RIGHT_EDIT;
        if(name == RIGHT_ACTION_PRINT)
            result |= BUILTIN_RIGHT_PRINT;
        if(name == RIGHT_ACTION_CLIPBOARD)
            result |= BUILTIN_RIGHT_CLIPBOARD;
        if(name == RIGHT_ACTION_SAVEAS)
            result |= BUILTIN_RIGHT_SAVEAS;
        if(name == RIGHT_ACTION_DECRYPT)
            result |= BUILTIN_RIGHT_DECRYPT;
        if(name == RIGHT_ACTION_SCREENCAP)
            result |= BUILTIN_RIGHT_SCREENCAP;
        if(name == RIGHT_ACTION_SEND)
            result |= BUILTIN_RIGHT_SEND;
        if(name == RIGHT_ACTION_CLASSIFY)
            result |= BUILTIN_RIGHT_CLASSIFY;
        if(name == RIGHT_ACTION_SHARE)
            result |= BUILTIN_RIGHT_SHARE;
        if(name == RIGHT_ACTION_DOWNLOAD)
            result |= BUILTIN_RIGHT_DOWNLOAD;
		//if (name == OBLIGATION_NAME_WATERMARK)
		//	result |= BUILTIN_OBRIGHT_WATERMARK;
    }
    return result;
}

void File::AdhocPolicy::setRights(uint64_t r)
{
    if(r & BUILTIN_RIGHT_VIEW)
        rights.insert(RIGHT_ACTION_VIEW);
    if(r & BUILTIN_RIGHT_EDIT)
        rights.insert(RIGHT_ACTION_EDIT);
    if(r & BUILTIN_RIGHT_PRINT)
        rights.insert(RIGHT_ACTION_PRINT);
    if(r & BUILTIN_RIGHT_CLIPBOARD)
        rights.insert(RIGHT_ACTION_CLIPBOARD);
    if(r & BUILTIN_RIGHT_SAVEAS)
        rights.insert(RIGHT_ACTION_SAVEAS);
    if(r & BUILTIN_RIGHT_DECRYPT)
        rights.insert(RIGHT_ACTION_DECRYPT);
    if(r & BUILTIN_RIGHT_SCREENCAP)
        rights.insert(RIGHT_ACTION_SCREENCAP);
    if(r & BUILTIN_RIGHT_SEND)
        rights.insert(RIGHT_ACTION_SEND);
    if(r & BUILTIN_RIGHT_CLASSIFY)
        rights.insert(RIGHT_ACTION_CLASSIFY);
    if(r & BUILTIN_RIGHT_SHARE)
        rights.insert(RIGHT_ACTION_SHARE);
    if(r & BUILTIN_RIGHT_DOWNLOAD)
        rights.insert(RIGHT_ACTION_DOWNLOAD);  
	//if (r & BUILTIN_OBRIGHT_WATERMARK)
	//	rights.insert(OBLIGATION_NAME_WATERMARK);
}

void File::AdhocPolicy::unsetRights(uint64_t r)
{
    if(r & BUILTIN_RIGHT_VIEW)
        rights.erase(RIGHT_ACTION_VIEW);
    if(r & BUILTIN_RIGHT_EDIT)
        rights.erase(RIGHT_ACTION_EDIT);
    if(r & BUILTIN_RIGHT_PRINT)
        rights.erase(RIGHT_ACTION_PRINT);
    if(r & BUILTIN_RIGHT_CLIPBOARD)
        rights.erase(RIGHT_ACTION_CLIPBOARD);
    if(r & BUILTIN_RIGHT_SAVEAS)
        rights.erase(RIGHT_ACTION_SAVEAS);
    if(r & BUILTIN_RIGHT_DECRYPT)
        rights.erase(RIGHT_ACTION_DECRYPT);
    if(r & BUILTIN_RIGHT_SCREENCAP)
        rights.erase(RIGHT_ACTION_SCREENCAP);
    if(r & BUILTIN_RIGHT_SEND)
        rights.erase(RIGHT_ACTION_SEND);
    if(r & BUILTIN_RIGHT_CLASSIFY)
        rights.erase(RIGHT_ACTION_CLASSIFY);
    if(r & BUILTIN_RIGHT_SHARE)
        rights.erase(RIGHT_ACTION_SHARE);
    if(r & BUILTIN_RIGHT_DOWNLOAD)
        rights.erase(RIGHT_ACTION_DOWNLOAD);
	//if (r & BUILTIN_OBRIGHT_WATERMARK)
	//	rights.erase(OBLIGATION_NAME_WATERMARK);
}

void File::AdhocPolicy::insertRight(const std::string& name)
{
    const std::string uName(toUpper<char>(name));
    if (uName == RIGHT_ACTION_VIEW
        || uName == RIGHT_ACTION_EDIT
        || uName == RIGHT_ACTION_PRINT
        || uName == RIGHT_ACTION_CLIPBOARD
        || uName == RIGHT_ACTION_SAVEAS
        || uName == RIGHT_ACTION_DECRYPT
        || uName == RIGHT_ACTION_SCREENCAP
        || uName == RIGHT_ACTION_SEND
        || uName == RIGHT_ACTION_CLASSIFY
        || uName == RIGHT_ACTION_SHARE
        || uName == RIGHT_ACTION_DOWNLOAD
		|| uName == OBLIGATION_NAME_WATERMARK)
    {
        rights.insert(uName);
    }
}

void File::AdhocPolicy::removeRight(const std::string& name)
{
    rights.erase(toUpper<char>(name));
}

bool File::AdhocPolicy::checkRight(const std::string& name)
{
    return (rights.end() != rights.find(toUpper<char>(name)));
}

std::string File::AdhocPolicy::getIssueTimeAsString() const
{
    if(0 == issueTime)
        return "1970-01-01T00:00:00Z";
    return RMCCORE::NXTime::timeToString(issueTime);
}

void File::AdhocPolicy::insertObligation(const Obligation& ob)
{
	removeObligation(ob.getName());
	obligations.push_back(ob);
}

void File::AdhocPolicy::removeObligation(const std::string& obName)
{
	for(Obligations::iterator it = obligations.begin(); it != obligations.end(); it++){
		std::string s((*it).getName());
		std::transform(s.begin(), s.end(), s.begin(), tolower);
		if (toUpper(obName).compare(s) == 0) {
			obligations.erase(it);
			break;
		}
	}
}

std::string File::AdhocPolicy::serialize() const
 {
     std::string s;
     if (empty())
     {
         return s;
     }

     try {
         nlohmann::json root = nlohmann::json::object();
         root["version"] = "1.0";
         root["issuer"] = ownerId;
         root["issueTime"] = getIssueTimeAsString();

         root["policies"] = nlohmann::json::array();
         nlohmann::json& policies = root["policies"];

         nlohmann::json policy = nlohmann::json::object();
         policy["id"] = 0;
         policy["name"] = "Ad-hoc";
         policy["action"] = 1;

         //rights
         policy["rights"] = rights;

         //conditions
         policy["conditions"] = nlohmann::json::object();
         nlohmann::json& condition = policy["conditions"];

         if (expireTime.getType() == Expiration::NeverExpire)
         { // fix 54170, for NeverExpire, still set the conditions part
             condition["subject"] = nlohmann::json::object();
             nlohmann::json& subject = condition["subject"];

             subject["type"] = 1;
             subject["operator"] = "=";
             subject["name"] = "application.is_associated_app";
             subject["value"] = true;
         }
         else
         {
             condition["subject"] = nlohmann::json::object();
             nlohmann::json& subject = condition["subject"];

             subject["type"] = 1;
             subject["operator"] = "=";
             subject["name"] = "application.is_associated_app";
             subject["value"] = true;

             condition["environment"] = nlohmann::json::object();
             nlohmann::json& envir = condition["environment"];
             if (expireTime.getType() == Expiration::RelativeExpire || expireTime.getType() == Expiration::AbsoluteExpire)
             {
                 envir["type"] = 1;
                 envir["operator"] = "<=";
                 envir["name"] = "environment.date";
                 envir["value"] = expireTime.getEndDate();
             }
             else if (expireTime.getType() == Expiration::RangeExpire)
             {
                 envir["type"] = 0;
                 envir["operator"] = "&&";
                 envir["expressions"] = nlohmann::json::array();
                 nlohmann::json& expr = envir["expressions"];

                 nlohmann::json enddate = nlohmann::json::object();
                 enddate["type"] = 1;
                 enddate["operator"] = "<=";
                 enddate["name"] = "environment.date";
                 enddate["value"] = expireTime.getEndDate();
                 expr.push_back(enddate);

                 nlohmann::json startdate = nlohmann::json::object();
                 startdate["type"] = 1;
                 startdate["operator"] = ">=";
                 startdate["name"] = "environment.date";
                 startdate["value"] = expireTime.getStartDate();
                 expr.push_back(startdate);
             }
         }

         //obligations
         policy["obligations"] = nlohmann::json::array();
         nlohmann::json& obs = policy["obligations"];
         for (const Obligation &s : obligations)
         {
             nlohmann::json obligation = nlohmann::json::object();
             obligation["name"] = s.getName();
             obligation["value"] = nlohmann::json::object();
             nlohmann::json& values = obligation["value"];
             values["text"] = s.getArg("text");

             obs.push_back(obligation);
         }

         policies.push_back(policy);
         s = root.dump();
     }
     catch (std::exception &e) {
         std::string strError = "File::AdhocPolicy::serialize, error : " + std::string(e.what());
         throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
     }
     catch (...) {
         // The JSON data is NOT correct
         throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "File::AdhocPolicy::serialize error");
     }

     return s;
}

File::AdhocPolicy File::AdhocPolicy::deserialize(const std::string& s)
{
    static const File::AdhocPolicy kNullPolicy("", Expiration(), 0);

    if (s.empty())
    return kNullPolicy;

    Expiration expireTime;
    try {
        nlohmann::json root = nlohmann::json::parse(s);
        if (!root.is_object() || (0 == root.size()))
        {
            return kNullPolicy;
        }

        std::string issuer = root.at("issuer").get<std::string>();
        if (issuer.empty())
            return kNullPolicy;

        std::string issueTime = root.at("issueTime").get<std::string>();
        if (issueTime.empty())
            return kNullPolicy;

        if (root.end() != root.find("expiry"))
        {
            nlohmann::json& expiry = root["expiry"];
            int option = expiry.at("option").get<int>();

            if (expiry.end() != expiry.find("startDate"))
            {
                uint64_t startData = expiry["startDate"].get<uint64_t>();
                expireTime.setStartDate(startData);
            }

            if (expiry.end() != expiry.find("endDate"))
            {
                uint64_t endData = expiry["endDate"].get<uint64_t>();
                expireTime.setEndData(endData);
            }

            switch (option)
            {
            case Expiration::RelativeExpire:
            {
                expireTime.setType(Expiration::RelativeExpire);
            }
            break;
            case Expiration::AbsoluteExpire:
            {
                expireTime.setType(Expiration::AbsoluteExpire);
            }
            break;
            case Expiration::RangeExpire:
            {
                expireTime.setType(Expiration::RangeExpire);
            }
            break;
            case Expiration::NeverExpire:
            default:
                expireTime.setType(Expiration::NeverExpire);
                break;
            }
        }

        File::AdhocPolicy adhPolicy(issuer, expireTime, RMCCORE::NXTime::stringToTime(issueTime));
        if (root.end() != root.find("policies"))
        {
            nlohmann::json& policies = root["policies"];
            if (policies.is_array() && (policies.size() > 0) && policies[0].is_object())
            {
                nlohmann::json& rights = policies[0].at("rights");
                for (auto it = rights.begin(); rights.end() != it; it++)
                {
                    adhPolicy.insertRight(*it);
                }

                nlohmann::json& condition = policies[0].at("conditions");
                if (condition.end() != condition.find("environment"))
                {
                    nlohmann::json& environment = condition["environment"];
                    uint32_t type = environment.at("type").get<uint32_t>();
                    if (0 == type) //Expiration::RangeExpire
                    {
                        expireTime.setType(Expiration::RangeExpire);
                        nlohmann::json& expressions = environment.at("expressions");
                        for (auto it = expressions.begin(); expressions.end() != it; it++)
                        {
                            std::string op = (*it).at("operator").get<std::string>();
                            uint64_t date = (*it).at("value").get<uint64_t>();
                            if (0 == op.compare(">="))
                            {
                                expireTime.setStartDate(date);
                            }
                            else if (0 == op.compare("<="))
                            {
                                expireTime.setEndData(date);
                            }
                        }
                        adhPolicy.setExpireTime(expireTime);
                    }
                    else if (1 == type)
                    {
                        std::string op = environment.at("operator").get<std::string>();
                        if (op.compare("<=") == 0)
                            expireTime.setType(Expiration::AbsoluteExpire); // old is Expiration::RelativeExpire

                        uint64_t enddate = environment.at("value").get<uint64_t>();
                        expireTime.setEndData(enddate);
                        adhPolicy.setExpireTime(expireTime);
                    }
                }


                nlohmann::json& obligations = policies[0].at("obligations");
                for (auto it = obligations.begin(); obligations.end() != it; it++)
                {
                    std::string name = (*it).at("name").get<std::string>();
                    Obligation ob(name);

                    nlohmann::json& value = (*it).at("value");
                    if (value.end() != value.find("text"))
                    {
                        std::string text = value["text"].get<std::string>();
                        ob.setArg("text", text);
                    }

                    adhPolicy.insertObligation(ob);
                }
            }
        }

        return adhPolicy;
    }
    catch (std::exception &e) {
        std::string strError = "AdhocPolicy::deserialize Json error : " + std::string(e.what());
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "AdhocPolicy::deserialize Json error");
    }

    return kNullPolicy;
}

File::Tags::Tags()
{
}

File::Tags::Tags(const File::Tags& rhs)
    : data(rhs.data)
{
}

File::Tags::~Tags()
{
}

void File::Tags::clear()
{
    data.clear();
}

void File::Tags::set(const std::string& key, const std::string& value)
{
    data[toLower<char>(key)].insert(toLower<char>(value));
}

std::set<std::string> File::Tags::get(const std::string& key) const
{
    auto it = data.find(toLower<char>(key));
    return (it != data.end()) ? it->second : std::set<std::string>();
}

void File::Tags::remove(const std::string& key)
{
    data.erase(toLower<char>(key));
}

void File::Tags::remove(const std::string& key, const std::string& value)
{
    auto it = data.find(toLower<char>(key));
    if(it != data.end())
        it->second.erase(toLower<char>(value));
}

std::string File::Tags::serialize() const
{
    std::string s;
    try {
        nlohmann::json root = nlohmann::json::object();
        for (const auto &item : data)
        {
            if (!item.first.empty() && !item.second.empty())
            {
                root[item.first] = nlohmann::json::array();
                nlohmann::json& va = root[item.first];
                for (auto s : item.second)
                {
                    va.push_back(s);
                }
            }
        }

        s = root.dump();
    }
    catch (std::exception &e) {
        std::string strError = "File::Tags::serialize, error : " + std::string(e.what());
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "File::Tags::serialize error");
    }

    return s;
}

File::Tags File::Tags::deserialize(const std::string& s)
{
    File::Tags tags;
    if (s.empty())
    {
        return tags;
    }

    try
    {
        nlohmann::json root = nlohmann::json::parse(s);
        if (!root.is_object())
        {
            return tags;
        }

        for (auto& element : root.items())
        {
            std::string strKey = element.key();
            auto& values = element.value();
            if (!values.is_array())
            {
                continue;
            }

            for (auto& item : values)
            {
                if (item.is_string())
                {
                    tags.set(strKey, item.get<std::string>());
                }
            }
        }
    }
    catch (...)
    {
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "File::Tags::deserialize Json error");
    }

    return tags;
}





}
}
