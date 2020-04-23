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

///TODO delete this block
/////////////////////////
#include <iostream>
using namespace std;

/////////////////////////

DataFile::DataFile(const string &input, string output, bool compress, int width=0) {
    this->output = std::move(output);
    this->width = width;
    this->compress = compress;
    this->image = new char[width*width];

    if(compress)
        load_uncompressed(input);
    else
        load_compressed(input);
}

DataFile::~DataFile(){
    delete[] image;
}

char *DataFile::get_image() {
    return this->image;
}

map<uint8_t, int> DataFile::get_probability() {
    map<uint8_t, int> prob;
    uint8_t value;
    map<uint8_t, int>::iterator it;
    for(int i = 0; i < width*width; i++){
        value = image[i];
        it = prob.find(value);
        if(it != prob.end())
            it->second++;
        else{
            prob.insert(pair<uint8_t, int>(value, 1));
        }
    }
    return prob;
}

void DataFile::read_header() {
    cout << "TODO" << endl;
}

void DataFile::load_uncompressed(const string& input) {
    ifstream file(input, ios::binary);
    file.unsetf(std::ios::skipws);

    file.seekg(0, ios::beg);
    file.read(image, width*width);
    file.close();
}

void DataFile::load_compressed(const string& input) {
    cout << "TODO" << endl;
    ifstream file(input, ios::binary);
    file.unsetf(std::ios::skipws);

    file.seekg(0, ios::beg);
    // TODO
//    file.read(image, width*width);
    file.close();
}

void DataFile::process() {
    if(compress){
        auto prob = get_probability();
        tree = new Tree(prob);
        tree->build();
    }
    else{
        read_header();
    }
}

bool DataFile::save_result() {
    ofstream file(output, ios::binary);
    if(compress){
        // write header size
        char header_size = tree->get_tree_size();
        write_byte(file, header_size);
        node_t *pt = tree->get_root();
        string code_word = "";
        while(pt != nullptr){
            if(is_leaf(*pt)){ // last tree element 11...1
                write_byte(file, pt->val);
                write_byte(file, code_word.length());
                write_codeword(file, code_word);
                codewords.insert(pair<uint8_t, string>(pt->val, code_word));
                pt = nullptr;
            }
            else{
                write_byte(file, pt->l->val);
                write_byte(file, code_word.length() + 1);
                write_codeword(file, code_word + "0");
                codewords.insert(pair<uint8_t, string>(pt->l->val, code_word + "0"));
                pt = pt->r;
                code_word += "1";
            }
        }
        compress_data(file);
        // TODO write 256
    }
    else {

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
    for(uint8_t i = 128; i > 0; i/=2)
        write_bit(file, byte & i);
}

void DataFile::compress_data(ofstream &file) {
    map<uint8_t, string>::iterator it;
    auto data = get_image();
    for(int i = 0; i < width*width; i++){
        it = codewords.find((int)data[i]);
        write_codeword(file, it->second);
    }
}
