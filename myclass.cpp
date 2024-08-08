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
#define INT_MAX 2147483647 

OAMST::OAMST(unordered_map<vertex, unordered_map<vertex, float>> adj_lists, set<vertex> PINS) {
    
    auto [d, p] = all_pins_shortest_paths(adj_lists, PINS);
    //
    all_paths = p;
    all_distances = d;

    auto [d2, p2] = makeAllPinsGraph(PINS);
    all_PINS_paths = p2;
    all_PINS_distances = d2;

    MST_PRIM(PINS);
}


pair<unordered_map<vertex, float>, unordered_map<vertex, vector<vertex>>> OAMST::dijkstra(const unordered_map<vertex, unordered_map<vertex, float>> adj_lists, vertex source) {
    unordered_map<vertex, float> distances;
    unordered_map<vertex, vector<vertex>> paths;
    // 用set來實作priority queue，除了set也會自動排序，也可有刪除元素的功能，反之queue不行，只能pop
    set<pair<float, vertex>> priority_queue;

    // 初始化distance和paths
    for (const auto& pair : adj_lists) {
        //cout<<"in first loop"<<endl;
        vertex vertex = pair.first;
        distances[vertex] = 2147483647;
        paths[vertex] = {};
    }

    distances[source] = 0.000;
    paths[source] = {source};
    priority_queue.insert({0.000, source});


    while (!priority_queue.empty()) {
        // 取出priority queue中的第一個元素，即最小的distance。這邊用dereference(*)是因為.begin()會回傳iterator，而不是元素本身
        auto [current_distance, current_vertex] = *priority_queue.begin();
        priority_queue.erase(priority_queue.begin());
        if (adj_lists.find(current_vertex) != adj_lists.end()) {
            for (const auto& neighbor : adj_lists.at(current_vertex)) {   // 這邊用.at()是因為adj_lists為const，所以不能用[]來存取元素，這樣更安全
            // neighbor為current_vertex的各個鄰居(unordered_map)，first為vertex，second為weight
            vertex neighbor_vertex = neighbor.first;
            float weight = neighbor.second;   // 這邊的weight即為current_vertex到neighbor_vertex的weight
            float new_distance = current_distance + weight;

            if (distances.find(neighbor_vertex) != distances.end() &&new_distance < distances[neighbor_vertex]) {
                // 比原本的distance小，則更新distance和path
                priority_queue.erase({distances[neighbor_vertex], neighbor_vertex});
                distances[neighbor_vertex] = new_distance;
                priority_queue.insert({new_distance, neighbor_vertex});

                // 更新path，將neighbor的path設為current的path，再加上neighbor自己
                paths[neighbor_vertex] = paths[current_vertex];
                paths[neighbor_vertex].push_back(neighbor_vertex);
            }
        }
        }
        
    }

    return {distances, paths};
}


pair<unordered_map<vertex, unordered_map<vertex, float>>, unordered_map<vertex, unordered_map<vertex, vector<vertex>>>> OAMST::all_pins_shortest_paths(const unordered_map<vertex, unordered_map<vertex, float>>  adj_lists, set<vertex> PINS) {
    unordered_map<vertex, unordered_map<vertex, vector<vertex>>> all_paths;
    unordered_map<vertex, unordered_map<vertex, float>> all_distances;
    
    for (auto pin : PINS) {
        
        auto [distances, paths] = dijkstra(adj_lists, pin);
        all_distances[pin] = distances;
        all_paths[pin] = paths;
    }


    return {all_distances, all_paths};
}


void OAMST::print_AllPaths_AllDistances() {
    cout << "All paths: " << endl;

    for (auto &pair : all_paths) {
        vertex source = pair.first;

        for (auto &pair2 : pair.second) {
            vertex destination = pair2.first;
            cout << "Source: (" << source.x << ", " << source.y << ") -> " ;
            cout << "Dest: (" << destination.x << ", " << destination.y << ")" << endl;
            cout << "Path: ";
            for (auto &vertex : pair2.second) {
                cout << "(" << vertex.x << ", " << vertex.y << ") ";
            }

            cout << "Distance: " << all_distances[source][destination] << endl;

            cout << endl;
        }
    }
}


pair<unordered_map<vertex, unordered_map<vertex, float>>, unordered_map<vertex, unordered_map<vertex, vector<vertex>>>> OAMST::makeAllPinsGraph(set<vertex> PINS) {
    unordered_map<vertex, unordered_map<vertex, vector<vertex>>> all_PINS_paths;
    unordered_map<vertex, unordered_map<vertex, float>> all_PINS_distances;

    // cout << "All PINS paths: " << endl;

    for (auto &pair : all_paths) {
        vertex source = pair.first;

        for (auto &pair2 : pair.second) {
            vertex destination = pair2.first;

            if (source == destination) {
                continue;
            }

            if (PINS.find(destination) != PINS.end()) {  // 如果destination是PIN
                // 存入all_PINS_paths和all_PINS_distances
                all_PINS_paths[source][destination] = pair2.second;
                all_PINS_distances[source][destination] = all_distances[source][destination];

                // cout << "Source: (" << source.x << ", " << source.y << ") -> " ;
                // cout << "Dest: (" << destination.x << ", " << destination.y << ")" << endl;
                // cout << "Path: ";
                // for (auto &vertex : pair2.second) {
                //     cout << "(" << vertex.x << ", " << vertex.y << ") ";
                // }
                // cout << "Distance: " << all_PINS_distances[source][destination] << endl;
            }
        }
    }

    return {all_PINS_distances, all_PINS_paths};
}

