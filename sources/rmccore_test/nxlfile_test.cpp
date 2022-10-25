#include "stdafx.h"

#include "rmccore/crypto/sha.h"
#include "rmccore/format/nxlfile.h"
#include "rmccore/format/nxlrights.h"
#include <boost/test/unit_test.hpp>

#include <vector>
#include <ctime>

#include "common.h"

using namespace RMCCORE;
using namespace RMCCORE::CRYPT;

/*
Membership ID: m14@skydrm.com
Recipients: Gavin.Ye@nextlabs.com
Rights: except download
Id: 2E14D330C7B0D5B3F9191A468FB7BDB8
Key: B91CBBB729EB16D1F441A271C825AB7241517433A5D74398D1CDEB09B198E13A
ML: 0
*/
static const uint8_t kTokenId[16] = {
    0x2E, 0x14, 0xD3, 0x30, 0xC7, 0xB0, 0xD5, 0xB3, 0xF9, 0x19, 0x1A, 0x46, 0x8F, 0xB7, 0xBD, 0xB8
};
static const uint8_t kTokenValue[32] = {
    0xB9, 0x1C, 0xBB, 0xB7, 0x29, 0xEB, 0x16, 0xD1, 0xF4, 0x41, 0xA2, 0x71, 0xC8, 0x25, 0xAB, 0x72,
    0x41, 0x51, 0x74, 0x33, 0xA5, 0xD7, 0x43, 0x98, 0xD1, 0xCD, 0xEB, 0x09, 0xB1, 0x98, 0xE1, 0x3A
};
static const uint32_t kTokenLevel = 0;
static const std::string kOwnerId("m14@skydrm.com");
static const std::string kRecipient("Gavin.Ye@nextlabs.com");

const ExeDir<char> kExeDir;

BOOST_AUTO_TEST_SUITE(RmcCoreNxlFileTest)

BOOST_AUTO_TEST_CASE(TestValidate)
{
    const std::string nxlFile(kExeDir.getExeDir() + "\\RMSTokenTest.txt.nxl");
    BOOST_CHECK(NXLFMT::File::validate(nxlFile));
}

BOOST_AUTO_TEST_CASE(TestRead)
{
    const std::string nxlFile(kExeDir.getExeDir() + "\\RMSTokenTest.txt.nxl");
    const std::string txtFile(kExeDir.getExeDir() + "\\RMSTokenTest.txt");
    NXLFMT::File file;
    BOOST_REQUIRE(file.open(nxlFile, true));
    BOOST_CHECK(16 == file.getTokenId().getId().size());
    BOOST_CHECK(0 == memcmp(kTokenId, file.getTokenId().getId().data(), 16));
    BOOST_REQUIRE(file.setTokenKey(std::vector<uint8_t>(kTokenValue, kTokenValue+32)));

    std::ifstream ifs;
    ifs.open(txtFile);
    BOOST_REQUIRE(ifs.is_open());
    ifs.seekg(0, ifs.end);
    const uint64_t txtSize = (uint64_t)ifs.tellg();
    BOOST_CHECK(file.getContentLength() == txtSize);

    uint64_t contentLength = txtSize;
    uint64_t offset = 0;
    ifs.seekg(0, ifs.beg);
    while (contentLength) {
        std::vector<uint8_t> buf(NXL_BLOCK_SIZE, 0);
        std::vector<uint8_t> buf2(NXL_BLOCK_SIZE, 0);
        uint32_t bytesToRead = 0;
        if(contentLength >= NXL_BLOCK_SIZE) {
            bytesToRead = NXL_BLOCK_SIZE;
            BOOST_CHECK(bytesToRead == file.read(offset, buf.data(), bytesToRead));
            ifs.read((char*)buf2.data(), bytesToRead);
            BOOST_CHECK(ifs.good());
            BOOST_CHECK(0 == memcmp(buf.data(), buf2.data(), NXL_BLOCK_SIZE));
            offset += bytesToRead;
            contentLength -= bytesToRead;
        }
        else {
            bytesToRead = (uint32_t)(((contentLength + 15) / 16) * 16);
            BOOST_CHECK(contentLength == file.read(offset, buf.data(), bytesToRead));
            ifs.read((char*)buf2.data(), contentLength);
            BOOST_CHECK(ifs.good());
            BOOST_CHECK(0 == memcmp(buf.data(), buf2.data(), (size_t)contentLength));
            offset += bytesToRead;
            contentLength = 0;
        }
    }
}

BOOST_AUTO_TEST_CASE(TestCreate)
{
}

static bool fileCompare(const std::string& file1, const std::string& file2)
{
    std::ifstream ifs1;
    std::ifstream ifs2;
    ifs1.open(file1);
    BOOST_REQUIRE(ifs1.is_open());
    ifs1.seekg(0, ifs1.end);
    const uint64_t fileSize1 = (uint64_t)ifs1.tellg();

    ifs2.open(file2);
    BOOST_REQUIRE(ifs2.is_open());
    ifs2.seekg(0, ifs2.end);
    const uint64_t fileSize2 = (uint64_t)ifs2.tellg();

    BOOST_REQUIRE(fileSize1 == fileSize2);

    std::vector<uint8_t> buf1(NXL_BLOCK_SIZE, 0);
    std::vector<uint8_t> buf2(NXL_BLOCK_SIZE, 0);
    ifs1.seekg(0, ifs1.beg);
    ifs2.seekg(0, ifs2.beg);
    uint64_t bytesToCompare = fileSize1;
    while (bytesToCompare) {
        const uint32_t bytesToRead = (uint32_t)((bytesToCompare > NXL_BLOCK_SIZE) ? NXL_BLOCK_SIZE : bytesToCompare);
        ifs1.read((char*)buf1.data(), bytesToRead);
        BOOST_REQUIRE(ifs1.good());
        ifs2.read((char*)buf2.data(), bytesToRead);
        BOOST_REQUIRE(ifs1.good());
        BOOST_REQUIRE(0 == memcmp(buf1.data(), buf2.data(), bytesToRead));
        bytesToCompare -= bytesToRead;
    }

    return true;
}

