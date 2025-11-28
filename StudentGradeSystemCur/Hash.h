#pragma once
#include <string>

class Hash {
public:
    static std::string simpleHash(const std::string& str) {
        size_t hash = 5381;
        for (char c : str) {
            hash = ((hash << 5) + hash) + static_cast<unsigned char>(c);
        }
        return std::to_string(hash);
    }
};
