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
#include"OARST.h"
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








void addEdge(unordered_map<vertex, unordered_map<vertex, float>> &adj_lists, vertex u, vertex v) {
    float weight = fabs(u.x - v.x) + fabs(u.y - v.y);
    adj_lists[u][v] = weight;
    adj_lists[v][u] = weight;
}

int find_longest_slant_edge(vector<pair<vertex, vertex>> MST){
    int index,length=0;
    if(MST.size()==0) return -1;
    for(int i=0;i<MST.size();i++){

            if((fabs(MST[i].first.x-MST[i].second.x)+fabs(MST[i].first.y-MST[i].second.y))>length && fabs(MST[i].first.x-MST[i].second.x)!=0 && fabs(MST[i].first.y-MST[i].second.y)!=0){
                length=fabs(MST[i].first.x-MST[i].second.x)+fabs(MST[i].first.y-MST[i].second.y);
                index=i;
            }
        }//find longest edge
    
    return index;
}
int find_longest_edge(vector<pair<vertex, vertex>> MST){
    int index,length=0;
    if(MST.size()==0) return -1;
    for(int i=0;i<MST.size();i++){

            if((fabs(MST[i].first.x-MST[i].second.x)+fabs(MST[i].first.y-MST[i].second.y))>length){
                length=fabs(MST[i].first.x-MST[i].second.x)+fabs(MST[i].first.y-MST[i].second.y);
                index=i;
            }
        }//find longest edge
    
    return index;
}
vector<pair<vertex, vertex>> find_neighbor(unordered_map <vertex, unordered_map<vertex, float>>adj_lists,vertex pt1,vertex pt2){
    vector<pair<vertex, vertex>> neighbor;
    for (const auto& kkk : adj_lists[pt1]) {
        pair <vertex,vertex> temp_pair;
        temp_pair.first=pt1;
        temp_pair.second=kkk.first;
        //keys.push_back(pair.first);
        if(kkk.first.x!=pt2.x || kkk.first.y!=pt2.y ){
            neighbor.push_back(temp_pair);
        }
            
    }
    for (const auto& kkk : adj_lists[pt2]) {
        pair <vertex,vertex> temp_pair;
        temp_pair.first=pt2;
        temp_pair.second=kkk.first;
        //keys.push_back(pair.first);
        if(kkk.first.x!=pt1.x || kkk.first.y!=pt1.y ){
            neighbor.push_back(temp_pair);
        }
            
    }


    return neighbor;

}

