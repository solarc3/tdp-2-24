#pragma once
#include "../include/TracyMacros.h"
#include "State.h"

class PairingHeap {
    public:
    struct Node {
        State *state;
        Node *leftChild;
        Node *nextSibling;
        Node *prev;
        Node(State *s);
    };

    static constexpr int INITIAL_CAPACITY = 4096;

    PairingHeap();
    ~PairingHeap();

    void push(State *state);
    State *pop();
    State *peek() const;
    void clear();
    bool empty() const;
    int size() const;

    private:
    Node *root_;
    int size_;

    Node *merge(Node *h1, Node *h2);
    Node *mergePairs(Node *firstSibling);
    void deleteTree(Node *node);
    static bool hasHigherPriority(const State *a, const State *b);
};
