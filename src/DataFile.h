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
    DataFile(const string& input, string output, bool compress, int width, bool adaptive, bool model);
    ~DataFile();

    /**
     * Save result to output file
     * @return
     */
    bool save_result();

private:
    /**
     * Scan whole image and get probability of pixels
     * @return count of each pixel in image
     */
    map<unsigned int, int> get_probability();

    /**
     * Load raw image into memory
     * @param input: disk path
     */
    void load_uncompressed(const string &input);

    /**
     * Load normally compressed image into memory
     * @param input: disk path
     */
    void load_compressed(const string &input);
    /**
     * Write single bit into file
     * @param open file descriptor
     * @param bit 1/0
     */
    void write_bit(ofstream &file, bool bit);
    /**
     * Check if node is leaf
     * @param node : tested node
     * @return true/false
     */
    bool is_leaf(const node_t &node);
    /**
     * Save codeword to file
     * @param file : open file descriptor
     * @param keyword : codeword to save
     */
    void write_codeword(ofstream &file, string keyword);
    /**
     * Write 8bits to file
     * @param file : open file descriptor
     * @param byte : value
     */
    void write_byte(ofstream &file, unsigned char byte);
    /**
     * Compress bare image data to file, no adaptive, no model, just bare compress
     * @param file : open file descriptor
     */
    void compress_data(ofstream &file);
    /**
     * Load normally compressed image data into memory
     * @param file : open file descriptor
     */
    void decompress_data(ifstream &file);
    /**
     *
     * @param file
     * @return
     */
    bool get_next_bit(ifstream &file);
    char get_next_byte(ifstream &file);
    void save_tree(ofstream &file);

    void normal_save(ofstream &file);
    void adaptive_save(ofstream &file);
    void load_compressed_a(const string& input);

    string output;
    int width;
    int size;
    int step = 1000;
    vector<unsigned char> image;
    bool compress;
    bool adaptive;
    bool model;
    Tree *tree;
    map<string, unsigned int> decodewords;
};


#endif //HUFF_CODEC_DATAFILE_H
