//
//  nxlfile.h
//  rmccore
//
//  Created by Gavin Ye on 10/20/17.
//  Copyright © 2017 NextLabs, Inc. All rights reserved.
//

#pragma once
#ifndef __RMCCORE_FORMAT_NXLFILE_H__
#define __RMCCORE_FORMAT_NXLFILE_H__

#include "rmccore/format/nxlfmt.h"
#include "nxlpolicy.h"
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <functional>
#include <unordered_map>

namespace RMCCORE {
namespace NXLFMT {

typedef enum {
    FeSuccess = 0,
    FeEmpty,
    FeIoError,
    FeCryptoError,
    FeIoEof,
    FeInvalidParameter,
    FeInvalidSize,
    FeInvalidHeaderSize,
    FeInvalidMagic,
    FeInvalidVersion,
    FeVersionTooOld,
    FeInvalidToken,
    FeInvalidAgreement,
    FeInvalidDuid,
    FeInvalidAlignment,
    FeInvalidAlgorithm,
    FeInvalidBlockSize,
    FeInvalidOwnerId,
    FeInvalidCryptIv,
    FeInvalidCipherCek,
    FeChecksumMismatch,
    FeNoAgreement,
    FeNoSectionNotFound,
    FeNoSectionFileInfo,
    FeNoSectionFilePolicy,
    FeNoSectionFileTag,
    FeNoKek,
    FeNoCek,
    FeDataOverflow,
    FeMax
} FileError;

class TokenId
{
public:
    TokenId()
        : level(0)
    {
    }
    TokenId(const std::vector<uint8_t>& i, const uint32_t l)
        : id(i)
        , level(l)
    {
    }
    TokenId(const TokenId& rhs)
        : id(rhs.id)
        , level(rhs.level)
    {
    }
    ~TokenId() {}
    TokenId& operator = (const TokenId& rhs)
    {
        if (this != &rhs) {
            id = rhs.id;
            level = rhs.level;
        }
        return *this;
    }

    inline bool empty() const {return id.empty();}
    inline void clear()
    {
        id.clear();
        level = 0;
    }

    inline const std::vector<uint8_t>& getId() const {return id;}
    inline uint32_t getLevel() const {return level;}

private:
    std::vector<uint8_t> id;
    uint32_t level;
};

class Token : public TokenId
{
public:
    Token()
        : TokenId()
    {
    }
    Token(const std::vector<uint8_t>& i, const uint32_t l, const std::vector<uint8_t>& k)
        : TokenId(i, l)
        , key(k)
    {
    }
    Token(const Token& rhs)
        : TokenId(rhs)
        , key(rhs.key)
    {
    }
    ~Token() {}
    Token& operator = (const Token& rhs)
    {
        if (this != &rhs) {
            TokenId::operator=(rhs);
            key = rhs.key;
        }
        return *this;
    }

    inline bool empty() const {return TokenId::empty();}
    inline void clear()
    {
        TokenId::clear();
        key.clear();
    }

    inline const std::vector<uint8_t>& getKey() const {return key;}

private:
    std::vector<uint8_t> key;
};

class Header : public NXL_HEADER2
{
public:
    Header();
    ~Header();

    FileError validate() const;
    FileError init(const Token& token,
                   const std::string& ownerId,
                   const std::vector<uint8_t>& agreement0,
                   const std::vector<uint8_t>& agreement1,
                   const std::string& info,
                   const std::string& policy,
                   const std::string& tags,
                   const std::vector<uint8_t>& recoveryKey);
    NXL_SECTION2* findSection(const std::string& name);

	std::vector<uint8_t> getAgreement0() const;
	std::vector<uint8_t> getAgreement1() const;
	std::vector<uint8_t> getIvSeed() const;
	std::string getOwnerId() const;
	uint64_t getContentLength() const;
	std::string getTenant() const;
	uint32_t getBlockSize() const;
	TokenId getTokenId() const;
	std::string getDuid() const;
};
    
class File
{
public:
    File();
    ~File();

    class Attributes
    {
    public:
        Attributes();
        Attributes(const Attributes& rhs);
        ~Attributes();

        bool empty() const { return data.empty(); }

        void clear();
        void set(const std::string& key, const std::string& value);
        std::string get(const std::string& key) const;
        void remove(const std::string& key);
        void remove_if(const std::function<bool(const std::pair<std::string, std::string>& item)>& func);

        std::string serialize() const;
        static Attributes deserialize(const std::string& s);

    private:
        std::unordered_map<std::string, std::string> data;
    };

    class AdhocPolicy
    {
    public:
    public:
        // ownerId - the document owner's id
        // expireTime - the time when this policy expire (a valid tinme_t value), or 0 means never expire
        // issueTime - the time when this policy issued (a valid time_t value), or 0 means current time
        AdhocPolicy(const std::string& ownerId, const Expiration& expireTime = Expiration(), uint64_t issueTime = 0);
        AdhocPolicy(const AdhocPolicy& rhs);
		AdhocPolicy() {};
        ~AdhocPolicy();

