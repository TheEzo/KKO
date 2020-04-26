/**
 * DataFile.cpp
 * Author: Tomas Willaschek
 * Login: xwilla00
 * Created: 22.04.2020
 * Brief: in header file
 */

#include "DataFile.h"
#include "Tree.h"

#include <utility>
#include <fstream>
#include <vector>

using std::ios;


DataFile::DataFile(const string &input, string output, bool compress, bool adaptive) {
    this->output = std::move(output);
    this->compress = compress;
    this->adaptive = adaptive;
    this->tree = nullptr;
    this->size = 0;

    if(compress){
        load_uncompressed(input);
        if(!adaptive){
            auto prob = get_probability();
            tree = new Tree();
            tree->build_with_probs(prob, size);
        }
    }
    else
        if(adaptive)
            load_compressed_a(input);
        else
            load_compressed(input);
}

DataFile::~DataFile(){
    if(tree != nullptr)
        delete tree;
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
    file.unsetf(std::ios::skipws);
    file.seekg(0, ios::beg);
    unsigned char byte;
    unsigned int val;
    string code_word;
    uint8_t header = file.get();
    // read header
    map<int,string>a;
    for(int i = 0; i < header + 1; i++){
        // read coded value
        byte = get_next_byte(file);
        val = (uint8_t) byte;
        // read codeword size
        byte = get_next_byte(file);
        code_word = "";
        for(int j = 0; j < (uint8_t)byte + 1; j++){
            code_word += get_next_bit(file) ? "1" : "0";
        }
        decodewords.insert({code_word, val});
        a.insert({val,code_word});
    }

    tree = new Tree;
    tree->build_tree_with_dw(decodewords);
    decompress_data(file);
    file.close();
}

bool DataFile::save_result() {
    ofstream file(output);
    file.unsetf(std::ios::skipws);
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
    auto codewords = tree->get_code_words();
    map<unsigned int, string>::iterator it;
    for(int i = 0; i < size; i++){
        it = codewords.find((unsigned int)image[i]);
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
    }
    size = image.size();
}

void DataFile::normal_save(ofstream &file) {
    save_tree(file);
    compress_data(file);
}

void DataFile::adaptive_save(ofstream &file) {
    bool loop = true;
    map<unsigned int, int> probs;
    map<unsigned int, int>::iterator it;
    int offset = 0;
    string code_word;
    while(loop){
        probs.clear();
        for(int i = 0; i < step; i++){
            if(i+offset == size){
                loop = false;
                break;
            }
            it = probs.find((unsigned int) image[i+offset]);
            if(it == probs.end())
                probs.insert({(uint8_t)image[i+offset], 1});
            else
                it->second++;
        }
        probs.insert({256, 1});
        delete tree;
        tree = new Tree();
        tree->build_with_probs(probs, size);
        save_tree(file);
        // insert sign to continue with 1k iterations
        if(loop)
            write_bit(file, true);
        else{
            write_bit(file, false);
            uint16_t cnt = size - offset;
            write_byte(file, (uint8_t)(cnt >> 8));
            write_byte(file, (uint8_t)cnt);
        }
        auto codewords = tree->get_code_words();
        map<unsigned int, string>::iterator it;
        for(int i = 0; i < step; i++){
            if(i+offset == size)
                break;
            it = codewords.find((uint8_t)image[i+offset]);
            write_codeword(file, it->second);
        }
        offset += step;
    }
    write_byte(file, '\0');
}

void DataFile::save_tree(ofstream &file) {
    // -1 to save max index only
    // remove key 256
    unsigned char header_size = tree->get_tree_size() - 2;
    write_byte(file, header_size);
    auto codewords = tree->get_code_words();
    for(auto cw: codewords){
        if(cw.first == 256)
            break;
        write_byte(file, (uint8_t)(cw.first));
        write_byte(file, (uint8_t)cw.second.size() - 1);
        write_codeword(file, cw.second);
    }
}

void DataFile::load_compressed_a(const string& input) {
    ifstream file(input);
    file.unsetf(std::ios::skipws);
    file.seekg(0, ios::beg);

    size = 0;
    uint8_t tree_size;
    unsigned int val;
    unsigned char byte;
    string code_word;
    int loop_len;
    bool bit, loop = true;
    while(loop){
        tree_size = (uint8_t)get_next_byte(file);
        decodewords.clear();
        for(int i = 0; i < tree_size + 1; i++){
            byte = get_next_byte(file);
            val = (uint8_t) byte;
            byte = get_next_byte(file);
            code_word = "";
            for(int j = 0; j < (uint8_t)byte + 1; j++){
                code_word += get_next_bit(file) ? "1" : "0";
            }
            decodewords.insert({code_word, val});
        }
        delete tree;
        tree = new Tree();
        tree->build_tree_with_dw(decodewords);
        bit = get_next_bit(file);
        if(bit)
            loop_len = step;
        else{
            byte = get_next_byte(file);
            uint16_t len = byte << 8;
            len |= (uint8_t) get_next_byte(file);
            loop = false;
            loop_len = len;
        }
        size += loop_len;
        for(int i = 0; i < loop_len; i++){
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
            image.push_back((uint8_t) pt->val);
        }
    }
}

char DataFile::get_next_byte(ifstream &file) {
    char byte = '\0';
    for(int j = 7; j >= 0; j--)
        byte |= (get_next_bit(file) ? 1 : 0)  << j;
    return byte;
}
