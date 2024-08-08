#ifndef MYCLASS_H
#define MYCLASS_H
#include <iostream>
#include <vector>
#include <list>
#include <unordered_map>
#include <set>
#include <queue>
#include<cmath>
#include<algorithm>
#include <fstream>  // Include necessary header for file I/O
#include"Mystruct.h"
namespace std {
    template <>
    struct hash<vertex>
    {
        size_t operator()(const vertex &key) const
        {
            return hash<float>()(key.x) ^ hash<float>()(key.y);
        }

    };
} 
class OAMST
{
public:
    OAMST(unordered_map<vertex, unordered_map<vertex, float>>, set<vertex>);
    pair<unordered_map<vertex, float>, unordered_map<vertex, vector<vertex>>> dijkstra(unordered_map<vertex, unordered_map<vertex, float>>, vertex);
    pair<unordered_map<vertex, unordered_map<vertex, float>>, unordered_map<vertex, unordered_map<vertex, vector<vertex>>>> all_pins_shortest_paths(unordered_map<vertex, unordered_map<vertex, float>>, set<vertex>);
    void print_AllPaths_AllDistances();
    pair<unordered_map<vertex, unordered_map<vertex, float>>, unordered_map<vertex, unordered_map<vertex, vector<vertex>>>> makeAllPinsGraph(set<vertex>);
    void print_AllPINSPaths_AllPINSDistances();
    void MST_PRIM(set<vertex>);
    void outputMST();

    vector<pair<vertex, vertex>> MST;

private:
    unordered_map <vertex, unordered_map<vertex, vector<vertex>>> all_paths;
    unordered_map <vertex, unordered_map<vertex, float>> all_distances;

    unordered_map <vertex, unordered_map<vertex, vector<vertex>>> all_PINS_paths;
    unordered_map <vertex, unordered_map<vertex, float>> all_PINS_distances;

    vector<pair<vertex, vertex>> MST_only_PINS;

};


#endif // MYCLASS_H