#include<iostream>
#include<vector>
#include<fstream>
#include<string>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include<fstream>
#include<iomanip>
#include"parser.h"
#include"OASG.h"
#include"OARST.h"
using namespace std;
// namespace std {
//     template <>
//     struct hash<vertex> 
//     {   
//         size_t operator()(const vertex &key) const
//         {
//             return hash<float>()(key.x) ^ hash<float>()(key.y);
//         }
        
//     };
// }
void region_block_change_unit(vector<region_area> & ,int);
vector<vertex>trans_into_vertex(vector<vector<float>>);
void find_TX_point(string &, vector<vector<float>> &, vector<NETS>&, vector<block_data>&, vector<region_area>&, int);
void find_RX_point(string &, vector<vector<float>> &, vector<NETS>&, vector<block_data>&, vector<region_area>&, int);
void find_must_through_point(string &, string &, string &, vector<vector<float>> &, vector<NETS>&, vector<block_data>&, vector<region_area>&, int);
void find_HMFT_point(string &, string &, string &, vector<vector<float>> &, vector<vector<float>> &, vector<NETS>&, vector<block_data>&, vector<region_area>&, int);
int round_to_0(float a){
    float x=(a*1+0.5);
    int result=x;
    return result;
}
float euclideanDistance(const vector<float>& point1, const vector<float>& point2) {
    float distance = 0.0;
    for (size_t i = 0; i < point1.size(); ++i) {
        distance += pow(point1[i] - point2[i], 2);
    }
    return sqrt(distance);
}

// 找出最接近的點，不考慮 target_point 本身
vector<float> findNearestPoint(const vector<vector<float>>& node_list, const vector<float>& target_point) {
    vector<float> nearest_point;
    float min_distance = 999999.9999;

    for (const auto& node : node_list) {
        // 跳過與 target_point 相同的點
        if (node == target_point) {
            continue;
        }

        float distance = euclideanDistance(node, target_point);
        if (distance < min_distance) {
            min_distance = distance;
            nearest_point = node;
        }
    }

    return nearest_point;
}

// 計算點到線段的最短距離
float pointToSegmentDistance(const vector<float>& p, const vector<float>& v, const vector<float>& w) {
    float l2 = euclideanDistance(v, w) * euclideanDistance(v, w);
    if (l2 == 0.0) return euclideanDistance(p, v);
    float t = max(0.0f, min(1.0f, ((p[0] - v[0]) * (w[0] - v[0]) + (p[1] - v[1]) * (w[1] - v[1])) / l2));
    vector<float> projection = {v[0] + t * (w[0] - v[0]), v[1] + t * (w[1] - v[1])};
    return euclideanDistance(p, projection);
}

// 找出最接近的 edge
vector<float> findNearestEdge(const vector<float>& nearest_point, const vector<vector<float>>& block_port_region) {
    vector<float> nearest_edge;
    float min_distance = 999999.9999;

    for (const auto& edge : block_port_region) {
        vector<float> v = {edge[0], edge[1]};
        vector<float> w = {edge[2], edge[3]};
        cout << " " << edge[0] << " "  << edge[1] << " " << edge[2]<< " " << edge[3] <<  endl;
        float distance = pointToSegmentDistance(nearest_point, v, w);
        if (distance < min_distance) {
            min_distance = distance;
            nearest_edge = edge;
        }
    }

    return nearest_edge;
}

bool compare_disto_tx(const pair<vertex, vertex>& edgeA, const pair<vertex, vertex>& edgeB, const vector<float>& TX_pts) {
    float dis_of_edgeA = min((abs(edgeA.first.x - TX_pts[0]) + abs(edgeA.first.y - TX_pts[1])),
                                  (abs(edgeA.second.x - TX_pts[0]) + abs(edgeA.second.y - TX_pts[1])));
    float dis_of_edgeB = min((abs(edgeB.first.x - TX_pts[0]) + abs(edgeB.first.y - TX_pts[1])),
                                  (abs(edgeB.second.x - TX_pts[0]) + abs(edgeB.second.y - TX_pts[1])));
    return dis_of_edgeA < dis_of_edgeB;
}

