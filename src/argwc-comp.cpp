#include "../include/ArgueWithCpp/argwc-comp.h"
#include "../include/ArgueWithCpp/Parser.h"
#include "../include/ArgueWithCpp/Tokenizer.h"
#include <fstream>
#include <iostream>
#include <unordered_set>

argwc_comp::argwc_comp(const std::string& infile, const std::string& outfile) {
    output_filepath = outfile;
    std::fstream teststream(outfile);
    if (!teststream) { // test if it's there before compiling to not waste time
        throw std::runtime_error("Couldn't open output file \"" + outfile + "\"");
    }
    teststream.close();

    std::ifstream readfile(infile);
    if (!readfile) {
        throw std::runtime_error("Couldn't open input file \"" + infile + "\"");
    }
    std::stringstream filestream;
    filestream << readfile.rdbuf();
    config_file = filestream.str();

    read_config();
    write_objects();
};
void argwc_comp::read_config() {
    Tokenizer tkn;
    tkn.tokenize(config_file);

    Parser parser;
    parser.load_tokens(tkn.get_tokens());
    parser.parse();

    objects = std::move(parser.entry_point->children);
}
void argwc_comp::write_objects() {
    /*
    ==================================== FORMAT ===================================
    | Object type (0->invalid, 1->argfile, 2->arg, 3->flag, 4->val)       1 byte  |
    | Info (hgfedcba, a->is required, b->is ordered)                      1 byte  |
    |                                                                             |
    | Name size                                                           1 byte  |
    | Name                                                      (name size) bytes |
    |                                                                             |
    | Varname size                                                        1 byte  |
    | Varname                                                (varname size) bytes |
    |                                                                             |
    | Child count (sucessors)                                             1 byte  |
    ===============================================================================
    */

    using byte = uint8_t;
    constexpr byte type_argfile = 1;
    constexpr byte type_arg = 2;
    constexpr byte type_flag = 3;
    constexpr byte type_val = 4;

    std::vector<byte> bytes;
    auto write_byte = [&bytes](byte info) { bytes.push_back(info); };
    auto write_string = [&](const std::string& str) {
        if (str.size() > UINT8_MAX) {
            throw std::runtime_error("String too long to serialize");
        }

        write_byte(static_cast<byte>(str.size()));

        bytes.insert(bytes.end(), reinterpret_cast<const byte*>(str.data()),
                     reinterpret_cast<const byte*>(str.data()) + str.size());
    };

    for (auto& obj : objects) {
        if (auto* arg = dynamic_cast<Object_Arg*>(obj.get())) {
            write_byte(type_arg);
            write_byte(arg->required ? byte(1) : byte(0));

            write_string(arg->name);
            write_byte(byte(0));

            write_byte(arg->block != nullptr ? byte(1) : byte(0));
        }
    }

    std::ofstream outfile(output_filepath, std::ios::binary);
    if (!outfile) {
        throw std::runtime_error("Couldn't open output file \"" + output_filepath +
                                 "\" after compilation.");
    }

    outfile.write(reinterpret_cast<const char*>(bytes.data()),
                  static_cast<std::streamsize>(bytes.size()));

    if (!outfile) {
        throw std::runtime_error("Failed to write to \"" + output_filepath + "\"");
    }
    outfile.close();
}