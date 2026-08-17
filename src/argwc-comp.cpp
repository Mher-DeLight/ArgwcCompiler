#include "../include/ArgueWithCpp/argwc-comp.h"
#include "../include/ArgueWithCpp/Parser.h"
#include "../include/ArgueWithCpp/Tokenizer.h"
#include <fstream>
#include <iostream>
#include <unordered_set>

argwc_comp::argwc_comp(int argc_, char** argv_, const std::string& filepath) {
    std::ifstream readfile(filepath);
    if (!readfile) {
        throw std::runtime_error("Couldn't open file \"" + filepath + "\"");
    }
    std::stringstream filestream;
    filestream << readfile.rdbuf();
    config_file = filestream.str();

    read_config();
};
void argwc_comp::read_config() {
    Tokenizer tkn;
    tkn.tokenize(config_file);

    Parser parser;
    parser.load_tokens(tkn.get_tokens());
    parser.parse();

    objects = std::move(parser.entry_point->children);
}