#ifndef __RIO_TOKEN_H__
#define __RIO_TOKEN_H__

#include <string>
#include <set>
#include <random>

namespace RikerIO {

class Token {

  public:

    class TokenException : public std::exception { };

    Token(unsigned int size, unsigned int maxTries) :
        size(size),
        maxTries(maxTries) { };

    const std::string create() {
        static std::string CharacterList = "abcdefghiklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        static std::mt19937 generator{std::random_device{}()};
        static std::uniform_int_distribution<unsigned int> distribution(0,CharacterList.length());


        for (unsigned int i = 0; i < maxTries; i += 1) {

            std::string token = "";

            // create token

            for (unsigned int i = 0; i < size; i += 1) {
                /* generate secret number between 1 and charecter list length */
                unsigned int charPos = distribution(generator) % CharacterList.length();
                token.append(1, CharacterList.at(charPos));
            }


            if (tokenSet.find(token) == tokenSet.end()) {
                return token;
            }

        }

        throw TokenException();

    }

    void release(const std::string& token) {

        tokenSet.erase(token);

    }

  private:

    unsigned int size = 0;
    unsigned int maxTries = 0;

    std::set<std::string> tokenSet;

};

}






#endif
