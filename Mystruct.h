#ifndef MYSTRUCT_H
#define MYSTRUCT_H
#include<iostream>
#include<vector>
#include<string>
#include <unordered_map>
using namespace std;

struct through_block_edge_net_num{
    float start_and_end[2][2];
    int number;
};
struct block_data{
    string blk_name;
    string blk_type;
    float place[2];
    string direction;//think another better name
     vector<vector<float>> original_vertex;
    vector<vector<float>> vertex; 
    int through_block_net_num;
    vector<through_block_edge_net_num> through_block_edge_net_num_lst;
    vector<vector<float>> block_port_region;
    bool is_feedthroughable;
    bool is_tile;
    vector<vector<float>> all_pts_after_split;
    vector<vector<float>> grids;


};

struct region_area{
    string reg_name;
    float left_down[2];//possible to be modified if the shape is not rectangle
    float right_up[2];
};

struct Through_region_description{
    string BLK_name;
    vector<float> ptr_data;
};
struct NETS{
    int ID;
    string TX;
    vector<string> RX;
    int NUM;
    vector<Through_region_description> MUST_THROUGH;
    vector<Through_region_description> HMFT_MUST_THROUGH;
    float TX_COORD[2];
    vector<vector<float>> RX_COORD;
};

struct poi
{
    float x,y;
    int id;
};

struct edge 
{
    // u is the start point, v is the end point and cost is the weight of the edge
    int u,v;
    float cost;
};

struct obstacle_block
{
    //p1 is the left bottom point, p2 is the right top point
    poi p1,p2;
};

struct result_edge{
    vector<float> pt1;
    vector<float> pt2;
    float cost;
};
//Ensure point v is in R1 region 
struct cmpset {
    bool operator()(const poi& a, const poi& b) const { // 添加 const 關鍵字
        if (a.x > b.x) return true;  // 使用 true 和 false 代替 1 和 0
        if (a.x == b.x && a.x - a.y > b.x - b.y) return true;
        return false;
    }
};
struct partitioned_block{
    poi pt1;
    poi pt2;
    string blk_name;

};
struct obstacle_grid{
    vector<vector<float>> grids;
    string blk_name;
};

struct vertex
{
    float x, y;
    vertex() : x(0), y(0) {}
    vertex(float x, float y) : x(x), y(y) {}

    // Define operator== to compare two vertex objects
    bool operator==(const vertex& other) const {
        return x == other.x && y == other.y;
    }

    // Define operator< to compare two vertex objects
    bool operator<(const vertex& other) const {
        return x < other.x || (x == other.x && y < other.y);
    }
};
#endif // MYSTRUCT_H