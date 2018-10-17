//
// Created by cmm on 2018/10/17.
// Copyright © 2018 bingyan. All rights reserved.
//
// header file set
//
//


#ifndef MY_WIN_BIT_TORRENT_H
#define MY_WIN_BIT_TORRENT_H

#include "common.h"
#include <vector>
#include <time.h>
#include <list>
#include <string>
#include <iterator>

using namespace std;

class ISocketReactor;
class ITorrentTask;
class ITrackerManager;
class ITracker;
class IPeerAcceptor;
class ITaskStorage;
class IPeerManager;
class IPeerLink;
class ITimerCallback;
class IBitSet;
class IRateMeasure;
class IRateMeasureClient;


typedef struct PeerInfo
{
    string strLinkID;
    string strIPAddr;
    int nPort;
    int nConnFailedCount;
    IPeerLink *pPeerLink;
} PeerInfo;

typedef struct TimerInfo
{
    int nTimerID;
    int nInterval;
    long long llLastshotTick;
    ITimerCallback *pCallback;
    bool bOneShot;
    bool bRemove;
} TimerInfo;

typedef struct PeerPieceRequest
{
    unsigned int nIndex;
    unsigned int nOffset;
    unsigned int nLen;
} PeerPieceRequest;

typedef struct PieceRequest
{
    unsigned int nOffset;
    unsigned int nLen;
    bool bRequested;
    string strData;
} PieceRequest;

typedef struct FileInfo
{
    string strFilePath;
    long long llFileSize;
    long long llOffset;
} FileInfo;

typedef struct StorageFileInfo
{
    FileInfo stFileInfo;
    int nHandle;
    time_t mtime;
} StorageFileInfo;

typedef struct PieceCache
{
    string strData;
    long long llLastAccessTime;
} PieceCache;

typedef struct Range
{
    unsigned int nBegin;
    unsigned int nEnd;
} Range;

#pragma pack(push, 1)

typedef struct ConnectMsg
{
    long long llConnectionID;
    unsigned int nActionID;
    unsigned int nTransactionID;
} ConnectMsg;

typedef struct AnnounceMsg
{
    long long llConnectionID;
    unsigned int nActionID;
    unsigned int nTransactionID;
    char szInfoHash[20];
    char szPeerID[20];
    long long llDownloaded;
    long long llLeft;
    long long llUploaded;
    unsigned int nEvent;
    unsigned int nIP;
    unsigned int nKey;
    int nNumWant;
    unsigned short nPort;
    unsigned short nExtensions;
} AnnounceMsg;

class ISocket
{
public:
    virtual ~ISocket(){};
    virtual void SetReactor(ISocketReactor *pReactor) = 0;
    virtual ISocketReactor *GetReactor() = 0;
    virtual void CreateTCPSocket() = 0;
    virtual int GetHandle() = 0;
    virtual int GetHandleMask() = 0;
    virtual void SetHandleMask(int nHandleMask) = 0;
    virtual void RemoveHandleMask(int nHandleMask) = 0;
    virtual void SetNonBlock() = 0;
    virtual void Close() = 0;
    virtual int HandleRead() = 0;
    virtual int HandleWrite() = 0;
    virtual void HandleClose() = 0;
    virtual bool Bind(const char *pIpAddr, int nPort) = 0;
    virtual void Listen() = 0;
    virtual void Connect( const char* pHostName, int nPort) = 0;
    virtual void Attach(int nSocketFd) = 0;
    virtual int Accept(string &strIpAddr, int &nPort) = 0;
};

class ISocketReactor
{
public:
    virtual ~ISocketReactor() {};
    virtual bool AddSocket(ISocket *pSocket) = 0;
    virtual void RemoveSocket(ISocket *pSocket) = 0;
    virtual bool Startup() = 0;
    virtual void Update() = 0;
    virtual void Shutdown() = 0;
    virtual int AddTimer(ITimerCallback *pCallback, int nInterval, bool bOneShot) = 0;
    virtual void RemoveTimer(int nTimerID) = 0;
    virtual void UpdateTimerList() = 0;
};

