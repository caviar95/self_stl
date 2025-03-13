#include "fixed_mem_pool_nvwa.hpp"

#include "gtest/gtest.h"

class Obj {
public:
    Obj() {}
private:
    char a[12];
    DECLARE_FIXED_MEM_POOL(Obj)
};


TEST(FixedMemPool, nvwa) {
    fixed_mem_pool<Obj>::initialize(4);
    Obj *p1 = new Obj();
    EXPECT_EQ(fixed_mem_pool<Obj>::deinitialize(), 1);
    Obj *p2 = new Obj();
    EXPECT_EQ(fixed_mem_pool<Obj>::deinitialize(), 2);
    Obj *p3 = new Obj();
    EXPECT_EQ(fixed_mem_pool<Obj>::deinitialize(), 3);
    Obj *p4 = new Obj();
    EXPECT_EQ(fixed_mem_pool<Obj>::deinitialize(), 4);
    EXPECT_THROW(new Obj(), std::bad_alloc);

    delete p1;
    delete p2;
    delete p3;
    delete p4;
    EXPECT_EQ(fixed_mem_pool<Obj>::deinitialize(), 0);
}