        bool empty() const { return rights.empty(); }
        const std::string& getOwnerId() const { return ownerId; }
		const Obligations& getObligations() const { return obligations; }
        uint64_t getIssueTime() const { return issueTime; }
		void setIssueTime(uint64_t itime) { issueTime = itime; }
		const Expiration& getExpireTime() const { return expireTime; }
        bool expired() const { return expireTime.expired(); }

        std::string getIssueTimeAsString() const;

        // Set expire time (zero or a valid time_t value)
        // The expTime must be zero or a valid time_t value which is greater than issueTime,
        // otherwise the call fails and return false.
        void setExpireTime(const Expiration& expTime) { expireTime = expTime; }
        // Get the seconds this policy still active
        int64_t getSecondsToLive() const { return expireTime.secondsToLive(); }

		// rights names are defined in file 'nxlrights.h'
		uint64_t getRights() const;
		const Obligations getObligations() { return obligations; }
        void setRights(uint64_t r);
        void unsetRights(uint64_t r);
        void insertRight(const std::string& name);
        void removeRight(const std::string& name);
        bool checkRight(const std::string& name);

        void insertObligation(const Obligation &ob);
        void removeObligation(const std::string & obname);

        // Object to string
        std::string serialize() const;
        // String to object
        static AdhocPolicy deserialize(const std::string& s);

    private:
        std::string ownerId;
        std::set<std::string> rights;
        Obligations obligations;
        uint64_t issueTime;
        Expiration expireTime;
    };

    class Tags
    {
    public:
        Tags();
        Tags(const Tags& rhs);
        ~Tags();

        bool empty() const { return data.empty(); }

        void clear();
        void set(const std::string& key, const std::string& value);
        std::set<std::string> get(const std::string& key) const;
        void remove(const std::string& key);
        void remove(const std::string& key, const std::string& value);

        std::string serialize() const;
        static Tags deserialize(const std::string& s);

    private:
        std::unordered_map<std::string, std::set<std::string>> data;
    };
    
    // Not copyable/movable
    File(const File& f) = delete;
    File(File&& f) = delete;

    inline bool good() const { return (FeSuccess == ferror); }
    inline bool opened() const { return fp.is_open(); }
    inline FileError getError() const { return ferror; }

    static bool validate(const std::string& file);
    bool create(const std::string& file,
                const Token& token,
                const std::string& ownerId,
                const std::vector<uint8_t>& agreement0,
                const std::vector<uint8_t>& agreement1,
                const std::string& info,
                const std::string& policy,
                const std::string& tags,
                const std::vector<uint8_t>& recoveryKey);
    
    bool open(const std::string& path, bool readOnly);
#ifdef _WIN32
    static bool validate(const std::wstring& file);
    bool create(const std::wstring& file,
                const Token& token,
                const std::string& ownerId,
                const std::vector<uint8_t>& agreement0,
                const std::vector<uint8_t>& agreement1,
                const std::string& info,
                const std::string& policy,
                const std::string& tags,
                const std::vector<uint8_t>& recoveryKey);
    bool open(const std::wstring& path, bool readOnly);
#endif
    void close();

    bool setTokenKey(const std::vector<uint8_t>& key);
    bool unpack(const std::string& plainFile, bool replaceExisting);
    bool changeToken(const Token& newToken);

    TokenId getTokenId() const;
    std::string getOwnerId() const;

    bool changeOwnerId(const std::string& ownerId);

    uint32_t getFileFlags() const;
    uint32_t getFileAlignment() const;
    uint32_t getAlgorithm() const;
    uint32_t getBlockSize() const;
    uint32_t getContentOffset() const;

    uint32_t getKeyMode() const;
    uint32_t getKeyFlags() const;
    std::vector<uint8_t> getIvSeed() const;
    std::vector<uint8_t> getAgreement0() const;
    std::vector<uint8_t> getAgreement1() const;

    uint64_t getContentLength() const;
	std::string getTenant() const;
    
    uint32_t read(uint64_t offset, uint8_t* buf, uint32_t bytesToRead);
    uint32_t write(uint64_t offset, const uint8_t* buf, uint32_t bytesToWrite, uint32_t bytesValid);
    
    FileError readInfoSection(std::string& s);
    FileError readPolicySection(std::string& s);
    FileError readTagSection(std::string& s);
    
    FileError writeInfoSection(const std::string& s);
    FileError writePolicySection(const std::string& s);
    FileError writeTagSection(const std::string& s);

	void SetHeader(Header _header) { header = _header; };
	std::vector<unsigned char> getEncryptKey() { return cek; };
	uint32_t getPolicyFromHeader(std::string& s);
	uint32_t getTagFromHeader(std::string& s);
    
protected:
    bool load();

    FileError readSection(const std::string& name, std::vector<uint8_t>& data, bool verify=false);
    FileError writeSection(const std::string& name, const uint8_t* data, uint32_t size);
    FileError flushHeader();
    FileError updateContentSize();
	uint32_t  getSection(const std::string& name, std::vector<uint8_t>& data);

private:
    std::fstream fp;
    bool readOnly;
    FileError ferror;
    Header header;
    std::vector<uint8_t> kek;
    std::vector<uint8_t> cek;
};
        
}
}

#endif /* __RMCCORE_FORMAT_NXLFILE_H__ */