vertex findCommonVertex(const pair<vertex, vertex>& E1, const pair<vertex, vertex>& E2) {
    // 检查 E1 和 E2 的所有端点，找出共同点
    if (E1.first == E2.first || E1.first == E2.second) {
        return E1.first;
    }
    if (E1.second == E2.first || E1.second == E2.second) {
        return E1.second;
    }

    // 如果没有共同点，返回一个默认的 vertex
    return vertex();
}
int Find_quadrant(pair<vertex, vertex> Edges){
    float x_diff=Edges.second.x-Edges.first.x;
    float y_diff=Edges.second.y-Edges.first.y;
    if(x_diff > 0 && y_diff>0){
        return 1;
    }
    else if(x_diff < 0 && y_diff>0){
        return 2;
    }
    else if(x_diff < 0 && y_diff<0){
        return 3;
    }
    else if(x_diff > 0 && y_diff<0){
        return 4;
    }
    else if(x_diff > 0 && y_diff == 0){
        return 5;//+x
    }
    else if(x_diff == 0 && y_diff > 0){
        return 6;//+y
    }
    else if(x_diff < 0 && y_diff == 0){
        return 7;//-x
    }
    else if(x_diff == 0 && y_diff < 0){
        return 8;//-y
    }
    return 9;

}
void push_in_OARST_oppo(pair<vertex, vertex> Edge,vertex Vertex_common,vector<pair<vertex, vertex>> & OARST_result){
    pair<vertex, vertex> temp_1,temp_2;
            temp_1.first=Edge.second;
            temp_1.second.x=Vertex_common.x;
            temp_1.second.y=Edge.second.y;
            OARST_result.push_back(temp_1);
            temp_2.first=Vertex_common;
            temp_2.second=Vertex_common;

            temp_2.first=temp_1.second;
            //temp_2.second=Edge.first;
            OARST_result.push_back(temp_2);

}
void push_in_OARST_neighbor(pair<vertex, vertex> E1,pair<vertex, vertex> E2
,vertex Vertex_common,vector<pair<vertex, vertex>> & OARST_result,int E1_quadrant,int E2_quadrant){
    cout<<"in neighbor"<<endl;
    pair<vertex, vertex> e1,e2,e3,e4;
    vertex pt1=E1.second,pt2=E2.second;
    if(max(E1_quadrant,E2_quadrant)==2){
        if(pt1.y>=pt2.y){
            e1.first=pt1;
            e1.second.x=Vertex_common.x;
            e1.second.y=pt1.y;
            e3.first=pt2;
            e3.second.x=Vertex_common.x;
            e3.second.y=pt2.y;
            e2.first=e1.second;
            e2.second=e3.second;
            e4.first=e2.second;
            e4.second=Vertex_common;
            
        }
        else{
            e1.first=pt2;
            e1.second.x=Vertex_common.x;
            e1.second.y=pt2.y;
            e3.first=pt1;
            e3.second.x=Vertex_common.x;
            e3.second.y=pt1.y;
            e2.first=e1.second;
            e2.second=e3.second;
            e4.first=e2.second;
            e4.second=Vertex_common;
            
        }
    }
    else if(max(E1_quadrant,E2_quadrant)==3){
        if(pt1.x<=pt2.x){
            e1.first=pt1;
            e1.second.x=pt1.x;
            e1.second.y=Vertex_common.y;
            e3.first=pt2;
            e3.second.x=pt2.x;
            e3.second.y=Vertex_common.y;
            e2.first=e1.second;
            e2.second=e3.second;
            e4.first=e2.second;
            e4.second=Vertex_common;
        }
        else{
            e1.first=pt2;
            e1.second.x=pt2.x;
            e1.second.y=Vertex_common.y;
            e3.first=pt1;
            e3.second.x=pt1.x;
            e3.second.y=Vertex_common.y;
            e2.first=e1.second;
            e2.second=e3.second;
            e4.first=e2.second;
            e4.second=Vertex_common;
        }
    }
    else if(max(E1_quadrant,E2_quadrant)==4 && min(E1_quadrant,E2_quadrant)==3){
        if(pt1.y<=pt2.y){
            e1.first=pt1;
            e1.second.x=Vertex_common.x;
            e1.second.y=pt1.y;
            e3.first=pt2;
            e3.second.x=Vertex_common.x;
            e3.second.y=pt2.y;
            e2.first=e1.second;
            e2.second=e3.second;
            e4.first=e2.second;
            e4.second=Vertex_common;
        }
        else{
            e1.first=pt2;
            e1.second.x=Vertex_common.x;
            e1.second.y=pt2.y;
            e3.first=pt1;
            e3.second.x=Vertex_common.x;
            e3.second.y=pt1.y;
            e2.first=e1.second;
            e2.second=e3.second;
            e4.first=e2.second;
            e4.second=Vertex_common;
        }            

    }
    else{
        if(pt1.x>=pt2.x){
            e1.first=pt1;
            e1.second.x=pt1.x;
            e1.second.y=Vertex_common.y;
            e3.first=pt2;
            e3.second.x=pt2.x;
            e3.second.y=Vertex_common.y;
            e2.first=e1.second;
            e2.second=e3.second;
            e4.first=e2.second;
            e4.second=Vertex_common;
        }
        else{
            e1.first=pt2;
            e1.second.x=pt2.x;
            e1.second.y=Vertex_common.y;
            e3.first=pt1;
            e3.second.x=pt1.x;
            e3.second.y=Vertex_common.y;
            e2.first=e1.second;
            e2.second=e3.second;
            e4.first=e2.second;
            e4.second=Vertex_common;
        }
    }   
    OARST_result.push_back(e1);
    OARST_result.push_back(e2);
    OARST_result.push_back(e3);
    OARST_result.push_back(e4);
}

