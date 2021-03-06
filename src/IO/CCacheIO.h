#ifndef CCACHEIO_H
#define CCACHEIO_H

#include <map>
#include <vector>
#include <mutex>
#include <atomic>
#include <thread>
#include <condition_variable>

#include "CBlockIO.h"
#include "CEncrypt.h"

class CCacheIO;

class CBlock
{
    friend class CCacheIO;
public:
    CBlock(CCacheIO &_bio, CEncrypt &_enc, int _blockidx, int size);
    int8_t* GetBufRead();
    int8_t* GetBufReadWrite();
    int8_t* GetBufUnsafe();
    void ReleaseBuf();


private:
    int nextdirtyidx;
    int blockidx;
    std::mutex mutex;
    CCacheIO &cio;
    CEncrypt &enc;
    std::vector<int8_t> buf;
    uint32_t count;
};

using CBLOCKPTR = std::shared_ptr<CBlock>;

class CCacheIO
{
    friend class CBlock;

public:
    CCacheIO(const std::shared_ptr<CAbstractBlockIO> &bio, CEncrypt &_enc, bool _cryptcache);
    ~CCacheIO();

    void Read(int64_t ofs, int64_t size, int8_t *d);
    void Write(int64_t ofs, int64_t size, const int8_t *d);
    void Zero(int64_t ofs, int64_t size);

    CBLOCKPTR GetBlock(int blockidx, bool read=true);
    //CBLOCKPTR GetWriteBlock(int blockidx);
    void CacheBlocks(int blockidx, int n);

    int64_t GetFilesize();
    int64_t GetNDirty();
    int64_t GetNCachedBlocks();
    void Sync();

    int blocksize;

private:
    void Async_Sync();
    void BlockReadForce(int blockidx, int n);
    std::shared_ptr<CAbstractBlockIO> bio;

    CEncrypt &enc;
    std::map<int, CBLOCKPTR> cache;
    std::mutex cachemtx;
    std::atomic<int> ndirty;
    std::atomic<int> lastdirtyidx;

    std::thread syncthread;
    std::atomic<bool> terminatesyncthread;
    std::mutex async_sync_mutex;
    std::condition_variable async_sync_cond;

    bool cryptcache;
};

#endif
