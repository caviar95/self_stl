#include <gtest/gtest.h>

#include "trans_order.hpp"



template <typename T>
void TestByteSwapIsInverse(T value) {
    T swapped = ByteSwap(value);
    T restored = ByteSwap(swapped);
    EXPECT_EQ(restored, value) << "ByteSwap not symmetric for value: 0x" << std::hex << value;
}


template <typename T>
void TestEndianConversions(T value) {
    auto order = GetSystemByteOrder();
    T big = ToBigEndian(value);
    T little = ToLittleEndian(value);

    if (order == ByteOrder::LittleEndian) {
        EXPECT_EQ(big, ByteSwap(value));
        EXPECT_EQ(little, value);
    } else {
        EXPECT_EQ(big, value);
        EXPECT_EQ(little, ByteSwap(value));
    }
}



struct Packet {
    uint16_t id;
    uint32_t length;
    uint64_t timestamp;
};

void ToBigEndian(Packet& pkt) {
    pkt.id        = ToBigEndian(pkt.id);
    pkt.length    = ToBigEndian(pkt.length);
    pkt.timestamp = ToBigEndian(pkt.timestamp);
}

void FromBigEndian(Packet& pkt) {
    pkt.id        = ToBigEndian(pkt.id);
    pkt.length    = ToBigEndian(pkt.length);
    pkt.timestamp = ToBigEndian(pkt.timestamp);
}

TEST(StructEndianTest, PacketRoundTrip) {
    Packet original{0x1234, 0xABCDEF01, 0x1122334455667788ULL};
    Packet network = original;

    ToBigEndian(network);
    FromBigEndian(network);

    EXPECT_EQ(network.id, original.id);
    EXPECT_EQ(network.length, original.length);
    EXPECT_EQ(network.timestamp, original.timestamp);
}










// template <typename T>
// void TestRoundTrip(T value) {
//     T swaped = ByteSwap(value);
//     T restored = ByteSwap(swaped);

//     if (swaped == restored) {
//         std::cout << "No need proc! value: 0x" << std::hex << value << std::endl;
//         return;
//     }

//     T result;

//     auto order = GetSystemByteOrder();

//     if (order == ByteOrder::BigEndian) {
//         result = ToBigEndian(ToLittleEndian(value));
//     } else {
//         result = ToLittleEndian(ToBigEndian(value));
//     }

//     EXPECT_EQ(result, value) << "RoundTrip failed for value: 0x" << std::hex << value;
// }

// TEST(ByteOrderTest, Uint16) {
//     TestRoundTrip<uint16_t>(0x1234);
//     TestRoundTrip<uint16_t>(0x1010);
// }

// TEST(ByteOrderTest, Uint32) {
//     TestRoundTrip<uint32_t>(0x12345678);
//     TestRoundTrip<uint32_t>(0x10101010);
// }

// TEST(ByteOrderTest, Uint64) {
//     TestRoundTrip<uint64_t>(0x1234567887654321);
//     TestRoundTrip<uint64_t>(0x1212121212121212);
// }

