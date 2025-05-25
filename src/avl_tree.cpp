/**
 * @file avl_tree.cpp
 * @brief Implements the AVLTree class for maintaining a balanced binary search tree of user scores.
 */
#include "../include/avl_tree.h"
#include <algorithm>

/**
 * @brief Constructs an empty AVLTree.
 */
AVLTree::AVLTree() : root(nullptr) {}

/**
 * @brief Destroys the AVLTree and frees all allocated nodes.
 */
AVLTree::~AVLTree() { destroy(root); }

/**
 * @brief Recursively destroys the subtree rooted at a node.
 * @param node The root of the subtree to destroy.
 */
void AVLTree::destroy(AVLNode* node) {
    if (!node) return;
    destroy(node->left);
    destroy(node->right);
    delete node;
}

/**
 * @brief Performs a right rotation on the given subtree.
 * @param y Root of the subtree to rotate.
 * @return New root after rotation.
 */
AVLNode* AVLTree::rotateRight(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = std::max(h(y->left), h(y->right)) + 1;
    x->height = std::max(h(x->left), h(x->right)) + 1;
    return x;
}

/**
 * @brief Performs a left rotation on the given subtree.
 * @param x Root of the subtree to rotate.
 * @return New root after rotation.
 */
AVLNode* AVLTree::rotateLeft(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = std::max(h(x->left), h(x->right)) + 1;
    y->height = std::max(h(y->left), h(y->right)) + 1;
    return y;
}

/**
 * @brief Compares two (score, user) pairs for ordering.
 * @param s1 Score of first pair.
 * @param u1 User ID of first pair.
 * @param s2 Score of second pair.
 * @param u2 User ID of second pair.
 * @return true if first pair is less than second.
 */
bool AVLTree::isLess(int s1, int u1, int s2, int u2) const {
    if (s1 != s2) return s1 < s2;           // menor score es "menor"
    return u1 < u2;                          // empate por ID
}

/**
 * @brief Inserts a user-score pair into the subtree rooted at node.
 * @param node Current subtree root.
 * @param user User ID to insert.
 * @param score Score associated with the user.
 * @return New subtree root after insertion and balancing.
 */
AVLNode* AVLTree::insert(AVLNode* node, int user, int score) {
    if (!node) return new AVLNode(user, score);

    if (isLess(score, user, node->score, node->user))
        node->left = insert(node->left, user, score);
    else if (isLess(node->score, node->user, score, user))
        node->right = insert(node->right, user, score);
    else
        return node; // duplicado

    node->height = std::max(h(node->left), h(node->right)) + 1;
    int bf = balance(node);

    // LL
    if (bf > 1 && isLess(score, user, node->left->score, node->left->user))
        return rotateRight(node);
    // RR
    if (bf < -1 && isLess(node->right->score, node->right->user, score, user))
        return rotateLeft(node);
    // LR
    if (bf > 1 && isLess(node->left->score, node->left->user, score, user)) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    // RL
    if (bf < -1 && isLess(score, user, node->right->score, node->right->user)) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

/**
 * @brief Inserts a user and score into the AVL tree.
 * @param user The user ID.
 * @param score The score for the user.
 */
void AVLTree::insert(int user, int score) {
    root = insert(root, user, score);
}

/**
 * @brief Performs reverse in-order traversal to collect top-k users.
 * @param node Current subtree root.
 * @param k Maximum number of users to collect.
 * @param count Reference to count of users collected so far.
 * @param out Vector to store collected user IDs.
 */
void AVLTree::inorderDesc(AVLNode* node, int k, int& count, std::vector<int>& out) const {
    if (!node || count >= k) return;
    inorderDesc(node->right, k, count, out);          // primero mayores
    if (count < k) {
        out.push_back(node->user);
        ++count;
    }
    inorderDesc(node->left, k, count, out);
}

/**
 * @brief Retrieves the top k users by highest score in descending order.
 * @param k Number of users to retrieve.
 * @return Vector of user IDs with highest scores.
 */
std::vector<int> AVLTree::topK(int k) const {
    std::vector<int> res; res.reserve(k);
    int cnt = 0;
    inorderDesc(root, k, cnt, res);
    return res;
}