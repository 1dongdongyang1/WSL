#pragma once

#include "Util.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

const std::string sep = "\r\n";
const std::string default_root = "./wwwroot";
const std::string home_page = "index.html";
const std::string html_404 = "wwwroot/404.html";

class HttpRequest
{
public:
    void parse()
    {
        // 1.从inbuffer中，拿到第一行，分隔符\r\n
        std::string line = Util::getOneLine(inbuffer, sep);
        if (line.empty()) return;
        // 2.从请求行中提取三个字段
        std::stringstream ss(line);
        ss >> method >> url >> httpversion;
        // std::cout << "line: " << line << std::endl;

        // 3.添加web默认路径
        path = default_root;
        path += url;
        if (path[path.size() - 1] == '/') path += home_page;

        // 4.获取path对应的资源后缀
        // ./wwwroot/index.html
        auto pos = path.rfind('.');
        if (pos == std::string::npos) suffix = ".html";
        else suffix = path.substr(pos);

        // 5.得到资源的大小
        struct stat st;
        int n = stat(path.c_str(), &st);
        if (n == 0) size = st.st_size;
        else size = -1;
    }
public:
    std::string inbuffer;

    std::string method;
    std::string url;
    std::string httpversion;
    std::string path;
    std::string suffix;
    int size;
};

class HttpResponse
{
public:
    std::string outbuffer;
};