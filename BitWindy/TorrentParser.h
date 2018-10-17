//
//  Created by cmm on 2018/10/17.
//  Copyright © 2018 bingyan. All rights reserved.
//
//  parse .torrent file
//

#ifndef TORRENT_PARSER_H
#define TORRENT_PARSER_H

#include "common.h"
#include "MyBitTorrent.h"
#include "deelx.h"
#include "sha1.h"
#include <string.h>
#include <assert.h>
#include <vector>

class CTorrentParser {
public:
    CTorrentParser(void);
    ~CTorrentParser(void);
    int LoadTorrentFile(const char *pTorrentFile);
    static bool FindPattern(const char *pContent, const char *pPattern, int &nStart, int &nEnd);
    bool GetMainAnnounce(string &strAnnounce);
    bool GetAnnounceList(vector<string> &vecAnnounce);
    bool GetFileList(vector<FileInfo> &vecFileInfo);
    bool GetFile(string &strFileName);
    bool IsMultiFiles();
    bool GetPieceLength(int &nPieceLength);
    bool GetPiecesHash(string &strPieceHash);
    bool GetComment(string &strComment);
    bool GetCreatedBy(string &strCreatedBy);
    bool GetCreationDate(string &strCreationDate);
    bool GetName(string &strName);
    bool GetInfoHash(unsigned char szInfoHash[20]);
    static bool ParseTrackInfo(const char *pAnnounce, string &strTrackerURL, int &nPort, string &strPath );
private:
    char *m_lpContent;  // pointer of the content
    long long m_llFileSize;  // size of the file
};

CTorrentParser::CTorrentParser(void) : m_lpContent(NULL){}

CTorrentParser::~CTorrentParser(void) {
    if (m_lpContent){
        delete m_lpContent;
        m_lpContent = NULL;
    }
}

int CTorrentParser::LoadTorrentFile( const char *pTorrentFile ) {
    assert(pTorrentFile != NULL);

    // open the torrent file in 'rb' mode
    FILE *fp = fopen(pTorrentFile, "rb");
    if (fp == NULL) {
        HandleErrMsg("open the torrent file error", __FILE__, errno, __LINE__); // errno is global
        return -1;
    }

    // get filesize of the torrent file
    fseek(fp, 0, SEEK_END);
    m_llFileSize = ftell(fp);
    if (m_llFileSize == -1) {
        HandleErrMsg("get the filesize error", __FILE__, errno, __LINE__);
        return -1;
    }

    // create a buffer
    m_lpContent = new char [m_llFileSize + 1];
    if (m_lpContent == NULL) {
        HandleErrMsg("create buffer error", __FILE__, errno, __LINE__);
        return -1;
    }

    // read the file content to buffer
    fseek(fp, 0, SEEK_SET);  // the same as => rewind(fp);
    fread(m_lpContent, 1, m_llFileSize, fp);
    m_lpContent[m_llFileSize] = '\0';

    // close file
    fclose(fp);
    return 0;
}

bool CTorrentParser::FindPattern( const char *pContent, const char *pPattern, int &nStart, int &nEnd )
{
    CRegexpT<char> Regexp(pPattern);
    MatchResult clResult = Regexp.Match(pContent);
    bool bFind = false;

    if (clResult.IsMatched()){
        nStart = clResult.GetStart();
        nEnd = clResult.GetEnd();
        bFind = true;
    }
    return bFind;
}

// 8:announce
bool CTorrentParser::GetMainAnnounce( string &strAnnounce ){
    int nStart = 0;
    int nEnd = 0;
    bool bFind = false;
    if (FindPattern(m_lpContent, "8:announce", nStart, nEnd) == true){
        const char* pContent = m_lpContent + nEnd;
        if (FindPattern(pContent, "[1-9]+[0-9]{0,}:", nStart, nEnd) == true)
            {
            strAnnounce.assign(pContent + nEnd, atol(pContent + nStart));
            bFind = true;
            }
        }
    return bFind;
}

// 13:announce-list
bool CTorrentParser::GetAnnounceList( vector<string> &vecAnnounce ){
    int nStart = 0;
    int nEnd = 0;
    if (FindPattern(m_lpContent, "13:announce-list", nStart, nEnd) == true){
        const char *pContent = m_lpContent +nEnd;
        while(FindPattern(pContent, "l[1-9]+[0-9]{0,}:", nStart, nEnd) == true){
            string strAnnounce;
            long nAnnounceLen = atol(pContent + nStart + 1);
            strAnnounce.assign(pContent + nEnd, nAnnounceLen);
            vecAnnounce.push_back(strAnnounce);

            pContent += nEnd;
            pContent += nAnnounceLen;

            if (*pContent == 'e' && *(pContent + 1) == 'e'){
                break;
            }
        }
    }
    return vecAnnounce.size() > 0;
}

