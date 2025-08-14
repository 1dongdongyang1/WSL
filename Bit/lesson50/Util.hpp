#pragma once

#include <iostream>
#include <string>
#include <fstream>

class Util
{
public:
    static std::string getOneLine(std::string& buffer, const std::string& sep)
    {
        auto pos = buffer.find(sep);
        if (pos == std::string::npos) return "";
        std::string sub = buffer.substr(0, pos);
        buffer.erase(0, sub.size() + sep.size());
        return sub;
    }

    static bool readFile(const std::string resource, std::string* out)
    {
        std::fstream in(resource, std::ios::binary | std::ios::in);
        if (!in.is_open()) return false;

        in.seekg(0, std::ios::end);
        std::streamsize size = in.tellg();
        in.seekg(0, std::ios::beg);

        out->resize(size);
        if (!in.read(out->data(), size)) return false;

        in.close();
        return true;
    }
};