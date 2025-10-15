#pragma once

#include <vector>
#include <string>
#include <fstream>

#include <muduo/base/Logging.h>

class FileUtil {
public:
    FileUtil(const std::string& filePath) : filePath_(filePath), fileStream_(filePath, std::ios::binary) {
        if (!fileStream_.is_open()) {
            LOG_ERROR << "Failed to open file: " << filePath_;
        }
    }

    ~FileUtil() {
        if (fileStream_.is_open()) {
            fileStream_.close();
        }
    }

    bool isValid() const {
        return fileStream_.is_open();
    }

    void resetDefaultFile(const std::string& filePath) {
        if (fileStream_.is_open()) {
            fileStream_.close();
        }
        filePath_ = filePath;
        fileStream_.open(filePath_, std::ios::binary);
        if (!fileStream_.is_open()) {
            LOG_ERROR << "Failed to open file: " << filePath_;
        }
    }

    uint64_t getFileSize() {
        if (!isValid()) {
            return 0;
        }
        fileStream_.seekg(0, std::ios::end);
        uint64_t size = fileStream_.tellg();
        fileStream_.seekg(0, std::ios::beg);
        return size;
    }

    void readFile(std::vector<char>& buffer) {
        if (!isValid()) {
            return;
        }
        fileStream_.seekg(0, std::ios::beg);
        buffer.resize(getFileSize());
        fileStream_.read(buffer.data(), buffer.size());
    }

private:
    std::string     filePath_;
    std::ifstream   fileStream_;
};