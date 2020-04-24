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

Tree::Tree(map<unsigned int, int> &prob) {
    vector<pair<unsigned int, int>> new_prob(prob.begin(), prob.end());
    sort(new_prob.begin(), new_prob.end(),
            [](pair<unsigned int, int>a, pair<unsigned int, int> b){
        if(a.second == b.second)
            return a.first < b.first;
        return a.second > b.second;
    });
    this->prob = new_prob;
    this->root = nullptr;
    this->prob_cnt = prob.size();
    this->lenghts = new int[prob_cnt];
}

Tree::~Tree(){
    delete[] lenghts;
    auto pt = root;
    while(pt != nullptr){
        delete pt->l;
        if(pt->r == nullptr)
            delete pt;
        else{
            pt = pt->r;
            delete pt->p;
        }
    }
}

void Tree::build() {
    comp_word_len();
    build_tree();
}

void Tree::comp_word_len() {
    int prob_size = prob.size();
    int m = prob_size;
    vector<int> hr(m * 2, 0);
    vector<pair<int, int>> heap;
    pair<int, int> a, b;
    int i = 0;

    for(auto freq: prob){
        heap.push_back(pair<unsigned int, int> (freq.second, i + m));
        i++;
    }

    while(m > 1){
        sort(heap.begin(), heap.end(),
                [](pair<unsigned int,int>a, pair<unsigned int,int>b){return a.first < b.first;});
        a = heap.back();
        heap.pop_back();
        b = heap.back();
        heap.pop_back();
        m--;
        hr[a.second] = hr[b.second] = m;
        heap.push_back(pair<unsigned int, int> (a.first + b.first, m));
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
    sort(lenghts, lenghts+prob_cnt, [](int a, int b){return a<b;});
}

void Tree::build_tree() {
    root = new node_t;
    root->r = root->l = root->p = nullptr;
    // insert first
    node_t *new_node = nullptr;
    auto pt = root;

    unsigned int i = 0;
    int last_len = 0;
    for(auto item: prob){
        if(last_len == lenghts[i]) {
            pt->val = item.first;
            pt->r = pt->l = nullptr;
            break;
        }

        new_node = new node_t;
        new_node->r = new_node->l = nullptr;
        new_node->val = item.first;
        new_node->p = pt;
        pt->l = new_node; // leaf
        new_node = new node_t;
        new_node->r = new_node->l = nullptr;
        new_node->p = pt;
        // last item will be inserted
        pt->r = new_node; // new node
        pt = new_node;
        last_len = lenghts[i];
        i++;
    }
    if(i+1 != prob.size())
        cerr << "err" << endl;
}

node_t *Tree::get_root() {
    return root;
}

int Tree::get_tree_size() {
    return prob.size();
}
