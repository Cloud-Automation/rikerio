#include "client/client.h"
#include "memory"
#include "common/CLI11.h"
#include "common/type.h"
#include "common/mem-position.h"

std::shared_ptr<RikerIO::RPCResponse> cmd_config_get(RikerIO::Client&);
std::shared_ptr<RikerIO::RPCResponse> cmd_memory_alloc(RikerIO::Client&, unsigned int, bool);
std::shared_ptr<RikerIO::RPCResponse> cmd_memory_dealloc(RikerIO::Client&, const std::string&);
std::shared_ptr<RikerIO::RPCResponse> cmd_memory_list(RikerIO::Client&);

std::shared_ptr<RikerIO::RPCResponse> cmd_data_add(RikerIO::Client&, const std::string&, const std::string&, RikerIO::Type&, RikerIO::MemoryPosition&);
std::shared_ptr<RikerIO::RPCResponse> cmd_data_remove(RikerIO::Client&, const std::string&, const std::string&);
std::shared_ptr<RikerIO::RPCResponse> cmd_data_list(RikerIO::Client&, const std::string&, bool, const std::string, bool);

int cmd_read(std::vector<std::string>&, RikerIO::Client&);

std::shared_ptr<RikerIO::RPCResponse> cmd_link_add(
    RikerIO::Client&,
    const std::string&,
    std::vector<std::string>&);

std::shared_ptr<RikerIO::RPCResponse> cmd_link_remove(
    RikerIO::Client&,
    const std::string&,
    std::vector<std::string>&);

std::shared_ptr<RikerIO::RPCResponse> cmd_link_list(
    RikerIO::Client&,
    const std::string&,
    const std::string&,
    bool,
    bool,
    bool);

void clientHandler(
    const std::string& profile,
    std::function<std::shared_ptr<RikerIO::RPCResponse>(RikerIO::Client&)> handler) {

    RikerIO::Client rpcClient(profile);

    auto response = handler(rpcClient);

    if (!response->ok()) {
        fprintf(stderr, "%s (%d)\n", response->get_message().c_str(), response->get_code());
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);

}

void simpleClientHandler(
    const std::string& profile,
    std::function<int(RikerIO::Client&)> handler) {

    RikerIO::Client rpcClient(profile);

    int res = handler(rpcClient);

    exit(res);


}


