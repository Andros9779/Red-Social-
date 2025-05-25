/**
 * @file hash_table.cpp
 * @brief Implements the HashTable class for mapping integer keys to LinkedList values using open addressing and chaining.
 */
#include <cassert>
#include "../include/hash_table.h"
#include <vector>
#include <cstring>
#include <stdexcept>
#include "../include/linked_list.h"

/**
 * @brief Constructs a HashTable with given capacity and load factor.
 * @param cap Initial number of buckets.
 * @param load Maximum load factor before rehashing.
 */
HashTable::HashTable(int cap, float load)
    : capacity(cap), sz(0), maxLoad(load) {
    keys = new int[capacity];
    values = new LinkedList*[capacity];
    used = new bool[capacity];
    for (int i = 0; i < capacity; ++i) {
        keys[i] = EMPTY;
        values[i] = nullptr;
        used[i] = false;
    }
}

/**
 * @brief Destroys the HashTable and frees all allocated memory.
 */
HashTable::~HashTable() {
    for (int i = 0; i < capacity; ++i) {
        if (used[i] && values[i]) delete values[i];
    }
    delete[] keys;
    delete[] values;
    delete[] used;
}

/**
 * @brief Computes the hash index for a given key.
 * @param key The key to hash.
 * @return Hash index in the current table capacity.
 */
int HashTable::hash(int key) const {
    long long h = 0;
    int k = key;
    while (k > 0) {
        int d = k % 10;
        h = h * 37 + d;
        k /= 10;
    }
    if (h < 0) h = -h;
    return static_cast<int>(h % capacity);
}

/**
 * @brief Finds the slot for a key using linear probing.
 * @param key The key to probe.
 * @return Index of the slot where the key is or should be.
 */
int HashTable::probe(int key) const {
    int idx = hash(key);
    while (used[idx] && keys[idx] != key) {
        idx = (idx + 1) % capacity;
    }
    return idx;
}

/**
 * @brief Resizes the table and reinserts all existing key-value pairs.
 */
void HashTable::rehash() {
    int oldCap = capacity;
    int* oldKeys = keys;
    LinkedList** oldVals = values;
    bool* oldUsed = used;

    capacity = capacity * 2 + 1;
    keys = new int[capacity];
    values = new LinkedList*[capacity];
    used = new bool[capacity];
    for (int i = 0; i < capacity; ++i) {
        keys[i] = EMPTY;
        values[i] = nullptr;
        used[i] = false;
    }
    sz = 0;
    for (int i = 0; i < oldCap; ++i) {
        if (oldUsed[i]) {
            put(oldKeys[i], oldVals[i]); // reinserta
        }
    }
    delete[] oldKeys;
    delete[] oldVals;
    delete[] oldUsed;
}

/**
 * @brief Retrieves the list of values associated with a key.
 * @param key The key to look up.
 * @return Pointer to the LinkedList of values, or nullptr if key is absent.
 */
LinkedList* HashTable::get(int key) const {
    int idx = probe(key);
    if (!used[idx]) return nullptr;
    return values[idx];
}

/**
 * @brief Inserts or updates a key-value mapping in the table.
 * @param key The key to insert.
 * @param value Pointer to the LinkedList of values for this key.
 */
void HashTable::put(int key, LinkedList* value) {
    int idx = probe(key);
    if (!used[idx]) {
        // nueva clave
        keys[idx] = key;
        used[idx] = true;
        values[idx] = value;
        ++sz;
        if (static_cast<float>(sz) / capacity > maxLoad) rehash();
    } else {
        // clave existente: actualiza valor
        if (values[idx] != value) {
            if (values[idx]) delete values[idx];
            values[idx] = value;
        }
    }
}

/**
 * @brief Checks whether a key exists in the table.
 * @param key The key to check.
 * @return True if the key exists, false otherwise.
 */
bool HashTable::containsKey(int key) const {
    int idx = probe(key);
    return used[idx];
}

/**
 * @brief Retrieves all keys currently stored in the table.
 * @return Vector of all stored keys.
 */
std::vector<int> HashTable::keySet() const {
    std::vector<int> v;
    v.reserve(sz);
    for (int i = 0; i < capacity; ++i) if (used[i]) v.push_back(keys[i]);
    return v;
}