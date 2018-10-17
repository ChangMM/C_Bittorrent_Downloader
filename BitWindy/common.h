#ifndef COMM_DEF_H
#define COMM_DEF_H

#include <sys/socket.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <dirent.h>
#include <errno.h>
#include <string>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>
#include "sha1.h"

using namespace std;


enum SocketMask
{
    NONE_MASK = 0x00,
    READ_MASK = 0x01,
    WRITE_MASK = 0x02
};

enum TrackerState
{
    TS_INIT,
    TS_CONNECTING,
    TS_ESTABLISHED,
    TS_REQUESTING,
    TS_ERROR,
    TS_OK
};

enum TrackerEvent
{
    TE_START,
    TE_STOP,
    TE_COMPLETE,
    TE_NONE
};

enum PeerState
{
    PS_INIT,
    PS_CONNECTING,
    PS_CONNECTFAILED,
    PS_ESTABLISHED,
    PS_CLOSED
};

static const int MAX_PRIORITY_LEVEL = 5;
static const int REQUEST_BLOCK_SIZE = 16 * 1024;
static const int RECV_BUFFER_SIZE = 8 * 1024;


static void HandleErrMsg(const char *pErrMsg, const char *pFileName, int nErrCode, int nLineNumber){
    if(pErrMsg == NULL){
        return;
    }
    char szErrBuff[1024];
    sprintf(szErrBuff, "%s File:%s, Error Code:%d, Line:%d.\n", pErrMsg, pFileName, nErrCode, nLineNumber);
    printf("%s", szErrBuff);
}

static long long StringToInt64(const char *pStr, int nLen){
    long long nResult = 0;
    for (int i = 0; i < nLen; i++)
        {
        nResult = nResult * 10 + static_cast<long long>(*(pStr + i) - '0');
        }

    return nResult;
}

static string URLEncode(const unsigned char *str, size_t len){
    static const char szUnreserved[] = "-_.!~*()ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    stringstream ret;
    ret << hex << setfill('0');

    for (size_t i = 0; i < len; ++i){
        if (count(szUnreserved, szUnreserved + sizeof(szUnreserved) - 1, *str) > 0){
            ret << *str;
        } else {
            ret << '%' << setw(2) << (int)(*str);
        }
        ++str;
    }
    return ret.str();
}

static bool CreateDir(string path){
    string strPath = path;
    string::size_type pos = strPath.rfind('/');
    if(pos == string::npos){
        return false;
    }
    if(pos != strPath.size()-1){
        strPath.erase(pos+1, strPath.size()-pos-1);
    }

    string dirPath;
    for(;;){
        std::string::size_type pos = strPath.find('/',1);
        if(pos == std::string::npos){
            break;
        }

        dirPath += strPath.substr(0, pos);
        DIR* dir = opendir(dirPath.c_str());
        if(dir == NULL){
            if(errno == ENOENT)
                {
                if(-1 == mkdir(dirPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
                    {
                    return false;
                    }
                }
            else
                {
                return false;
                }
            }

        if(dir != NULL)
            {
            closedir(dir);
            }

        strPath.erase(0, pos);
        }

    return true;
}

static string ShaString(char *pData, int nLen){
    unsigned char szBuff[20];
    sha1_block((unsigned char *)pData, nLen, szBuff);

    string strResult;
    strResult.append((const char *)szBuff, sizeof(szBuff));
    return strResult;
}

static unsigned int GetTickCount(){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (unsigned int)(tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

// this two methods have been implemented

////主机序转网络序
//static long long htonll(long long llNumber){
//    if (htons(1) == 1) {
//        return llNumber;
//    } else {
//        return (((unsigned long long )htonl((int)((llNumber << 32) >> 32))) << 32) | (unsigned int)htonl((int)(llNumber >> 32));
//    }
//}
//
////网络序转主机序
//static long long ntohll(long long llNumber){
//    if(htons(1) == 1){
//        return llNumber;
//    } else {
//        return (((unsigned long long )ntohl((int)((llNumber << 32) >> 32))) << 32) | (unsigned int)ntohl((int)(llNumber >> 32));
//    }
//}
#endif