int main(int argc,char *files[]){
    
    float die_area[2][2];
    int unit;
    int block_num;
    int region_num;
    string folder=files[2],cfg_file=files[3],connectin_file=files[4];
    int track_um=stoi(files[1]);
    cout<<endl<<"file name: "<<folder<<" "<<cfg_file<<" "<<connectin_file<<" "<<endl;
    
    int track_num=stoi(files[1]);
    cout<<"kkk"<<track_num;
    //cout<<track_num<<endl;
    string out_file_name=folder;
    out_file_name.erase(remove(out_file_name.begin(), out_file_name.end(), '/'), out_file_name.end());
    ofstream outFile(out_file_name+"_net.rpt");
    
    vector<block_data> block_placing;
    vector<region_area> region_placing;
    vector<NETS> connection_matrix;


    read_chip_top(block_placing,region_placing,die_area,unit,block_num, region_num,folder);//access into the chip_top file, get the data of each block and the placement of region
    //cout<<block_num<<endl;
    float boarder_x=die_area[1][0]/unit,boarder_y=die_area[1][1]/unit;
    for(int i=0;i<block_num;i++){ 
        //cout<<i<<" ";
        read_blk_area(i,block_placing, folder);//access into the file of various types of blocks, get the data of area
    }
    read_cfg(block_placing,cfg_file);

    int max_num_net=0;
    parse_connection_matrix(connection_matrix,connectin_file,max_num_net);
    cout<<"max number of net: "<<max_num_net<<endl;
    convert_block_data(unit,block_placing);
    for(int i=0;i<block_num;i++){ 
        //cout<<i<<" ";
        if( block_placing[i].vertex.size()>2){
            block_placing[i].vertex=split_polygon(block_placing[i].vertex);
            block_placing[i].grids=build_grid_list(block_placing[i].original_vertex);
        }   
        // block_placing[i].vertex=split_polygon(block_placing[i].vertex);
        // block_placing[i].grids=build_grid_list(block_placing[i].original_vertex);
    }
    
    change_unit(block_placing,unit);
    region_block_change_unit(region_placing,unit);
    
    vector<block_data>non_through_block;
    vector<vector<float>> bpr_thrable;
    //vector<vector<float>> node_list, vector<block_data>non_through_block
    for(int i=0;i<block_placing.size();i++){
        //cout<<block_placing[i].blk_name<<" "<<block_placing[i].is_feedthroughable<<" ";
        //cout<<block_placing[i].place[0]<<" "<<block_placing[i].place[1]<<" "<<block_placing[i].direction<<" ";
        if(!block_placing[i].is_feedthroughable){
            non_through_block.push_back(block_placing[i]);
        }
        if(block_placing[i].is_feedthroughable){
            for(int j=0;j<block_placing[i].block_port_region.size();j++){
                float x=(block_placing[i].block_port_region[j][0]+block_placing[i].block_port_region[j][2])/2;
                float y=(block_placing[i].block_port_region[j][1]+block_placing[i].block_port_region[j][3])/2;
                bpr_thrable.push_back({x,y});
            }
            
        }

        
    }
    

    int set_ID=0;
    for(int i=set_ID;i<connection_matrix.size();i++){
        

        vector<vector<float>> node_list;
        vector<vector<float>>PIN_list;
        vector<vector<float>> HMFT_new_node_list;
        string TX_blockname;
        string RX_blockname;
        string MUST_Through_blk_name;
        string HMFT_MUST_Through_blk_name;

        cout << "======================" << endl;
        cout << "ID: " << connection_matrix[i].ID << endl;

        //TX node
  
        find_TX_point(TX_blockname, node_list, connection_matrix, block_placing, region_placing, i);//testing func.
        //TX node end
        //RX node

        find_RX_point(RX_blockname, node_list, connection_matrix, block_placing, region_placing, i);
        //RX node end
        //must through
        
        find_must_through_point(MUST_Through_blk_name, TX_blockname, RX_blockname, node_list, connection_matrix, block_placing, region_placing, i);
        //must through end
        //HMFT
        // 
        find_HMFT_point(HMFT_MUST_Through_blk_name, TX_blockname, RX_blockname, node_list, HMFT_new_node_list, connection_matrix, block_placing, region_placing, i);
        //HMFT END

        //adding block port region
        if(TX_blockname[0]=='B')
        {
            int T_OR_N = 1;
            auto it = find_if(non_through_block.begin(), non_through_block.end(), [&TX_blockname](const block_data& TX) {
            return TX.blk_name == TX_blockname;
            });
            for(int m=0; m<connection_matrix[i].HMFT_MUST_THROUGH.size(); m++)
            {
                HMFT_MUST_Through_blk_name = connection_matrix[i].HMFT_MUST_THROUGH[m].BLK_name;
                HMFT_MUST_Through_blk_name.erase(remove(HMFT_MUST_Through_blk_name.begin(), HMFT_MUST_Through_blk_name.end(), ' '), HMFT_MUST_Through_blk_name.end());
                if(TX_blockname == HMFT_MUST_Through_blk_name)
                {
                    T_OR_N = 0;
                    break;
                }
            }
            cout << T_OR_N << endl;
            if(it->block_port_region.size()!=0){
                if (it != non_through_block.end() && T_OR_N)
                    {
                        vector<float> target_point = {node_list[0][0],node_list[0][1]};
                        vector<float> nearest_point = findNearestPoint(node_list, target_point);
                        vector<float> nearest_edge = findNearestEdge(nearest_point, it->block_port_region);
                        float tmp_x = (nearest_edge[0] + nearest_edge[2])/2;
                        float tmp_y = (nearest_edge[1] + nearest_edge[3])/2;
                        cout << "tmp_x: " << tmp_x <<" tmp_y: "<< tmp_y << endl;
                        node_list.push_back({tmp_x, tmp_y});
                        HMFT_new_node_list.push_back({node_list[0][0],node_list[0][1]});
                        HMFT_new_node_list.push_back({tmp_x, tmp_y});
                    }
            }
            
        }

        for(int j=0; j<connection_matrix[i].RX.size(); j++)
        {
            RX_blockname = connection_matrix[i].RX[j];
            RX_blockname.erase(remove(RX_blockname.begin(), RX_blockname.end(), ' '), RX_blockname.end());
            if(RX_blockname[0]=='B')
            {
                int T_OR_N = 1;
                auto it = find_if(non_through_block.begin(), non_through_block.end(), [&RX_blockname](const block_data& RX) {
                return RX.blk_name == RX_blockname;
                });
                for(int m=0; m<connection_matrix[i].HMFT_MUST_THROUGH.size(); m++)
                {
                    HMFT_MUST_Through_blk_name = connection_matrix[i].HMFT_MUST_THROUGH[m].BLK_name;
                    HMFT_MUST_Through_blk_name.erase(remove(HMFT_MUST_Through_blk_name.begin(), HMFT_MUST_Through_blk_name.end(), ' '), HMFT_MUST_Through_blk_name.end());
                    if(RX_blockname == HMFT_MUST_Through_blk_name)
                    {
                        T_OR_N = 0;
                        break;
                    }
                }
                cout << T_OR_N << endl;
                if(it->block_port_region.size()!=0){
                    if (it != non_through_block.end() && T_OR_N)
                    {
                        vector<float> target_point = {node_list[j+1][0],node_list[j+1][1]};
                        vector<float> nearest_point = findNearestPoint(node_list, target_point);
                        vector<float> nearest_edge = findNearestEdge(nearest_point, it->block_port_region);
                        float tmp_x = (nearest_edge[0] + nearest_edge[2])/2;
                        float tmp_y = (nearest_edge[1] + nearest_edge[3])/2;
                        node_list.push_back({tmp_x, tmp_y});
                        HMFT_new_node_list.push_back({node_list[j+1][0],node_list[j+1][1]});
                        HMFT_new_node_list.push_back({tmp_x, tmp_y});
                    }
                }
            }
        }
        //adding block port region end
        cout<<" before OASG ID "<<connection_matrix[i].ID<<endl;
        cout<<"nolist size and points"<<node_list.size()<<endl;
        for(int k=0;k<node_list.size();k++){
            cout<<endl<<node_list[k][0]<<","<<node_list[k][1]<<","<<endl;
        }
        vector<vector<float>> node_plus_bpr;
        node_plus_bpr.insert(node_plus_bpr.end(),node_list.begin(),node_list.end());
        node_plus_bpr.insert(node_plus_bpr.end(),bpr_thrable.begin(),bpr_thrable.end());
        cout<<"fewfew1111 "<<node_plus_bpr.size()-node_list.size()<<endl;
        for(int k=0;k<node_plus_bpr.size();k++){
            cout<<endl<<node_plus_bpr[k][0]<<","<<node_plus_bpr[k][1]<<","<<endl;
        }
        // string IDS=to_string(set_ID);
        // ofstream outFile("ID_"+IDS+out_file_name+".txt");
        // for(int k=0;k<node_list.size();k++){
        //     outFile<<"("<<node_list[k][0]<<" "<<node_list[k][1]<<")";
        //     outFile<<endl;
        // }
        
        //cout<<"print block end";
        //outFile.close();
        //float width=connection_matrix[i].NUM/track_num*0;
        unordered_map <vertex,unordered_map<vertex, float>> OASG_result=generate_OASG(node_plus_bpr,non_through_block,boarder_x,boarder_y);
        node_plus_bpr.clear();
        
        
        //HMFT_new_node_list
        for(int k=0;k<HMFT_new_node_list.size()/2;k++){
            addEdge(OASG_result,{HMFT_new_node_list[k*2][0],HMFT_new_node_list[k*2][1]},{HMFT_new_node_list[k*2+1][0],HMFT_new_node_list[k*2+1][1]});

        }
        
        
        //cout<<
        vector<vertex> node_list_lertex=trans_into_vertex(node_list);
        set<vertex> PINS(node_list_lertex.begin(), node_list_lertex.end());

        //cout<<" before OARST ID "<<connection_matrix[i].ID<<" "<<PINS.size()<<" "<<OASG_result.size()<<endl;
        cout<<"into OARST"<<endl;
        vector<pair<vertex, vertex>> Result_OARST=generate_OARST(OASG_result,PINS);//之後要加回來
        //sort(Result_OARST.begin(), Result_OARST.end(), compare_disto_tx);
        vector<float>TX_pts=node_list[0];
        sort(Result_OARST.begin(), Result_OARST.end(), [&TX_pts](const pair<vertex, vertex>& edgeA, const pair<vertex, vertex>& edgeB) {
            return compare_disto_tx(edgeA, edgeB, TX_pts);
        });
        outFile<<"[ID "<<connection_matrix[i].ID<<"]"<<endl;
        
        if(Result_OARST.size()==0){
            
            outFile<<"("<<(int)(node_list[0][0]*unit)<<","<<(int)(node_list[0][1]*unit)<<")"<<","<<"("<<(int)(node_list[0][0]*unit)<<","<<(int)(node_list[0][1]*unit)<<")"<<endl;
            //outFile<<"("<<round_to_0(node_list[0][0])<<","<<round_to_0(node_list[0][1])<<")"<<","<<"("<<round_to_0(node_list[0][0])<<","<<round_to_0(node_list[0][1])<<")"<<endl;
            
             
        }
        else{

            for(int k=0;k<Result_OARST.size();k++){
                 if((abs(Result_OARST[k].first.x - TX_pts[0]) + abs(Result_OARST[k].first.y - TX_pts[1]))<=((abs(Result_OARST[k].second.x - TX_pts[0]) + abs(Result_OARST[k].second.y - TX_pts[1])))){
                     outFile<<"("<<(int)(Result_OARST[k].first.x*unit)<<","<<(int)(Result_OARST[k].first.y*unit)<<")"<<","<<"("<<(int)(Result_OARST[k].second.x*unit)<<","<<(int)(Result_OARST[k].second.y*unit)<<")"<<endl;
                 }
                 else{
                     outFile<<"("<<(int)(Result_OARST[k].second.x*unit)<<","<<(int)(Result_OARST[k].second.y*unit)<<")"<<","<<"("<<(int)(Result_OARST[k].first.x*unit)<<","<<(int)(Result_OARST[k].first.y*unit)<<")"<<endl;
                 }
            // if((abs(Result_OARST[k].first.x - TX_pts[0]) + abs(Result_OARST[k].first.y - TX_pts[1]))<=((abs(Result_OARST[k].second.x - TX_pts[0]) + abs(Result_OARST[k].second.y - TX_pts[1])))){
            //     outFile<<"("<<round_to_0(Result_OARST[k].first.x)<<","<<round_to_0(Result_OARST[k].first.y)<<")"<<","<<"("<<round_to_0(Result_OARST[k].second.x)<<","<<round_to_0(Result_OARST[k].second.y)<<")"<<endl;
            // }
            // else{
            //     outFile<<"("<<round_to_0(Result_OARST[k].second.x)<<","<<round_to_0(Result_OARST[k].second.y)<<")"<<","<<"("<<round_to_0(Result_OARST[k].first.x)<<","<<round_to_0(Result_OARST[k].first.y)<<")"<<endl;
            // }
            // for(int k=0;k<Result_OARST.size();k++){
            //if((abs(Result_OARST[k].first.x - TX_pts[0]) + abs(Result_OARST[k].first.y - TX_pts[1]))<=((abs(Result_OARST[k].second.x - TX_pts[0]) + abs(Result_OARST[k].second.y - TX_pts[1])))){
            //    outFile<<" "<<Result_OARST[k].first.x<<" "<<Result_OARST[k].first.y<<" "<<" "<<" "<<Result_OARST[k].second.x<<" "<<Result_OARST[k].second.y<<" "<<endl;
            //}
            //else{
             //   outFile<<" "<<Result_OARST[k].second.x<<" "<<Result_OARST[k].second.y<<" "<<" "<<" "<<Result_OARST[k].first.x<<" "<<Result_OARST[k].first.y<<" "<<endl;
            //}
            }
        
            
        }//之後要加回來
        //for python plotting output start
        // outFile<<"[ID "<<connection_matrix[i].ID<<" ";
        // for(int k=0;k<node_list.size();k++)outFile<<node_list[k][0]<<" "<<node_list[k][1]<<" ";
        // outFile<<endl;
        // for(int k=0;k<Result_OARST.size();k++){
        //     if((abs(Result_OARST[k].first.x - TX_pts[0]) + abs(Result_OARST[k].first.y - TX_pts[1]))<=((abs(Result_OARST[k].second.x - TX_pts[0]) + abs(Result_OARST[k].second.y - TX_pts[1])))){
        //         outFile<<" "<<Result_OARST[k].first.x<<" "<<Result_OARST[k].first.y<<" "<<" "<<" "<<Result_OARST[k].second.x<<" "<<Result_OARST[k].second.y<<" "<<endl;
        //     }
        //     else{
        //         outFile<<" "<<Result_OARST[k].second.x<<" "<<Result_OARST[k].second.y<<" "<<" "<<" "<<Result_OARST[k].first.x<<" "<<Result_OARST[k].first.y<<" "<<endl;
        //     }
            
        // }
        // outFile<<endl;
        //for python plotting output end
        //cout<<"end: "<<Result_OARST.size();
        OASG_result.clear();
        Result_OARST.clear();//要加回來
       
        if(i==set_ID && set_ID!=0)break;
        //cout << "finish" <<connection_matrix[i].ID<< endl ;
        //cout << "======================" << endl;
    }
    

    //cout<<"finish";
    
    
    return 0;
}

