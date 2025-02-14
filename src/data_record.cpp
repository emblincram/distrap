// data_record.cpp
#include "data_record.hpp"
#include <iomanip>

DataRecord::DataRecord() : id(0), values{0, 0, 0, 0} {
    timestamp = std::chrono::system_clock::now();
}

DataRecord::DataRecord(int id, float v1, float v2, float v3, float v4) : id(id), values{v1, v2, v3, v4} {
    timestamp = std::chrono::system_clock::now();
}

std::string DataRecord::serialize() const {
    std::ostringstream oss;
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(timestamp.time_since_epoch()).count();
    oss << time << " " << id << " " << values[0] << " " << values[1] << " " << values[2] << " " << values[3];
    return oss.str();
}

DataRecord DataRecord::deserialize(const std::string& data) {
    std::istringstream iss(data);
    long long time;
    int id;
    float v1, v2, v3, v4;
    
    if (iss >> time >> id >> v1 >> v2 >> v3 >> v4) {
        DataRecord record(id, v1, v2, v3, v4);
        record.timestamp = std::chrono::system_clock::time_point(std::chrono::milliseconds(time));
        return record;
    }
    
    throw std::runtime_error("Failed to deserialize DataRecord");
}

void DataRecord::print() const {
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(timestamp.time_since_epoch()).count();
    std::cout << "Timestamp: " << time << ", ID: " << id 
              << ", Values: [" << values[0] << ", " << values[1] << ", "
              << values[2] << ", " << values[3] << "]" << std::endl;
}
