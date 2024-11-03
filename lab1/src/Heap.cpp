#include "../include/Heap.h"

PairingHeap::PairingHeap() : root_(nullptr), size_(0) { TRACE_SCOPE; }

PairingHeap::~PairingHeap() {
    TRACE_SCOPE;
    clear();
}

void PairingHeap::push(State *state) {
    TRACE_SCOPE;
    Node *newNode = new Node(state);
    if (!root_) {
        root_ = newNode;
    } else {
        root_ = merge(root_, newNode);
    }
    size_++;
    TRACE_PLOT("PairingHeap size", static_cast<int64_t>(size_));
}

State *PairingHeap::pop() {
    TRACE_SCOPE;
    if (!root_) {
        return nullptr;
    }

    Node *oldRoot = root_;
    State *result = oldRoot->state;

    if (root_->leftChild) {
        root_ = mergePairs(root_->leftChild);
        if (root_) {
            root_->prev = nullptr;
        }
    } else {
        root_ = nullptr;
    }

    delete oldRoot;
    size_--;
    TRACE_PLOT("PairingHeap size", static_cast<int64_t>(size_));
    return result;
}

State *PairingHeap::peek() const {
    TRACE_SCOPE;
    return root_ ? root_->state : nullptr;
}

void PairingHeap::clear() {
    TRACE_SCOPE;
    deleteTree(root_);
    root_ = nullptr;
    size_ = 0;
    TRACE_PLOT("PairingHeap size", static_cast<int64_t>(0));
}

bool PairingHeap::empty() const {
    TRACE_SCOPE;
    return root_ == nullptr;
}

// Helper function to merge two heaps
PairingHeap::Node *PairingHeap::merge(Node *h1, Node *h2) {
    TRACE_SCOPE;
    if (!h1)
        return h2;
    if (!h2)
        return h1;

    // Ensure h1 has higher priority
    if (!hasHigherPriority(h1->state, h2->state)) {
        Node *temp = h1;
        h1 = h2;
        h2 = temp;
    }

    // Make h2 the leftmost child of h1
    h2->nextSibling = h1->leftChild;
    h2->prev = h1;
    h1->leftChild = h2;

    return h1;
}

// Helper function to merge a list of siblings in pairs
PairingHeap::Node *PairingHeap::mergePairs(Node *firstSibling) {
    TRACE_SCOPE_NAMED("PairingHeap_mergePairs");
    if (!firstSibling || !firstSibling->nextSibling) {
        return firstSibling;
    }

    // First pass: merge adjacent pairs
    Node *first = firstSibling;
    Node *second = first->nextSibling;
    Node *rest = second->nextSibling;

    first->nextSibling = nullptr;
    first->prev = nullptr;
    second->nextSibling = nullptr;
    second->prev = nullptr;

    Node *merged = merge(first, second);

    if (!rest) {
        return merged;
    }

    // Recursively merge the rest
    Node *temp = mergePairs(rest);

    // Merge the results
    return merge(merged, temp);
}

void PairingHeap::deleteTree(Node *node) {
    TRACE_SCOPE;
    if (!node)
        return;

    // Delete siblings first
    if (node->nextSibling) {
        deleteTree(node->nextSibling);
    }

    // Delete children
    if (node->leftChild) {
        deleteTree(node->leftChild);
    }

    delete node;
}
