#include "disk/DiskManager.hpp"

bool DiskManager::createDisk(std::string path, int size, char fit) {
    #ifdef _WIN32
    for (char& c : path) {
        if (c == '/') c = '\\';
    }
    #endif
    std::cerr << "DEBUG: createDisk called with path='" << path << "', size=" << size << ", fit=" << fit << std::endl;
    std::string dir = getDirectory(path);
    std::cerr << "DEBUG: getDirectory returned dir='" << dir << "'" << std::endl;
    if (!createDirectoryIfNotExists(dir)) {
        std::cerr << "DEBUG: createDirectoryIfNotExists failed for dir='" << dir << "'" << std::endl;
        return false;
    }
    std::cerr << "DEBUG: directory exists or created" << std::endl;
    
    std::ofstream disk(path, std::ios::binary | std::ios::out);
    if (!disk.is_open()) {
        std::cerr << "Error: No se pudo crear el disco en " << path << std::endl;
        return false;
    }
    std::cerr << "DEBUG: disk file opened successfully" << std::endl;
    
    char buffer[1024];
    memset(buffer, 0, 1024);
    
    int fullChunks = size / 1024;
    int remainingBytes = size % 1024;
    
    for (int i = 0; i < fullChunks; i++) {
        disk.write(buffer, 1024);
    }
    
    if (remainingBytes > 0) {
        disk.write(buffer, remainingBytes);
    }
    
    disk.close();
    
    MBR mbr;
    memset(&mbr, 0, sizeof(MBR));
    mbr.mbr_tamano = size;
    mbr.mbr_fecha_creacion = time(nullptr);
    mbr.dsk_fit = fit;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 999999);
    mbr.mbr_dsk_signature = dis(gen);
    
    return writeMBR(path, mbr);
}

bool DiskManager::deleteDisk(std::string path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: El disco no existe en " << path << std::endl;
        return false;
    }
    file.close();
    
    if (remove(path.c_str()) != 0) {
        std::cerr << "Error: No se pudo eliminar el disco" << std::endl;
        return false;
    }
    
    return true;
}

MBR DiskManager::readMBR(std::string path) {
    MBR mbr;
    memset(&mbr, 0, sizeof(MBR));
    std::ifstream disk(path, std::ios::binary | std::ios::in);
    
    if (!disk.is_open()) {
        std::cerr << "Error: No se pudo abrir el disco " << path << std::endl;
        return mbr;
    }
    
    disk.read(reinterpret_cast<char*>(&mbr), sizeof(MBR));
    disk.close();
    
    return mbr;
}

bool DiskManager::writeMBR(std::string path, MBR mbr) {
    std::fstream disk(path, std::ios::binary | std::ios::in | std::ios::out);
    
    if (!disk.is_open()) {
        std::cerr << "Error: No se pudo abrir el disco para escribir" << std::endl;
        return false;
    }
    
    disk.seekp(0, std::ios::beg);
    disk.write(reinterpret_cast<const char*>(&mbr), sizeof(MBR));
    disk.close();
    
    return true;
}

EBR DiskManager::readEBR(std::string path, int ebrPosition) {
    EBR ebr;
    std::ifstream disk(path, std::ios::binary | std::ios::in);
    
    if (!disk.is_open()) {
        std::cerr << "Error: No se pudo abrir el disco para leer EBR" << std::endl;
        return ebr;
    }
    
    disk.seekg(ebrPosition, std::ios::beg);
    disk.read(reinterpret_cast<char*>(&ebr), sizeof(EBR));
    disk.close();
    
    return ebr;
}

bool DiskManager::writeEBR(std::string path, int ebrPosition, EBR ebr) {
    std::fstream disk(path, std::ios::binary | std::ios::in | std::ios::out);
    
    if (!disk.is_open()) {
        std::cerr << "Error: No se pudo abrir el disco para escribir EBR" << std::endl;
        return false;
    }
    
    disk.seekp(ebrPosition, std::ios::beg);
    disk.write(reinterpret_cast<const char*>(&ebr), sizeof(EBR));
    disk.close();
    
    return true;
}

