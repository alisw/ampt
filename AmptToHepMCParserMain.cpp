#include <iostream>
#include <string>
#include <cstdio>

#include"AmptToHepMCParser.h"

using namespace std;

// small example program that starts the conversion progress

int main (int argc, char** argv) {
    if(argc < 2) {
        printf(" %s is an ampt to HepMC converter\n", argv[0]);
        printf(" Usage: %s [<input file name> <output file name>]\n", argv[0]);
        return 0;
    }
    // loop over argument counts
    for(int i = 1; i < argc; i++) {
        const string first_argument = argv[i];
        if (first_argument == "--help" || first_argument == "-h" || argc == 2 || argc > 3) {
            // print a short help
            printf(" %s is an ampt to HepMC converter\n", argv[0]);
            printf(" Usage: %s [<input file name> <output file name>]\n", argv[0]);
            return 0;
        } 
    }
    // create a new parser
    AmptToHepMCParser parser;
    parser.set_input_file(argv[1]);
    parser.set_output_file(argv[2]);
    
    // start the parser
    parser.run();
}
