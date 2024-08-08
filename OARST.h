#ifndef OARST_H
#define OARST_H
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
#include"myclass.h"
// #include <climits>
// #include <cmath>
// #include "routing_struct.hpp"
#define INT_MAX 2147483647 
using namespace std;

// 如果有include routing_struct.hpp，則不需要再定義vertex struct和hash function


// 因為vertex是自定義的struct，所以要自己定義hash function
// namespace std {
//     template <>
//     struct hash<vertex>
//     {
//         size_t operator()(const vertex &key) const
//         {
//             return hash<float>()(key.x) ^ hash<float>()(key.y);
//         }

//     };
// } // namespace std


// 該函數是生成adjacency list的函數，用來存儲圖中每個節點的鄰接節點和權重
void addEdge(unordered_map<vertex, unordered_map<vertex, float>> &adj_lists, vertex u, vertex v);


int find_longest_edge(vector<pair<vertex, vertex>> MST);
vector<pair<vertex, vertex>> find_neighbor(unordered_map <vertex, unordered_map<vertex, float>>adj_lists,vertex pt1,vertex pt2);
vertex findCommonVertex(const pair<vertex, vertex>& E1, const pair<vertex, vertex>& E2);
int Find_quadrant(pair<vertex, vertex> Edges);
void push_in_OARST_oppo(pair<vertex, vertex> Edge,vertex Vertex_common,vector<pair<vertex, vertex>> & OARST_result);
void push_in_OARST_neighbor(pair<vertex, vertex> E1,pair<vertex, vertex> E2
,vertex Vertex_common,vector<pair<vertex, vertex>> & OARST_result,int E1_quadrant,int E2_quadrant);
void push_in_OARST_same(pair<vertex, vertex> E1,pair<vertex, vertex> E2,vertex Vertex_common
,vector<pair<vertex, vertex>> & OARST_result,int E1_quadrant,int E2_quadrant);
void push_in_OARST_with_ev(pair<vertex, vertex> E_ev,pair<vertex, vertex> E_slant,vertex Vertex_common
,vector<pair<vertex, vertex>> & OARST_result,int E_ev_quadrant,int E_slant_quadrant);
void partition_edge(vector<pair<vertex, vertex>> & OARST_result,pair<vertex, vertex> E1, pair<vertex, vertex> E2);
void remove_edge(vector<pair<vertex, vertex>>& MST,pair<vertex, vertex> E1);
unordered_map <vertex, unordered_map<vertex, float>> build_adj_lists(vector<pair<vertex, vertex>> MST);
vector<pair<vertex, vertex>> generate_OARST(unordered_map <vertex, unordered_map<vertex, float>> adj_lists,set<vertex> PINS);
#endif // OARST_H