void region_block_change_unit(vector<region_area> & region_placing,int unit)
{
    for (int i=0;i<region_placing.size();i++)
    {
        region_placing[i].left_down[0]/=unit;
        region_placing[i].left_down[1]/=unit;
    }

}

vector<vertex>trans_into_vertex(vector<vector<float>> node_list){
    
    vector<vertex> result;
    for(int i=0;i<node_list.size();i++){
        vertex pt1;
        pt1.x=node_list[i][0];
        pt1.y=node_list[i][1];
        result.push_back(pt1);
    }
    return result;


}
void find_TX_point(string &TX_blockname, vector<vector<float>> &node_list, vector<NETS> &connection_matrix, vector<block_data> &block_placing, vector<region_area> &region_placing, int i)
{
    TX_blockname = connection_matrix[i].TX;
    TX_blockname.erase(remove(TX_blockname.begin(), TX_blockname.end(), ' '), TX_blockname.end());
    if(TX_blockname[0]=='R')
    {
        auto it = find_if(region_placing.begin(), region_placing.end(), [&TX_blockname](const region_area& TX) {
        return TX.reg_name == TX_blockname;
        });
        if (it != region_placing.end()) {
            cout << "TX_Name: " << it->reg_name << endl;
            node_list.push_back({connection_matrix[i].TX_COORD[0], connection_matrix[i].TX_COORD[1]});
        }
    }
    else if(TX_blockname[0]=='B')
    {
        auto it = find_if(block_placing.begin(), block_placing.end(), [&TX_blockname](const block_data& TX) {
        return TX.blk_name == TX_blockname;
        });
        if (it != block_placing.end()) {
            cout << "TX_Name: " << it->blk_name << endl;
            node_list.push_back({(it->place[0]) + connection_matrix[i].TX_COORD[0], (it->place[1]) + connection_matrix[i].TX_COORD[1]});
            cout << it->blk_name << ": (" << it->place[0] << ", "<< it->place[1] << ")" << endl;
        }

    }
    else
    {
        cout << "NOOOOOOO" << endl;
    }
}