bool CTorrentParser::GetFileList( vector<FileInfo> &vecFileInfo ){
    int nStart = 0;
    int nEnd = 0;
    const char *pContent = m_lpContent;
    long long llOffset = 0;

    FileInfo stInfo;
    if (FindPattern(m_lpContent, "5:files", nStart, nEnd) == true){
        pContent = m_lpContent + nEnd;
        while(FindPattern(pContent, "6:length", nStart, nEnd) == true){
            pContent += nEnd;

            // Get the length of the file
            if (FindPattern(pContent, "i[1-9]+[0-9]{0,}e", nStart, nEnd) == true) {
                stInfo.llFileSize = StringToInt64(pContent + nStart + 1, nEnd - nStart - 2);
                stInfo.llOffset = llOffset;
                pContent += nEnd;
                llOffset += stInfo.llFileSize;
            }

            // Get the path of the file
            if (FindPattern(pContent, "4:path", nStart, nEnd) == true) {
                pContent += nEnd;
                if (FindPattern(pContent, "l[1-9]+[0-9]{0,}:", nStart, nEnd) == true) {
                    long nFileNameLen = atol(pContent + nStart + 1);
                    stInfo.strFilePath.assign(pContent + nEnd, nFileNameLen);
                    pContent += nEnd;
                    pContent += nFileNameLen;
                    vecFileInfo.push_back(stInfo);
                }
            }

            if (*pContent == 'e' && *(pContent + 1) == 'e' && *(pContent + 2) == 'e') {
                break;
            }
        }
    } else {
        if (GetFile(stInfo.strFilePath)) {
            // Get file length
            if (FindPattern(pContent, "6:length", nStart, nEnd) == true) {
                pContent += nEnd;

                // Get the length of the file
                if (FindPattern(pContent, "i[1-9]+[0-9]{0,}e", nStart, nEnd) == true) {
                    stInfo.llFileSize = StringToInt64(pContent + nStart + 1, nEnd - nStart - 2);
                    stInfo.llOffset = 0;
                    vecFileInfo.push_back(stInfo);
                }
            }
        }
    }
    return vecFileInfo.size() > 0;
}

bool CTorrentParser::GetFile( string &strFileName ){
    bool bFind = false;
    int nStart = 0;
    int nEnd = 0;

    const char *pContent = m_lpContent;
    if (FindPattern(pContent, "4:name", nStart, nEnd) == true){
        pContent += nEnd;
        if (FindPattern(pContent, "[1-9]+[0-9]{0,}:", nStart, nEnd) == true){
            strFileName.assign(pContent + nEnd, atol(pContent + nStart));
            bFind = true;
        }
    }
    return bFind;
}

bool CTorrentParser::IsMultiFiles(){
    int nStart = 0;
    int nEnd = 0;
    return FindPattern(m_lpContent, "5:files", nStart, nEnd);
}

bool CTorrentParser::GetPieceLength( int &nPieceLength ){
    int nStart = 0;
    int nEnd = 0;
    bool bFind = false;
    const char *pContent = m_lpContent;
    if (FindPattern(pContent, "12:piece\\s+length", nStart, nEnd) == true){
        pContent += nEnd;
        if (FindPattern(pContent, "i[1-9]+[0-9]{0,}e", nStart, nEnd) == true){
            nPieceLength = atol(pContent + nStart + 1);
            bFind = true;
        }
    }
    return bFind;
}

bool CTorrentParser::GetPiecesHash( string &strPieceHash ){
    int nStart = 0;
    int nEnd = 0;
    bool bFind = false;
    const char *pContent = m_lpContent;
    if (FindPattern(pContent, "6:pieces", nStart, nEnd) == true){
        pContent += nEnd;
        if (FindPattern(pContent, "[1-9]+[0-9]{0,}:", nStart, nEnd) == true){
            strPieceHash.assign(pContent + nEnd, atol(pContent + nStart));
            bFind = true;
        }
    }

    return bFind;
}

// 7:comment
bool CTorrentParser::GetComment( string &strComment ){
    int nStart = 0;
    int nEnd = 0;
    bool bFind = false;
    const char *pContent = m_lpContent;
    if (FindPattern(pContent, "7:comment", nStart, nEnd) == true){
        pContent += nEnd;
        if (FindPattern(pContent, "[1-9]+[0-9]{0,}:", nStart, nEnd) == true){
            strComment.assign(pContent + nEnd, atol(pContent + nStart));
            bFind = true;
        }
    }
    return bFind;
}

