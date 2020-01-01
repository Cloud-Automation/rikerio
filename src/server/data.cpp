#include "server/data.h"
#include <fnmatch.h>

using namespace RikerIO;

bool Data::isValidId(const std::string& id) {

    return id.length() > 0 &&
           id.find('*') == std::string::npos &&
           id.find('?') == std::string::npos &&
           id.find(";") == std::string::npos;

}


