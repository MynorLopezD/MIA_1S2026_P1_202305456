#include "Mkfs.hpp"
#include "../disk/DiskManager.hpp"
#include "../disk/MountedPartitionManager.hpp"
#include "../structures/Structures.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

std::string Mkfs::execute(Command cmd) {
    if (cmd.params.find("ID") == cmd.params.end() && cmd.params.find("PATH") == cmd.params.end()) {
        return "Error: Parametros insuficientes para mkfs. Use -id=X o -path=X";
    }
    
    std::string id = "";
    std::string diskPath = "";
    std::string partName = "";
    
    if (cmd.params.find("ID") != cmd.params.end()) {
        id = cmd.params["ID"];
        auto* partition = MountedPartitionManager::getPartitionById(id);
        if (!partition) {
            return "Error: No se encontro la particion con id " + id;
        }
        diskPath = partition->path;
        partName = partition->name;
    } else {
        diskPath = cmd.params["PATH"];
        if (cmd.params.find("NAME") != cmd.params.end()) {
            partName = cmd.params["NAME"];
        }
    }
    
    MBR mbr = DiskManager::readMBR(diskPath);
    
    int partitionIndex = -1;
    int partitionStart = 0;
    int partitionSize = 0;
    
    for (int i = 0; i < PARTITION_COUNT; i++) {
        std::string pname(mbr.mbr_partitions[i].part_name);
        if (pname == partName && mbr.mbr_partitions[i].part_s > 0) {
            partitionIndex = i;
            partitionStart = mbr.mbr_partitions[i].part_start;
            partitionSize = mbr.mbr_partitions[i].part_s;
            break;
        }
    }
    
    if (partitionIndex == -1) {
        return "Error: No se encontro la particion " + partName;
    }
    
    std::cout << "DEBUG MKFS: Creating EXT2 filesystem on partition " << partName << std::endl;
    std::cout << "DEBUG MKFS: partition start=" << partitionStart << " size=" << partitionSize << std::endl;
    
    int n = DiskManager::calculateStructuresCount(partitionSize);
    std::cout << "DEBUG MKFS: n=" << n << " structures" << std::endl;
    
    int inodeCount = n;
    int blockCount = 3 * n;
    
    int sbStart = partitionStart;
    int bmInodeStart = sbStart + sizeof(SuperBloque);
    int bmBlockStart = bmInodeStart + inodeCount;
    int inodeStart = bmBlockStart + blockCount;
    int blockStart = inodeStart + inodeCount * sizeof(Inode);
    
    std::cout << "DEBUG MKFS: sbStart=" << sbStart << std::endl;
    std::cout << "DEBUG MKFS: bmInodeStart=" << bmInodeStart << std::endl;
    std::cout << "DEBUG MKFS: bmBlockStart=" << bmBlockStart << std::endl;
    std::cout << "DEBUG MKFS: inodeStart=" << inodeStart << std::endl;
    std::cout << "DEBUG MKFS: blockStart=" << blockStart << std::endl;
    
    SuperBloque sb;
    sb.s_filesystem_type = 2;
    sb.s_inodes_count = inodeCount;
    sb.s_blocks_count = blockCount;
    sb.s_free_blocks_count = blockCount - 1;
    sb.s_free_inodes_count = inodeCount - 1;
    sb.s_mtime = time(nullptr);
    sb.s_umtime = time(nullptr);
    sb.s_mnt_count = 1;
    sb.s_magic = 0xEF53;
    sb.s_inode_s = sizeof(Inode);
    sb.s_block_s = BLOCK_SIZE;
    sb.s_firts_ino = 1;
    sb.s_first_blo = 1;
    sb.s_bm_inode_start = bmInodeStart;
    sb.s_bm_block_start = bmBlockStart;
    sb.s_inode_start = inodeStart;
    sb.s_block_start = blockStart;
    
    if (!DiskManager::writeSuperBloque(diskPath, sbStart, sb)) {
        return "Error: No se pudo escribir el superbloque";
    }
    
    std::cout << "DEBUG MKFS: Superblock written successfully" << std::endl;
    
    std::vector<char> inodeBitmap(inodeCount, '0');
    inodeBitmap[0] = '1';
    
    if (!DiskManager::writeBitmap(diskPath, bmInodeStart, inodeBitmap)) {
        return "Error: No se pudo escribir el bitmap de inodos";
    }
    
    std::cout << "DEBUG MKFS: Inode bitmap written successfully" << std::endl;
    
    std::vector<char> blockBitmap(blockCount, '0');
    blockBitmap[0] = '1';
    
    if (!DiskManager::writeBitmap(diskPath, bmBlockStart, blockBitmap)) {
        return "Error: No se pudo escribir el bitmap de bloques";
    }
    
    std::cout << "DEBUG MKFS: Block bitmap written successfully" << std::endl;
    
    Inode rootInode;
    rootInode.i_uid = 0;
    rootInode.i_gid = 0;
    rootInode.i_s = sizeof(FolderBlock);
    rootInode.i_atime = time(nullptr);
    rootInode.i_ctime = time(nullptr);
    rootInode.i_mtime = time(nullptr);
    rootInode.i_type = '0';
    rootInode.i_perm[0] = 'r';
    rootInode.i_perm[1] = 'w';
    rootInode.i_perm[2] = 'x';
    rootInode.i_block[0] = 0;
    for (int i = 1; i < INODE_POINTERS; i++) {
        rootInode.i_block[i] = -1;
    }
    
    if (!DiskManager::writeInode(diskPath, inodeStart, 0, rootInode)) {
        return "Error: No se pudo escribir el inodo raiz";
    }
    
    std::cout << "DEBUG MKFS: Root inode written successfully" << std::endl;
    
    FolderBlock rootBlock;
    strncpy(rootBlock.b_content[0].b_name, ".", FILE_NAME_SIZE - 1);
    rootBlock.b_content[0].b_inodo = 0;
    strncpy(rootBlock.b_content[1].b_name, "..", FILE_NAME_SIZE - 1);
    rootBlock.b_content[1].b_inodo = 0;
    for (int i = 2; i < FOLDER_CONTENT_COUNT; i++) {
        rootBlock.b_content[i].b_inodo = -1;
    }
    
    if (!DiskManager::writeBlock(diskPath, blockStart, 0, &rootBlock, sizeof(FolderBlock))) {
        return "Error: No se pudo escribir el bloque de carpeta raiz";
    }
    
    std::cout << "DEBUG MKFS: Root folder block written successfully" << std::endl;
    
    mbr.mbr_partitions[partitionIndex].part_correlative = 1;
    std::string partId = "vd" + std::string(1, 'a' + 0) + std::to_string(1);
    strncpy(mbr.mbr_partitions[partitionIndex].part_id, partId.c_str(), PARTITION_ID_SIZE - 1);
    
    DiskManager::writeMBR(diskPath, mbr);
    
    std::cout << "DEBUG MKFS: Filesystem created successfully!" << std::endl;
    
    return "Filesystem EXT2 created successfully on partition " + partName + 
           " (n=" + std::to_string(n) + " inodos=" + std::to_string(inodeCount) + 
           " bloques=" + std::to_string(blockCount) + ")";
}
