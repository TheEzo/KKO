/**
 * DataFile.cpp
 * Author: Tomas Willaschek
 * Login: xwilla00
 * Created: 22.04.2020
 */

#include "DataFile.h"
#include "Tree.h"

#include <utility>
#include <fstream>
#include <vector>
#include <cstdlib>

///TODO delete this block
/////////////////////////
#include <iostream>
#include <algorithm>
using namespace std;

/////////////////////////

DataFile::DataFile(const string &input, string output, bool compress, int width, bool adaptive, bool model) {
    this->output = std::move(output);
    this->width = width;
    this->compress = compress;
    this->adaptive = adaptive;
    this->model = model;
    this->tree = nullptr;

    if(compress){
        load_uncompressed(input);
        if(!adaptive){
            auto prob = get_probability();
            tree = new Tree();
            tree->build_with_probs(prob);
        }
    }
    else
        load_compressed(input);
}

DataFile::~DataFile(){
    if(tree != nullptr)
        delete tree;
}

vector<unsigned char> DataFile::get_image() {
    return image;
}

map<unsigned int, int> DataFile::get_probability() {
    map<unsigned int, int> prob;
    unsigned int value;
    map<unsigned int, int>::iterator it;
    for(int i = 0; i < size; i++){
        value = (uint8_t)image[i];
        it = prob.find(value);
        if(it != prob.end())
            it->second++;
        else{
            prob.insert({value, 1});
        }
    }
    prob.insert(pair<unsigned int, int>(256, 1));
    int a = prob.size();
    return prob;
}

void DataFile::load_uncompressed(const string& input) {
    ifstream file(input);
    file.unsetf(std::ios::skipws);
    file.seekg (0, ios::end);
    size = file.tellg();
    file.seekg (0, ios::beg);
    unsigned char byte;
    for(int i = 0; i < size; i++){
        file >> byte;
        image.push_back(byte);
    }
    file.close();
}

void DataFile::load_compressed(const string& input) {
    ifstream file(input);

    file.seekg(0, ios::beg);
    unsigned char byte;
    unsigned int val;
    string code_word;
    uint8_t header = file.get();
    // read header
    for(int i = 0; i < header + 1; i++){
        // read coded value
        byte = '\0';
        for(int j = 7; j >= 0; j--)
            byte |= (get_next_bit(file) ? 1 : 0) << j;
        val = (uint8_t) byte;
        // read codeword size
        byte = '\0';
        for(int j = 7; j >= 0; j--)
            byte |= (get_next_bit(file) ? 1 : 0) << j;
        code_word = "";
        for(int j = 0; j < (uint8_t)byte + 1; j++){
            code_word += get_next_bit(file) ? "1" : "0";
        }
        decodewords.insert({code_word, val});
    }
    // insert 256
    code_word = code_word.substr(0, code_word.size() - 1);
    decodewords.insert({code_word + "1", 256});
    int a = decodewords.size();
    tree = new Tree;
    tree->build_tree_with_dw(decodewords);
    decompress_data(file);
    file.close();
}

bool DataFile::save_result() {
    ofstream file(output);
    if(compress){
        if(adaptive)
            adaptive_save(file);
        else
            normal_save(file);
    }
    else {
        for(int i = 0; i < size; i++)
            file << image[i];
    }
    file.close();

    return true;
}

void DataFile::write_bit(ofstream &file, bool bit) {
    static char byte = '\0';
    static int byte_pos = 7;

    byte |= (bit ? 1 : 0) << byte_pos;
    byte_pos--;
    if(byte_pos < 0){
        file << byte;
        byte_pos = 7;
        byte = '\0';
    }
}

bool DataFile::is_leaf(const node_t &node) {
    return node.r == nullptr && node.l == nullptr;
}

void DataFile::write_codeword(ofstream &file, const string keyword) {
    for(unsigned int i = 0; i < keyword.size(); i++){
        write_bit(file, keyword[i] == '1');
    }
}

