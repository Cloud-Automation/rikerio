#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "rikerio.h"
#include "version.h"

char profile[255] = "default";
char alias[255] = "";
rio_key_t* keys = NULL;
unsigned int keyCount = 0;

static struct option long_options[] = {
    { "id", required_argument, NULL, 'i' },
    { "help", no_argument, NULL, 'h' },
    { "version", no_argument, NULL, 'v' },
    { NULL, 0, NULL, 0 }
};

static void printHelp() {

    printf("Usage: rio-alias-rm OPTIONS alias key...\n\n");
    printf("Options:\n");
    printf("\t-i|--id\t\tName of the memory profile.\n");
    printf("\t-v|--version\tPrint version.\n");
    printf("\t-h|--help\t\tPrint this help.\n\n");
    printf("Created by Stefan Pöter<rikerio@cloud-automation.de>.\n");

}

static void printVersion() {
    printf("%s\n", VERSION_SHORT);
}

static int parseArguments(int argc, char* argv[]) {

    while (1) {

        int option_index = 0;
        int c = getopt_long(argc, argv, "i::hv", long_options, &option_index);

        if (c == -1) {
            break;
        }

        char* s;

        switch (c) {
        case 'i':
            s = optarg;
            if (strlen(s) == 0) {
                fprintf(stderr, "Invalid id.\n");
                return -1;
            }
            memset(profile, 0, 255);
            sprintf(profile, "%s", s);

            break;
        case 'h':
            printHelp();
            exit(EXIT_SUCCESS);
        case 'v':
            printVersion();
            exit(EXIT_SUCCESS);
        case '?':
            printHelp();
            exit(EXIT_SUCCESS);
        }

    }


    if (optind == argc) {
        fprintf(stderr, "Missing alias.\n");
        exit(EXIT_FAILURE);
    }

    strcpy(alias, argv[optind]);

    unsigned int lIndex = 0;
    for (int index = optind + 1; index < argc; index += 1) {
        keys = realloc(keys, (lIndex + 1) * sizeof(rio_key_t));
        memcpy(&keys[lIndex++], argv[index], strlen(argv[index]));
        keyCount += 1;
    }

    return 1;

}



int main(int argc, char** argv) {

    int retVal = EXIT_SUCCESS;

    parseArguments(argc, argv);

    if (keyCount == 0) {
        if (rio_alias_rm(profile, alias, NULL) == -1) {
            return EXIT_FAILURE;
        }
    }

    for (unsigned int index = 0; index < keyCount; index += 1) {
        if (rio_alias_rm(profile, alias, keys[index]) == -1) {
            fprintf(stderr, "Error adding alias (%s).\n", strerror(errno));
            retVal = EXIT_FAILURE;
            goto exit;
        }
    }

exit:

    free(keys);
    return retVal;

}
