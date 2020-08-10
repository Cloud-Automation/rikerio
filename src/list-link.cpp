#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "rikerio.h"
#include "version.h"
#include "iostream"
#include "iomanip"
#include "algorithm"

std::string profile_id = "default";
std::string link_key = "";
std::vector<RikerIO::DataPoint> data_points;
bool in_detail = false;

static struct option long_options[] = {
    { "id", required_argument, NULL, 'i' },
    { "help", no_argument, NULL, 'h' },
    { "version", no_argument, NULL, 'v' },
    { "long", no_argument, NULL, 'l' },
    { NULL, 0, NULL, 0 }
};

static void printHelp() {

    printf("Usage: rio-alias-ls OPTIONS alias\n\n");
    printf("Options:\n");
    printf("\t-i|--id\t\tProfile ID.\n");
    printf("\t-l|--long\t\tPrint in detail.\n");
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
        int c = getopt_long(argc, argv, "i::lhv", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
        case 'i':
            if (strlen(optarg) == 0) {
                fprintf(stderr, "Invalid id.\n");
                return -1;
            }
            profile_id = optarg;

            break;
        case 'l':
            in_detail = true;
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

    return 1;

}

int main(int argc, char** argv) {

    parseArguments(argc, argv);

    RikerIO::Profile profile;

    if (RikerIO::init(profile_id, profile) == RikerIO::result_error) {
        std::cerr << "Error initializing Profile." << std::endl;
        return EXIT_FAILURE;
    }

    std::vector<std::string> data_ids;

    if (RikerIO::Link::get(profile, link_key, data_ids) == RikerIO::result_error) {
        std::cerr << "Error getting link." << std::endl;
        return EXIT_FAILURE;
    }

    unsigned int max_length = 0;

    for (auto &id : data_ids) {
        max_length = std::max(static_cast<unsigned int>(id.length()), max_length);
    }

    unsigned int err_count = 0;
    for (auto& id : data_ids) {

        RikerIO::DataPoint dp;

        std::cout << std::left << std::setw(max_length + 1) << id;

        if (!in_detail) {
            std::cout << std::endl;
            continue;
        }

        if (RikerIO::Data::get(profile, id, dp) == RikerIO::result_error) {
            err_count += 1;
            std::cout << std::endl;
            continue;
        }

        std::string tmp = std::to_string(dp.byte_offset) + "." + std::to_string(dp.bit_index);

        std::cout << std::setw(7) << std::right << tmp << " ";
        std::cout << std::setw(8) << std::left << RikerIO::type_to_string(dp.type);
        std::cout << std::setw(7) << std::left << dp.bit_size << std::endl;
    }

    return err_count == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
