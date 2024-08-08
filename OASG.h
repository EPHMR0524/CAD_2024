#ifndef OASG_H
#define OASG_H

#include<iostream>
#include<vector>
#include<fstream>
#include<string>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include"Mystruct.h"

using namespace std;
float ManhattanDistance(poi p1, poi p2);
void swap(float &x, float &y);
void add(int u, int v, int cost);
bool compare(poi p1, poi p2);
int clas(poi x);
void init_map();
void init(int n,int m);
bool collison_block(poi a,poi b);
void regionOASG();
void OASG();
bool judge_inside_obstacle(poi p1,poi p2, vector<vector<float>>C_data,vector<partitioned_block> obstacles,vector<block_data>non_through_block);
unordered_map <vertex,unordered_map<vertex, float>> generate_OASG(vector<vector<float>>node_list, vector<block_data>non_through_block);
#endif