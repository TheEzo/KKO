/**
 * DataFile.h
 * Author: Tomas Willaschek
 * Login: xwilla00
 * Created: 22.04.2020
 * Brief:
 */

#ifndef HUFF_CODEC_DATAFILE_H
#define HUFF_CODEC_DATAFILE_H

#include "Tree.h"

#include <string>
#include <fstream>
#include <map>

using std::string;
using std::map;
using std::ofstream;

class DataFile {
public:
    DataFile(const string& input, string output, bool compress, int width);
    ~DataFile();

    char *get_image();
    void process();
    bool save_result();


private:
    map<uint8_t, int> get_probability();
    void read_header();

    void load_uncompressed(const string &input);
    void load_compressed(const string &input);
    void write_bit(ofstream &file, bool bit);
    bool is_leaf(const node_t &node);
    void write_codeword(ofstream &file, const string keyword);
    void write_byte(ofstream &file, const unsigned char byte);
    void compress_data(ofstream &file);

    string output;
    int width;
    char *image;
    bool compress;
    Tree *tree;
    map<uint8_t, string> codewords;
};


#endif //HUFF_CODEC_DATAFILE_H
