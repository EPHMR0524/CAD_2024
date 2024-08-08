#include<iostream>
#include<vector>
#include<fstream>
#include<string>
#include <sstream>
#include <algorithm>
#include"parser.h"
using namespace std;
vector<string> splitString(const string& str, char delimiter) {
    vector<string> tokens;
    string token;
    stringstream ss(str);

    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

vector<string> eliminate_space(string line){
        istringstream iss(line);//seperate the lines by space(" ")
        vector<string> temp;//object to store all elements of a line
        string temps;//object to store the segment of the line

        while (iss >> temps) {//start process lines
            temp.push_back(temps);
            //cout<<temps<<" ";
        }//end process lines
        return temp;
}

string delete_symbol(string obj){
    replace(obj.begin(), obj.end(), '"', ' ');
    replace(obj.begin(), obj.end(), '[', ' ');
    replace(obj.begin(), obj.end(), ']', ' ');
    replace(obj.begin(), obj.end(), '(', ' ');
    replace(obj.begin(), obj.end(), ')', ' ');
    replace(obj.begin(), obj.end(), ',', ' ');
    replace(obj.begin(), obj.end(), '{', ' ');
    replace(obj.begin(), obj.end(), '}', ' ');
    return obj;
}
void rotate_block(vector<vector<float>> &vertex,const string direction,const float ref_x,const float ref_y){
    float  max_x=0,max_y=0;
    
    for(int i=0;i<vertex.size();i++){//find the maximum of x and y
        if(vertex[i][0]>max_x) max_x=vertex[i][0];
        if(vertex[i][1]>max_y) max_y=vertex[i][1];
    }
    if(direction=="N"){
        for (int i=0;i<vertex.size();i++){
            vertex[i][0]=(vertex[i][0]+ref_x);
            vertex[i][1]=(vertex[i][1]+ref_y);
        }
    }
    else if(direction=="S"){
        for (int i=0;i<vertex.size();i++){
            vertex[i][0]=((max_x-vertex[i][0])+ref_x);
            vertex[i][1]=((max_y-vertex[i][1])+ref_y);
        }
    }
    else if(direction=="W"){
        for (int i=0;i<vertex.size();i++){
            vertex[i][0]=((max_y-vertex[i][1])+ref_x);
            vertex[i][1]=((vertex[i][0])+ref_y);
        }
    }
    else if(direction=="E"){
        for (int i=0;i<vertex.size();i++){
            vertex[i][0]=((vertex[i][1])+ref_x);
            vertex[i][1]=((max_x-vertex[i][0])+ref_y);
        }
    }
    else if(direction=="FN"){
        for (int i=0;i<vertex.size();i++){
            vertex[i][0]=((max_x-vertex[i][0])+ref_x);
            vertex[i][1]=((vertex[i][1])+ref_y);
        }
    }
    else if(direction=="FS"){
        for (int i=0;i<vertex.size();i++){
            vertex[i][0]=((vertex[i][0])+ref_x);
            vertex[i][1]=((max_y-vertex[i][1])+ref_y);
        }
    }
    else if(direction=="FW"){
        for (int i=0;i<vertex.size();i++){
            vertex[i][0]=((vertex[i][0])+ref_x);
            vertex[i][1]=((vertex[i][1])+ref_y);
        }
    }
    else if(direction=="FE"){
        for (int i=0;i<vertex.size();i++){
            vertex[i][0]=((max_y-vertex[i][1])+ref_x);
            vertex[i][1]=((max_x-vertex[i][0])+ref_y);
        }
    }

}
void read_chip_top(vector<block_data> & block_placing,vector<region_area>& region_placing,float die_area[2][2],int&unit,int &block_num,int& region_num,string folder){
    //open chip_top.def
    ifstream file;
    file.open("."+folder+"/chip_top.def");//之後改成不要寫死===================================!!!!!!!!!!!!
     if (!file.is_open()) {
        cout<<"no such file chip top"<<endl;
    }
    int flag=0;//use to verify getting data of block or region
    string line;
    while (getline(file, line)) {
        vector<string> temp=eliminate_space(line);//seperate the lines by space(" ")
        if(temp.size()==0){//skip lines with nothing
            cout<<"";
        }
        else if(temp[0]=="END"){//update flag to store the region
            flag++;
        }
        else if(temp[0]=="UNITS"){//store the units
            unit=stoi(temp[3]);
        }
        else if(temp[0]=="DIEAREA"){//store the 

            die_area[0][0]=stof(temp[2]);
            die_area[0][1]=stof(temp[3]);
            die_area[1][0]=stof(temp[6]);
            die_area[1][1]=stof(temp[7]);

        }

        else if (flag==0 && temp[0] == "-"){//store block data
            block_data block_data;
            block_data.blk_name=temp[1];
            block_data.blk_type=temp[2];
            block_data.place[0]=stof(temp[6]);
            block_data.place[1]=stof(temp[7]);
            block_data.direction=temp[9];
            block_placing.push_back(block_data);
        }

        else if (flag==1 && temp[0] == "-"){
            region_area region_data;
            region_data.reg_name=temp[1];
            region_data.left_down[0]=stof(temp[3]);
            region_data.left_down[1]=stof(temp[4]);
            region_data.right_up[0]=stof(temp[7]);
            region_data.right_up[1]=stof(temp[8]);
            region_placing.push_back(region_data);
        }

    }
    block_num=block_placing.size();
    region_num=region_placing.size();
    file.close();
}



void read_blk_area(const int num_of_block,vector<block_data> & block_placing, string folder){
    ifstream file;
    string blk_type=block_placing[num_of_block].blk_type;
    string file_place="."+folder+"/"+blk_type+".def";
    file.open(file_place);
     if (!file.is_open()) {
        cout<<"no such file exist block area"<<endl;
    }
    string line;
    while (getline(file, line)) {
        vector<string> temp=eliminate_space(line);//seperate the lines by space(" ")
        if(temp.size()!=0 && temp[0]=="DIEAREA"){//store the vertex of blocks
                int flag=0;//when flag==1 the vertex_tuple will be deleted and push back into the block_placing.vertex
                vector<float> vertex_tuple;
                for(int i=0;i<temp.size();i++){
                    
                    if(temp[i][0]-'0'>=0 && temp[i][0]-'0'<10){
                        vertex_tuple.push_back(stof(temp[i]));
                        if(flag==0){
                            flag+=1;
                        }
                        else if(flag==1){
                            flag=0;
                            block_placing[num_of_block].vertex.push_back(vertex_tuple);
                            vertex_tuple.clear();
                        }
                    }
                }
        }

    }
    block_placing[num_of_block].original_vertex=block_placing[num_of_block].vertex;
    file.close();
}

void read_cfg(vector<block_data> & block_placing,string cfg_file){
    ifstream file;
    string file_place=cfg_file;
    file.open(file_place);
     if (!file.is_open()) {
        cout<<"no such file exist cfg"<<endl;
    }
    string line;
    while (getline(file, line)) {
        char delimiter = ':';
        vector<string> result = splitString(line, delimiter);
        int index;
        //cout<<"MME"<<result[0]<<endl;
        if(result[0]=="    \"block_name\""){
            //vector<string> temp=splitString(result[1], '_');
            //vector<string> temp2=splitString(result[1], '\"');
            
            for(int i=0;i<block_placing.size();i++){
                string temp=result[1];
                replace(temp.begin(), temp.end(), '"', ' ');
                replace(temp.begin(), temp.end(), ',', ' ');
                vector<string> temp2=eliminate_space(temp);
                //cout<<"FFL:"<<block_placing[i].blk_name<<""<<temp2[0]<<endl;
                if(block_placing[i].blk_name==temp2[0]){
                    //
                    index=i;
                    //cout<<"FFF:"<<block_placing[i].blk_name<<" "<<temp2[0]<<endl;
                }
            }
            //index=stoi(temp2[0]);
        }
        else if(result[0]=="    \"through_block_net_num\""){
            result[1].pop_back();
            //cout<<result[1]<<endl;
            block_placing[index].through_block_net_num=stoi(result[1]);
            //cout<<block_placing[index].blk_name<<" "<<block_placing[index].through_block_net_num<<endl;
        }
        else if(result[0]=="    \"through_block_edge_net_num\""){
            result[1]=delete_symbol(result[1]);
            vector<string> numbers=eliminate_space(result[1]);//object to store all elements of a line
            for(int i=0;i<numbers.size()/5;i++){
                through_block_edge_net_num temp;
                temp.start_and_end[0][0]=stof(numbers[i*5+0]);
                temp.start_and_end[0][1]=stof(numbers[i*5+1]);
                temp.start_and_end[1][0]=stof(numbers[i*5+2]);
                temp.start_and_end[1][1]=stof(numbers[i*5+3]);
                temp.number=stof(numbers[i*5+4]);
                block_placing[index].through_block_edge_net_num_lst.push_back(temp);
            }
        }
        else if(result[0]=="    \"block_port_region\""){
            result[1]=delete_symbol(result[1]);
            vector<string> numbers=eliminate_space(result[1]);//object to store all elements of a line
            for(int i=0;i<numbers.size()/2;i++){
                vector<float> region_tuple;
                region_tuple.push_back(stof(numbers[i*2+0]));
                region_tuple.push_back(stof(numbers[i*2+1]));
                block_placing[index].block_port_region.push_back(region_tuple);
                region_tuple.clear();
            }
        }
        else if(result[0]=="    \"is_feedthroughable\""){
            result[1].pop_back();
            //cout<<result[1];
            if(result[1]=="\"True\""){
                //cout<<1;
                block_placing[index].is_feedthroughable=true;
            }
            else{
                //cout<<2;
                block_placing[index].is_feedthroughable=false;
            }
        }
        else if(result[0]=="    \"is_tile\""){
            result[1].pop_back();
            if(result[1]=="\"True\""){
                block_placing[index].is_tile=true;
            }
            else{
                block_placing[index].is_tile=false;
            }
        }
        
    }

    file.close();
}

void parse_connection_matrix(vector<NETS> &connection_matrix ,string connectin_file){
    ifstream file;
    string file_place=connectin_file;
    file.open(file_place);
     if (!file.is_open()) {
        cout<<"no such file exist connect matrix"<<endl;
    }
    string line;
    NETS temp_net;
    while (getline(file, line)) {
        char delimiter = ':';
        vector<string> result = splitString(line, delimiter);
        if(result[0]=="    \"ID\""){
            temp_net.ID=stoi(result[1]);
            //cout<<temp_net.ID<<",";
        }
        else if(result[0]=="    \"TX\""){
            result[1]=delete_symbol(result[1]);
            vector<string> splitted_result=eliminate_space(result[1]);
            //cout<<splitted_result[0]<<",";
            temp_net.TX=splitted_result[0];
        }
        else if(result[0]=="    \"RX\""){
           result[1]=delete_symbol(result[1]);
           vector<string> splitted_result=eliminate_space(result[1]);
           for(int i=0;i<splitted_result.size();i++){
               temp_net.RX.push_back(splitted_result[i]);
               //cout<<splitted_result[i]<<",";
           } 
        }
        else if(result[0]=="    \"NUM\""){
           result[1]=delete_symbol(result[1]);
           temp_net.NUM=stoi(result[1]);
           //cout<<temp_net.NUM<<",";
        }
        else if(result[0]=="    \"MUST_THROUGH\""){
            Through_region_description temp_des;
            for(int i=1;i<result.size();i++){
                
                result[i]=delete_symbol(result[i]);
                 vector<string> splitted_result=eliminate_space(result[i]);
                if(splitted_result.size()==1){//第一個要存block name,理論上只會執行一次
                    //cout<<"KKK"<<endl;
                    temp_des.BLK_name=result[i];
                }
                else{
                    for(int j=0;j<splitted_result.size();j++){
                        //cout<<splitted_result[j]<<" ";
                        if(splitted_result[j][0]=='B' || splitted_result[j][0]=='R' || j==splitted_result.size()-1){
                            if(splitted_result[j][0]=='B' || splitted_result[j][0]=='R'){
                                temp_net.MUST_THROUGH.push_back(temp_des);//遇到block name年在數字後面要做例外處裡(儲存至temp_net並刪除原有資料)
                                temp_des.BLK_name=splitted_result[j];
                                temp_des.ptr_data.clear();
                            }
                            else{
                                temp_des.ptr_data.push_back(stof(splitted_result[j]));
                                temp_net.MUST_THROUGH.push_back(temp_des);
                                temp_des.ptr_data.clear();
                                
                            }
                            
                        }
                        else temp_des.ptr_data.push_back(stof(splitted_result[j]));
                    }
                }
            }
        }
        else if(result[0]=="    \"HMFT_MUST_THROUGH\""){
            Through_region_description temp_des;
            for(int i=1;i<result.size();i++){
                
                result[i]=delete_symbol(result[i]);
                 vector<string> splitted_result=eliminate_space(result[i]);
                if(splitted_result.size()==1){//第一個要存block name,理論上只會執行一次
                    //cout<<"KKK"<<endl;
                    temp_des.BLK_name=result[i];
                }
                else{
                    for(int j=0;j<splitted_result.size();j++){
                        //cout<<splitted_result[j]<<" ";
                        if(splitted_result[j][0]=='B' || splitted_result[j][0]=='R' || j==splitted_result.size()-1){
                            if(splitted_result[j][0]=='B' || splitted_result[j][0]=='R'){
                                temp_net.HMFT_MUST_THROUGH.push_back(temp_des);//遇到block name年在數字後面要做例外處裡(儲存至temp_net並刪除原有資料)
                                temp_des.BLK_name=splitted_result[j];
                                temp_des.ptr_data.clear();
                                
                            }
                            else{
                                temp_des.ptr_data.push_back(stof(splitted_result[j]));
                                temp_net.HMFT_MUST_THROUGH.push_back(temp_des);
                                temp_des.ptr_data.clear();
                            }
                            
                        }
                        else temp_des.ptr_data.push_back(stof(splitted_result[j]));
                    }
                }
            }
        }
        else if(result[0]=="    \"TX_COORD\""){
            result[1]=delete_symbol(result[1]);
            vector<string> splitted_result=eliminate_space(result[1]);
            temp_net.TX_COORD[0]=stof(splitted_result[0]);
            temp_net.TX_COORD[1]=stof(splitted_result[1]);
        }
        else if(result[0]=="    \"RX_COORD\""){
            result[1]=delete_symbol(result[1]);
            vector<string> splitted_result=eliminate_space(result[1]);
            for(int i=0;i<splitted_result.size()/2;i++){
                vector<float> ptr;
                ptr.push_back(stof(splitted_result[i*2+0]));
                ptr.push_back(stof(splitted_result[i*2+1]));
                temp_net.RX_COORD.push_back(ptr);
                ptr.clear();
            }
            connection_matrix.push_back(temp_net);
            temp_net.RX.clear();
            temp_net.MUST_THROUGH.clear();
            temp_net.HMFT_MUST_THROUGH.clear();
            temp_net.RX_COORD.clear();

        }


    }
    file.close();

}
void convert_block_data(const int unit,vector<block_data>&block_placing ){

    for (int i=0;i<block_placing.size();i++){
        block_placing[i].place[0];//deal with the conversion of place
        block_placing[i].place[1];
        //cout<<block_placing[i].place[0]<<" "<<block_placing[i].place[1]<<endl;/=unit
        //cout<<block_placing[i].blk_name<<" ";
        for(int j=0;j<block_placing[i].vertex.size();j++){//deal with the conversion of vertex of block
            //cout<<block_placing[i].vertex[j][0]<<" "<<block_placing[i].vertex[j][1]<<" ";
            block_placing[i].vertex[j][0];
            block_placing[i].vertex[j][1];
            
        }
        //cout<<endl;
        rotate_block(block_placing[i].vertex,block_placing[i].direction,block_placing[i].place[0],block_placing[i].place[1]);
        for(int j=0;j<block_placing[i].through_block_edge_net_num_lst.size();j++){
            block_placing[i].through_block_edge_net_num_lst[j].start_and_end[0][0]+=block_placing[i].place[0];
            block_placing[i].through_block_edge_net_num_lst[j].start_and_end[0][1]+=block_placing[i].place[1];
            block_placing[i].through_block_edge_net_num_lst[j].start_and_end[1][0]+=block_placing[i].place[0];
            block_placing[i].through_block_edge_net_num_lst[j].start_and_end[1][1]+=block_placing[i].place[1];
        }
        for(int j=0;j<block_placing[i].block_port_region.size();j++){
            block_placing[i].block_port_region[j][0]+=block_placing[i].place[0];
            block_placing[i].block_port_region[j][1]+=block_placing[i].place[1];
        }
        rotate_block(block_placing[i].original_vertex,block_placing[i].direction,block_placing[i].place[0],block_placing[i].place[1]);
        
        
    }
}

bool compareByY(const vector<float>& a, const vector<float>& b) {
    return a[1] > b[1];
}
float abso(float a){
    if (a>=0) return a;
    else return -a;
}
bool is_thisrectangle(vector<vector<float>> rectangle){
    if(rectangle.size()==4){
        return true;
    }
    else{
        return false;
    }
}

int is_in_vector(vector<vector<float>> rectangle,vector<float> tuples){
    for(int i=0;i<(int)rectangle.size();i++){
        if(tuples[0]==rectangle[i][0] && tuples[1]==rectangle[i][1])
            return  i+1;
    }
    return 0;
}
vector<vector<float>> build_grid_list(vector<vector<float>> rectangle){
    vector<float> y_grids;
    vector<float> x_grids;
    vector<vector<float>> all_grid;
    //float x_temp,y_temp;
    for(int i=0;i<(int)rectangle.size();i++){
        if(x_grids.size()==0){
            x_grids.push_back(rectangle[i][0]);
        }
        if(y_grids.size()==0){
            y_grids.push_back(rectangle[i][1]);
        }
        if(find(x_grids.begin(),x_grids.end(),rectangle[i][0])==x_grids.end()){
            //cout<<rectangle[i][0]<<"kkk"<<endl;
            x_grids.push_back(rectangle[i][0]);
        }
        if(find(y_grids.begin(),y_grids.end(),rectangle[i][1])==y_grids.end()){
            //cout<<rectangle[i][1]<<"lll"<<endl;
            y_grids.push_back(rectangle[i][1]);
        }
    }
    sort(y_grids.begin(),y_grids.end());
    sort(x_grids.begin(),x_grids.end());
    //vector<float> tuples;
    for(int i=0;i<(int)rectangle.size();i++){
        //cout<<i<<" ";
        vector<float> a=rectangle[i],b=rectangle[(i+1)%(rectangle.size())];
        //cout<<a[0]<<" "<<a[1]<<" "<<b[0]<<" "<<b[1]<<endl;
        if(a[0]==b[0]){// verticle  case
            if(b[1]-a[1]>0){
                    for(int j=0;j<(int)y_grids.size();j++){//begin
                    if(y_grids[j]>= min(a[1],b[1]) &&  y_grids[j]<= max(a[1],b[1]) ){
                        //cout<<"kk";
                        if(!is_in_vector( all_grid,{a[0],y_grids[j]})){
                            //cout<<i<<" "<<a[0]<<" "<<y_grids[j]<<" verticle"<<endl;
                            all_grid.push_back({a[0],y_grids[j]});
                        }


                    }
                }//end
            }
            else{
                for(int j=(int)y_grids.size()-1;j>=0;j--){//begin
                    if(y_grids[j]>= min(a[1],b[1]) &&  y_grids[j]<= max(a[1],b[1]) ){
                        //cout<<"kk";
                        if(!is_in_vector( all_grid,{a[0],y_grids[j]})){
                            //cout<<i<<" "<<a[0]<<" "<<y_grids[j]<<" verticle"<<endl;
                            all_grid.push_back({a[0],y_grids[j]});
                        }


                    }
                }//end
            }

        }
        else if(a[1]==b[1]){// horizontal  case
            if(b[0]-a[0]>0){
                    for(int j=0;j<(int)x_grids.size();j++){//begin
                    if(x_grids[j]>= min(a[0],b[0]) &&  x_grids[j]<= max(a[0],b[0]) ){
                        //cout<<"kk";
                        if(!is_in_vector( all_grid,{x_grids[j],a[1]})){
                            //cout<<i<<" "<<x_grids[j]<<" "<<a[1]<<" horizontal"<<endl;
                            all_grid.push_back({x_grids[j],a[1]});
                        }

                    }
                }//end
            }
            else{
                for(int j=(int)x_grids.size()-1;j>=0;j--){//begin
                    if(x_grids[j]>= min(a[0],b[0]) &&  x_grids[j]<= max(a[0],b[0]) ){
                        //cout<<"kk";
                        if(!is_in_vector( all_grid,{x_grids[j],a[1]})){
                            //cout<<i<<" "<<x_grids[j]<<" "<<a[1]<<" horizontal"<<endl;
                            all_grid.push_back({x_grids[j],a[1]});
                        }

                    }
                }//end
            }

        }


    }
    return all_grid;


}
void sort_the_vertex(vector<vector<float>> &rectangle){
    float x_leftmost=100000000000,y_downmost=1000000000000;
    vector<vector<float>> result;
    for(int i=0;i<(int)rectangle.size();i++){
        if(rectangle[i][0]<x_leftmost){
            x_leftmost=rectangle[i][0];
            //cout<<x_leftmost<<endl;

        }
    }
    for(int i=0;i<(int)rectangle.size();i++){
        if(rectangle[i][1]<y_downmost && rectangle[i][0]==x_leftmost){
            y_downmost=rectangle[i][1];
            //cout<<y_downmost<<endl;
        }
    }
    int index;
    for(int i=0;i<(int)rectangle.size();i++){
        if(rectangle[i][0]==x_leftmost && rectangle[i][1]==y_downmost){
            index=i;
        }
    }
    for(int i=index;i<(int)index+(int)rectangle.size();i++){
        result.push_back(rectangle[i%(rectangle.size())]);
    }
    rectangle=result;

}

int is_allowed_cut(vector<float> pt1,vector<float> pt2,vector<float> pt3,vector<float> pt4 ,vector<vector<float>> all_grids){

 vector<float> pt3_next;
 vector<float> pt1_form;
 for(int i=0;i<all_grids.size();i++){
    if(all_grids[i][0]==pt1[0] && all_grids[i][1]==pt1[1] ){
        pt1_form=all_grids[(i+all_grids.size()-1)%all_grids.size()];
    }
    if(all_grids[i][0]==pt3[0] && all_grids[i][1]==pt3[1] ){
        pt3_next=all_grids[(i+all_grids.size()+1)%all_grids.size()];
    }

 }
 if(pt4[0]==pt1_form[0] && pt4[1]==pt1_form[1]) return 1;
 else if(pt4[0]==pt3_next[0] && pt4[1]==pt3_next[1]) return 3;
 else return 0;


}
vector<vector<float>> split_polygon(vector<vector<float>> rectangle){

    vector<vector<float>> result;
    /*cout<<"all grid"<<endl;
    for(int i=0;i<all_grids.size();i++){
        cout<<"("<<all_grids[i][0]<<","<<all_grids[i][1]<<")"<<endl;
    }

     cout<<"all vertex"<<endl;
    for(int i=0;i<(int)rectangle.size();i++){
        cout<<"("<<rectangle[i][0]<<","<<rectangle[i][1]<<")"<<endl;
    }*/
    while(!is_thisrectangle(rectangle)){
        sort_the_vertex(rectangle);
        vector<vector<float>> all_grids=build_grid_list(rectangle);
        //cout<<"";
        for(int i=0;i<rectangle.size();i++){
            vector<float> pt1=rectangle[i],pt2=rectangle[(i+1)%rectangle.size()],pt3=rectangle[(i+2)%rectangle.size()];
            vector<float>pt4={pt3[0]-(pt2[0]-pt1[0]),pt3[1]-(pt2[1]-pt1[1])};
            if(is_allowed_cut(pt1,pt2,pt3,pt4,all_grids)){
                for(int j=0;j<(int)rectangle.size();j++){
                    //cout<<"before: "<<j<<" "<<(int)rectangle.size()<<"("<<rectangle[j][0]<<","<<rectangle[j][1]<<")"<<endl;
                }
                if(is_in_vector(rectangle,pt4)){
                    //cout<<"KK:"<<pt1[0]<<","<<pt1[1]<<","<<pt2[0]<<","<<pt2[1]<<","<<pt3[0]<<","<<pt3[1]<<","<<pt4[0]<<","<<pt4[1]<<endl;
                    rectangle.erase(rectangle.begin()+i,rectangle.begin()+i+3);
                    int index=is_in_vector(rectangle,pt4);
                    rectangle.erase(rectangle.begin()+index-1);


                }
                else{
                    //cout<<"LL:"<<pt1[0]<<","<<pt1[1]<<","<<pt2[0]<<","<<pt2[1]<<","<<pt3[0]<<","<<pt3[1]<<","<<pt4[0]<<","<<pt4[1]<<endl;
                    rectangle.insert(rectangle.begin()+i,pt4);//insert before element i ,pt4 is the new i
                    rectangle.erase(rectangle.begin()+i+1,rectangle.begin()+i+4);

                }

                //for(int j=0;j<(int)rectangle.size();j++){
                  //  cout<<"after: "<<j<<" "<<(int)rectangle.size()<<"("<<rectangle[j][0]<<","<<rectangle[j][1]<<")"<<endl;
                //}

                vector<float> left_down,right_up;
                if((pt3[1]-pt1[1])*(pt3[0]-pt1[0])>0){
                    if(pt3[0]>pt1[0]){
                        left_down=pt1;
                        right_up=pt3;
                    }
                    else{
                        left_down=pt3;
                        right_up=pt1;
                    }
                }
                else{
                    if(pt4[0]>pt2[0]){
                        left_down=pt2;
                        right_up=pt4;
                    }
                    else{
                        left_down=pt4;
                        right_up=pt2;
                    }
                }
                result.push_back(left_down);
                result.push_back(right_up);
                break;
            }
        }

    }
    sort_the_vertex(rectangle);
    result.push_back(rectangle[0]);
    result.push_back(rectangle[2]);
    return result;


}
void change_unit(vector<block_data> & block_placing,int unit){
    for (int i=0;i<block_placing.size();i++){
        block_placing[i].place[0]/=unit;
        block_placing[i].place[1]/=unit;
       /* struct block_data{
            string blk_name;
            string blk_type;
            float place[2]; ========================================
            string direction;//think another better name 
            vector<vector<float>> original_vertex; =================================
            vector<vector<float>> vertex; =======================================
            int through_block_net_num;
            vector<through_block_edge_net_num> through_block_edge_net_num_lst;
            vector<vector<float>> block_port_region;====================================
            bool is_feedthroughable;
            bool is_tile;
            vector<vector<float>> grids;

        }*/
        for(int j=0;j<block_placing[i].block_port_region.size();j++){
            block_placing[i].block_port_region[j][0]/=unit;
            block_placing[i].block_port_region[j][1]/=unit;
        }
        for(int j=0;j<block_placing[i].original_vertex.size();j++){
            block_placing[i].original_vertex[j][0]/=unit;
            block_placing[i].original_vertex[j][1]/=unit;
        }
        for(int j=0;j<block_placing[i].vertex.size();j++){
            block_placing[i].vertex[j][0]/=unit;
            block_placing[i].vertex[j][1]/=unit;
        }
        for(int j=0;j<block_placing[i].grids.size();j++){
            block_placing[i].grids[j][0]/=unit;
            block_placing[i].grids[j][1]/=unit;
        }


        
    }

}