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

DataFile::DataFile(const string &input, string output, bool compress, int width) {
    this->output = std::move(output);
    this->width = width;
    this->compress = compress;

    if(compress){
        image = (char *) malloc(width*width*sizeof(char));
        load_uncompressed(input);
    }
    else
        load_compressed(input);
}

DataFile::~DataFile(){
    free(image);
}

char *DataFile::get_image() {
    return image;
}

map<unsigned int, int> DataFile::get_probability() {
    map<unsigned int, int> prob;
    unsigned int value;
    map<unsigned int, int>::iterator it;
    for(int i = 0; i < width*width; i++){
        value = (uint8_t)image[i];
        it = prob.find(value);
        if(it != prob.end())
            it->second++;
        else{
            prob.insert(pair<unsigned int, int>(value, 1));
        }
    }
    prob.insert(pair<unsigned int, int>(256, 1));
    return prob;
}

void DataFile::load_uncompressed(const string& input) {
    ifstream file(input, ios::binary);
    file.unsetf(std::ios::skipws);

    file.seekg(0, ios::beg);
    file.read(image, width*width);
    file.close();
}

void DataFile::load_compressed(const string& input) {
    ifstream file(input, ios::binary);
    file.unsetf(std::ios::skipws);

    file.seekg(0, ios::beg);
    unsigned char byte;
    unsigned int val;
    string code_word;
    uint8_t header = file.get();
    // read header
    for(int i = 0; i < (uint8_t)header + 1; i++){
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
        for(int j = 0; j < (uint8_t)byte; j++){
            code_word += get_next_bit(file) ? "1" : "0";
        }
        decodewords.insert(pair<string, unsigned int>(code_word, val));
    }
    // insert 256
    code_word = code_word.substr(0, code_word.size() - 1);
    decodewords.insert(pair<string, unsigned int>(code_word + "1", 256));

    decompress_data(file);
    file.close();
}

void DataFile::process() {
    if(compress){
        auto prob = get_probability();
        tree = new Tree(prob);
        tree->build();
    }
    else{
        // nothing to do
    }
}

bool DataFile::save_result() {
    ofstream file(output, ios::binary);
    if(compress){
        // write header size
        // remove key 256, -1 to save max index only
        unsigned char header_size = tree->get_tree_size() - 2;
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
                write_byte(file, code_word.length() + 1);
                write_codeword(file, code_word + "0");
                codewords.insert(pair<unsigned int, string>(pt->l->val, code_word + "0"));
                pt = pt->r;
                code_word += "1";
            }
        }
        for(auto i: codewords)
            cout << i.first << ": " << i.second << endl;
        compress_data(file);
    }
    else {
        for(int i = 0; i < width*width; i++)
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
    for(int i = 0; i < keyword.size(); i++){
        write_bit(file, keyword[i] == '1');
    }
}

void DataFile::write_byte(ofstream &file, const unsigned char byte) {
    for(int i = 128; i > 0; i/=2)
        write_bit(file, byte & i);
}

void DataFile::compress_data(ofstream &file) {
    map<unsigned int, string>::iterator it;
    auto data = get_image();
    for(int i = 0; i < width*width; i++){
        it = codewords.find((uint8_t)data[i]);
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
    int default_size = 512*512;
    width = 512;
    image = (char *)malloc(default_size * sizeof(char));
    int index = 0;
    string code_word;
    map<string, unsigned int>::iterator it;
    while(true){
        code_word = "";
        while(true){
            code_word += get_next_bit(file) ? "1" : "0";
            it = decodewords.find(code_word);
            if(it != decodewords.end()){
                break;
            }
        }
        if(it->second == 256)
            break;
        if(index > default_size){
            width++;
            image = (char *) realloc(image, width*width*sizeof(char));
        }
        image[index] = (uint8_t) it->second;
        index++;
    }
}
