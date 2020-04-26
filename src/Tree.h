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
    node_t *p;
    node_t *r;
    node_t *l;
    double prob_sum;
    int val;
};


class Tree {
public:
    Tree();
    ~Tree();

    void build_with_probs(map<unsigned int, int> &prob, int inp_size);
    node_t *get_root();
    int get_tree_size();
    void build_tree_with_dw(map<string, unsigned int> dw);
    map<unsigned int, string> get_code_words();

private:
    void comp_word_len();
    void build_tree();
    void insert_nodes(vector<pair<unsigned int, double>> &items, node_t *location);
    void fix_tree(node_t *node);
    void build_code_words(node_t *node, string prefix);
    void delete_tree(node_t *node);

    vector<pair<unsigned int, double>> prob;
    node_t *root;
    int *lenghts;
    int prob_cnt;
    map<unsigned int, string> code_words;
};


#endif //HUFF_CODEC_TREE_H