BOOST_AUTO_TEST_CASE(TestUnpack)
{
    const std::string nxlFile(kExeDir.getExeDir() + "\\RMSTokenTest.txt.nxl");
    const std::string txtFile1(kExeDir.getExeDir() + "\\RMSTokenTest.txt");
    const std::string txtFile2(kExeDir.getExeDir() + "\\RMSTokenTest2.txt");
    NXLFMT::File file;
    BOOST_REQUIRE(file.open(nxlFile, true));
    BOOST_CHECK(16 == file.getTokenId().getId().size());
    BOOST_CHECK(0 == memcmp(kTokenId, file.getTokenId().getId().data(), 16));
    BOOST_REQUIRE(file.setTokenKey(std::vector<uint8_t>(kTokenValue, kTokenValue+32)));

    std::ofstream ofs;
    ofs.open(txtFile2, ofs.binary | ofs.trunc);
    BOOST_REQUIRE(ofs.is_open());

    uint64_t contentLength = file.getContentLength();
    uint64_t offset = 0;
    while (contentLength) {
        std::vector<uint8_t> buf(NXL_BLOCK_SIZE, 0);
        uint32_t bytesToRead = 0;
        if(contentLength >= NXL_BLOCK_SIZE) {
            bytesToRead = NXL_BLOCK_SIZE;
            BOOST_CHECK(bytesToRead == file.read(offset, buf.data(), bytesToRead));
            ofs.write((char*)buf.data(), bytesToRead);
            BOOST_CHECK(ofs.good());
            offset += bytesToRead;
            contentLength -= bytesToRead;
        }
        else {
            bytesToRead = (uint32_t)(((contentLength + 15) / 16) * 16);
            BOOST_CHECK(contentLength == file.read(offset, buf.data(), bytesToRead));
            ofs.write((char*)buf.data(), contentLength);
            BOOST_CHECK(ofs.good());
            offset += bytesToRead;
            contentLength = 0;
        }
    }
    ofs.close();

    fileCompare(txtFile1, txtFile2);
}

BOOST_AUTO_TEST_CASE(TestChangeToken)
{
}

BOOST_AUTO_TEST_CASE(TestFileAttributesObject)
{
    NXLFMT::File::Attributes attrs;
    attrs.set("fileExt", "doc");
    attrs.set("fileType", "Word Document");
    attrs.set("fileName", "foo.doc");

    const std::string& s = attrs.serialize();
    BOOST_REQUIRE(!s.empty());
    BOOST_REQUIRE("doc" == attrs.get("fileext"));
    BOOST_REQUIRE("Word Document" == attrs.get("filetype"));
    BOOST_REQUIRE("foo.doc" == attrs.get("fileName"));
}

BOOST_AUTO_TEST_CASE(TestReadFileAttributes)
{
    NXLFMT::File::Attributes attrs;

}

BOOST_AUTO_TEST_CASE(TestWriteFileAttributes)
{
}

BOOST_AUTO_TEST_CASE(TestAdhocPolicyObject)
{
    NXLFMT::File::AdhocPolicy policy("m139@skydrm.com");
	NXLFMT::Obligation ob(OBLIGATION_NAME_WATERMARK);

    policy.insertRight(RIGHT_ACTION_VIEW);
    policy.insertRight(RIGHT_ACTION_EDIT);
    policy.insertRight(RIGHT_ACTION_DOWNLOAD);
	ob.setArg("text", "This will be the new watermark text. ");

    const NXLFMT::Expiration expireTime(NXLFMT::Expiration::AbsoluteExpire, 0, std::time(nullptr) + 5);
    policy.setExpireTime(expireTime);
    BOOST_REQUIRE(!policy.expired());

    const std::string& s = policy.serialize();
    BOOST_REQUIRE(!s.empty());
    const std::string& sIssueTime = policy.getIssueTimeAsString();
    BOOST_REQUIRE(!sIssueTime.empty());
    BOOST_REQUIRE("m139@skydrm.com" == policy.getOwnerId());
    BOOST_REQUIRE(policy.checkRight(RIGHT_ACTION_VIEW));
    BOOST_REQUIRE(!policy.checkRight(RIGHT_ACTION_PRINT));

    Sleep(6000);
    BOOST_REQUIRE(policy.expired());
}

BOOST_AUTO_TEST_CASE(TestReadAdhocPolicy)
{
}

BOOST_AUTO_TEST_CASE(TestWriteAdhocPolicy)
{
}

BOOST_AUTO_TEST_CASE(TestFileTags)
{
    NXLFMT::File::Tags tags;

    tags.set("classes", "Math");
    tags.set("classes", "Literature");
    tags.set("classes", "History");

    const std::set<std::string>& values = tags.get("classes");
    BOOST_REQUIRE(3 == values.size());
}

BOOST_AUTO_TEST_CASE(TestReadFileTags)
{
}

BOOST_AUTO_TEST_CASE(TestWriteFileTags)
{
}

BOOST_AUTO_TEST_SUITE_END()
