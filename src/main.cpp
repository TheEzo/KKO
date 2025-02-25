/**
 * main.cpp
 * Author: Tomas Willaschek
 * Login: xwilla00
 * Created: 22.04.2020
 * Brief: Check args and run algorithm
 */

#include <iostream>
#include <getopt.h>
#include <cstdlib>

#include "DataFile.h"

using namespace std;

void help(){
    cout << "Usage: ./huff_codec [OPTION...]" << endl;
    cout << "\t-c, --compress\t\tCompress input" << endl;
    cout << "\t-d, --decompress\tDecompress input" << endl;
    cout << "\t-a, --adaptive\t\tTODO" << endl;
    cout << "\t-m, --model\t\tTODO" << endl;
    cout << "\t-i, --input=FILE\tInput file to process" << endl;
    cout << "\t-o, --output=FILE\tOutput location" << endl;
    cout << "\t-w, --width=VALUE\tImage width, required when compression enabled" << endl;
    cout << "\t-h, --help\t\tShow help and exit" << endl;
}

int main(int argc, char **argv) {
    struct option long_options[] = {
        {"compress", no_argument, nullptr, 'c'},
        {"decompress", no_argument, nullptr, 'd'},
        {"adaptive", no_argument, nullptr, 'a'},
        {"model", no_argument, nullptr, 'm'},
        {"input", required_argument, nullptr, 'i'},
        {"output", required_argument, nullptr, 'o'},
        {"width", optional_argument, nullptr, 'w'},
        {"help", no_argument, nullptr, 'h'},
        {nullptr, 0, nullptr, 0}
    };
    const char *short_options = "cdami:o:w:h";
    string input, output;
    int option_index;
    int c;
    bool c_in = false, d_in = false, adaptive = false;
    while ((c = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1){
        switch (c) {
            case 'c':
                c_in = true;
                break;
            case 'd':
                d_in = true;
                break;
            case 'a':
                adaptive = true;
                break;
            case 'm':
                break;
            case 'i':
                input = optarg;
                break;
            case 'o':
                output = optarg;
                break;
            case 'w':
                break;
            case 'h':
                help();
                exit(0);
            default:
                exit(1);
        }
    }
    if(input.empty() || output.empty() || (!c_in && !d_in)){
        cerr << "Required parameters: -i, -o" << endl << "Required one of: -c, -d" << endl;
        cerr << "Width required when -c" << endl << "Please see help..." << endl;
        exit(1);
    }

    auto codec = new DataFile(input, output, c_in, adaptive);
    codec->save_result();

    return 0;
}
