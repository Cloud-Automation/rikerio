#include "client/stubclient.h"
#include <jsonrpccpp/client/connectors/unixdomainsocketclient.h>


int main(int argc, char** argv) {

    if (argc != 2) {
        return 1;
    }

    try {
        jsonrpc::UnixDomainSocketClient socketClient("/var/run/rikerio/default/socket");
        StubClient rpcClient(socketClient);

        Json::Value result = rpcClient.task_unregister(std::string(argv[1]));

        int code = result["code"].asInt();

        if (code == 0)  {
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
