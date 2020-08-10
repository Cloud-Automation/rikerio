#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "rikerio.h"
#include "version.h"
#include "iostream"

std::string profile_id = "default";
std::string link_key = "";
std::vector<std::string> data_ids;

static struct option long_options[] = {
    { "id", required_argument, NULL, 'i' },
    { "help", no_argument, NULL, 'h' },
    { "version", no_argument, NULL, 'v' },
    { NULL, 0, NULL, 0 }
};

static void printHelp() {

    printf("Usage: rio-link-rm OPTIONS id key...\n\n");
    printf("Options:\n");
    printf("\t-i|--id\t\tProfile ID.\n");
    printf("\t-v|--version\tPrint version.\n");
    printf("\t-h|--help\tPrint this help.\n\n");
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
            profile_id = s;

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

    link_key = argv[optind];

    for (int index = optind + 1; index < argc; index += 1) {
        data_ids.push_back(std::string(argv[index]));
    }

    return 1;

}



int main(int argc, char** argv) {

    parseArguments(argc, argv);

    RikerIO::Profile profile;

    if (RikerIO::init(profile_id, profile) == RikerIO::result_error) {
        std::cerr << "Error initializing profile." << std::endl;
        return EXIT_FAILURE;
    }

    if (data_ids.size() == 0) {

        if (RikerIO::Link::remove(profile, link_key) == RikerIO::result_error) {
            std::cerr << "Error removing link." << std::endl;
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;

    }

    unsigned int err_count = 0;
    for (auto& id : data_ids) {

        if (RikerIO::Link::remove(profile, link_key, id) == RikerIO::result_error) {
            err_count += 1;
        };

    }

    if (err_count > 0) {
        std::cerr << "Errors during operation." << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

}
