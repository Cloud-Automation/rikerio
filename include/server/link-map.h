#ifndef __RIKERIO_LINK_MAP_H__
#define __RIKERIO_LINK_MAP_H__

#include <map>
#include <set>
#include <functional>

namespace RikerIO {

class LinkMap : public std::multimap<const std::string, const std::string> {

  public:
    LinkMap(const std::string);
    bool exist(const std::string&, const std::string&) const;
    bool add(const std::string&, const std::string&);

    std::set<std::string> keys() const;
    unsigned int remove(const std::string&);
    bool remove(const std::string&, const std::string&);

    void iterate(std::function<void(const std::string&, const std::string&)>);

    void serialize();
    void deserialize();

  private:
    const std::string filename;

};

}


#endif
