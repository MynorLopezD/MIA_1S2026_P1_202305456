#ifndef STRUCTURES_HPP
#define STRUCTURES_HPP

#include <cstdint>
#include <ctime>
#include <string>
#include <vector>
#include <cstring>

#define PARTITION_COUNT 4
#define INODE_POINTERS 15
#define FOLDER_CONTENT_COUNT 4
#define POINTER_COUNT 16
#define BLOCK_SIZE 64
#define FILE_NAME_SIZE 12
#define PARTITION_NAME_SIZE 16
#define PARTITION_ID_SIZE 4
#define PERM_SIZE 3

#pragma pack(push, 1)

struct Partition {
    char part_status;
    char part_type;
    char part_fit;
    int part_start;
    int part_s;
    char part_name[PARTITION_NAME_SIZE];
    int part_correlative;
    char part_id[PARTITION_ID_SIZE];
    
    Partition() {
        part_status = '0';
        part_type = 'P';
        part_fit = 'W';
        part_start = 0;
        part_s = 0;
        part_correlative = -1;
        memset(part_name, 0, PARTITION_NAME_SIZE);
        memset(part_id, 0, PARTITION_ID_SIZE);
    }
};

struct MBR {
    int mbr_tamano;
    time_t mbr_fecha_creacion;
    int mbr_dsk_signature;
    char dsk_fit;
    Partition mbr_partitions[PARTITION_COUNT];
    
    MBR() {
        mbr_tamano = 0;
        mbr_fecha_creacion = 0;
        mbr_dsk_signature = 0;
        dsk_fit = 'F';
        for (int i = 0; i < PARTITION_COUNT; i++) {
            mbr_partitions[i] = Partition();
        }
    }
};

struct EBR {
    char part_mount;
    char part_fit;
    int part_start;
    int part_s;
    int part_next;
    char part_name[PARTITION_NAME_SIZE];
    
    EBR() {
        part_mount = '0';
        part_fit = 'W';
        part_start = 0;
        part_s = 0;
        part_next = -1;
        memset(part_name, 0, PARTITION_NAME_SIZE);
    }
};

struct Content {
    char b_name[FILE_NAME_SIZE];
    int b_inodo;
    
    Content() {
        memset(b_name, 0, FILE_NAME_SIZE);
        b_inodo = -1;
    }
};

struct FolderBlock {
    Content b_content[FOLDER_CONTENT_COUNT];
    
    FolderBlock() {
        for (int i = 0; i < FOLDER_CONTENT_COUNT; i++) {
            b_content[i] = Content();
        }
    }
};

struct FileBlock {
    char b_content[BLOCK_SIZE];
    
    FileBlock() {
        memset(b_content, 0, BLOCK_SIZE);
    }
};

struct PointerBlock {
    int b_pointers[POINTER_COUNT];
    
    PointerBlock() {
        for (int i = 0; i < POINTER_COUNT; i++) {
            b_pointers[i] = -1;
        }
    }
};

struct Inode {
    int i_uid;
    int i_gid;
    int i_s;
    time_t i_atime;
    time_t i_ctime;
    time_t i_mtime;
    int i_block[INODE_POINTERS];
    char i_type;
    char i_perm[PERM_SIZE];
    
    Inode() {
        i_uid = 0;
        i_gid = 0;
        i_s = 0;
        i_atime = 0;
        i_ctime = 0;
        i_mtime = 0;
        i_type = '0';
        memset(i_perm, 0, PERM_SIZE);
        for (int i = 0; i < INODE_POINTERS; i++) {
            i_block[i] = -1;
        }
    }
};

struct SuperBloque {
    int s_filesystem_type;
    int s_inodes_count;
    int s_blocks_count;
    int s_free_blocks_count;
    int s_free_inodes_count;
    time_t s_mtime;
    time_t s_umtime;
    int s_mnt_count;
    int s_magic;
    int s_inode_s;
    int s_block_s;
    int s_firts_ino;
    int s_first_blo;
    int s_bm_inode_start;
    int s_bm_block_start;
    int s_inode_start;
    int s_block_start;
    
    SuperBloque() {
        s_filesystem_type = 2;
        s_inodes_count = 0;
        s_blocks_count = 0;
        s_free_blocks_count = 0;
        s_free_inodes_count = 0;
        s_mtime = 0;
        s_umtime = 0;
        s_mnt_count = 0;
        s_magic = 0xEF53;
        s_inode_s = 0;
        s_block_s = 0;
        s_firts_ino = 0;
        s_first_blo = 0;
        s_bm_inode_start = 0;
        s_bm_block_start = 0;
        s_inode_start = 0;
        s_block_start = 0;
    }
};

struct MountedPartition {
    std::string id;
    std::string path;
    std::string name;
    int number;
    char fit;
    
    MountedPartition() {
        number = 0;
        fit = 'F';
    }
};

#pragma pack(pop)

#endif
