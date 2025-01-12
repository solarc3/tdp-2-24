#include "../include/Heap.h"

PairingHeap::PairingHeap() {
    TRACE_SCOPE;
    this->root = nullptr;
    this->size = 0;
}
PairingHeap::Node::Node(State *s) {
    this->state = s;
    this->leftChild = nullptr;
    this->nextSibling = nullptr;
    this->prev = nullptr;
}
PairingHeap::~PairingHeap() {
    TRACE_SCOPE;
    clear();
}

bool PairingHeap::hasHigherPriority(const State *a, const State *b) {
    return a->weight < b->weight;
}
void PairingHeap::push(State *state) {
    TRACE_SCOPE;
    Node *newNode = new Node(state);
    if (!root) {
        root = newNode;
    } else {
        root = merge(root, newNode);
    }
    size++;
    TRACE_PLOT("Heap/Stats/Size", static_cast<int64_t>(size));
    TRACE_PLOT("Heap/Operations/Push", static_cast<int64_t>(1));
    TRACE_PLOT("Heap/Values/Current", static_cast<int64_t>(state->weight));
    TRACE_PLOT("Heap/Stats/MaxDepth", static_cast<int64_t>(state->depth));
}

State *PairingHeap::pop() {
    TRACE_SCOPE;
    TRACE_PLOT("Heap/Values/Popped", static_cast<int64_t>(root->state->weight));
    if (!root) {
        return nullptr;
    }

    Node *oldRoot = root;
    State *result = oldRoot->state;
    if (root->leftChild) {
        root = mergePairs(root->leftChild);
        if (root) {
            root->prev = nullptr;
        }
    } else {
        root = nullptr;
    }

    delete oldRoot;
    size--;
    return result;
}

State *PairingHeap::peek() const {
    TRACE_SCOPE;
    return root ? root->state : nullptr;
}

void PairingHeap::clear() {
    TRACE_SCOPE;
    deleteTree(root);
    root = nullptr;
    size = 0;
    TRACE_PLOT("Heap/Stats/Size", static_cast<int64_t>(0));
}

bool PairingHeap::empty() const {
    TRACE_SCOPE;
    return root == nullptr;
}
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

    Node *temp = mergePairs(rest);
    return merge(merged, temp);
}

void PairingHeap::deleteTree(Node *node) {
    TRACE_SCOPE;
    if (!node)
        return;
    if (node->nextSibling) {
        deleteTree(node->nextSibling);
    }
    if (node->leftChild) {
        deleteTree(node->leftChild);
    }

    delete node;
}