void find_RX_point(string &RX_blockname, vector<vector<float>> &node_list, vector<NETS>& connection_matrix, vector<block_data>& block_placing, vector<region_area>& region_placing, int i)
{
    for(int j=0; j<connection_matrix[i].RX.size(); j++)
    {
        RX_blockname = connection_matrix[i].RX[j];
        RX_blockname.erase(remove(RX_blockname.begin(), RX_blockname.end(), ' '), RX_blockname.end());
        if(RX_blockname[0]=='R')//region_placing
        {
            auto it = find_if(region_placing.begin(), region_placing.end(), [&RX_blockname](const region_area& RX) {
            return RX.reg_name == RX_blockname;
            });
            if (it != region_placing.end()) {
                cout << "RX_Name: " << it->reg_name << endl;
                //cout << "X: " << connection_matrix[i].RX_COORD[j][0] << "," << "Y: " << connection_matrix[i].RX_COORD[j][1] << endl;
                node_list.push_back({connection_matrix[i].RX_COORD[j][0], connection_matrix[i].RX_COORD[j][1]});
            }
        }
        else if(RX_blockname[0]=='B')//block_placing
        {
            auto it = find_if(block_placing.begin(), block_placing.end(), [&RX_blockname](const block_data& RX) {
            return RX.blk_name == RX_blockname;
            });
            if (it != block_placing.end()) {
                cout << "RX_Name: " << it->blk_name << endl;
                cout << it->blk_name << ": (" << it->place[0] << ", "<< it->place[1] << ")" << endl;
                //cout << "X: " << connection_matrix[i].RX_COORD[j][0] << "," << "Y: " << connection_matrix[i].RX_COORD[j][1] << endl;
                node_list.push_back({(it->place[0]) + connection_matrix[i].RX_COORD[j][0], (it->place[1]) + connection_matrix[i].RX_COORD[j][1]});
            }
        }
        else// I don't know but may be occur
        {
            cout << "NOOOOOOO" << endl;
            break;
        }
    }

}

