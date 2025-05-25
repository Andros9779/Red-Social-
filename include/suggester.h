/**
 * @file suggester.h
 * @brief Defines the Suggester class for generating friend suggestions based on mutual friends, shared tags, and network distance.
 */
// === include/suggester.h ===
#ifndef SUGGESTER_H
#define SUGGESTER_H

#include "graph.h"
#include "avl_tree.h"
#include <vector>
#include <climits>

/**
 * @class Suggester
 * @brief Provides algorithms to suggest potential friends in the social network graph.
 */
class Suggester {
private:
    Graph* g;
    // Pesos para la función de puntaje
    int wMutuos = 2;
    int wTags   = 1;
    int wDist   = 1;
public:
    /**
     * @brief Sets the weight factors for the scoring function.
     * @param mutuos Weight for number of mutual friends.
     * @param tags Weight for number of shared tags.
     * @param dist Weight for network distance (shorter paths score higher).
     */
    void setWeights(int mutuos, int tags, int dist) {
        wMutuos = mutuos;
        wTags   = tags;
        wDist   = dist;
    }

    /**
     * @brief Counts the number of shared tags between two user profiles.
     * @param a ID of the first user.
     * @param b ID of the second user.
     * @return Number of tags that both users share.
     */
    int commonTags(int a, int b) const;

    /**
     * @brief Constructs a Suggester instance for the given social graph.
     * @param graph Pointer to the Graph containing users and friendships.
     */
    explicit Suggester(Graph* graph) : g(graph) {}

    /**
     * @brief Generates up to k friend suggestions for a user within a given radius.
     * @param u The user ID for whom to generate suggestions.
     * @param k Maximum number of suggestions to return (default 5).
     * @param radius Maximum number of hops in BFS (default 3).
     * @return Vector of suggested user IDs ordered by score.
     */
    std::vector<int> suggest(int u, int k = 5, int radius = 3) const;
};

#endif // SUGGESTER_H