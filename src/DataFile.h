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
using std::ifstream;

class DataFile {
public:
    DataFile(const string& input, string output, bool compress, int width, bool adaptive);
    ~DataFile();

    char *get_image();
    void process();
    bool save_result();

private:
    map<unsigned int, int> get_probability();
    void read_header();

    void load_uncompressed(const string &input);
    void load_compressed(const string &input);
    void write_bit(ofstream &file, bool bit);
    bool is_leaf(const node_t &node);
    void write_codeword(ofstream &file, string keyword);
    void write_byte(ofstream &file, unsigned char byte);
    void compress_data(ofstream &file);
    void decompress_data(ifstream &file);
    bool get_next_bit(ifstream &file);

    string output;
    int width;
    char *image;
    bool compress;
    bool adaptive;
    Tree *tree;
    map<unsigned int, string> codewords;
    map<string, unsigned int> decodewords;
};


#endif //HUFF_CODEC_DATAFILE_H
