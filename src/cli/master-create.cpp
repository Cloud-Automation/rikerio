#include "client/stubclient.h"
#include <jsonrpccpp/client/connectors/unixdomainsocketclient.h>
#include <getopt.h>
#include "version.h"

std::string profile = "default";
std::string label = "unlabeled";

static struct option long_options[] = {
    { "id", required_argument, NULL, 'i' },
    { "help", no_argument, NULL, 'h' },
    { "version", no_argument, NULL, 'v' },
    { NULL, 0, NULL, 0 }
};

static void printHelp() {

    printf("Usage: rio-master-register [OPTIONS] label\n\n");
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

        switch (c) {
        case 'i':
            if (strlen(optarg) == 0) {
                fprintf(stderr, "Invalid id.\n");
                return -1;
            }
            profile = optarg;

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
        fprintf(stderr, "Missing label.\n");
        exit(EXIT_FAILURE);
    }

    label = argv[optind];

    return 1;

}



int main(int argc, char** argv) {

    parseArguments(argc, argv);

    try {
        jsonrpc::UnixDomainSocketClient socketClient("/var/run/rikerio/" + profile + "/socket");
        StubClient rpcClient(socketClient);

        Json::Value result = rpcClient.master_register(label, -1);

        int code = result["code"].asInt();
        std::string token = result["data"].asString();

        if (code == 0)  {
            printf("%s\n", token.c_str());
            return 0;
        }
        printf("%d\n", code);

        return 1;

    } catch (jsonrpc::JsonRpcException& e) {

        fprintf(stderr, "%s\n", e.what());
        printf("1\n");
        return 1;

    }


}