void push_in_OARST_same(pair<vertex, vertex> E1,pair<vertex, vertex> E2,vertex Vertex_common
,vector<pair<vertex, vertex>> & OARST_result,int E1_quadrant,int E2_quadrant){
    pair<vertex, vertex> e1,e2,e3,e4;
    vertex pt1=E1.second,pt2=E2.second;
    vertex pt3,pt4;
    float E1_slope=(E1.second.y-E1.first.y)/(E1.second.x-E1.first.x);
    float E2_slope=(E2.second.y-E2.first.y)/(E2.second.x-E2.first.x);
    if(E1_quadrant==1){
        pt3.x=min(pt1.x,pt2.x);
        pt3.y=min(pt1.y,pt2.y);
        if(pt3==pt1 || pt3==pt2){
            if(pt1==pt3)pt4=pt2;
            else pt4=pt1;
            e1.first=pt4;
            e1.second.x=pt4.x;
            e1.second.y=pt3.y;
            e2.first=e1.second;
            e2.second=pt3;
            e3.first=e2.second;
            e3.second.x=Vertex_common.x;
            e3.second.y=pt3.y;
            e4.first=e3.second;
            e4.second=Vertex_common;
            OARST_result.push_back(e1);
            OARST_result.push_back(e2);
            OARST_result.push_back(e3);
            OARST_result.push_back(e4);

        }
        else{
            e1.first=pt1;
            e1.second=pt3;
            e2.first=pt2;
            e2.second=pt3;
            e3.first=pt3;
            e3.second.x=Vertex_common.x;
            e3.second.y=pt3.y;
            e4.first=e3.second;
            e4.second=Vertex_common;
            OARST_result.push_back(e1);
            OARST_result.push_back(e2);
            OARST_result.push_back(e3);
            OARST_result.push_back(e4);
        }
    }
    else if(E1_quadrant==2){
        pt3.x=max(pt1.x,pt2.x);
        pt3.y=min(pt1.y,pt2.y);
        if(pt3==pt1 || pt3==pt2){
            if(pt1==pt3)pt4=pt2;
            else pt4=pt1;
            e1.first=pt4;
            e1.second.x=pt4.x;
            e1.second.y=pt3.y;
            e2.first=e1.second;
            e2.second=pt3;
            e3.first=e2.second;
            e3.second.x=Vertex_common.x;
            e3.second.y=pt3.y;
            e4.first=e3.second;
            e4.second=Vertex_common;
            OARST_result.push_back(e1);
            OARST_result.push_back(e2);
            OARST_result.push_back(e3);
            OARST_result.push_back(e4);
        }
        else{
            e1.first=pt1;
            e1.second=pt3;
            e2.first=pt2;
            e2.second=pt3;
            e3.first=pt3;
            e3.second.x=Vertex_common.x;
            e3.second.y=pt3.y;
            e4.first=e3.second;
            e4.second=Vertex_common;
            OARST_result.push_back(e1);
            OARST_result.push_back(e2);
            OARST_result.push_back(e3);
            OARST_result.push_back(e4);
        }
    }
    else if(E1_quadrant==3){
        pt3.x=max(pt1.x,pt2.x);
        pt3.y=max(pt1.y,pt2.y);
        if(pt3==pt1 || pt3==pt2){
            if(pt1==pt3)pt4=pt2;
            else pt4=pt1;
            e1.first=pt4;
            e1.second.x=pt4.x;
            e1.second.y=pt3.y;
            e2.first=e1.second;
            e2.second=pt3;
            e3.first=e2.second;
            e3.second.x=Vertex_common.x;
            e3.second.y=pt3.y;
            e4.first=e3.second;
            e4.second=Vertex_common;
            OARST_result.push_back(e1);
            OARST_result.push_back(e2);
            OARST_result.push_back(e3);
            OARST_result.push_back(e4);
        }
        else{
            e1.first=pt1;
            e1.second=pt3;
            e2.first=pt2;
            e2.second=pt3;
            e3.first=pt3;
            e3.second.x=Vertex_common.x;
            e3.second.y=pt3.y;
            e4.first=e3.second;
            e4.second=Vertex_common;
            OARST_result.push_back(e1);
            OARST_result.push_back(e2);
            OARST_result.push_back(e3);
            OARST_result.push_back(e4);
        }
    }
    else if(E1_quadrant==4){
        pt3.x=min(pt1.x,pt2.x);
        pt3.y=max(pt1.y,pt2.y);
        if(pt3==pt1 || pt3==pt2){
            if(pt1==pt3)pt4=pt2;
            else pt4=pt1;
            e1.first=pt4;
            e1.second.x=pt4.x;
            e1.second.y=pt3.y;
            e2.first=e1.second;
            e2.second=pt3;
            e3.first=e2.second;
            e3.second.x=Vertex_common.x;
            e3.second.y=pt3.y;
            e4.first=e3.second;
            e4.second=Vertex_common;
            OARST_result.push_back(e1);
            OARST_result.push_back(e2);
            OARST_result.push_back(e3);
            OARST_result.push_back(e4);
        }
        else{
            e1.first=pt1;
            e1.second=pt3;
            e2.first=pt2;
            e2.second=pt3;
            e3.first=pt3;
            e3.second.x=Vertex_common.x;
            e3.second.y=pt3.y;
            e4.first=e3.second;
            e4.second=Vertex_common;
            OARST_result.push_back(e1);
            OARST_result.push_back(e2);
            OARST_result.push_back(e3);
            OARST_result.push_back(e4);
        }
    }
}
void push_in_OARST_with_ev(pair<vertex, vertex> E_ev,pair<vertex, vertex> E_slant,vertex Vertex_common
,vector<pair<vertex, vertex>> & OARST_result,int E_ev_quadrant,int E_slant_quadrant){
    vertex pt1,pt2,pt3;
    pt1=E_slant.second;//傾斜
    pt2=E_ev.second;//垂直or水平
    pair<vertex, vertex> e1,e2,e3;
    if(E_ev_quadrant==5){//+x
        if(E_slant_quadrant==1){
            pt3.x=min(pt1.x,pt2.x);
            pt3.y=min(pt1.y,pt2.y);
            if(pt3==pt2){
                e1.first=pt1;
                e1.second.x=pt1.x;
                e1.second.y=pt2.y;
                e2.first=e1.second;
                e2.second=pt3;
                e3.first=e2.second;
                e3.second=Vertex_common;
                OARST_result.push_back(e1);
                OARST_result.push_back(e2);
                OARST_result.push_back(e3);
                return;
            }
            else{
                e1.first=pt1;
                e1.second=pt3;
                e2.first=pt2;
                e2.second=pt3;
                e3.first=pt3;
                e3.second=Vertex_common;
                OARST_result.push_back(e1);
                OARST_result.push_back(e2);
                OARST_result.push_back(e3);
                return;
            }
        }
        else if(E_slant_quadrant==2){
            e1.first=pt1;
            e1.second.x=Vertex_common.x;
            e1.second.y=pt1.y;
            e2.first=e1.second;
            e2.second=Vertex_common;
            e3.first=pt2;
            e3.second=Vertex_common;
            OARST_result.push_back(e1);
            OARST_result.push_back(e2);
            OARST_result.push_back(e3);
            return;
        }
        else if(E_slant_quadrant==3){
            e1.first=pt1;
            e1.second.x=Vertex_common.x;
            e1.second.y=pt1.y;
            e2.first=e1.second;
            e2.second=Vertex_common;
            e3.first=pt2;
            e3.second=Vertex_common;
            OARST_result.push_back(e1);
            OARST_result.push_back(e2);
            OARST_result.push_back(e3);
            return;
        }
        else if(E_slant_quadrant==4){
            pt3.x=min(pt1.x,pt2.x);
            pt3.y=max(pt1.y,pt2.y);
            if(pt3==pt2){
                e1.first=pt1;
                e1.second.x=pt1.x;
                e1.second.y=pt2.y;
                e2.first=e1.second;
                e2.second=pt3;
                e3.first=e2.second;
                e3.second=Vertex_common;
                OARST_result.push_back(e1);
                OARST_result.push_back(e2);
                OARST_result.push_back(e3);
                return;
            }
            else{
                e1.first=pt1;
                e1.second=pt3;
                e2.first=pt2;
                e2.second=pt3;
                e3.first=pt3;
                e3.second=Vertex_common;
                OARST_result.push_back(e1);
                OARST_result.push_back(e2);
                OARST_result.push_back(e3);
                return;
            }
        }
    }
    else if(E_ev_quadrant==6){//+y
        if(E_slant_quadrant==1){
            pt3.x=min(pt1.x,pt2.x);
            pt3.y=min(pt1.y,pt2.y);
            if(pt3==pt2){
                e1.first=pt1;
                e1.second.x=pt3.x;
                e1.second.y=pt1.y;
                e2.first=e1.second;
                e2.second=pt3;
                e3.first=e2.second;
                e3.second=Vertex_common;
                OARST_result.push_back(e1);
                OARST_result.push_back(e2);
                OARST_result.push_back(e3);
                return;
            }
            else{
                e1.first=pt1;
                e1.second=pt3;
                e2.first=pt2;
                e2.second=pt3;
                e3.first=pt3;
                e3.second=Vertex_common;
                OARST_result.push_back(e1);
                OARST_result.push_back(e2);
                OARST_result.push_back(e3);
                return;
            }
        }
        else if(E_slant_quadrant==2){
            pt3.x=max(pt1.x,pt2.x);
            pt3.y=min(pt1.y,pt2.y);
            if(pt3==pt2){
                e1.first=pt1;
                e1.second.x=pt3.x;
                e1.second.y=pt1.y;
                e2.first=e1.second;
                e2.second=pt3;
                e3.first=e2.second;
                e3.second=Vertex_common;
                OARST_result.push_back(e1);
                OARST_result.push_back(e2);
                OARST_result.push_back(e3);
                return;
            }
            else{
                e1.first=pt1;
                e1.second=pt3;
                e2.first=pt2;
                e2.second=pt3;
                e3.first=pt3;
                e3.second=Vertex_common;
                OARST_result.push_back(e1);
                OARST_result.push_back(e2);
                OARST_result.push_back(e3);
                return;
            }
        }
        else if(E_slant_quadrant==3){
            e1.first=pt1;
            e1.second.x=pt1.x;
            e1.second.y=Vertex_common.y;
            e2.first=e1.second;
            e2.second=Vertex_common;
            e3.first=pt2;
            e3.second=Vertex_common;
            OARST_result.push_back(e1);
            OARST_result.push_back(e2);
            OARST_result.push_back(e3);
            return;
        }
        else if(E_slant_quadrant==4){
            e1.first=pt1;
            e1.second.x=pt1.x;
            e1.second.y=Vertex_common.y;
            e2.first=e1.second;
            e2.second=Vertex_common;
            e3.first=pt2;
            e3.second=Vertex_common;
            OARST_result.push_back(e1);
            OARST_result.push_back(e2);
            OARST_result.push_back(e3);
            return;
        }
    }
    else if(E_ev_quadrant==7){//-x
        if(E_slant_quadrant==1){
            e1.first=pt1;
            e1.second.x=pt1.x;
            e1.second.y=Vertex_common.y;
            e2.first=e1.second;
            e2.second=Vertex_common;
            e3.first=pt2;
            e3.second=Vertex_common;
            OARST_result.push_back(e1);
            OARST_result.push_back(e2);
            OARST_result.push_back(e3);
            return;
        }
        else if(E_slant_quadrant==2){
            pt3.x=max(pt1.x,pt2.x);
            pt3.y=min(pt1.y,pt2.y);
            if(pt3==pt2){
                e1.first=pt1;
                e1.second.x=pt1.x;
                e1.second.y=pt3.y;
                e2.first=e1.second;
                e2.second=pt3;
                e3.first=e2.second;
                e3.second=Vertex_common;
                OARST_result.push_back(e1);
                OARST_result.push_back(e2);
                OARST_result.push_back(e3);
                return;
            }
            else{
                e1.first=pt1;
                e1.second=pt3;
                e2.first=pt2;
                e2.second=pt3;
                e3.first=pt3;
                e3.second=Vertex_common;
                OARST_result.push_back(e1);
                OARST_result.push_back(e2);
                OARST_result.push_back(e3);
                return;
            }
        }
        else if(E_slant_quadrant==3){
            pt3.x=max(pt1.x,pt2.x);
            pt3.y=max(pt1.y,pt2.y);
            if(pt3==pt2){
                e1.first=pt1;
                e1.second.x=pt1.x;
                e1.second.y=pt3.y;
                e2.first=e1.second;
                e2.second=pt3;
                e3.first=e2.second;
                e3.second=Vertex_common;
                OARST_result.push_back(e1);
                OARST_result.push_back(e2);
                OARST_result.push_back(e3);
                return;
            }
            else{
                e1.first=pt1;
                e1.second=pt3;
                e2.first=pt2;
                e2.second=pt3;
                e3.first=pt3;
                e3.second=Vertex_common;
                OARST_result.push_back(e1);
                OARST_result.push_back(e2);
                OARST_result.push_back(e3);
                return;
            }
        }
        else if(E_slant_quadrant==4){
            e1.first=pt1;
            e1.second.x=Vertex_common.x;
            e1.second.y=pt1.y;
            e2.first=e1.second;
            e2.second=Vertex_common;
            e3.first=pt2;
            e3.second=Vertex_common;
            OARST_result.push_back(e1);
            OARST_result.push_back(e2);
            OARST_result.push_back(e3);
            return;
        }
    }
    else if(E_ev_quadrant==8){//-y
        if(E_slant_quadrant==1){
            e1.first=pt1;
            e1.second.x=pt1.x;
            e1.second.y=Vertex_common.y;
            e2.first=e1.second;
            e2.second=Vertex_common;
            e3.first=pt2;
            e3.second=Vertex_common;
            OARST_result.push_back(e1);
            OARST_result.push_back(e2);
            OARST_result.push_back(e3);
            return;
        }
        else if(E_slant_quadrant==2){
            e1.first=pt1;
            e1.second.x=pt1.x;
            e1.second.y=Vertex_common.y;
            e2.first=e1.second;
            e2.second=Vertex_common;
            e3.first=pt2;
            e3.second=Vertex_common;
            OARST_result.push_back(e1);
            OARST_result.push_back(e2);
            OARST_result.push_back(e3);
            return;
        }
        else if(E_slant_quadrant==3){
            pt3.x=max(pt1.x,pt2.x);
            pt3.y=max(pt1.y,pt2.y);
            if(pt3==pt2){
                e1.first=pt1;
                e1.second.x=pt3.x;
                e1.second.y=pt1.y;
                e2.first=e1.second;
                e2.second=pt3;
                e3.first=e2.second;
                e3.second=Vertex_common;
                OARST_result.push_back(e1);
                OARST_result.push_back(e2);
                OARST_result.push_back(e3);
                return;
            }
            else{
                e1.first=pt1;
                e1.second=pt3;
                e2.first=pt2;
                e2.second=pt3;
                e3.first=pt3;
                e3.second=Vertex_common;
                OARST_result.push_back(e1);
                OARST_result.push_back(e2);
                OARST_result.push_back(e3);
                return;
            }
        }
        else if(E_slant_quadrant==4){
            pt3.x=min(pt1.x,pt2.x);
            pt3.y=max(pt1.y,pt2.y);
            if(pt3==pt2){
                e1.first=pt1;
                e1.second.x=pt3.x;
                e1.second.y=pt1.y;
                e2.first=e1.second;
                e2.second=pt3;
                e3.first=e2.second;
                e3.second=Vertex_common;
                OARST_result.push_back(e1);
                OARST_result.push_back(e2);
                OARST_result.push_back(e3);
                return;
            }
            else{
                e1.first=pt1;
                e1.second=pt3;
                e2.first=pt2;
                e2.second=pt3;
                e3.first=pt3;
                e3.second=Vertex_common;
                OARST_result.push_back(e1);
                OARST_result.push_back(e2);
                OARST_result.push_back(e3);
                return;
            }
        }
    }

}
void partition_edge(vector<pair<vertex, vertex>> & OARST_result,pair<vertex, vertex> E1, pair<vertex, vertex> E2){
    vertex Vertex_common=findCommonVertex(E1,E2);
    if(E1.first.y!=Vertex_common.y && E1.first.x!=Vertex_common.x){
        E1.second=E1.first;
        E1.first=Vertex_common;
    }
    if(E2.first.y!=Vertex_common.y && E2.first.x!=Vertex_common.x){
        E2.second=E2.first;
        E2.first=Vertex_common;
    }
    int E1_quadrant=Find_quadrant(E1),E2_quadrant=Find_quadrant(E2);
    cout<<"E1_quadrant: "<<E1_quadrant<<" E2_quadrant "<<E2_quadrant<<endl;
    cout<<E1.first.x<<" "<<E1.first.y<<" "<<E1.second.x<<" "<<E1.second.y<<endl<<E2.first.x<<" "<<E2.first.y<<" "<<E2.second.x<<" "<<E2.second.y<<" "<<endl;
    if(abs(E1_quadrant-E2_quadrant)==2 && E1_quadrant<5 &&E2_quadrant<5 ){//at opponent quadrant //both slant
        //cout<<"at opponent quadrant"<<endl;
        //cout<<E1.first.x<<" "<<E1.first.y<<" "<<E1.second.x<<" "<<E1.second.y<<" "<<E2.first.x<<" "<<E2.first.y<<" "<<E2.second.x<<" "<<E2.second.y<<" "<<endl;
        float E1_length=abs(E1.second.x-E1.first.x)+abs(E1.second.y-E1.first.y);
        float E2_length=abs(E2.second.x-E2.first.x)+abs(E2.second.y-E2.first.y);
        if(E1_length>=E2_length){
            remove_edge(OARST_result,E1);
            push_in_OARST_oppo(E1,Vertex_common,OARST_result);
            
        }
        else{
            remove_edge(OARST_result,E2);
            push_in_OARST_oppo(E2,Vertex_common,OARST_result);
            
        }

    }
    else if((abs(E1_quadrant-E2_quadrant)==1 || abs(E1_quadrant-E2_quadrant)==3) && E1_quadrant<5 &&E2_quadrant<5 ){//at neighbor quadrant //both slant
        remove_edge(OARST_result,E1);
        remove_edge(OARST_result,E2);
        push_in_OARST_neighbor(E1,E2,Vertex_common,OARST_result,E1_quadrant,E2_quadrant);
        
    }
    else if(abs(E1_quadrant-E2_quadrant)==0 && E1_quadrant<5 &&E2_quadrant<5 ){//at same quadrant //both slant
        remove_edge(OARST_result,E1);
        remove_edge(OARST_result,E2);
        push_in_OARST_same(E1,E2,Vertex_common,OARST_result,E1_quadrant,E2_quadrant );
        
    }
    else if(E1_quadrant>=5 || E2_quadrant>=5){
        if(E1_quadrant>=5 && E2_quadrant>=5){
            // OARST_result.push_back(E1);
            // OARST_result.push_back(E2);
            // remove_edge(OARST_result,E1);
            // remove_edge(OARST_result,E2);
            return ;
        }
        else if(E1_quadrant>=5 && E2_quadrant<5){
            push_in_OARST_with_ev(E1,E2,Vertex_common,OARST_result,E1_quadrant,E2_quadrant );
            
            remove_edge(OARST_result,E2);
            //OARST_result.push_back(E2);
        }
        else if(E1_quadrant<5 && E2_quadrant>=5){
            //cout<<"fewfe"<<endl;
            //OARST_result.push_back(E1);
            push_in_OARST_with_ev(E2,E1,Vertex_common,OARST_result,E2_quadrant,E1_quadrant );
            
            remove_edge(OARST_result,E1);
        }
    }






}
bool is_there_slant(vector<pair<vertex, vertex>> MST ){
    for(int i=0;i<MST.size();i++){
        if((MST[i].first.x-MST[i].second.x)!=0 && (MST[i].first.y-MST[i].second.y)!=0)return 1;
        
    }
    return 0;
}
void remove_edge(vector<pair<vertex, vertex>>& MST,pair<vertex, vertex> E1){
    pair<vertex, vertex> temp;
    temp.first=E1.second;
    temp.second=E1.first;
    if(find(MST.begin(),MST.end(),E1)!=MST.end()){
        auto new_end = remove(MST.begin(), MST.end(), E1);
        MST.erase(new_end, MST.end());
    }
    else{
        auto new_end = remove(MST.begin(), MST.end(), temp);
        MST.erase(new_end, MST.end());
    }



}
unordered_map <vertex, unordered_map<vertex, float>> build_adj_lists(vector<pair<vertex, vertex>> MST){
    unordered_map <vertex, unordered_map<vertex, float>> result_adj_list;
    for(int i=0;i<MST.size();i++){
        addEdge(result_adj_list, MST[i].first, MST[i].second);
    }
    return result_adj_list;
}
vector<pair<vertex, vertex>> generate_OARST(unordered_map <vertex, unordered_map<vertex, float>> adj_lists,set<vertex> PINS){
      // set找元素的時間複雜度為O(logn)，比vector的O(n)快 
    //cout<<"In generate_OARST"<<adj_lists.size()<<" "<<PINS.size()<<endl;;
    OAMST oamst(adj_lists, PINS);
    
    //oamst.outputMST();  // 將MST和MST_only_PINS寫入output_OAMST.txt
    //cout<<"kkk"<<endl;
    vector<pair<vertex, vertex>> MST = oamst.MST;   // 結果存放在MST中
    cout<<endl<<"MST_pairs"<<endl;
    
    //test case input end
    cout<<"before while"<<endl;
    //ofstream outFile("_OARST.rpt");
    
    int counter=0;
    while(is_there_slant(MST) && MST.size()!=1 ){
        counter+=1;//counter<=20
        //outFile<<"in while "<<counter<<endl;
        //  for (int i=0;i<MST.size();i++)
        // {
        //     outFile<<real_result[i].pt1[0]<<" "<<real_result[i].pt1[1]<<" "<<" "<<" "<<real_result[i].pt2[0]<<" "<<real_result[i].pt2[1]<<" "<<endl;
        // }
        //for(int i=0;i<MST.size();i++){
            //outFile<<MST[i].first.x<<" "<<MST[i].first.y<<" "<<MST[i].second.x<<" "<<MST[i].second.y<<" "<<endl;
        //}
        int E1_index=find_longest_slant_edge(MST);
        //cout<<"int while 2"<<endl;
        unordered_map <vertex, unordered_map<vertex, float>> MST_adj_list=build_adj_lists(MST);
        //cout<<"int while 3"<<endl;
        pair<vertex, vertex> E1=MST[E1_index];
        vector<pair<vertex, vertex>> neighbors=find_neighbor(MST_adj_list,E1.first,E1.second);
        //cout<<"int while 4"<<endl;
        cout<<counter<<" neighbors.size() "<<neighbors.size()<<endl;
        int E2_index=find_longest_edge(neighbors);
        //cout<<E2_index<<endl;
        
        pair<vertex, vertex> E2=neighbors[E2_index];

        //outFile<<"E1、E2: "<<E1.first.x<<" "<<E1.first.y<<" "<<E1.second.x<<" "<<E1.second.y<<" "<<endl;
        //outFile<<E2.first.x<<" "<<E2.first.y<<" "<<E2.second.x<<" "<<E2.second.y<<" "<<endl;
        
        //cout<<"int while 6"<<endl;
        partition_edge(MST,E1,E2);
        //cout<<"int while 7"<<endl;
        neighbors.clear();
        MST_adj_list.clear();
        //cout<<"int while "<<is_there_slant(MST)<<endl;
       
     }
     vector<pair<vertex, vertex>> OARST_result;
     if( MST.size()==1){
        pair<vertex, vertex> e1,e2;
        pair<vertex, vertex> E1=MST[0];
        cout<<"neighbor.size()==0 && MST.size()==1  333: "<<E1.first.x<<" "<<E1.first.y<<" "<<endl;
        e1.first=E1.first;
        e1.second.x=E1.first.x;
        e1.second.y=E1.second.y;
        e2.first=e1.second;
        e2.second=E1.second;
        OARST_result.push_back(e1);
        OARST_result.push_back(e2);
     }
     else{
        OARST_result=MST;
     }
     cout<<"finish_OARST"<<endl;
    
    
    MST.clear();
    adj_lists.clear();
    unordered_map <vertex, unordered_map<vertex, float>> OARST_result_adj_list=build_adj_lists(OARST_result);
    //set<vertex> PINS
    
    //cout<<"adj_list.size(): "<<OARST_result.size()<<" PINS.size(): "<<PINS.size()<<" "<<OARST_result_adj_list.size()<<endl;
    OAMST oamst_result(OARST_result_adj_list, PINS);
    //cout<<" FInal result: " <<endl;
    OARST_result=oamst_result.MST;
    //cout<<"22adj_list.size(): "<<OARST_result.size()<<" PINS.size(): "<<PINS.size()<<endl;
    //for(int i=0;i<OARST_result.size();i++){
    //            cout<<OARST_result[i].first.x<<" "<<OARST_result[i].first.y<<" "<<OARST_result[i].second.x<<" "<<OARST_result[i].second.y<<" "<<endl;
    //}
    //cout<<"finish"<<endl;
    
    return OARST_result;

}