// 10:created by
bool CTorrentParser::GetCreatedBy( string &strCreatedBy ){
    int nStart = 0;
    int nEnd = 0;
    bool bFind = false;
    const char *pContent = m_lpContent;

    if (FindPattern(pContent, "10:created\\s+by", nStart, nEnd) == true){
        pContent += nEnd;
        if (FindPattern(pContent, "[1-9]+[0-9]{0,}:", nStart, nEnd) == true){
            strCreatedBy.assign(pContent + nEnd, atol(pContent + nStart));
            bFind = true;
        }
    }
    return bFind;
}

bool CTorrentParser::GetCreationDate( string &strCreationDate ){
    int nStart = 0;
    int nEnd = 0;
    bool bFind = false;
    const char *pContent = m_lpContent;

    if (FindPattern(pContent, "13:creation\\s+date", nStart, nEnd) == true){
        pContent += nEnd;
        if (FindPattern(pContent, "i[1-9]+[0-9]{0,}e", nStart, nEnd) == true){
            strCreationDate.assign(pContent + nStart + 1, nEnd - nStart - 2);
            bFind = true;
        }
    }
    return bFind;
}

bool CTorrentParser::GetName( string &strName ){
    int nStart = 0;
    int nEnd = 0;
    bool bFind = false;
    const char *pContent = m_lpContent;

    if (FindPattern(pContent, "4:name", nStart, nEnd) == true){
        pContent += nEnd;
        if (FindPattern(pContent, "[1-9]+[0-9]{0,}:", nStart, nEnd) == true){
            strName.assign(pContent + nEnd, atol(pContent + nStart));
            bFind = true;
        }
    }
    return bFind;
}

bool CTorrentParser::GetInfoHash( unsigned char szInfoHash[20] ){
    int nPushPop = 0;
    int i = 0;
    int nBegin = 0;
    int nEnd = 0;
    bool bFind = false;
    if (FindPattern(m_lpContent, "4:info", nBegin, nEnd) == true){
        nBegin = nBegin + 6;
        i = nBegin;
    } else {
        return false;
    }

    for (; i < m_llFileSize; ){
        if (m_lpContent[i] == 'd'){
            nPushPop++;
            i++;
        } else if(m_lpContent[i] == 'l') {
            nPushPop++;
            i++;
        } else if(m_lpContent[i] == 'i') {
            i++; //skip i
            if (i == m_llFileSize) {
                return false;
            }

            while(m_lpContent[i] != 'e') {
                if ( i + 1 == m_llFileSize) {
                    return false;
                } else {
                    i++;
                }
            }
            i++; //skip e
        } else if ((m_lpContent[i] >= '0') && (m_lpContent[i] <= '9')) {
            int number = 0;
            while((m_lpContent[i] >= '0') && (m_lpContent[i] <= '9')) {
                number = number * 10 + m_lpContent[i] - '0';
                i++;
            }
            i++;    //skip :
            i = i + number;
        } else if (m_lpContent[i] == 'e') {
            nPushPop--;
            if (nPushPop == 0) {
                nEnd = i;
                break;
            } else {
                i++;
            }
        } else {
            return false;
        }
    }

    //Generate info hash
    sha1_block((unsigned char *)(m_lpContent + nBegin), nEnd - nBegin + 1, szInfoHash);
    return true;
}

bool CTorrentParser::ParseTrackInfo( const char *pAnnounce, string &strTrackerURL, int &nPort, string &strPath ){
    int nStart = 0;
    int nEnd = 0;
    bool bFind = false;

    if (FindPattern(pAnnounce, "://", nStart, nEnd) == true){
        pAnnounce += nEnd;

        if (FindPattern(pAnnounce, ":", nStart, nEnd) == true){
            strTrackerURL.assign(pAnnounce, nStart);
            nPort = atol(pAnnounce + nEnd);
            if (FindPattern(pAnnounce, "/", nStart, nEnd) == true){
                strPath = pAnnounce + nStart;
            } else {
                strPath = "/";
            }
            bFind = true;
        } else {
            nPort = 80;
            if (FindPattern(pAnnounce, "/", nStart, nEnd) == true) {
                strTrackerURL.assign(pAnnounce, nStart);
                strPath = pAnnounce + nStart;
            } else {
                strTrackerURL = pAnnounce;
                strPath = "/";
            }
            bFind = true;
        }
    }

    return bFind;
}


#endif
