#ifndef CSIMPLEFS_H
#define CSIMPLEFS_H

#include <cstring>
#include <string>
#include <map>
#include <memory>
#include <vector>
#include <mutex>
#include <cstdint>

#include"../IO/CCacheIO.h"
#include"CFragment.h"

#include"../CFilesystem.h"

#include"CSimpleFSDirectory.h"

// ----------------------------------------------------------

class CSimpleFilesystem : public CFilesystem
{
    friend class CSimpleFSDirectory;
    friend class CSimpleFSInode;
    friend class CPrintCheckRepair;

public:
    explicit CSimpleFilesystem(const std::shared_ptr<CCacheIO> &_bio);
    ~CSimpleFilesystem();

    CInodePtr OpenNode(const CPath &path) override;
    CInodePtr OpenNode(int id) override;

    CDirectoryPtr OpenDir(const CPath &path) override;
    CDirectoryPtr OpenDir(int id) override;

    CInodePtr OpenFile(const CPath &path) override;
    CInodePtr OpenFile(int id) override;

    void Rename(const CPath &path, CDirectoryPtr newdir, const std::string &filename) override;
    void Unlink(const CPath &path) override;
    void StatFS(CStatFS *buf) override;

    void PrintInfo() override;
    void PrintFragments() override;
    void Check() override;

    void CreateFS();

    int64_t GetNInodes();

private:

    CSimpleFSInodePtr OpenNodeInternal(int id);
    CSimpleFSInodePtr OpenNodeInternal(const CPath &path);

    CSimpleFSDirectoryPtr OpenDirInternal(int id);

    int MakeDirectory(CSimpleFSDirectory& dir, const std::string& name);
    int MakeFile(CSimpleFSDirectory& dir, const std::string& name);

    int64_t Read(CSimpleFSInode &node, int8_t *d, int64_t ofs, int64_t size);
    void Write(CSimpleFSInode &node, const int8_t *d, int64_t ofs, int64_t size);
    void Truncate(CSimpleFSInode &node, int64_t size, bool dozero);

    void GrowNode(CSimpleFSInode &node, int64_t size);
    void ShrinkNode(CSimpleFSInode &node, int64_t size);

    int CreateNode(CSimpleFSDirectory &dir, const std::string &name, INODETYPE );
    INODETYPE GetType(int id);

    void MaybeRemove(CSimpleFSInode &node);

    std::shared_ptr<CCacheIO> bio;

    std::mutex inodescachemtx;

    CFragmentList fragmentlist;

    std::map<int32_t, CSimpleFSInodePtr > inodes;

    // Statistics
    std::atomic<int> nopendir;
    std::atomic<int> nopenfiles;
    std::atomic<int> ncreatedir;
    std::atomic<int> ncreatefiles;
    std::atomic<int> nread;
    std::atomic<int> nwritten;
    std::atomic<int> nrenamed;
    std::atomic<int> nremoved;
    std::atomic<int> nunlinked;
    std::atomic<int> ntruncated;
};

#endif
