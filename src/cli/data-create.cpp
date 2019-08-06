#include "client/stubclient.h"
#include <jsonrpccpp/client/connectors/unixdomainsocketclient.h>
#include <getopt.h>
#include "version.h"
#include "common/data.h"
#include "memory.h"

std::string profile = "default";
std::string dataId = "unlabeled";
std::string token = "";
Json::Value jsonData;


static struct option long_options[] = {
    { "id", required_argument, NULL, 'i' },
    { "token", required_argument, NULL, 't' },
    { "help", no_argument, NULL, 'h' },
    { "version", no_argument, NULL, 'v' },
    { NULL, 0, NULL, 0 }
};

static void printHelp() {

    printf("Usage: rio-data-add [OPTIONS] id offset bitSize datatype\n\n");
    printf("Options:\n");
    printf("\t-i|--id\t\tName of the memory profile.\n");
    printf("\t-t||--token\tMaster Token.\n");
    printf("\t-v|--version\tPrint version.\n");
    printf("\t-h|--help\t\tPrint this help.\n\n");
    printf("Example:\n");
    printf("\trio-data-add --id=io my-data 10.3 1 bool\n\n");
    printf("Created by Stefan Pöter<rikerio@cloud-automation.de>.\n");

}

static void printVersion() {
    printf("%s\n", VERSION_SHORT);
}

static void dataParseOffset(std::string str, Json::Value& value) {

}

static int parseArguments(int argc, char* argv[]) {

    while (1) {

        int option_index = 0;
        int c = getopt_long(argc, argv, "i::t::hv", long_options, &option_index);

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
        case 't':
            if (strlen(optarg) == 0) {
                fprintf(stderr, "Invalid token.\n");
                return -1;
            }
            token = optarg;
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

    std::string id;
    unsigned int byteOffset;
    unsigned int bitOffset;
    unsigned int bitSize;
    std::string datatype;

    unsigned int nonOptPos = 0;
    while (optind < argc) {

        printf("%d : %s\n", nonOptPos++, argv[optind++]);

        switch (nonOptPos) {

        case(0) :

            break;
        case(1):
            break;
        case(2):
            break;
        case(3):
            break;

        }

    }


    return 1;

}



int main(int argc, char** argv) {

    parseArguments(argc, argv);

    try {
        jsonrpc::UnixDomainSocketClient socketClient("/var/run/rikerio/" + profile + "/socket");
        StubClient rpcClient(socketClient);

        /*        Json::Value result = rpcClient.master_register(label, -1);

                int code = result["code"].asInt();
                std::string token = result["data"].asString();

                if (code == 0)  {
                    printf("%s\n", token.c_str());
                    return 0;
                }
                printf("%d\n", code);
        */
        return 1;

    } catch (jsonrpc::JsonRpcException& e) {

        fprintf(stderr, "%s\n", e.what());
        printf("1\n");
        return 1;

    }


}