void OAMST::print_AllPINSPaths_AllPINSDistances() {
    cout << "All PINS paths: " << endl;

    for (auto &pair : all_PINS_paths) {
        vertex source = pair.first;

        for (auto &pair2 : pair.second) {
            vertex destination = pair2.first;
            cout << "Source: (" << source.x << ", " << source.y << ") -> " ;
            cout << "Dest: (" << destination.x << ", " << destination.y << ")" << endl;
            cout << "Path: ";
            for (auto &vertex : pair2.second) {
                cout << "(" << vertex.x << ", " << vertex.y << ") ";
            }

            cout << "Distance: " << all_PINS_distances[source][destination] << endl;

            cout << endl;
        }
    }
}


void OAMST::MST_PRIM(const set<vertex> PINS) {
    int m = PINS.size();
    // 沒有涉及改權重，所以用priority_queue刪除最小值較快
    priority_queue<pair<float, vertex>, vector<pair<float, vertex>>, greater<pair<float, vertex>>> priority_queue;
    vector<float> key(m, INT_MAX);     // 用來存放每個節點到MST的最短距離
    vector<int> parent(m, -1);
    vector<bool> inMST(m, false);

    unordered_map<vertex, int> PinsMap;
    int index = 0;
    for (auto pin : PINS) {
        PinsMap[pin] = index;
        index++;
    }

    // 用第一個PIN當作起點
    vertex start = *PINS.begin();
    priority_queue.push({0.000, start});
    key[0] = 0.000;


    while (!priority_queue.empty()) {
        auto winner = priority_queue.top();
        vertex winner_vertex = winner.second;
        int winner_index = PinsMap[winner_vertex];

        priority_queue.pop();

        if (!inMST[winner_index]) {
            // cout << "Winner: (" << winner_vertex.x << ", " << winner_vertex.y << ")" << endl;
            inMST[winner_index] = true;

            for (auto neighbor : all_PINS_distances[winner_vertex]) {
                int neighbor_index = PinsMap[neighbor.first];
                vertex neighbor_vertex = neighbor.first;
                float weight = all_PINS_distances[winner_vertex][neighbor_vertex];

                if (!inMST[neighbor_index] && weight < key[neighbor_index]) {
                    key[neighbor_index] = weight;
                    priority_queue.push({weight, neighbor.first});
                    // cout << "push (" << weight << ", (" << neighbor.first.x << "," << neighbor.first.y << "))" << endl;
                    parent[neighbor_index] = winner_index;
                }
            }
        }
    }

    // print parent
    // cout << "Parent: " << endl;
    // for (int i = 0; i < parent.size(); i++) {
    //     cout << "Parent of " << i << ": " << parent[i] << endl;
    // }

    for (int i = 1; i < PINS.size(); i++) {
        MST_only_PINS.push_back({*next(PINS.begin(), parent[i]), *next(PINS.begin(), i)});
    }

    // make MST
    for (auto edge : MST_only_PINS) {
        vertex start = edge.first;
        vertex end = edge.second;

        int iter = 1;
        for (auto vertex : all_PINS_paths[start][end]) {
            if (vertex == end) {
                break;
            }
            MST.push_back({vertex, all_PINS_paths[start][end][iter]});
            iter++;
        }
    }

    return;
}

void OAMST::outputMST() {
    // print MST_only_PINS
    cout << "MST_only_PINS: " << endl;
    for (auto edge : MST_only_PINS) {
        cout << "Edge: (" << edge.first.x << ", " << edge.first.y << ") -> (" << edge.second.x << ", " << edge.second.y << ")" << endl;
    }

    // print MST
    cout << "MST: " << endl;
    for (auto edge : MST) {
        cout << "Edge: (" << edge.first.x << ", " << edge.first.y << ") -> (" << edge.second.x << ", " << edge.second.y << ")" << endl;
    }


    ofstream output_file("output_OAMST.txt");
    if (!output_file) {
        cerr << "Error opening file for writing." << endl;
        return;
    }

    output_file << "OAMST_only_PINS: " << endl;
    for (auto edge : MST_only_PINS) {
        output_file << "(" << edge.first.x << "," << edge.first.y << "),(" << edge.second.x << "," << edge.second.y << ")" << endl;
    }
    output_file << "OAMST: " << endl;
    for (auto edge : MST) {
        output_file << "(" << edge.first.x << "," << edge.first.y << "),(" << edge.second.x << "," << edge.second.y << ")" << endl;
    }

    output_file.close();

}
