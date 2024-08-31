#ifndef parser_H
#define parser_H

#include<iostream>
#include<vector>
#include<fstream>
#include<string>
#include <sstream>
#include <algorithm>
#include"Mystruct.h"
using namespace std;

vector<string> splitString(const string& str, char delimiter);
vector<string> eliminate_space(string line);
string delete_symbol(string obj);
void rotate_block(vector<vector<float>> &vertex,const string direction,const float ref_x,const float ref_y);
void read_chip_top(vector<block_data> & block_placing,vector<region_area>& region_placing,float die_area[2][2],int&unit,int &block_num,int& region_num,string folder);
void read_blk_area(const int num_of_block,vector<block_data> & block_placing,string folder);
void read_cfg(vector<block_data> & block_placing,string cfg_file);
void parse_connection_matrix(vector<NETS> &connection_matrix ,string connectin_file,int &max_num);
void convert_block_data(const int unit,vector<block_data>&block_placing );

bool compareByY(const vector<float>& a, const vector<float>& b) ;
float abso(float a);
bool is_thisrectangle(vector<vector<float>> rectangle);

int is_in_vector(vector<vector<float>> rectangle,vector<float> tuples);
vector<vector<float>> build_grid_list(vector<vector<float>> rectangle);
void sort_the_vertex(vector<vector<float>> &rectangle);
int is_allowed_cut(vector<float> pt1,vector<float> pt2,vector<float> pt3,vector<float> pt4 ,vector<vector<float>> all_grids);
vector<vector<float>> split_polygon(vector<vector<float>> rectangle);
void change_unit(vector<block_data> & block_placing,int unit);
 

#endif