#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <set>
#include <unordered_map>
#include<fstream>
#include "OASG.h"
#include"OARST.h"
#include"parser.h"
#include"myclass.h"
using namespace std;
const int INF=2100000000;
int region=0,numOASGedge=0,node=0,obstacle_num=0;
//int k=0;
//const int MAXN = 1000;  // 假设最大节点数为 1000
//vector<int> nex(MAXN),head(MAXN),e(MAXN),len(MAXN),pre(MAXN);
//int ee = 0;             // 初始化边的计数器

//use for OASG left and bottom bound
set<int> bottom_bound;
set<int> left_bound;
//point property: x,y,id
// struct poi
// {
//     float x,y;
//     int id;
// };

// struct edge 
// {
//     // u is the start point, v is the end point and cost is the weight of the edge
//     int u,v;
//     float cost;
// };

// struct obstacle_block
// {
//     //p1 is the left bottom point, p2 is the right top point
//     poi p1,p2;
// };

// struct result_edge{
//     vector<float> pt1;
//     vector<float> pt2;
//     float cost;
// };
// //Ensure point v is in R1 region 
// struct cmpset {
//     bool operator()(const poi& a, const poi& b) const { // 添加 const 關鍵字
//         if (a.x > b.x) return true;  // 使用 true 和 false 代替 1 和 0
//         if (a.x == b.x && a.x - a.y > b.x - b.y) return true;
//         return false;
//     }
// };

map<poi,int,cmpset> mp;
vector<edge> OASGedge;
//A_data is the original data AFTER mapping sorted by x+y, B_data is USED FOR ocatant mapping
//p_run is the point set used for OASG, p_inital is the initial point set
vector<poi> A_data,B_data,p_run,p_inital;
//obs_run is the obstacle set used for OASG, obs_inital is the initial obstacle set
vector<obstacle_block> obs_run,obs_inital;
float ManhattanDistance(poi p1, poi p2)
{   
    //cout<<"ManhattanDistance"<<endl;
    return fabs(p1.x - p2.x) + fabs(p1.y - p2.y);
}

void swap(float &x, float &y)
{   //cout<<"swap"<<endl;
    float temp = x;
    x = y;
    y = temp;
}


bool compare(poi p1, poi p2)
{   //cout<<"compare"<<endl;
    //true p1 is in front of p2
    return p1.x+p1.y<p2.x+p2.y;
}
//judge pin vertex is obstacle or not and which point the obstacle is
int clas(poi x)
{   //cout<<"clas"<<endl;
    if (x.id<=node) return 0;
    return (x.id-node-1) % 4+1;
}

void init_map()
{   //cout<<"init_map"<<endl;
    int i,n=0;
    mp.clear();
    for (i=1;i<=node;i++)
        mp.insert(pair<poi,int>({p_inital[i].x,p_inital[i].y,0},++n));
    for (i=1;i<=obstacle_num;i++)
    {
        mp.insert(pair<poi,int>({obs_inital[i].p1.x,obs_inital[i].p1.y,0},++n));
        mp.insert(pair<poi,int>({obs_inital[i].p1.x,obs_inital[i].p2.y,0},++n));
        mp.insert(pair<poi,int>({obs_inital[i].p2.x,obs_inital[i].p2.y,0},++n));
        mp.insert(pair<poi,int>({obs_inital[i].p2.x,obs_inital[i].p1.y,0},++n));
    }
}

void init(int n,int m)
{   //cout<<"init"<<endl;
    numOASGedge=region=0;
    A_data.clear();A_data.resize(n);
    B_data.clear();B_data.resize(2*n);
    p_run.clear();p_run.resize(node+1);
    obs_run.clear();obs_run.resize(obstacle_num+1);
    OASGedge.clear();OASGedge.resize(m);
    //e.clear();e.resize(m,0);
    //len.clear();len.resize(m,0);
    //head.clear();head.resize(n,0);
}

