

#pragma once 

#include <iostream>
#include <sstream>
#include <chrono>
#include <string>
#include <vector>

class DataRecord {
public:
    DataRecord();
    DataRecord(int id, float v1, float v2, float v3, float v4);
    
    std::string serialize() const;
    static DataRecord deserialize(const std::string& data);
    
    void print() const;
    
private:
    std::chrono::system_clock::time_point timestamp;
    int id;
    float values[4];
};
