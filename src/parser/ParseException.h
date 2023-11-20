#ifndef ULTRA_RUBY_PARSEEXCEPTION_H
#define ULTRA_RUBY_PARSEEXCEPTION_H

#include <exception>
#include <string>
#include <utility>

namespace UltraRuby {
namespace Parser {

class ParseException : public std::exception {
public:
    explicit ParseException(std::string reason) : reason(std::move(reason)) {}

    const char *what() const noexcept override {
        return reason.c_str();
    }

private:
    std::string reason;
};

} // UltraRuby
} // Parser

#endif //ULTRA_RUBY_PARSEEXCEPTION_H
