#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>

#include "test.h"
#include "top_strings.h"


int main(int argc, char *argv[]) {
    int topSize = 10;                     // -n
    int fileUnit_size = 100; // in MB     // -s
    int num_of_threads = 1;               // -j
    bool generateTest = false;            // -t
    char *outputFile = "output.txt";      // -o

    char *file = "input.txt";

    const char *optString = "n:s:j:to:h?";

    static const struct option longOpts[] = {
        { "top-size", required_argument, NULL, 'n' },
        { "fileunit-size", required_argument, NULL, 's' },
        { "threads", required_argument, NULL, 'j' },
        { "generate-test", no_argument, NULL, 't'},
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
                fileUnit_size = atoi(optarg);
                break;
            }
            case 'o': {
                outputFile = optarg;
                break;
            }
            case 't': {
                generateTest = true;
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

    if (generateTest) {
        GenerateTest(file);
    } else {
        FindTopStrings(file, outputFile, topSize);
    }

    return 0;
}

