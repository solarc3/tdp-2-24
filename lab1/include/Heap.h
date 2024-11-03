#pragma once
#include "../include/TracyMacros.h"
#include "State.h"

class PairingHeap {
    public:
    // Node structure for the pairing heap
    struct Node {
        State *state;
        Node *leftChild;
        Node *nextSibling;
        Node *prev;

        Node(State *s)
            : state(s), leftChild(nullptr), nextSibling(nullptr),
              prev(nullptr) {}
    };

    // Constants from original heap
    static constexpr int INITIAL_CAPACITY = 4096;

    // Constructor and destructor
    PairingHeap();
    ~PairingHeap();

    // Main operations
    void push(State *state);
    State *pop();
    State *peek() const;
    void clear();

    // Getters
    bool empty() const;

    private:
    Node *root_;
    int size_;

    // Helper functions
    Node *merge(Node *h1, Node *h2);
    Node *mergePairs(Node *firstSibling);
    void deleteTree(Node *node);
    static bool hasHigherPriority(const State *a, const State *b) {
        return a->weight < b->weight;
    }
};
