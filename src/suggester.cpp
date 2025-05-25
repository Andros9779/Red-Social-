/**
 * @file suggester.cpp
 * @brief Implements the Suggester class methods for generating friend suggestions based on mutual friends, tags, and network distance.
 */
#include "../include/suggester.h"
#include <unordered_set>
#include <unordered_map>
#include <algorithm>

// ------------------------------------------------------------------
// Utilidades
// ------------------------------------------------------------------
/**
 * @brief Counts shared interest tags between two users.
 * @param a ID of the first user.
 * @param b ID of the second user.
 * @return Number of tags both users share.
 */
int Suggester::commonTags(int a, int b) const {
    User* ua = g->getUser(a);
    User* ub = g->getUser(b);
    if (!ua || !ub) return 0;
    std::unordered_set<std::string> setA(ua->tags.begin(), ua->tags.end());
    int c = 0;
    for (const std::string& t : ub->tags)
        if (setA.count(t)) ++c;
    return c;
}

/**
 * @brief Generates up to k friend suggestions for a user within a specified radius.
 * @param u The user ID for whom to generate suggestions.
 * @param k Maximum number of suggestions to return.
 * @param radius Maximum network distance (hops) to consider.
 * @return A vector of suggested user IDs ordered by descending composite score.
 */
std::vector<int> Suggester::suggest(int u, int k, int radius) const {
    LinkedList* neigh = g->neighbors(u);
    if (!neigh) return {};

    // amigos directos + yo
    std::unordered_set<int> already;
    already.reserve(neigh->size() * 2 + 1);
    already.insert(u);
    for (Node* p = neigh->begin(); p; p = p->next) already.insert(p->key);

    // Acumular #mutuos por candidato
    std::unordered_map<int,int> mutualCnt;

    for (Node* p = neigh->begin(); p; p = p->next) {
        int friendId = p->key;
        LinkedList* neigh2 = g->neighbors(friendId);
        if (!neigh2) continue;
        for (Node* q = neigh2->begin(); q; q = q->next) {
            int v = q->key;
            if (already.count(v)) continue;

            int d = g->shortestPath(u, v);
            if (d == -1 || d > radius) continue;

            ++mutualCnt[v];      // acumula mutuo
        }
    }

    // Construir AVL con puntaje compuesto
    AVLTree tree;
    for (auto& [cand, mutCnt] : mutualCnt) {
        int d = g->shortestPath(u, cand);
        int tagC = commonTags(u, cand);
        int score = wMutuos * mutCnt + wTags * tagC - wDist * d;
        tree.insert(cand, score);
    }

    return tree.topK(k);
}