int main(int argc, char** argv) {

    std::string profile = "default";

    CLI::App app;

    app
    .fallthrough()
    ->add_option("-p,--profile", profile, "RikerIO Profile.", true)
    ->envname("RIO_PROFILE");

    auto configApp = app.add_subcommand("config", "Get Profile Configuration.");
    auto memoryApp = app.add_subcommand("memory", "Memory related commands.");
    auto dataApp = app.add_subcommand("data", "Data related commands.");
    auto linkApp = app.add_subcommand("link", "Link related commands.");
    auto readApp = app.add_subcommand("read", "Read data/link values.");

    auto memoryAllocApp = memoryApp->add_subcommand("alloc", "Allocate Memory.");
    auto memoryDeallocApp = memoryApp->add_subcommand("dealloc", "Deallocate Memory.");
    auto memoryListApp = memoryApp->add_subcommand("ls", "List Memory allocations.");
    auto dataAddApp = dataApp->add_subcommand("add", "Create new data point.");
    auto dataListApp = dataApp->add_subcommand("ls", "List data points.");
    auto dataRemoveApp = dataApp->add_subcommand("rm", "Remove data points.");
    auto linkAddApp = linkApp->add_subcommand("add", "Create new Link.");
    auto linkRemoveApp = linkApp->add_subcommand("rm", "Remove link.");
    auto linkListApp = linkApp->add_subcommand("ls", "List links.");

    struct {
        unsigned int size = 0;
        bool tokenOnly = false;
    } memoryAllocReq;

    struct {
        std::string token = "";
    } memoryDeallocReq;

    struct {
        unsigned int size;
        std::string type;
    } dataAddType;

    struct {
        std::string id = "";
        std::string token = "";
        std::string typeStr = "";
        std::string offsetStr = "";
        RikerIO::Type type;
        RikerIO::MemoryPosition offset;
    } dataAddReq;

    struct {
        std::string pattern = "*";
        std::string sortBy = "";
        bool extendedList = false;
        bool sortDesc = false;
    } dataListReq;

    struct {
        std::string token = "";
        std::string pattern = "";
    } dataRemoveReq;

    struct {
        std::string key;
        std::vector<std::string> list;
    } linkAddReq;

    struct {
        std::string pattern = "*";
        std::string sortBy = "";
        bool extendedList = false;
        bool sortDesc = false;
        bool hideEmptyLinks = false;
    } linkListReq;

    struct {
        std::string pattern = "*";
        std::vector<std::string> list;
    } linkRemoveReq;


    struct {
        std::string pattern = "";
        std::vector<std::string> list;
    } readReq;

    memoryAllocApp->add_option("-s,--size", memoryAllocReq.size, "Allocation Bytesize")->required();
    memoryAllocApp->add_flag("-t", memoryAllocReq.tokenOnly, "Return token only");
    memoryDeallocApp->add_option("-t,--token", memoryDeallocReq.token, "Allocation Token.")->required();

    dataAddApp->add_option("label", dataAddReq.id, "Id")->required();
    dataAddApp->add_option("-t,--token", dataAddReq.token, "Memory Token");
    dataAddApp->add_option("-o,--offset", dataAddReq.offsetStr, "Offset, global when no token, local otherwise.")
    ->required()
    ->check([&] (const std::string& val) {

        try {
            dataAddReq.offset = RikerIO::MemoryPosition(val);
        } catch (...) {
            return std::string("Not a valid type/size.");
        }
        return std::string();
    });

    std::string datatypeTmp = "";

    dataAddApp->add_option("-d,--type",
                           dataAddReq.typeStr,
                           "Use Datatypes like bit, uint8, int32, float etc. or simple bit/byte sizes like 1bit, 7bit, 1byte etc.")
    ->required()
    ->expected(1)
    ->check([&] (const std::string& val) {

        try {
            dataAddReq.type = RikerIO::Type(val);
        } catch (...) {
            return std::string("Not a valid type/size.");
        }
        return std::string();
    });

    dataListApp->add_option("pattern", dataListReq.pattern, "Filter pattern.");
    dataListApp->add_option("-s,--sort", dataListReq.sortBy, "Sort by column offset/id.");
    dataListApp->add_flag("-l", dataListReq.extendedList, "Long listing format");
    dataListApp->add_flag("-d", dataListReq.sortDesc, "Reverse order");

    dataRemoveApp->add_option("pattern", dataRemoveReq.pattern, "Filter pattern.");
    dataRemoveApp->add_option("-t,--token", dataRemoveReq.token, "Memory token.");

    linkAddApp->add_option("key", linkAddReq.key, "Link Key.")->required();
    linkAddApp->add_option("data", linkAddReq.list, "Data IDs")->required();

    linkListApp->add_option("pattern", linkListReq.pattern, "Filter pattern.");
    linkListApp->add_option("-s,--sort", linkListReq.sortBy, "Sort by column key/offset/id.");
    linkListApp->add_flag("-l,--long", linkListReq.extendedList, "Long listing format");
    linkListApp->add_flag("-d,--desc", linkListReq.sortDesc, "Reverse order");
    linkListApp->add_flag("-e,--hide-empty", linkListReq.hideEmptyLinks, "Hide Empty Links");

    linkRemoveApp->add_option("linkname", linkRemoveReq.pattern, "Link ID")->required();
    linkRemoveApp->add_option("data", linkRemoveReq.list, "Data IDs.");

    readApp->add_option("pattern", readReq.list, "Data/List patterns")->required();

    configApp->callback([&] () {
        clientHandler(profile, [](RikerIO::Client& client) {
            return cmd_config_get(client);
        });
    });

    memoryAllocApp->callback([&] () {
        clientHandler(profile, [&](RikerIO::Client& client) {
            return cmd_memory_alloc(client, memoryAllocReq.size, memoryAllocReq.tokenOnly);
        });
    });

    memoryDeallocApp->callback([&]() {
        clientHandler(profile, [&](RikerIO::Client& client)  {
            return cmd_memory_dealloc(client, memoryDeallocReq.token);
        });
    });

    memoryListApp->callback([&] {
        clientHandler(profile, [&](RikerIO::Client& client) {
            return cmd_memory_list(client);
        });
    });

    dataAddApp->callback([&] () {

        clientHandler(profile, [&](RikerIO::Client& client) {
            return cmd_data_add(
                       client,
                       dataAddReq.id,
                       dataAddReq.token,
                       dataAddReq.type,
                       dataAddReq.offset);
        });

    });

    dataRemoveApp->callback([&] () {
        clientHandler(profile, [&] (RikerIO::Client& client) {
            return cmd_data_remove(client, dataRemoveReq.token, dataRemoveReq.pattern);
        });
    });

    dataListApp->callback([&] () {
        clientHandler(profile, [&](RikerIO::Client& client) {
            return cmd_data_list(client, dataListReq.pattern, dataListReq.extendedList, dataListReq.sortBy, dataListReq.sortDesc);
        });
    });



    linkAddApp->callback([&] () {
        clientHandler(profile, [&](RikerIO::Client& client) {
            return cmd_link_add(client, linkAddReq.key, linkAddReq.list);
        });
    });

    linkRemoveApp->callback([&] () {
        clientHandler(profile, [&](RikerIO::Client& client) {
            return cmd_link_remove(client, linkRemoveReq.pattern, linkRemoveReq.list);
        });
    });

    linkListApp->callback([&] () {
        clientHandler(profile, [&](RikerIO::Client& client) {
            return cmd_link_list(client, linkListReq.pattern, linkListReq.sortBy, linkListReq.extendedList, linkListReq.sortDesc, linkListReq.hideEmptyLinks);
        });
    });

    readApp->callback([&]() {
        simpleClientHandler(profile, [&](RikerIO::Client& client) {
            return cmd_read(readReq.list, client);
        });
    });

    CLI11_PARSE(app, argc, argv);

}
