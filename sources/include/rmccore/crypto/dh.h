//
//  dh.h
//  rmccore
//
//  Created by Gavin Ye on 10/19/17.
//  Copyright © 2017 NextLabs, Inc. All rights reserved.
//

#pragma once
#ifndef __RMCCORE_CRYPTO_DH_H__
#define __RMCCORE_CRYPTO_DH_H__

#include "rmccore/rmccore.h"

#include <vector>

namespace RMCCORE {
namespace CRYPT {

std::vector<uint8_t> loadDhPublicKeyFromCert(const std::string& der);

class DhKey
{
public:
    DhKey(const uint8_t* vp, const uint8_t* vg, uint32_t vsize);
    DhKey(const uint8_t* vp, const uint8_t* vg, const uint8_t vx, const uint8_t vy, uint32_t vsize);
    DhKey(const DhKey& rhs);
    virtual ~DhKey();

    DhKey& operator = (const DhKey& rhs);
    static DhKey loadFromPem(const std::string& s);

    inline bool empty() const { return y.empty(); }
    inline size_t getSize() const { return (8 * p.size()); }
    inline const std::vector<uint8_t>& getP() const { return p; }
    inline const std::vector<uint8_t>& getG() const { return g; }
    inline const std::vector<uint8_t>& getX() const { return x; }
    inline const std::vector<uint8_t>& getY() const { return y; }
    inline std::vector<uint8_t>& getP() { return p; }
    inline std::vector<uint8_t>& getG() { return g; }
    inline std::vector<uint8_t>& getX() { return x; }
    inline std::vector<uint8_t>& getPriKey() { return x; }
    inline std::vector<uint8_t>& getY() { return y; }
    inline std::vector<uint8_t>& getPubKey() { return y; }
    inline void clear()
    {
        p.clear();
        g.clear();
        x.clear();
        y.clear();
    }

    bool reset();
    std::string getPubkeyPem(bool withHeader = false) const;

private:
    DhKey() {}
    void init();

private:
    std::vector<uint8_t> p;
    std::vector<uint8_t> g;
    std::vector<uint8_t> x;
    std::vector<uint8_t> y;
};

class DhKey1024 : public DhKey
{
public:
    DhKey1024();
    DhKey1024(const uint8_t* vp, const uint8_t* vg);
    virtual ~DhKey1024();

    virtual uint32_t getSize() const { return 1024; }

private:
    DhKey1024(const DhKey1024& rhs) = delete;
};

class DhKey2048 : public DhKey
{
public:
    DhKey2048();
    DhKey2048(const uint8_t* vp, const uint8_t* vg);
    virtual ~DhKey2048();

    virtual uint32_t getSize() const { return 2048; }

private:
    DhKey2048(const DhKey2048& rhs) = delete;
};

}
}

#endif /* __RMCCORE_CRYPTO_DH_H__ */
