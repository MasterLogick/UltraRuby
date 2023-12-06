#ifndef ULTRA_RUBY_SOURCECODEEXCEPTION_H
#define ULTRA_RUBY_SOURCECODEEXCEPTION_H

#include <exception>
#include <string>

namespace UltraRuby {
namespace Lexer {

class SourceCodeException : public std::exception {
public:
    explicit SourceCodeException(std::string reason, int row, int col) :
            whatString(std::to_string(row) + ":" + std::to_string(col) + ": " + reason),
            reason(std::move(reason)), row(row), col(col) {}

    const char *what() const noexcept override {
        return whatString.c_str();
    }

    int getRow() const {
        return row;
    }

    int getCol() const {
        return col;
    }

private:
    std::string whatString;
    std::string reason;
    int row;
    int col;
};

} // UltraRuby
} // Lexer

#endif //ULTRA_RUBY_SOURCECODEEXCEPTION_H