void find_must_through_point(string &MUST_Through_blk_name, string &TX_blockname, string &RX_blockname, vector<vector<float>> &node_list, vector<NETS>& connection_matrix, vector<block_data>& block_placing, vector<region_area> &region_placing, int i)
{
    for(int j=0;j<connection_matrix[i].MUST_THROUGH.size();j++)
    {
        float node_01_x = 0.0,node_01_y = 0.0,node_02_x = 0.0 ,node_02_y = 0.0;
        float tmp_x = 0.0 ,tmp_y = 0.0; // store the block's or the region's left down point
        MUST_Through_blk_name = connection_matrix[i].MUST_THROUGH[j].BLK_name;
        MUST_Through_blk_name.erase(remove(MUST_Through_blk_name.begin(), MUST_Through_blk_name.end(), ' '), MUST_Through_blk_name.end());

        cout << "MUST_THROUGH_Name:" << MUST_Through_blk_name << endl;

        if(MUST_Through_blk_name[0] =='R')//region_placing
        {
            auto it = find_if(region_placing.begin(), region_placing.end(), [&MUST_Through_blk_name](const region_area& MUST) {
            return MUST.reg_name == MUST_Through_blk_name;
            });
            if (it != region_placing.end()) {
                tmp_x = 0;
                tmp_y = 0;
                cout << "tmp_x: " << tmp_x << ", " << "tmp_y: " << tmp_y << endl;
            }
        }
        else if(MUST_Through_blk_name[0] =='B')//block_placing
        {
            auto it = find_if(block_placing.begin(), block_placing.end(), [&MUST_Through_blk_name](const block_data& MUST) {
            return MUST.blk_name == MUST_Through_blk_name;
            });
            if (it != block_placing.end()) {
                tmp_x = it->place[0];
                tmp_y = it->place[1];
                cout << "tmp_x: " << tmp_x << ", " << "tmp_y: " << tmp_y << endl;
            }
        }
        else// I don't know but may be occur
        {
            cout << "FFFFFFFFFFFFFFFFF" << endl;
        }

        if(connection_matrix[i].MUST_THROUGH[j].ptr_data.size() == 8)
        {
            for(int k=0;k<connection_matrix[i].MUST_THROUGH[j].ptr_data.size();k++)
            {
                cout << "MUST_THROUGH_data" << k << ": " << connection_matrix[i].MUST_THROUGH[j].ptr_data[k] << endl;
            }
            cout << "!!!!!!!!!!!!!!!!" << endl;

            if(connection_matrix[i].MUST_THROUGH[j].ptr_data[0]==connection_matrix[i].MUST_THROUGH[j].ptr_data[4] && connection_matrix[i].MUST_THROUGH[j].ptr_data[2]==connection_matrix[i].MUST_THROUGH[j].ptr_data[6])
            {//上下邊為平行,所以希望畫出的線是垂直
                node_01_x = (connection_matrix[i].MUST_THROUGH[j].ptr_data[0]+connection_matrix[i].MUST_THROUGH[j].ptr_data[2])/2;//需要優化或是不用
                node_01_y = connection_matrix[i].MUST_THROUGH[j].ptr_data[1];
                node_02_x = node_01_x;
                node_02_y = connection_matrix[i].MUST_THROUGH[j].ptr_data[5];

                node_list.push_back({node_01_x + tmp_x, node_01_y + tmp_y});
                node_list.push_back({node_02_x + tmp_x, node_02_y + tmp_y});
            }
            else if(connection_matrix[i].MUST_THROUGH[j].ptr_data[1]==connection_matrix[i].MUST_THROUGH[j].ptr_data[5] && connection_matrix[i].MUST_THROUGH[j].ptr_data[3]==connection_matrix[i].MUST_THROUGH[j].ptr_data[7])
            {//左右邊為平行,所以希望畫出的線是平行
                node_01_x = connection_matrix[i].MUST_THROUGH[j].ptr_data[0];
                node_01_y = (connection_matrix[i].MUST_THROUGH[j].ptr_data[1]+connection_matrix[i].MUST_THROUGH[j].ptr_data[3])/2;//需要優化或是不用
                node_02_x = connection_matrix[i].MUST_THROUGH[j].ptr_data[4];
                node_02_y = node_01_y;

                node_list.push_back({node_01_x + tmp_x, node_01_y + tmp_y});
                node_list.push_back({node_02_x + tmp_x, node_02_y + tmp_y});
            }
            else
            {
                node_01_x = (connection_matrix[i].MUST_THROUGH[j].ptr_data[0]+connection_matrix[i].MUST_THROUGH[j].ptr_data[2])/2;
                node_01_y = (connection_matrix[i].MUST_THROUGH[j].ptr_data[1]+connection_matrix[i].MUST_THROUGH[j].ptr_data[3])/2;
                node_02_x = (connection_matrix[i].MUST_THROUGH[j].ptr_data[4]+connection_matrix[i].MUST_THROUGH[j].ptr_data[6])/2;
                node_02_y = (connection_matrix[i].MUST_THROUGH[j].ptr_data[5]+connection_matrix[i].MUST_THROUGH[j].ptr_data[7])/2;

                node_list.push_back({node_01_x + tmp_x, node_01_y + tmp_y});
                node_list.push_back({node_02_x + tmp_x, node_02_y + tmp_y});
            }
        }
        else if (connection_matrix[i].MUST_THROUGH[j].ptr_data.size() == 4)
        {
            if(connection_matrix[i].MUST_THROUGH[j].ptr_data[0]==connection_matrix[i].MUST_THROUGH[j].ptr_data[2])
            {
                if(MUST_Through_blk_name == TX_blockname)
                {
                    if((connection_matrix[i].MUST_THROUGH[j].ptr_data[1]<=connection_matrix[i].TX_COORD[1] && connection_matrix[i].MUST_THROUGH[j].ptr_data[3]>=connection_matrix[i].TX_COORD[1])
                        || (connection_matrix[i].MUST_THROUGH[j].ptr_data[1]>=connection_matrix[i].TX_COORD[1] && connection_matrix[i].MUST_THROUGH[j].ptr_data[3]<=connection_matrix[i].TX_COORD[1]))
                    {
                        node_01_x = connection_matrix[i].MUST_THROUGH[j].ptr_data[0];
                        node_01_y = connection_matrix[i].TX_COORD[1];

                        node_list.push_back({node_01_x + tmp_x, node_01_y + tmp_y});
                    }
                    else
                    {
                        node_01_x = connection_matrix[i].MUST_THROUGH[j].ptr_data[0];
                        node_01_y = (connection_matrix[i].MUST_THROUGH[j].ptr_data[1]+connection_matrix[i].MUST_THROUGH[j].ptr_data[3])/2;

                        node_list.push_back({node_01_x + tmp_x, node_01_y + tmp_y});
                    }
                }
                else
                {
                    for(int a=0; a<connection_matrix[i].RX.size(); a++)
                    {
                        RX_blockname = connection_matrix[i].RX[a];
                        RX_blockname.erase(remove(RX_blockname.begin(), RX_blockname.end(), ' '), RX_blockname.end());
                        if(MUST_Through_blk_name == RX_blockname)
                        {
                            if((connection_matrix[i].MUST_THROUGH[j].ptr_data[1]<=connection_matrix[i].RX_COORD[a][1] && connection_matrix[i].MUST_THROUGH[j].ptr_data[3]>=connection_matrix[i].RX_COORD[a][1])
                                || (connection_matrix[i].MUST_THROUGH[j].ptr_data[1]>=connection_matrix[i].RX_COORD[a][1] && connection_matrix[i].MUST_THROUGH[j].ptr_data[3]<=connection_matrix[i].RX_COORD[a][1]))
                            {
                                node_01_x = connection_matrix[i].MUST_THROUGH[j].ptr_data[0];
                                node_01_y = connection_matrix[i].RX_COORD[a][1];

                                node_list.push_back({node_01_x + tmp_x, node_01_y + tmp_y});
                            }
                            else
                            {
                                node_01_x = connection_matrix[i].MUST_THROUGH[j].ptr_data[0];
                                node_01_y = (connection_matrix[i].MUST_THROUGH[j].ptr_data[1]+connection_matrix[i].MUST_THROUGH[j].ptr_data[3])/2;

                                node_list.push_back({node_01_x + tmp_x, node_01_y + tmp_y});
                            }

                            break;
                        }
                    }
                }
            }
            else if(connection_matrix[i].MUST_THROUGH[j].ptr_data[1]==connection_matrix[i].MUST_THROUGH[j].ptr_data[3])
            {
                if(MUST_Through_blk_name == TX_blockname)
                {
                    if((connection_matrix[i].MUST_THROUGH[j].ptr_data[0]<=connection_matrix[i].TX_COORD[0] && connection_matrix[i].MUST_THROUGH[j].ptr_data[2]>=connection_matrix[i].TX_COORD[0])
                        || (connection_matrix[i].MUST_THROUGH[j].ptr_data[0]>=connection_matrix[i].TX_COORD[0] && connection_matrix[i].MUST_THROUGH[j].ptr_data[2]<=connection_matrix[i].TX_COORD[0]))
                    {
                        node_01_x = connection_matrix[i].TX_COORD[0];
                        node_01_y = connection_matrix[i].MUST_THROUGH[j].ptr_data[1];

                        node_list.push_back({node_01_x + tmp_x, node_01_y + tmp_y});
                    }
                    else
                    {
                        node_01_x = (connection_matrix[i].MUST_THROUGH[j].ptr_data[0]+connection_matrix[i].MUST_THROUGH[j].ptr_data[2])/2;
                        node_01_y = connection_matrix[i].MUST_THROUGH[j].ptr_data[1];

                        node_list.push_back({node_01_x + tmp_x, node_01_y + tmp_y});
                    }
                }
                else
                {
                    for(int a=0; a<connection_matrix[i].RX.size(); a++)
                    {
                        RX_blockname = connection_matrix[i].RX[a];
                        RX_blockname.erase(remove(RX_blockname.begin(), RX_blockname.end(), ' '), RX_blockname.end());
                        if(MUST_Through_blk_name == RX_blockname)
                        {
                            if((connection_matrix[i].MUST_THROUGH[j].ptr_data[0]<=connection_matrix[i].RX_COORD[a][0] && connection_matrix[i].MUST_THROUGH[j].ptr_data[2]>=connection_matrix[i].RX_COORD[a][0])
                                || (connection_matrix[i].MUST_THROUGH[j].ptr_data[0]>=connection_matrix[i].RX_COORD[a][0] && connection_matrix[i].MUST_THROUGH[j].ptr_data[2]<=connection_matrix[i].RX_COORD[a][0]))
                            {
                                node_01_x = connection_matrix[i].RX_COORD[a][0];
                                node_01_y = connection_matrix[i].MUST_THROUGH[j].ptr_data[1];

                                node_list.push_back({node_01_x + tmp_x, node_01_y + tmp_y});
                            }
                            else
                            {
                                node_01_x = (connection_matrix[i].MUST_THROUGH[j].ptr_data[0]+connection_matrix[i].MUST_THROUGH[j].ptr_data[2])/2;
                                node_01_y = connection_matrix[i].MUST_THROUGH[j].ptr_data[1];

                                node_list.push_back({node_01_x + tmp_x, node_01_y + tmp_y});
                            }

                            break;
                        }
                    }
                }

            }
            else
            {
                cout << "How does we get this step?" << endl;
            }
            for(int k=0;k<connection_matrix[i].MUST_THROUGH[j].ptr_data.size();k++)
            {
                cout << "MUST_THROUGH_data" << k << ": " << connection_matrix[i].MUST_THROUGH[j].ptr_data[k] << endl;
            }
            cout << "!!!!!!!!!!!!!!!!" << endl;
        }
        else
        {
            cout << "NO four data and eight data!!!!!!" << endl;
        }
    }

}

