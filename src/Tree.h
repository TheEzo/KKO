/**
 * Tree.cpp
 * Author: Tomas Willaschek
 * Login: xwilla00
 * Created: 22.04.2020
 * Brief: Create huffman tree, used for getting codewords or decoding io data
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

/**
 * Structure for code word tree
 * parent, left/right node pointers
 * sum of children probability
 * value
 */
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

    /**
     * Build tree with probability vector, used in coding
     * @param prob : vector of probabilities
     * @param inp_size : count of image pixels
     */
    void build_with_probs(map<unsigned int, int> &prob, int inp_size);
    /**
     * Get root of builded tree
     * @return root pointer
     */
    node_t *get_root();
    /**
     * Return count of code words in tree
     * @return count of code words in tree
     */
    int get_tree_size();
    /**
     * Build tree with decode words map, used in decoding
     * @param dw : decode words - value, code word
     */
    void build_tree_with_dw(map<string, unsigned int> dw);
    /**
     * Build map of value-codeword from tree
     * @return map
     */
    map<unsigned int, string> get_code_words();

private:
    /**
     * Code word length computation from KKO slides
     * Probably not used, can be deleted
     */
    void comp_word_len();
    /**
     * Build tree, available only when probability selected
     */
    void build_tree();
    /**
     * Insert vector of nodes in correct place into tree
     * @param items : vector of nodes
     * @param location : root node for selected vector
     */
    void insert_nodes(vector<pair<unsigned int, double>> &items, node_t *location);
    /**
     * Remove invalid/empty nodes recursively
     * @param node root node to fix
     */
    void fix_tree(node_t *node);
    /**
     * Create map of code words from tree recursively
     * @param node : node to build words from
     * @param prefix : prefix for node leafs
     */
    void build_code_words(node_t *node, string prefix);
    /**
     * Recursive destructor part
     * @param node : node to delete
     */
    void delete_tree(node_t *node);

    /**
     * Vector of pixel probability
     */
    vector<pair<unsigned int, double>> prob;
    /**
     * Pointer to tree root
     */
    node_t *root;
    /**
     * Length of codewords, not used, can be deleted
     */
    int *lenghts;
    /**
     * Not used as well
     */
    int prob_cnt;
    /**
     * Map of code word for each pixel from image
     */
    map<unsigned int, string> code_words;
};


#endif //HUFF_CODEC_TREE_H