void addOASGedge(poi p1, poi p2)
{
    //cout<<"addOASGedge"<<endl;
    //maping and rotating the points
    if (region==1)
    {
        p1.y=-p1.y;
        p2.y=-p2.y;
    }
    else if(region==2)
    {
        swap(p1.x, p1.y);
        swap(p2.x, p2.y);
    }
    else if(region==3)
    {
        p1.y=-p1.y;
        p2.y=-p2.y;
        swap(p1.x, p1.y);
        swap(p2.x, p2.y);
    }
    else 
    {
        //do nothing
    }
    //transfer to float may be wrong
    OASGedge[++numOASGedge]={mp.find(p1)->second,mp.find(p2)->second,ManhattanDistance(p1,p2)};
    //p1.id p2.id weight
    //cout<<"("<<p1.x<<","<<p1.y<<")"<<" "<<"("<<p2.x<<","<<p2.y<<")"<<" "<<ManhattanDistance(p1,p2)<<endl;
    //add(OASGedge[numOASGedge].u,OASGedge[numOASGedge].v,OASGedge[numOASGedge].cost);
    //add(OASGedge[numOASGedge].v,OASGedge[numOASGedge].u,OASGedge[numOASGedge].cost);
    //cout<<"numOASGedge "<<numOASGedge<<endl;
}

bool collison_block(poi a,poi b)
{
    //cout<<"collison_block"<<endl;
    set<int>::iterator i;
    poi p1,p2;
    for (i=left_bound.begin();i!=left_bound.end();i++)
    {
        //i is the point id
        p1=B_data[(*i)];//left bottom point1
        p2=B_data[(*i)+1];//left top point2
        //ensure line ab's y is in the range of the block
        if (a.y>p1.y && a.y<p2.y && b.y>p1.y && b.y<p2.y)
        {
            //ensure line ab's x is in the range of the block
            if ((a.x-p2.x)*(b.x-p2.x)<0)
            {
                return true;
            }
            if ((a.x-p2.x)*(b.x-p2.x)==0 && a.x+b.x>p2.x+p1.x)
            {
                return true;
            }
        }
    }
    for (i=bottom_bound.begin();i!=bottom_bound.end();i++)
    {
        //i is the point id
        p1=B_data[(*i)];//left bottom point1
        p2=B_data[(*i)+3];//right bottom point2
        //ensure line ab's x is in the range of the block
        if (a.x>p1.x && a.x<p2.x && b.x>p1.x && b.x<p2.x)
        {
            //ensure line ab's y is in the range of the block
            if ((a.y-p2.y)*(b.y-p2.y)<0)
            {
                return true;
            }
            if ((a.y-p2.y)*(b.y-p2.y)==0 && a.y+b.y>p2.y+p1.y)
            {
                return true;
            }
        }
    }
    return false;
}

