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
    cout << prob.size() << endl;
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
        auto tree = new Tree(prob);
        tree->build();
    }
    else{
        read_header();
    }
}
