#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>

#include "test.h"
#include "top_strings.h"


int main(int argc, char *argv[]) {
    int topSize = 10;                     // -n
    int maxRAM = 200; // in MB            // -s
    int num_of_threads = 1;               // -j
    int testSize = 0;
    char *outputFile = "output.txt";      // -o

    char *file = "input.txt";

    const char *optString = "n:s:j:o:h?";

    static const struct option longOpts[] = {
        { "top-size", required_argument, NULL, 'n' },
        { "fileunit-size", required_argument, NULL, 's' },
        { "threads", required_argument, NULL, 'j' },
        { "generate-test", required_argument, NULL, 't'},
        { "output-file", required_argument, NULL, 'o'},
        { "help", no_argument, NULL, 'h' },
        { NULL, no_argument, NULL, 0 }
    };
    int longIndex;

    int opt = getopt_long(argc, argv, optString, longOpts, &longIndex);
    while (opt != -1) {
        switch (opt) {
            case 'j': {
                num_of_threads = atoi(optarg);
                break;
            }
            case 's': {
                maxRAM = atoi(optarg);
                break;
            }
            case 'o': {
                outputFile = optarg;
                break;
            }
            case 't': {
                testSize = atoi(optarg);
                break;
            }
            case 'h':
            case '?': {
                break;
            }
            default:
                break;
        }
        opt = getopt_long(argc, argv, optString, longOpts, &longIndex);
    }

    if (argv[optind])
        file = argv[optind];

    if (testSize) {
        GenerateTest(file, maxRAM);
    } else {
        FindTopStrings(file, outputFile, topSize);
    }

    return 0;
}