class ITorrentFile
{
public:
    virtual ~ITorrentFile() {};
    virtual void Load(const char *pFilePath) = 0;
    virtual void SetTorrentTask(ITorrentTask *pTask) = 0;
    virtual ITorrentTask *GetTorrentTask() = 0;
    virtual const string &GetTorrentFilePath() = 0;
    virtual const string &GetMainAnnounce() = 0;
    virtual const vector<FileInfo> &GetFileList() = 0;
    virtual const vector<string> &GetAnnounceList() = 0;
    virtual int GetPieceLength() = 0;
    virtual int GetPieceCount() = 0;
    virtual const string &GetPiecesHash() = 0;
    virtual string GetPieceHash(int nPieceIndex) = 0;
    virtual const string &GetComment() = 0;
    virtual const string &GetCreatedBy() = 0;
    virtual const string &GetCreationDate() = 0;
    virtual const string &GetName() = 0;
    virtual bool IsMultiFiles() = 0;
    virtual const unsigned char *GetInfoHash() = 0;
    virtual long long GetTotalFileSize() = 0;
    virtual int GetFileCount() = 0;
};

class ITorrentTask
{
public:
    virtual ~ITorrentTask() {};
    virtual bool Startup() = 0;
    virtual void Shutdown() = 0;
    virtual const string &GetPeerID() = 0;
    virtual void LoadTorrentFile(const char *pTorrentFilePath) = 0;
    virtual ITorrentFile *GetTorrentFile() = 0;
    virtual ISocketReactor *GetSocketReactor() = 0;
    virtual IPeerAcceptor *GetAcceptor() = 0;
    virtual ITaskStorage *GetTaskStorage() = 0;
    virtual IPeerManager *GetPeerManager() = 0;
    virtual IRateMeasure *GetRateMeasure() = 0;
    virtual void AddDownloadCount(int nCount) = 0;
    virtual void AddUploadCount(int nCount) = 0;

    virtual long long GetDownloadCount() = 0;
    virtual long long GetUploadCount() = 0;
    virtual long long GetDownloadSpeed() = 0;
    virtual long long GetUploadSpeed() = 0;
    virtual int GetMaxPeerLink() = 0;
    virtual void SetMaxPeerLink(int nMaxPeerLink) = 0;
    virtual int GetMaxConnectingPeerLink() = 0;
    virtual string GetDstPath() = 0;
    virtual void SetDstPath(const char *pPath) = 0;
    virtual string GetTaskName() = 0;
    virtual long long GetCacheSize() = 0;
    virtual void SetCacheSize(long long llCacheSize) = 0;
    virtual int GetMaxUploadPeerLink() = 0;
    virtual void SetMaxUploadPeerLink(int nMaxUploadPeerLink) = 0;
    virtual long long CheckDownloadSpeed() = 0;
    virtual long long CheckUploadSpeed() = 0;
};

class IPeerAcceptor
{
public:
    virtual ~IPeerAcceptor() {};
    virtual bool Startup() = 0;
    virtual void Shutdown() = 0;
    virtual int GetPort() = 0;
    virtual void SetTorrentTask(ITorrentTask *pTask) = 0;
    virtual ITorrentTask *GetTorrentTask() = 0;
};

class ITimerCallback
{
public:
    virtual ~ITimerCallback(){};
    virtual void OnTimer(int nTimerID) = 0;
};

class IRateMeasureClient
{
public:
    IRateMeasureClient() {};
    virtual void BlockWrite(bool bBlock) = 0;
    virtual void BlockRead(bool bBlock) = 0;

    virtual void SetWritePriority(int nPriority) = 0;
    virtual int GetWritePriority() = 0;
    virtual void SetReadPriority(int nPriority) = 0;
    virtual int GetReadPriority() = 0;

    virtual bool CanWrite() = 0;
    virtual bool CanRead() = 0;

    virtual int DoWrite(long long llCount) = 0;
    virtual int DoRead(long long llCount) = 0;
    virtual ~IRateMeasureClient(){};
};

class IRateMeasure
{
public:
    virtual ~IRateMeasure() {};
    static const unsigned int nNoLimitedSpeed = 0xFFFFFFFF;
    virtual void AddClient(IRateMeasureClient *pClient) = 0;
    virtual void RemoveClient(IRateMeasureClient *pClient) = 0;
    virtual void Update() = 0;
    virtual void SetUploadSpeed(long long nSpeed) = 0;
    virtual void SetDownloadSpeed(long long nSpeed) = 0;
    virtual long long GetUploadSpeed() = 0;
    virtual long long GetDownloadSpeed() = 0;
};

