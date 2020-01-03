#ifndef __RIKERIO_CLIENT_RESPONSE_H__
#define __RIKERIO_CLIENT_RESPONSE_H__

#include <string>
#include <memory>
#include <json/json.h>
#include <jsonrpccpp/client.h>

#include "common/error.h"

namespace RikerIO {

class RPCResponse {

  public:
    RPCResponse(Json::Value& result) {

        if (!result.isObject()) {
            code = jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE;
            msg = result.toStyledString();
            return;
        }

        code = result["code"].asInt();

        if (code != RikerIO::NO_ERROR)  {
            msg = result["message"].asString();
            return;
        }


    }
    RPCResponse() : code(RikerIO::NO_ERROR), msg("") { }
    RPCResponse(int code, const std::string msg) :
        code(code),
        msg(msg) { }

    int get_code() {
        return code;
    }

    const std::string& get_message() {
        return msg;
    }

    bool ok() {
        return get_code() == RikerIO::NO_ERROR;
    }

  private:

    int code;
    std::string msg;

};

}

#endif
