#pragma once
#include <atomic>


class LockFreeList {
    struct Node {
        int val;
        Node *next;
    };
    std::atomic<Node *> head = nullptr;

public:
    void push_front(int val) {
        Node *new_node = new Node{val, nullptr};

        Node* old_head = head.load();
        do {
            new_node->next = old_head;
        } while (!head.compare_exchange_weak(old_head, new_node, std::memory_order_seq_cst, std::memory_order_relaxed));
    }

    int pop_front() {
        Node* old_head = head.load();
        do {
            if (old_head == nullptr) return -1;
        } while (!head.compare_exchange_weak(old_head, old_head->next, std::memory_order_seq_cst, std::memory_order_relaxed));
        int val = old_head->val;
        delete old_head;
        return val;
    }
};