void find_HMFT_point(string &HMFT_MUST_Through_blk_name, string &TX_blockname, string &RX_blockname, vector<vector<float>> &node_list, vector<vector<float>> &HMFT_new_node_list, vector<NETS>& connection_matrix, vector<block_data> &block_placing, vector<region_area>& region_placing, int i)
{
    for(int j=0;j<connection_matrix[i].HMFT_MUST_THROUGH.size();j++)
    {
        float node_01_x = 0.0,node_01_y = 0.0,node_02_x = 0.0 ,node_02_y = 0.0;
        float tmp_x = 0.0 ,tmp_y = 0.0; // store the block's or the region's left down point
        HMFT_MUST_Through_blk_name = connection_matrix[i].HMFT_MUST_THROUGH[j].BLK_name;
        HMFT_MUST_Through_blk_name.erase(remove(HMFT_MUST_Through_blk_name.begin(), HMFT_MUST_Through_blk_name.end(), ' '), HMFT_MUST_Through_blk_name.end());

        cout << "HMFT_MUST_THROUGH_Name:" << HMFT_MUST_Through_blk_name << endl;

        if(HMFT_MUST_Through_blk_name[0] =='R')//region_placing
        {
            auto it = find_if(region_placing.begin(), region_placing.end(), [&HMFT_MUST_Through_blk_name](const region_area& HMFT) {
            return HMFT.reg_name == HMFT_MUST_Through_blk_name;
            });
            if (it != region_placing.end()) {
                tmp_x = 0;
                tmp_y = 0;
                cout << "tmp_x: " << tmp_x << ", " << "tmp_y: " << tmp_y << endl;
            }
        }
        else if(HMFT_MUST_Through_blk_name[0] =='B')//block_placing
        {
            auto it = find_if(block_placing.begin(), block_placing.end(), [&HMFT_MUST_Through_blk_name](const block_data& HMFT) {
            return HMFT.blk_name == HMFT_MUST_Through_blk_name;
            });
            if (it != block_placing.end()) {
                tmp_x = it->place[0];
                tmp_y = it->place[1];
                cout << "tmp_x: " << tmp_x << ", " << "tmp_y: " << tmp_y << endl;
            }
        }
        else// I don't know but may be occur
        {
            cout << "FFFFFFFFFFFFFFFFF" << endl;
        }

        if(connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data.size() == 8)
        {
            for(int k=0;k<connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data.size();k++)
            {
                cout << "HMFT_MUST_THROUGH_data" << k << ": " << connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[k] << endl;
            }
            cout << "!!!!!!!!!!!!!!!!" << endl;

            if(connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[0]==connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[4] && connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[2]==connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[6])
            {//上下邊為平行,所以希望畫出的線是垂直
                node_01_x = (connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[0]+connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[2])/2;//需要優化或是不用
                node_01_y = connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[1];
                node_02_x = node_01_x;
                node_02_y = connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[5];

                node_list.push_back({node_01_x + tmp_x, node_01_y + tmp_y});
                node_list.push_back({node_02_x + tmp_x, node_02_y + tmp_y});
            }
            else if(connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[1]==connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[5] && connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[3]==connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[7])
            {//左右邊為平行,所以希望畫出的線是平行
                node_01_x = connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[0];
                node_01_y = (connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[1]+connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[3])/2;//需要優化或是不用
                node_02_x = connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[4];
                node_02_y = node_01_y;

                node_list.push_back({node_01_x + tmp_x, node_01_y + tmp_y});
                node_list.push_back({node_02_x + tmp_x, node_02_y + tmp_y});
            }
            else
            {
                node_01_x = (connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[0]+connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[2])/2;
                node_01_y = (connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[1]+connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[3])/2;
                node_02_x = (connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[4]+connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[6])/2;
                node_02_y = (connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[5]+connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[7])/2;

                node_list.push_back({node_01_x + tmp_x, node_01_y + tmp_y});
                node_list.push_back({node_02_x + tmp_x, node_02_y + tmp_y});
            }
        }
        else if (connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data.size() == 4)
        {
            if(connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[0]==connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[2])
            {
                if(HMFT_MUST_Through_blk_name == TX_blockname)
                {
                    if((connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[1]<=connection_matrix[i].TX_COORD[1] && connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[3]>=connection_matrix[i].TX_COORD[1])
                        || (connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[1]>=connection_matrix[i].TX_COORD[1] && connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[3]<=connection_matrix[i].TX_COORD[1]))
                    {
                        node_01_x = connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[0];
                        node_01_y = connection_matrix[i].TX_COORD[1];

                        node_list.push_back({node_01_x + tmp_x, node_01_y + tmp_y});
                    }
                    else
                    {
                        node_01_x = connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[0];
                        node_01_y = (connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[1]+connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[3])/2;

                        node_list.push_back({node_01_x + tmp_x, node_01_y + tmp_y});
                    }

                    HMFT_new_node_list.push_back({connection_matrix[i].TX_COORD[0] + tmp_x, connection_matrix[i].TX_COORD[1] + tmp_y});
                    HMFT_new_node_list.push_back({node_01_x + tmp_x, node_01_y + tmp_y});
                }
                else
                {
                    for(int a=0; a<connection_matrix[i].RX.size(); a++)
                    {
                        RX_blockname = connection_matrix[i].RX[a];
                        RX_blockname.erase(remove(RX_blockname.begin(), RX_blockname.end(), ' '), RX_blockname.end());
                        if(HMFT_MUST_Through_blk_name == RX_blockname)
                        {
                            if((connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[1]<=connection_matrix[i].RX_COORD[a][1] && connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[3]>=connection_matrix[i].RX_COORD[a][1])
                                || (connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[1]>=connection_matrix[i].RX_COORD[a][1] && connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[3]<=connection_matrix[i].RX_COORD[a][1]))
                            {
                                node_01_x = connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[0];
                                node_01_y = connection_matrix[i].RX_COORD[a][1];

                                node_list.push_back({node_01_x + tmp_x, node_01_y + tmp_y});
                            }
                            else
                            {
                                node_01_x = connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[0];
                                node_01_y = (connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[1]+connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[3])/2;

                                node_list.push_back({node_01_x + tmp_x, node_01_y + tmp_y});
                            }

                            HMFT_new_node_list.push_back({connection_matrix[i].RX_COORD[a][0] + tmp_x, connection_matrix[i].RX_COORD[a][1] + tmp_y});
                            HMFT_new_node_list.push_back({node_01_x + tmp_x, node_01_y + tmp_y});

                            break;
                        }
                    }
                }
            }
            else if(connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[1]==connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[3])
            {
                if(HMFT_MUST_Through_blk_name == TX_blockname)
                {
                    if((connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[0]<=connection_matrix[i].TX_COORD[0] && connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[2]>=connection_matrix[i].TX_COORD[0])
                        || (connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[0]>=connection_matrix[i].TX_COORD[0] && connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[2]<=connection_matrix[i].TX_COORD[0]))
                    {
                        node_01_x = connection_matrix[i].TX_COORD[0];
                        node_01_y = connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[1];

                        node_list.push_back({node_01_x + tmp_x, node_01_y + tmp_y});
                    }
                    else
                    {
                        node_01_x = (connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[0]+connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[2])/2;
                        node_01_y = connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[1];

                        node_list.push_back({node_01_x + tmp_x, node_01_y + tmp_y});
                    }

                    HMFT_new_node_list.push_back({connection_matrix[i].TX_COORD[0] + tmp_x, connection_matrix[i].TX_COORD[1] + tmp_y});
                    HMFT_new_node_list.push_back({node_01_x + tmp_x, node_01_y + tmp_y});
                }
                else
                {

                    for(int a=0; a<connection_matrix[i].RX.size(); a++)
                    {
                        RX_blockname = connection_matrix[i].RX[a];
                        RX_blockname.erase(remove(RX_blockname.begin(), RX_blockname.end(), ' '), RX_blockname.end());
                        if(HMFT_MUST_Through_blk_name == RX_blockname)
                        {
                            if((connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[0]<=connection_matrix[i].RX_COORD[a][0] && connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[2]>=connection_matrix[i].RX_COORD[a][0])
                                || (connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[0]>=connection_matrix[i].RX_COORD[a][0] && connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[2]<=connection_matrix[i].RX_COORD[a][0]))
                            {
                                node_01_x = connection_matrix[i].RX_COORD[a][0];
                                node_01_y = connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[1];

                                node_list.push_back({node_01_x + tmp_x, node_01_y + tmp_y});
                            }
                            else
                            {
                                node_01_x = (connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[0]+connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[2])/2;
                                node_01_y = connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[1];

                                node_list.push_back({node_01_x + tmp_x, node_01_y + tmp_y});
                            }
                            HMFT_new_node_list.push_back({connection_matrix[i].RX_COORD[a][0] + tmp_x, connection_matrix[i].RX_COORD[a][1] + tmp_y});
                            HMFT_new_node_list.push_back({node_01_x + tmp_x, node_01_y + tmp_y});
                            cout << "here" << endl;
                            break;
                        }

                    }
                    node_01_x = (connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[0]+connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[2])/2;
                    node_01_y = (connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[1]+connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[3])/2;
                    node_list.push_back({node_01_x + tmp_x, node_01_y + tmp_y});

                }

            }
            else
            {
                cout << "How does we get this step?" << endl;
            }
            for(int k=0;k<connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data.size();k++)
            {
                cout << "HMFT_MUST_THROUGH_data" << k << ": " << connection_matrix[i].HMFT_MUST_THROUGH[j].ptr_data[k] << endl;
            }
            cout << "!!!!!!!!!!!!!!!!" << endl;

        }
        else
        {
            cout << "NO four data and eight data!!!!!!" << endl;
        }

    }

}
