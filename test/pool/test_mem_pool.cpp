#include "mem_pool.hpp"

#include "gtest/gtest.h"

    // 模拟网络数据包结构
struct Packet {
    char data[128];  // 假设每个数据包固定128字节
    uint32_t id;
};

TEST(MemPool, Basic)
{
    // 创建内存池：支持100个Packet对象的分配
    MemoryPool pool(sizeof(Packet), 100);

    // 分配内存块并构造对象
    Packet* pkt1 = static_cast<Packet*>(pool.allocate());
    pkt1->id = 1;
    std::cout << "Allocated Packet ID: " << pkt1->id << std::endl;

    Packet* pkt2 = static_cast<Packet*>(pool.allocate());
    pkt2->id = 2;
    std::cout << "Allocated Packet ID: " << pkt2->id << std::endl;

    // 释放内存块
    pool.deallocate(pkt1);
    pool.deallocate(pkt2);

    // 再次分配（复用已释放的内存）
    Packet* pkt3 = static_cast<Packet*>(pool.allocate());
    pkt3->id = 3;
    std::cout << "Reused memory for Packet ID: " << pkt3->id << std::endl;
}