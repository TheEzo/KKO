/**
 * Tree.h
 * Author: Tomas Willaschek
 * Login: xwilla00
 * Created: 22.04.2020
 */

#include "Tree.h"

#include <vector>
#include <algorithm>

///TODO delete this block
/////////////////////////
#include <iostream>
using namespace std;

/////////////////////////

using std::pair;
using std::vector;

Tree::Tree(map<uint8_t, int> &prob) {
    this->prob = prob;
    this->root = nullptr;
    this->lenghts = new int[prob.size()];
}

Tree::~Tree(){
    delete[] lenghts;
};

void Tree::build() {
    comp_word_len();
}

void Tree::comp_word_len() {
    int prob_size = 4; //prob.size();
    int m = prob_size;
    vector<int> hr(m * 2, 0);
    vector<pair<int, int>> heap;
    pair<int, int> a, b;
    int i = 0;

//    for(auto freq: prob){
//        heap.push_back(pair<int, int> (freq.second, i + m));
//        i++;
//    }

    heap.push_back(pair<int, int> (30,4));
    heap.push_back(pair<int, int> (15, 5));
    heap.push_back(pair<int, int> (10,6));
    heap.push_back(pair<int, int> (7,7));
//    heap.push_back(pair<int, int> (1,8));

    while(m > 1){
        sort(heap.begin(), heap.end(), [](pair<int,int>a, pair<int,int>b){return a.first < b.first;});
        a = heap.back();
        heap.pop_back();
        b = heap.back();
        heap.pop_back();
        m--;
        hr[a.second] = hr[b.second] = m;
        heap.push_back(pair<int, int> (a.first + b.first, m));
    }

    int j, l;
    for(i = 0; i < prob_size; i++){
        j = hr[prob_size + i];
        l = 1;
        while (j > 1){
            j = hr[j];
            l++;
        }
        lenghts[i] = l;
    }

    for(i = prob_size-1; i >= 0; i--)
        cout << lenghts[i] << ",";
    cout << endl;
}
