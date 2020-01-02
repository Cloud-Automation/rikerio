#include "client/client.h"
#include "common/CLI11.h"
#include "common/utils.h"

#include <memory>
#include <iostream>

std::shared_ptr<RikerIO::AbstractResponse> cmd_config_get(RikerIO::Client&);
std::shared_ptr<RikerIO::AbstractResponse> cmd_memory_alloc(RikerIO::Client&, unsigned int, bool);
std::shared_ptr<RikerIO::AbstractResponse> cmd_memory_dealloc(RikerIO::Client&, const std::string&);
std::shared_ptr<RikerIO::AbstractResponse> cmd_memory_list(RikerIO::Client&);

std::shared_ptr<RikerIO::AbstractResponse> cmd_data_add(RikerIO::Client&, const std::string&, const std::string&, const std::string&, unsigned int, unsigned int);
std::shared_ptr<RikerIO::AbstractResponse> cmd_data_add(RikerIO::Client&, const std::string&, const std::string&, unsigned int, unsigned int, unsigned int);
std::shared_ptr<RikerIO::AbstractResponse> cmd_data_remove(RikerIO::Client&, const std::string&, const std::string&);
std::shared_ptr<RikerIO::AbstractResponse> cmd_data_list(RikerIO::Client&, const std::string&, bool, const std::string, bool);

std::shared_ptr<RikerIO::AbstractResponse> cmd_link_add(
    RikerIO::Client&,
    const std::string&,
    std::vector<std::string>&);

std::shared_ptr<RikerIO::AbstractResponse> cmd_link_remove(
    RikerIO::Client&,
    const std::string&,
    std::vector<std::string>&);

std::shared_ptr<RikerIO::AbstractResponse> cmd_link_list(
    RikerIO::Client&,
    const std::string&,
    const std::string&,
    bool,
    bool,
    bool);

void clientHandler(
    const std::string& profile,
    std::function<std::shared_ptr<RikerIO::AbstractResponse>(RikerIO::Client&)> handler) {

    RikerIO::Client rpcClient(profile);

    auto response = handler(rpcClient);

    if (!response->ok()) {
        fprintf(stderr, "%s (%d)\n", response->get_message().c_str(), response->get_code());
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);


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
        RikerIO::Utils::Datatype type = RikerIO::Utils::Datatype::UNDEFINED;
        std::string token = "";
        unsigned int size = 0;
        unsigned int index = 0;
        unsigned int offset = 0;
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

    memoryAllocApp->add_option("-s,--size", memoryAllocReq.size, "Allocation Bytesize")->required();
    memoryAllocApp->add_flag("-t", memoryAllocReq.tokenOnly, "Return token only");
    memoryDeallocApp->add_option("-t,--token", memoryDeallocReq.token, "Allocation Token.")->required();

    dataAddApp->add_option("label", dataAddReq.id, "Id")->required();
    dataAddApp->add_option("-t,--token", dataAddReq.token, "Memory Token");
    dataAddApp->add_option("-i,--index", dataAddReq.index, "Bitindex (for datatypes < 8 bit)");
    dataAddApp->add_option("-o,--offset", dataAddReq.offset, "Offset, global when no token, local otherwise.")->required();

    std::string datatypeTmp = "";

    dataAddApp->add_option("-d,--datatype",
                           datatypeTmp,
                           "Use Datatypes like bit, uint8, int32, float etc. or simple bit/byte sizes like 1bit, 7bit, 1byte etc.")
    ->required()
    ->expected(1)
    ->check([&] (const std::string& val) {

        dataAddReq.type = RikerIO::Utils::GetTypeFromString(val);

        if (dataAddReq.type != RikerIO::Utils::Datatype::UNDEFINED) {
            dataAddReq.size = RikerIO::Utils::DatatypeSize[dataAddReq.type];
            return std::string();
        }

        if (RikerIO::Utils::IsWordSize(val)) {
            dataAddReq.size = RikerIO::Utils::GetBitSize(val);
            return std::string();
        }

        return std::string("Not a valid type/size.");
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

        if (dataAddReq.type != RikerIO::Utils::Datatype::UNDEFINED) {
            clientHandler(profile, [&](RikerIO::Client& client) {
                return cmd_data_add(
                           client,
                           dataAddReq.token,
                           dataAddReq.id,
                           RikerIO::Utils::GetStringFromType(dataAddReq.type),
                           dataAddReq.index,
                           dataAddReq.offset);
            });
        } else {
            clientHandler(profile, [&](RikerIO::Client& client) {
                return cmd_data_add(
                           client,
                           dataAddReq.token,
                           dataAddReq.id,
                           dataAddReq.size,
                           dataAddReq.index,
                           dataAddReq.offset);
            });
        }

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

    CLI11_PARSE(app, argc, argv);

}
