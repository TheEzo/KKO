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

using std::map;

struct node_t {
    node_t *r;
    node_t *l;
    uint8_t val;
    int count;
};


class Tree {
public:
    explicit Tree(map<uint8_t, int> &prob);
    ~Tree();

    void build();

private:
    void comp_word_len();

    map<uint8_t, int> prob;
    node_t *root;
    int *lenghts;
};


#endif //HUFF_CODEC_TREE_H
