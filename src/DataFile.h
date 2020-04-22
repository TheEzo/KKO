/**
 * DataFile.h
 * Author: Tomas Willaschek
 * Login: xwilla00
 * Created: 22.04.2020
 * Brief:
 */

#ifndef HUFF_CODEC_DATAFILE_H
#define HUFF_CODEC_DATAFILE_H

#include <string>
#include <map>

using std::string;
using std::map;

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

    string output;
    int width;
    char *image;
    bool compress;
};


#endif //HUFF_CODEC_DATAFILE_H