class ITrackerManager
{
public:
    virtual ~ITrackerManager() {};
    virtual bool Startup() = 0;
    virtual void Shutdown() = 0;
    virtual void SetTorrentTask(ITorrentTask *pTask) = 0;
    virtual ITorrentTask *GetTorrentTask() = 0;
};

class ITracker
{
public:
    virtual ~ITracker() {};
    virtual void SetTrackerManager(ITrackerManager *pTrackerManager) = 0;
    virtual bool IsProtocolSupported(const char * pProtocol) = 0;
    virtual void SetURL(const char *pUrl) = 0;
    virtual void Update() = 0;
    virtual void Shutdown() = 0;
    virtual int GetSeedCount() = 0;
    virtual int GetPeerCount() = 0;
    virtual int GetInterval() = 0;
    virtual long long GetNextUpdateTick() = 0;
    virtual int GetTrackerState() = 0;
};

class ITaskStorage
{
public:
    virtual ~ITaskStorage() {};
    virtual bool Startup() = 0;
    virtual void Shutdown() = 0;
    virtual bool Finished() = 0;
    virtual IBitSet *GetBitSet() = 0;
    virtual int GetPieceLength(int nPieceIndex) = 0;
    virtual int GetPieceTask(IBitSet *pBitSet) = 0;
    virtual void WritePiece(int nPieceIndex, string &strData) = 0;
    virtual string ReadData(int nPieceIndex, long long llOffset, int nLen) = 0;
    virtual string ReadPiece(int nPieceIndex) = 0;
    virtual void SaveToReadCache(int nPieceIndex, string &strData) = 0;
    virtual float GetFinishedPercent() = 0;
    virtual long long GetLeftCount() = 0;
    virtual void SetTorrentTask(ITorrentTask *pTorrentTask) = 0;
    virtual ITorrentTask *GetTorrentTask() = 0;
    virtual string GetBitField() = 0;
    virtual void AbandonPieceTask(int nPieceIndex) = 0;
};

class IPeerManager
{
public:
    virtual ~IPeerManager(){};
    virtual bool Startup() = 0;
    virtual void Shutdown() = 0;
    virtual void AddPeerInfo(const char *pIpAddr, int nPort) = 0;
    virtual bool AddAcceptedPeer(int nHandle, const char *pIpAddr, int nPort) = 0;
    virtual void SetTorrentTask(ITorrentTask *pTask) = 0;
    virtual ITorrentTask *GetTorrentTask() = 0;
    virtual void OnDownloadComplete() = 0;
    virtual void BroadcastHave(int nPieceIndex) = 0;
    virtual int GetConnectedPeerCount() = 0;

};

class IPeerLink
{
public:
    virtual ~IPeerLink() {};
    virtual void SetPeerManager(IPeerManager *pManager) = 0;
    virtual int GetPeerState() = 0;
    virtual void Connect(const char *IpAddr, int nPort) = 0;
    virtual void CloseLink() = 0;
    virtual bool ShouldClose() = 0;
    virtual void ComputeSpeed() = 0;
    virtual bool IsAccepted() = 0;
    virtual void NotifyHavePiece(int nPieceIndex) = 0;
    virtual void Attach(int nHandle, const char *IpAddr, int nPort, IPeerManager *pManager) = 0;
    virtual unsigned int GetDownloadCount() = 0;
    virtual unsigned int GetUploadCount() = 0;
    virtual void OnDownloadComplete() = 0;
    virtual bool PeerChoked() = 0;
    virtual bool PeerInterested() = 0;
    virtual void ChokePeer(bool bChoke) = 0;
    virtual void SetUploadSpeed(unsigned int nSpeed) = 0;
    virtual void SetDownloadSpeed(unsigned int nSpeed) = 0;
    virtual unsigned int GetUploadSpeed() = 0;
    virtual unsigned int GetDownloadSpeed() = 0;
    virtual bool NeedRemoteData() = 0;
    virtual long long GetLastUnchokeTime() = 0;
    virtual bool IsEmpty() = 0;
};

class IBitSet
{
public:
    virtual ~IBitSet() {};
    virtual void Alloc(int nSize) = 0;
    virtual void Alloc(string &strBit, int nSize) = 0;
    virtual bool IsSet(int nIndex) = 0;
    virtual void Set(int nIndex, bool bSet) = 0;
    virtual bool IsAllSet() = 0;
    virtual bool IsEmpty() = 0;
    virtual int GetSetCount() = 0;
    virtual string &GetBits() = 0;
    virtual int GetSize() = 0;

};

#endif
