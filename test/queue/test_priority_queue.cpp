#include "priority_queue.hpp"
#include "gtest/gtest.h"

#include <ranges>

// using namespace std::ranges;

TEST(PriorityQueue, max_priority_queue)
{
    PriorityQueue<int> pq;

    std::vector<int> v{3, 2, 1, 5, 4};

    for (auto i : v) {
        pq.Push(i);
    }

    std::ranges::sort(v, std::greater<int>());
    for (auto i = 0; i < v.size(); i++) {
        EXPECT_EQ(pq.Top(), v[i]);
        pq.Pop();
    }
}

TEST(PriorityQueue, min_priority_queue)
{
    PriorityQueue<int, std::greater<int>> pq;

    std::vector<int> v{3, 2, 1, 5, 4};

    for (auto i : v) {
        pq.Push(i);
    }

    std::ranges::sort(v, std::less<int>());
    for (auto i = 0; i < v.size(); i++) {
        EXPECT_EQ(pq.Top(), v[i]);
        pq.Pop();
    }
}

struct Message {
    int priority;
    std::string content;

    Message(int p, const std::string& c) : priority(p), content(c) {}
};



TEST(PriorityQueue, max_priority_queue_with_custom_compare)
{
    struct CompareMessage {
        bool operator()(const Message& lhs, const Message& rhs) const {
            return lhs.priority < rhs.priority;
        }
    };

    std::vector<Message> v{
        Message(2, "test2"),
        Message(1, "test1"),
        Message(3, "test3")
    };

    PriorityQueue<Message, CompareMessage> pq;

    for (auto &msg : v) {
        pq.Push(msg);
    }

    // pq.Push({1, "test11"});


    std::ranges::sort(v, [](const Message& lhs, const Message& rhs) {
        return lhs.priority > rhs.priority;
    });
    
    for (auto i = 0; i < v.size(); i++) {
        EXPECT_EQ(pq.Top().priority, v[i].priority);
        EXPECT_EQ(pq.Top().content, v[i].content);
        pq.Pop();
    }
}

TEST(PriorityQueue, min_priority_queue_with_custom_compare)
{
    struct CompareMessage {
        bool operator()(const Message& lhs, const Message& rhs) const {
            return lhs.priority > rhs.priority;
        }
    };

    std::vector<Message> v{
        Message(2, "test2"),
        Message(1, "test1"),
        Message(3, "test3")
    };

    PriorityQueue<Message, CompareMessage> pq;

    for (auto &msg : v) {
        pq.Push(msg);
    }

    // pq.Push({1, "test11"});


    std::ranges::sort(v, [](const Message& lhs, const Message& rhs) {return lhs.priority < rhs.priority;});
    
    for (auto i = 0; i < v.size(); i++) {
        EXPECT_EQ(pq.Top().priority, v[i].priority);
        EXPECT_EQ(pq.Top().content, v[i].content);
        pq.Pop();
    }
}

// int main()
// {
//     test_max_priority_queue();
//     test_min_priority_queue();
//     test_max_priority_queue_with_custom_compare();
//     test_min_priority_queue_with_custom_compare();

//     return 0;
// }
