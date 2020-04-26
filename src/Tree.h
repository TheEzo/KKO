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
using std::string;

struct node_t {
    node_t *r;
    node_t *l;
    int val;
};


class Tree {
public:
    Tree();
    ~Tree();

    void build_with_probs(map<unsigned int, int> &prob);
    node_t *get_root();
    int get_tree_size();
    void build_tree_with_dw(map<string, unsigned int> dw);

private:
    void comp_word_len();
    void build_tree();

    vector<pair<unsigned int, int>> prob;
    node_t *root;
    int *lenghts;
    int prob_cnt;
};


#endif //HUFF_CODEC_TREE_H
