/**
 * Tree.h
 * Author: Tomas Willaschek
 * Login: xwilla00
 * Created: 22.04.2020
 * Brief: in header file
 */

#include "Tree.h"

#include <vector>
#include <algorithm>

using std::ios;
using std::pair;
using std::vector;
using std::sort;
using std::reverse;

Tree::Tree() {
    this->root = nullptr;
    this->prob_cnt = 0;
    this->lenghts = nullptr;
}

Tree::~Tree(){
    delete[] lenghts;
    delete_tree(root);
}

void Tree::delete_tree(node_t *node){
    if(node->r != nullptr)
        delete_tree(node->r);
    if(node->l != nullptr)
        delete_tree(node->l);
    delete node;
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
    vector<pair<unsigned int, double>> to_insert;
    double to_insert_sum = 0;
    double total_sum = 0;
    for(unsigned int i = 0; i < prob.size(); i++){
        total_sum += prob[i].second;
    }
    root = new node_t;
    root->val = -1;
    root->l = root->r = root->p = nullptr;
    root->prob_sum = total_sum;
    auto pt = root;
    for(auto item: prob){
        to_insert_sum += item.second;
        to_insert.push_back(item);
        total_sum -= item.second;
        if(to_insert_sum >= total_sum){
            auto n = new node_t;
            n->val = -1;
            n->p = pt;
            n->l = n->r = nullptr;
            n->prob_sum = to_insert_sum;
            pt->l = n;
            insert_nodes(to_insert, n);
            to_insert.clear();
            to_insert_sum = 0;
            n = new node_t;
            n->p = pt;
            n->val = -1;
            n->prob_sum = total_sum;
            n->l = n->r = nullptr;
            pt->r = n;
            pt = pt->r;
        }
    }
    fix_tree(root);
    build_code_words(root, "");
}

map<unsigned int, string> Tree::get_code_words(){
    return code_words;
}

void Tree::fix_tree(node_t *node){
    node_t *pt;
    if(node->r != nullptr && node->l != nullptr) {
        if(node->r->val == -1 && node->r->l == nullptr && node->r->r == nullptr){
            if(node->p->l == node){
                node->p->l = node->l;
            } else{
                node->p->r = node->l;
            }
//            pt = node->l;
//            node->prob_sum = pt->prob_sum;
//            node->val = pt->val;
//            delete node->l;
//            delete node->r;
//            node->r = node->l = nullptr;
        } else{
            fix_tree(node->l);
            fix_tree(node->r);
        }
    }
    else{
        return;
    }
    if(node->r == nullptr && node->l == nullptr && node->val == -1){
        node_t *tmp = node->p;
        pt = node->p->p;
        delete node;
        tmp->r = nullptr;
        if(pt->r == tmp){
            pt->r = tmp->l;
            pt->r->p = pt;
        }
        else{
            pt->l = tmp->l;
            pt->l->p = pt;
        }
        delete tmp;
    }
}

void Tree::insert_nodes(vector<pair<unsigned int, double>> &items, node_t *location){
    node_t *pt = location;
    if(items.size() == 2){
        location->prob_sum = items[0].second + items[1].second;
        auto l = new node_t;
        auto r = new node_t;
        l->r = l->l = r->r = r->l = nullptr;
        l->val = items[0].first;
        l->prob_sum = items[0].second;
        r->val = items[1].first;
        r->prob_sum = items[1].second;
        location->l = l;
        location->r = r;
    }
    else if(items.size() == 1){
        location->l = location->r = nullptr;
        location->prob_sum = items[0].second;
        location->val = items[0].first;
    }
    else{
        double total_sum = 0;
        double add_sum = 0;
        for(auto item: items)
            total_sum += item.second;
        vector<pair<unsigned int, double>> to_insert;
        for(auto item: items){
            add_sum += item.second;
            to_insert.push_back(item);
            total_sum -= item.second;
            if(add_sum > total_sum){
                auto n = new node_t;
                n->p = pt;
                n->val = -2;
                n->l = n->r = nullptr;
                pt->l = n;
                n->prob_sum = add_sum;
                insert_nodes(to_insert, n);
                to_insert.clear();
                add_sum = 0;
                n = new node_t;
                n->p = pt;
                n->val = -1;
                n->prob_sum = total_sum;
                n->l = n->r = nullptr;
                pt->r = n;
                pt = pt->r;
            }
        }
    }
}

node_t *Tree::get_root() {
    return root;
}

int Tree::get_tree_size() {
    return code_words.size();
}

void Tree::build_with_probs(map<unsigned int, int> &prob, int inp_size) {
//    vector<pair<unsigned int, int>> new_prob(prob.begin(), prob.end());
    vector<pair<unsigned int, double>> new_prob;
    for(auto item: prob){
        new_prob.push_back({item.first, (double)item.second/inp_size});
    }
    sort(new_prob.begin(), new_prob.end(),
         [](pair<unsigned int, double>a, pair<unsigned int, double> b){
             if(a.second == b.second)
                 return a.first < b.first;
             return a.second > b.second;
         });
    this->prob = new_prob;
    this->prob_cnt = new_prob.size();
    this->lenghts = new int[prob_cnt];
    comp_word_len();
    build_tree();
}

void Tree::build_tree_with_dw(map<string, unsigned int> dw) {
    root = new node_t;
    root->val = -1;
    root->l = root->r = nullptr;
    node_t *pt;
    string cw;
    for(auto word: dw){
        pt = root;
        cw = word.first;
        while(cw.size()){
            if(cw[0] == '0'){
                if(pt->l == nullptr){
                    auto n = new node_t;
                    n->l = n->r = nullptr;
                    n->p = pt;
                    n->val = -1;
                    pt->l = n;
                }
                pt = pt->l;
            }
            else{
                if(pt->r == nullptr){
                    auto n = new node_t;
                    n->l = n->r = nullptr;
                    n->p = pt;
                    n->val = -1;
                    pt->r = n;
                }
                pt = pt->r;
            }
            cw = cw.substr(1,cw.size());
        }
        pt->val = word.second;
    }
    // insert 256
    pt = root;
    while(pt->r != nullptr)
        pt = pt->r;
    auto n = new node_t;
    n->r = n->l = nullptr;
    n->val = 256;
    pt->r = n;
    build_code_words(root,"");
}

void Tree::build_code_words(node_t *node, string prefix){
    if(node->r == nullptr && node->l == nullptr){
        code_words.insert({node->val, prefix});
    } else{
        build_code_words(node->r, prefix + "1");
        build_code_words(node->l, prefix + "0");
    }
}