bool DiskManager::createPartition(std::string diskPath, int size, char type, char fit, std::string name) {
    MBR mbr = readMBR(diskPath);
    
    std::cerr << "DEBUG: readMBR - mbr_tamano=" << mbr.mbr_tamano << std::endl;
    
    int primaryCount = 0;
    int extendedCount = 0;
    for (int i = 0; i < PARTITION_COUNT; i++) {
        std::cerr << "DEBUG: partition " << i << " part_s=" << mbr.mbr_partitions[i].part_s 
                  << " part_type=" << mbr.mbr_partitions[i].part_type << std::endl;
        if (mbr.mbr_partitions[i].part_s > 0) {
            if (mbr.mbr_partitions[i].part_type == 'P') primaryCount++;
            if (mbr.mbr_partitions[i].part_type == 'E') extendedCount++;
        }
    }
    
    std::cerr << "DEBUG: primaryCount=" << primaryCount << " extendedCount=" << extendedCount << std::endl;
    
    if (type == 'P' && primaryCount >= 4) {
        std::cerr << "Error: Maximo 4 particiones primarias" << std::endl;
        return false;
    }
    
    if (type == 'E' && extendedCount >= 1) {
        std::cerr << "Error: Solo puede haber una particion extendida" << std::endl;
        return false;
    }
    
    if (type == 'L' && extendedCount == 0) {
        std::cerr << "Error: Debe existir una particion extendida para crear logicas" << std::endl;
        return false;
    }
    
    int partitionIndex = -1;
    for (int i = 0; i < PARTITION_COUNT; i++) {
        if (mbr.mbr_partitions[i].part_s == 0) {
            partitionIndex = i;
            break;
        }
    }
    
    if (partitionIndex == -1) {
        std::cerr << "Error: No hay espacio disponible para nuevas particiones" << std::endl;
        return false;
    }
    
    int startByte = sizeof(MBR);
    for (int i = 0; i < PARTITION_COUNT; i++) {
        if (mbr.mbr_partitions[i].part_s > 0) {
            startByte = std::max(startByte, mbr.mbr_partitions[i].part_start + mbr.mbr_partitions[i].part_s);
        }
    }
    
    mbr.mbr_partitions[partitionIndex].part_start = startByte;
    mbr.mbr_partitions[partitionIndex].part_s = size;
    mbr.mbr_partitions[partitionIndex].part_type = type;
    mbr.mbr_partitions[partitionIndex].part_fit = fit;
    mbr.mbr_partitions[partitionIndex].part_status = '0';
    mbr.mbr_partitions[partitionIndex].part_correlative = -1;
    
    strncpy(mbr.mbr_partitions[partitionIndex].part_name, name.c_str(), PARTITION_NAME_SIZE - 1);
    
    if (!writeMBR(diskPath, mbr)) {
        return false;
    }
    
    if (type == 'E') {
        EBR ebr;
        ebr.part_fit = fit;
        ebr.part_start = startByte + sizeof(EBR);
        ebr.part_s = size - sizeof(EBR);
        ebr.part_next = -1;
        ebr.part_mount = '0';
        memset(ebr.part_name, 0, PARTITION_NAME_SIZE);
        
        if (!writeEBR(diskPath, startByte, ebr)) {
            return false;
        }
    }
    
    return true;
}

bool DiskManager::deletePartition(std::string diskPath, std::string name) {
    MBR mbr = readMBR(diskPath);
    
    for (int i = 0; i < PARTITION_COUNT; i++) {
        std::string partitionName(mbr.mbr_partitions[i].part_name);
        if (partitionName == name) {
            mbr.mbr_partitions[i] = Partition();
            return writeMBR(diskPath, mbr);
        }
    }
    
    std::cerr << "Error: Particion no encontrada" << std::endl;
    return false;
}

int DiskManager::calculateStructuresCount(int partitionSize) {
    int inodeSize = sizeof(Inode);
    int blockSize = BLOCK_SIZE;
    
    int n = 1;
    while (true) {
        int total = inodeSize + n + 3 * n + n * inodeSize + 3 * n * blockSize;
        if (total > partitionSize) {
            return n - 1;
        }
        n++;
    }
}

std::string DiskManager::getDirectory(std::string path) {
    size_t lastSlash = path.find_last_of("/\\");
    if (lastSlash == std::string::npos) {
        return ".";
    }
    std::string dir = path.substr(0, lastSlash);
    if (dir.length() == 2 && dir[1] == ':') {
        return ".";
    }
    return dir;
}

bool DiskManager::createDirectoryIfNotExists(std::string path) {
    if (path == "." || path.empty()) {
        return true;
    }
    
    struct stat st;
    if (stat(path.c_str(), &st) == 0) {
        return S_ISDIR(st.st_mode);
    }
    
    std::string dir = getDirectory(path);
    if (!createDirectoryIfNotExists(dir)) {
        return false;
    }
    
    #ifdef _WIN32
    return CreateDirectory(path.c_str(), NULL) != 0;
    #else
    return mkdir(path.c_str(), 0755) == 0;
    #endif
}
