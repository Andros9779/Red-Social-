/**
 * @file test_hash.cpp
 * @brief Unit tests for the HashTable class, verifying insertion, collision handling, rehashing, and lookup.
 */
#include <cassert>
#include "../include/hash_table.h"
#include "../include/linked_list.h"

/**
 * @brief Executes tests for HashTable functionality.
 *
 * Tests include insertion, containsKey, rehashing behavior, and handling of nonexistent keys.
 * @return 0 on success.
 */
int main() {
    // -------- Create HashTable and force collisions/rehash --------
    HashTable ht(11, 0.6f);

    // -------- Insert keys and verify containsKey --------
    for (int i = 1; i <= 20; ++i) {
        ht.put(i, new LinkedList());
        assert(ht.containsKey(i));
    }

    // -------- Verify rehash increased capacity --------
    assert(ht.getCapacity() > 11);

    // -------- Verify lookup for nonexistent key --------
    assert(!ht.containsKey(999));

    return 0;   // éxito
}