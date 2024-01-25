#include <arpa/inet.h>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


void error(const char* msg) {
    perror(msg);
    exit(1);
}

std::string readFile(const std::filesystem::path& json_path) {
    // read file contents into string

    std::ifstream data { json_path };
    std::string contents;
    char c;

    if(!data) {
        perror("!data");
    }

    while(!data.eof()) {
        data >> c;
        contents.push_back(c);
    }

    return contents;
}

void parseContents(
        const std::string& contents,
        std::map<std::string, std::string>* result) {

    // this parser realisation is not pretending to be universal
    // but works well enough for our test case

    for(auto it = result->begin(); it != result->end(); it++) {
        std::string value;

        std::string keySubstring = "\"" + (*it).first + "\"";

        std::string::size_type n = contents.find(keySubstring);
        std::string::size_type delimiterPosition = n + keySubstring.length();

        char delimiter = contents.at(delimiterPosition);

        if(delimiter != ':') {
            std::string errorString =
                    std::string("Incorrect json delimiter: ") + delimiter;
            error(errorString.data());
        }

        std::string::size_type currentPosition = delimiterPosition + 1;

        while(contents.at(currentPosition) != ','
              && contents.at(currentPosition) != '}'
              ) {

            if(contents.at(currentPosition) != '"') {
                value.push_back(contents.at(currentPosition));
            }

            currentPosition++;
        }

        (*result)[(*it).first] = value;
    }
}

int main() {
    // path to json file
    std::filesystem::path json_path { "./data.json" };

    // reading file content into string
    std::string contents = readFile(json_path);

    // preparing result data structure for parsing
    std::map<std::string, std::string> parseResult;
    parseResult["ip"] = "";
    parseResult["port"] = "";

    // parse string contents into map values
    parseContents(contents, &parseResult);

    sockaddr_in serverSocket;

    // setting socket port
    serverSocket.sin_family = AF_INET;

    // setting socket ip
    inet_pton(AF_INET, parseResult["ip"].data(), &serverSocket.sin_addr.s_addr);

    // setting socket port
    auto port = std::atoi(parseResult["port"].data());
    serverSocket.sin_port = htons(port);

    int backlog = 5;

    // create socket
    int socketListenFd = socket(AF_INET, SOCK_STREAM, 0);

    if(socketListenFd == -1) {
        error("Socket creation error");
    }

    // bind socket
    int bindResult = bind(socketListenFd, (sockaddr*)&serverSocket, sizeof(serverSocket));

    if(bindResult == -1) {
        error("Socket binding error");
    }

    // listen on socket
    listen(socketListenFd, backlog);

    // now port is opened in state LISTEN
    // check with 'netstat -tln' in Linux

    // awaiting for connection emulation
    int socketConnectFd = accept(socketListenFd, NULL, 0);

    // sending some data to clients ...

    // close opened file descriptors
    close(socketListenFd);
    close(socketConnectFd);

    return 0;
}
