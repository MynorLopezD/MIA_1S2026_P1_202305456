#ifndef DISK_MANAGER_HPP
#define DISK_MANAGER_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sys/stat.h>
#include <ctime>
#include <random>
#include "../structures/Structures.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

class DiskManager {
public:
    static bool createDisk(std::string path, int size, char fit);
    static bool deleteDisk(std::string path);
    static MBR readMBR(std::string path);
    static bool writeMBR(std::string path, MBR mbr);
    
    static bool createPartition(std::string diskPath, int size, char type, char fit, std::string name);
    static bool deletePartition(std::string diskPath, std::string name);
    
    static bool writeEBR(std::string path, int ebrPosition, EBR ebr);
    static EBR readEBR(std::string path, int ebrPosition);
    
    static int calculateStructuresCount(int partitionSize);
    
private:
    static std::string getDirectory(std::string path);
    static bool createDirectoryIfNotExists(std::string path);
};

#endif
