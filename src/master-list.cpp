#include "stubclient.h"
#include <jsonrpccpp/client/connectors/unixdomainsocketclient.h>


int main(int argc, char** argv) {

    try {
        jsonrpc::UnixDomainSocketClient socketClient("/var/run/rikerio/default/socket");
        StubClient rpcClient(socketClient);

        Json::Value result = rpcClient.master_list();

        int code = result["code"].asInt();

        if (code != 0)  {
            printf("%d\n", code);
            return 1;
        }


        Json::Value list = result["data"];

        unsigned int i = 0;

        while (list[i]) {

            printf("%s\n", list[i].asString().c_str());

            i += 1;

        }


        return 1;

    } catch (jsonrpc::JsonRpcException& e) {

        fprintf(stderr, "%s\n", e.what());
        printf("1\n");
        return 1;

    }


}
