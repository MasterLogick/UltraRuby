#ifndef ULTRA_RUBY_CODEGENEXCEPTION_H
#define ULTRA_RUBY_CODEGENEXCEPTION_H

#include <exception>
#include <string>

namespace UltraRuby {
namespace IR {

class CodegenException : public std::exception {
public:
    explicit CodegenException(std::string reason) : reason(std::move(reason)) {}

    const char *what() const noexcept override {
        return reason.c_str();
    }

private:
    std::string reason;
};

} // UltraRuby
} // IR

#endif //ULTRA_RUBY_CODEGENEXCEPTION_H
