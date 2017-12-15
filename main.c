#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>


#include "test.h"
#include "top_strings.h"


int main(int argc, char *argv[]) {
    size_t fileUnit_size = 100; // in MB     // -s
    size_t num_of_threads = 1;               // -j
    bool generateTest = false;               // -t

    char *file;

    const char *optString = "s:j:to:h?";

    static const struct option longOpts[] = {
        { "fileunit-size", required_argument, NULL, 's' },
        { "threads", required_argument, NULL, 'j' },
        { "generate-test", no_argument, NULL, 't'},
        { "help", no_argument, NULL, 'h' },
        { NULL, no_argument, NULL, 0 }
    };
    int longIndex;

    int opt = getopt_long(argc, argv, optString, longOpts, &longIndex);
    while (opt != -1) {
        switch (opt) {
            case 'j': {
                num_of_threads = optarg;
                break;
            }
            case 's': {
                fileUnit_size = optarg;
                break;
            }
            case 'o': {
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

    file = argv[optind];

    if (generateTest) {
        GenerateTest(file);
    } else {
        FindTopStrings("input.txt", NULL, 10);
    }


//    size_t size = 1024 * 1024 * 1024;
//    void *lots_of_memory = calloc(size, 1);
//    if (!lots_of_memory) {
//        printf("Failed\n");
//    } else {
//        printf("Success\n");
//    }
//
//    getc(stdin);
//
//    free(lots_of_memory);

    return 0;
}