void regionOASG()
{   
    //cout<<"regionOASG"<<endl;
    //cout<<"node:"<<node<<endl;
    //cout<<"obstacle_num:"<<obstacle_num<<endl;
    int nums=node+4*obstacle_num;
    int test=node;
    poi sentinel,nearest_point;
    //active is a set of points in R1 region BY cmpset
    multiset<poi,cmpset> Active;

    //misunderstand the meaning of S
    vector<poi> S(4*obstacle_num+node+3);
    //used for sentinel for the first point
    Active.insert({-INF, 0, 0});//(x,y,id)
    for (int i=1;i<=node;i++)
    {
        //cout<<"node:"<<p_run[i].x<<" "<<p_run[i].y<<" "<<i<<endl;
        A_data[i]={p_run[i].x,p_run[i].y,i};
        //cout<<"A_data:"<<A_data[i].x<<" "<<A_data[i].y<<" "<<A_data[i].id<<endl;
    }
    for (int i=1;i<=obstacle_num;i++)
    {
        //1 is the left bottom point, 2 is the left top point, 3 is the right top point 4 is the right bottom point
        A_data[++test]={obs_run[i].p1.x,obs_run[i].p1.y,test};
        //cout<<"A_data:"<<A_data[test].x<<" "<<A_data[test].y<<" "<<A_data[test].id<<endl;
        A_data[++test]={obs_run[i].p1.x,obs_run[i].p2.y,test};
        //cout<<"A_data:"<<A_data[test].x<<" "<<A_data[test].y<<" "<<A_data[test].id<<endl;
        A_data[++test]={obs_run[i].p2.x,obs_run[i].p2.y,test};
        //cout<<"A_data:"<<A_data[test].x<<" "<<A_data[test].y<<" "<<A_data[test].id<<endl;
        A_data[++test]={obs_run[i].p2.x,obs_run[i].p1.y,test};
        //cout<<"A_data:"<<A_data[test].x<<" "<<A_data[test].y<<" "<<A_data[test].id<<endl;
    }
    for (int i=1;i<=nums;i++)
    {
        B_data[i]=(A_data[i]);
        //cout<<"B_data:"<<B_data[i].x<<" "<<B_data[i].y<<" "<<B_data[i].id<<endl;
    }
    //first is the sentinel
    sort(A_data.begin()+1,A_data.begin()+nums+1,compare);
    for (int i=1;i<=nums;i++)
    {
        bool flag=false;
        int counter=0;
        nearest_point={-INF,0,0};
        //subset of v
        multiset<poi,cmpset>::iterator x;
        sentinel = {A_data[i].x, -INF, 0};
        //find the nearest point in active set
        //ENSURE OTHER DARA IS LARGER THAN THE SENTINEL's x
        x=Active.lower_bound(sentinel);
        while((*x).x >-INF)
        {
            if((*x).x<=A_data[i].x && ((*x).x-(*x).y)>=(A_data[i].x-A_data[i].y))
            {
                //REGION R1
                S[++counter]=(*x);
            }
            else
            {
                break;
            }
            if (collison_block((*x),A_data[i])==0)
            {
                if(ManhattanDistance((*x),A_data[i])<ManhattanDistance(nearest_point,A_data[i]))
                {
                    nearest_point=(*x);
                }
            }
            x++;   
        }
        if (nearest_point.id!=0)
        {
            if(nearest_point.x==A_data[i].x && nearest_point.y==A_data[i].y)
            {
                flag=true;
            }
            // else if(i!=nums && A_data[i].x==A_data[i+1].x && A_data[i].y==A_data[i+1].y)
            // {
            //     flag=true;
            //     //no add edge
            // }
            else
            {   //cout<<"before addOASGedge 1"<<endl;
                addOASGedge(nearest_point,A_data[i]);
            }
            //addOASGedge(nearest_point,A_data[i]);
        }
        //DELETE ALL POINTS IN S FROM ACTIVE
        for(int j=1;j<=counter;j++)
        {
            Active.erase(S[j]);
        }
        if(clas(A_data[i])==1)
        {
            bottom_bound.insert(A_data[i].id);
            left_bound.insert(A_data[i].id);
        }
        else if (clas(A_data[i])==2)
        {
            left_bound.erase(A_data[i].id-1);
        }
        else if (clas(A_data[i])==4)
        {
            bottom_bound.erase(A_data[i].id-3);
            sentinel = {A_data[i].x, -INF, 0};
            //find the nearest point in active set
            //ENSURE OTHER DARA IS LARGER THAN THE SENTINEL's x
            x=Active.lower_bound(sentinel);
            while((*x).x >-INF)
            {
                if((*x).x<B_data[A_data[i].id-3].x) break;
                //current is left bottom point delete
                //if(((*x).x-(*x).y<=A_data[i].x-A_data[i].y)&&(((*x).y<A_data[i].y)||((*x).y==A_data[i].y&&(*x).id!=A_data[i].id-3)))
                if(((*x).x-(*x).y<=A_data[i].x-A_data[i].y)&&(((*x).y<=A_data[i].y)))
                {
                    Active.erase(x++);
                }
                else x++;
            }
        }
        else
        {
            //do nothing
        }
        Active.insert(A_data[i]);
        //else //do nothing
        //{
            //do nothing
        //}
    }
}
bool judge_inside_obstacle(poi p1,poi p2, vector<vector<float>>&C_data,vector<partitioned_block>& obstacles,vector<block_data>&non_through_block)
{   
     //cout<<"ob_vertex: "<<endl;
    // for(int i=0;i<C_data.size();i++){
    //     cout<<C_data[i][0]<<" "<<C_data[i][1]<<endl;
    // }
    //cout<<"node:  "<<node;
    //cout<<obstacles.size()<<" "<<(p1.id-node)/4<<" "<<(p2.id-node)/4<<endl;
    if((p1.id-node-1)/4>=obstacles.size() || (p2.id-node-1)/4>= obstacles.size())
      cout<<obstacles.size()<<" "<<(p1.id-node)<<" "<<p2.id-node<<endl;
    if(p1.id<=node || p2.id<=node){
        //cout<<"p1.id<=node || p2.id<=node "<<endl;
        return false;
    } 
    //else if((p1.id-node)/4>=obstacles.size() || (p2.id-node)/4>= obstacles.size()){
        
      //  cout<<p1.id<<" "<<p2.id<<endl;
        //return false;
    //}
    //not same obstacle
    else if(obstacles[(p1.id-node-1)/4].blk_name!=obstacles[(p2.id-node-1)/4].blk_name){//可能改
        //cout<<"2 "<<size<<endl;
        //cout<<"obstacles[(p1.id-node)/4].blk_name!=obstacles[(p2.id-node)/4].blk_name "<<endl;
        return false;
    } 
    else
    {   //cout<<"else "<<endl;
        string blk_name=obstacles[(p1.id-node-1)/4].blk_name;
        //cout<<"blk_datas "<<(p1.id-node)/4<<" "<<blk_name<<endl;
        int kkk=0;
        for(int i=0;i<non_through_block.size();i++){

            if(blk_name==non_through_block[i].blk_name){
                kkk=1;
                C_data=non_through_block[i].original_vertex;
                break;
            }
        }
        //cout<<"kkk: "<<kkk<<endl;
        if(C_data.size()==2){
            return false;
        }
        // cout<<"p1,P2: "<<p1.x<<" "<<p1.y<<" "<<p2.x<<" "<<p2.y<<" "<<endl;
        // cout<<"obstacle: "<<endl;
        // for(int i=0;i<C_data.size();i++){
        //       cout<<C_data[i][0]<<" "<<C_data[i][1]<<endl;
        // }
        //cout<<endl;
        int counter=0;
        //cout<<"kk3"<<endl;
        int size=C_data.size();
        for (int i=0;i<C_data.size();i++)
        {   //cout<<"kk4 "<<i<<endl;
            if((p1.y==C_data[i][1]&&p1.x==C_data[i][0]) || (p2.y==C_data[i][1]&&p2.x==C_data[i][0]))
            {   
                counter++;
            }
            if(p1.x==p2.x&&p1.x==C_data[i][0]&&C_data[(i+1)%size][0]==C_data[i][0])
            {   //cout<<1<<endl;
                if(min(p1.y,p2.y)>=min(C_data[i][1], C_data[(i+1)%size][1]) && max(p1.y,p2.y)<=max(C_data[i][1], C_data[(i+1)%size][1])) return false;
            }
            else if(p1.y==p2.y&&p1.y==C_data[i][1]&&C_data[(i+1)%size][1]==C_data[i][1])
            {   //cout<<2<<endl;
                if(min(p1.x,p2.x)>=min(C_data[i][0], C_data[(i+1)%size][0]) && max(p1.x,p2.x)<=max(C_data[i][0], C_data[(i+1)%size][0])) return false;
            }
            if(counter==2){
                //cout<<3<<endl;
                return false;

            } 
        }
        //cout<<4<<endl;
        return true;
    }
    return false;
}
void OASG()
{   //cout<<"OASG"<<endl;
    region=1;
    for (int i=1;i<=node;i++)
    {
        p_run[i].x=p_inital[i].x;
        p_run[i].y=-p_inital[i].y;
    }
    for (int i=1;i<=obstacle_num;i++)
    {
        obs_run[i].p1.x=obs_inital[i].p1.x;
        obs_run[i].p1.y=-obs_inital[i].p1.y;
        obs_run[i].p2.x=obs_inital[i].p2.x;
        obs_run[i].p2.y=-obs_inital[i].p2.y;
        if (obs_run[i].p1.x>obs_run[i].p2.x) swap(obs_run[i].p1.x,obs_run[i].p2.x);
        if (obs_run[i].p1.y>obs_run[i].p2.y) swap(obs_run[i].p1.y,obs_run[i].p2.y);
    }
    //cout<<"before regionOASG 1"<<endl;
    regionOASG();

    region=2;
    for (int i=1;i<=node;i++)
    {
        p_run[i].x=p_inital[i].y;
        p_run[i].y=p_inital[i].x;
    }
    for (int i=1;i<=obstacle_num;i++)
    {
        obs_run[i].p1.x=obs_inital[i].p1.y;
        obs_run[i].p1.y=obs_inital[i].p1.x;
        obs_run[i].p2.x=obs_inital[i].p2.y;
        obs_run[i].p2.y=obs_inital[i].p2.x;
        if (obs_run[i].p1.x>obs_run[i].p2.x) swap(obs_run[i].p1.x,obs_run[i].p2.x);
        if (obs_run[i].p1.y>obs_run[i].p2.y) swap(obs_run[i].p1.y,obs_run[i].p2.y);
    }
    //cout<<"before regionOASG 2"<<endl;
    regionOASG();

    region=3;
    for (int i=1;i<=node;i++)
    {
        p_run[i].x=p_inital[i].y;
        p_run[i].y=-p_inital[i].x;
    }
    for (int i=1;i<=obstacle_num;i++)
    {
        obs_run[i].p1.x=obs_inital[i].p1.y;
        obs_run[i].p1.y=-obs_inital[i].p1.x;
        obs_run[i].p2.x=obs_inital[i].p2.y;
        obs_run[i].p2.y=-obs_inital[i].p2.x;
        if (obs_run[i].p1.x>obs_run[i].p2.x) swap(obs_run[i].p1.x,obs_run[i].p2.x);
        if (obs_run[i].p1.y>obs_run[i].p2.y) swap(obs_run[i].p1.y,obs_run[i].p2.y);
    }
    //cout<<"before regionOASG 3"<<endl;
    regionOASG();
    //k=numOASGedge;
    region=4;
    for (int i=1;i<=node;i++)
    {
        p_run[i].x=p_inital[i].x;
        p_run[i].y=p_inital[i].y;
    }
    for (int i=1;i<=obstacle_num;i++)
    {
        obs_run[i].p1.x=obs_inital[i].p1.x;
        obs_run[i].p1.y=obs_inital[i].p1.y;
        obs_run[i].p2.x=obs_inital[i].p2.x;
        obs_run[i].p2.y=obs_inital[i].p2.y;
        if (obs_run[i].p1.x>obs_run[i].p2.x) swap(obs_run[i].p1.x,obs_run[i].p2.x);
        if (obs_run[i].p1.y>obs_run[i].p2.y) swap(obs_run[i].p1.y,obs_run[i].p2.y);
    }
    //cout<<"before regionOASG 4"<<endl;
    regionOASG();
}
bool is_close(float a,float b){
    if(abs(a-b)<=0.0001)
        return true;
    else
        return false;
}
unordered_map <vertex,unordered_map<vertex, float>>generate_OASG(vector<vector<float>> &node_list, vector<block_data>&non_through_block,float boarder_x,float boarder_y)
{   //cout<<"generate_OASG"<<endl;
    //外面要建好:node_list、non_through_block、
    vector<partitioned_block> obstacles;
    //vector<vector<float>> blockage;//儲存edge所在的block
    //vector<vector<float>> all_grids=build_grid_list(obstacle_pts);
    //for(int i=0;i<all_grids.size();i++)cout<<"("<<all_grids[i][0]<<","<<all_grids[i][1]<<")"<<endl;
    node=node_list.size();
    //cout<<"NODE NUMBER:";
    //cin>>node;
    //cout<<"obstacle number:";
    //vector<vector<float>>obstacle_pts;////should try improve the traver version
    vector<int> obstacles_pt_num;////should try improve the traver version
    vector<vector<float>>grids_pts;////should try improve the traver version
    vector<vector<float>>block_obstacle_pts;
    vector<int> grids_pt_num;////should try improve the traver version
    unordered_map <vertex, unordered_map<vertex, float>> adj_lists;
    for(int i=0;i<non_through_block.size();i++){
        obstacle_num+=non_through_block[i].vertex.size()/2;
        obstacles_pt_num.push_back(non_through_block[i].original_vertex.size());
        grids_pt_num.push_back(non_through_block[i].grids.size());
        for(int j=0;j<non_through_block[i].vertex.size()/2;j++){
            //obstacle_pts.push_back(non_through_block[i].vertex[2*j+0]);
            //obstacle_pts.push_back(non_through_block[i].vertex[2*j+1]);
            partitioned_block temp;  
            temp.pt1.x=non_through_block[i].vertex[2*j+0][0];
            temp.pt1.y=non_through_block[i].vertex[2*j+0][1];
            temp.pt2.x=non_through_block[i].vertex[2*j+1][0];
            temp.pt2.y=non_through_block[i].vertex[2*j+1][1];   
            temp.blk_name=non_through_block[i].blk_name;
            obstacles.push_back(temp);         
        }
        for(int j=0;j<non_through_block[i].grids.size();j++){
            grids_pts.push_back(non_through_block[i].grids[j]);
        }
        for(int j=0;j<non_through_block[i].original_vertex.size();j++){
            block_obstacle_pts.push_back(non_through_block[i].original_vertex[j]);
        }    
    }
    //cin>>obstacle_num;
    cout<<"obstacle_datas:"<<obstacles_pt_num.size()<<" "<<grids_pt_num.size()<<" "<<endl;
    p_inital.resize(node+1);
    obs_inital.resize(obstacle_num+1);
    //32 
    //A_data.push_back({0,0,0});
    //B_data.push_back({0,0,0});
    for (int i=1;i<=node;i++)
    {
        //cout<<"NODE x y"<<":"<<endl;
        //cin>>p_inital[i].x>>p_inital[i].y;
        p_inital[i].x=node_list[i-1][0];
        p_inital[i].y=node_list[i-1][1];
        p_inital[i].id=i;
    }
    for (int i=1;i<=obstacle_num;i++)
    {
        //cout<<"NODE x1 y1 x2 y2"<<":"<<endl;
        //cin>>obs_inital[i].p1.x>>obs_inital[i].p1.y>>obs_inital[i].p2.x>>obs_inital[i].p2.y;
        obs_inital[i].p1.x=obstacles[i-1].pt1.x;
        obs_inital[i].p1.y=obstacles[i-1].pt1.y;
        obs_inital[i].p2.x=obstacles[i-1].pt2.x;
        obs_inital[i].p2.y=obstacles[i-1].pt2.y;
        //obstacles[i-1].blk_name


    }
    init(node+obstacle_num*4+2,16*(node+obstacle_num*4)+2);
    for (int i=1;i<=node;i++)
    {
        p_inital[i].x=-p_inital[i].x;
        p_inital[i].y=-p_inital[i].y;
    }
    for (int i=1;i<=obstacle_num;i++)
    {
        obs_inital[i].p1.x=-obs_inital[i].p1.x;
        obs_inital[i].p1.y=-obs_inital[i].p1.y;
        obs_inital[i].p2.x=-obs_inital[i].p2.x;
        obs_inital[i].p2.y=-obs_inital[i].p2.y;
    }
    init_map();
    OASG();
    //cout<<"OASG_finish:"<<endl;
    //A_data.push_back({0,0,0});
    //B_data.push_back({0,0,0});
    
    for (int i=1;i<=node;i++)
    {
        p_inital[i].x=-p_inital[i].x;
        p_inital[i].y=-p_inital[i].y;
    }
    for (int i=1;i<=obstacle_num;i++)
    {
        obs_inital[i].p1.x=-obs_inital[i].p1.x;
        obs_inital[i].p1.y=-obs_inital[i].p1.y;
        obs_inital[i].p2.x=-obs_inital[i].p2.x;
        obs_inital[i].p2.y=-obs_inital[i].p2.y;
    }
    init_map();
    //cout << "hahahahahah!!!" << endl;
    OASG();
    cout << "hello!!!" <<numOASGedge<< endl;
    
    //cout<<"start point and end point and cost:"<<endl;
    //cout<<"numOASGedge:"<<numOASGedge<<endl;
    
    vector<result_edge> result;
    for (int i = 1; i <= numOASGedge; i++) {
        // 获取边的坐标
        //if(i==k+1) cout<<"region2"<<endl;
        //cout<<OASGedge[i].v<<" "<<OASGedge[i].u<<" ";
        poi p1 = B_data[OASGedge[i].u];
        poi p2 = B_data[OASGedge[i].v];
        float cost = OASGedge[i].cost;
        result_edge temp;
        temp.pt1.push_back(p1.x);
        temp.pt1.push_back(p1.y);
        temp.pt2.push_back(p2.x);
        temp.pt2.push_back(p2.y);
        temp.cost=cost;
        vector<vector<float>>C_data;
        bool is_on_boarder=0;
        //cout<<i<<endl;
        //is_on_boarder=(p1.x<=0 || p1.y<=0 || p1.x>=boarder_x || p1.y>=boarder_y|| p2.x<=0 || p2.y<=0 || p2.x>=boarder_x || p2.y>=boarder_y);
        //cout<<1<<endl;
        //cout<<"before judge"<<endl;
        if(!(judge_inside_obstacle(p1,p2,C_data,obstacles,non_through_block)) && !is_on_boarder){
            //cout <<i<<" "<<  p1.x << " " << p1.y << " " << p2.x << " " << p2.y << endl;
            //cout<<2<<endl;
            result.push_back(temp);
            //cout<<3<<endl;
            addEdge(adj_lists,{p1.x,p1.y},{p2.x,p2.y});
            //cout<<4<<endl;
            //cout<<"yesyesyes"<<endl;
        }
        //cout<<5<<endl;
        temp.pt1.clear();
        //cout<<6<<endl;
        temp.pt2.clear();
        //cout<<7<<endl;
        //cout<<"kk2"<<endl;
        C_data.clear();
        //cout<<8<<endl;
        //else continue;
        
    }
    cout<<"end judging"<<endl;
    cout<<"finishOASG";
    //obstacle_pts.clear();////should try improve the traver version
    obstacles_pt_num.clear();////should try improve the traver version

    grids_pts.clear();////should try improve the traver version
    grids_pt_num.clear();////should try improve the traver version
    A_data.clear();
    B_data.clear();
    OASGedge.clear();
    obstacles.clear();
    p_inital.clear();
    block_obstacle_pts.clear();
    obs_inital.clear();
    region=0;numOASGedge=0;node=0;obstacle_num=0;
    return adj_lists;
}