void DataFile::write_byte(ofstream &file, const unsigned char byte) {
    for(int i = 128; i > 0; i/=2)
        write_bit(file, byte & i);
}

void DataFile::compress_data(ofstream &file) {
    map<unsigned int, string>::iterator it;
    for(int i = 0; i < size; i++){
        it = codewords.find((uint8_t)image[i]);
        write_codeword(file, it->second);
    }
    it = codewords.find(256);
    write_codeword(file, it->second);
    // finish last byte with zeros
    write_byte(file, '\0');
}

bool DataFile::get_next_bit(ifstream &file) {
    static unsigned char byte = '\0';
    static int mask = 0;

    if(mask == 0){
        byte = file.get();
        mask = 128;
    }

    bool bit = byte & mask;
    mask /= 2;
    return bit;
}

void DataFile::decompress_data(ifstream &file) {
    int index = 0;
    string code_word;
    map<string, unsigned int>::iterator it;
    bool bit;
    while(true){
        auto pt = tree->get_root();
        code_word = "";
        while(true){
            bit = get_next_bit(file);
            code_word += (bit ? "1" : "0");
            if(bit)
                pt = pt->r;
            else
                pt = pt->l;
            if(is_leaf(*pt))
                break;
        }
        if(pt->val == 256)
            break;
        image.push_back((uint8_t) pt->val);
        index++;
    }
    size = image.size();
}

void DataFile::normal_save(ofstream &file) {
    save_tree(file, true);
    compress_data(file);
}

void DataFile::adaptive_save(ofstream &file) {
    int step = 1000;
    bool loop = true;
    map<unsigned int, int> probs;
    map<unsigned int, int>::iterator it;
    int start_index = 0, data_size = width*width;
    node_t *pt;
    string code_word;
    while(loop){
        for(int i = 0; i < step && loop; i++){
            it = probs.find((unsigned int) image[i+start_index]);
            if(it == probs.end())
                probs.insert({(uint8_t)image[i+start_index], 1});
            else
                it->second++;
            if(i+start_index == data_size-1)
                loop = false;
        }
        delete tree;
        tree = new Tree();
        tree->build_with_probs(probs);
        save_tree(file, false);
        // insert sign to continue with 1k iterations
        if(loop)
            write_bit(file, true);
        else{
            write_bit(file, false);
            uint16_t cnt = data_size - start_index;
            write_byte(file, (uint8_t)(cnt >> 8));
            write_byte(file, (uint8_t)cnt);
        }
        for(int i = 0; loop && i < step; i++){
            pt = tree->get_root();
            code_word = "";
            while(true){
                if(pt->l->val == (uint8_t)image[i+start_index]){
                    pt = pt->l;
                    code_word += "0";
                }
                else{
                    pt = pt->r;
                    code_word += "1";
                }
                if(is_leaf(*pt))
                    break;
            }
            write_codeword(file, code_word);
        }
        start_index += step;
    }
    write_byte(file, '\0');
}

void DataFile::save_tree(ofstream &file, bool normal) {
    // write header size
    // -1 to save max index only
    unsigned char header_size = tree->get_tree_size() - 1;
    // remove key 256
    if(normal)
        header_size -= 1;
    write_byte(file, header_size);
    node_t *pt = tree->get_root();
    string code_word = "";
    while(pt != nullptr){
        if(is_leaf(*pt)){
            // last tree element 11..1
            // value 256 - EOF
            codewords.insert(pair<unsigned int, string>(pt->val, code_word));
            pt = nullptr;
        }
        else{
            write_byte(file, pt->l->val);
            write_byte(file, code_word.size());
            write_codeword(file, code_word + "0");
            codewords.insert(pair<unsigned int, string>(pt->l->val, code_word + "0"));
            pt = pt->r;
            code_word += "1";
        }
    }
}
