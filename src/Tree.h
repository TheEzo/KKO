/**
 * Tree.cpp
 * Author: Tomas Willaschek
 * Login: xwilla00
 * Created: 22.04.2020
 * Brief:
 */

#ifndef HUFF_CODEC_TREE_H
#define HUFF_CODEC_TREE_H

#include <iostream>
#include <map>
#include <vector>

using std::pair;
using std::map;
using std::vector;


struct node_t {
    node_t *p;
    node_t *r;
    node_t *l;
    uint8_t val;
};


class Tree {
public:
    explicit Tree(map<uint8_t, int> &prob);
    ~Tree();

    void build();
    node_t *get_root();
    uint8_t get_tree_size();

private:
    void comp_word_len();
    void build_tree();

    vector<pair<uint8_t, int>> prob;
    node_t *root;
    int *lenghts;
    int prob_cnt;
};


#endif //HUFF_CODEC_TREE_H
