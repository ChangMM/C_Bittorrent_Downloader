//
//  main.cpp
//  BitWindy
//
//  Created by cmm on 2018/10/16.
//  Copyright © 2018 bingyan. All rights reserved.
//

#include <iostream>
#include "TorrentParser.h"
using namespace std;

int main(int argc, const char * argv[]) {
    CTorrentParser *parser = new CTorrentParser();
    parser->LoadTorrentFile("/Users/cmm/Desktop/ubuntu-18.04.1-desktop-amd64.iso.torrent");
    string fileName;
    parser->GetName(fileName);
    cout<<fileName;
    return 0;
}
