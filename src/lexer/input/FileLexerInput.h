#ifndef ULTRA_RUBY_LEXER_INPUt_FILELEXERINPUT_H
#define ULTRA_RUBY_LEXER_INPUt_FILELEXERINPUT_H

#include <string>
#include <fstream>
#include "../LexerInput.h"

namespace UltraRuby {
namespace Lexer {

class FileLexerInput : public LexerInput {
public:
    explicit FileLexerInput(std::string path) : ifstream(path, std::ios::binary) {}

    int getNextChar() override;

    bool eof() override;
private:
    std::ifstream ifstream;
};

} // UltraRuby
} // Lexer

#endif //ULTRA_RUBY_LEXER_INPUt_FILELEXERINPUT_H
