#include "../include/ArgueWithCpp/argwc-comp.h"
#include "../include/ArgueWithCpp/Parser.h"
#include "../include/ArgueWithCpp/Tokenizer.h"
#include <fstream>
#include <iomanip>
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
void argwc_comp::write_object(Object* obj, std::vector<uint8_t>& bytes) {
    using byte = uint8_t;
    constexpr byte type_block = 1;
    constexpr byte type_arg = 2;
    constexpr byte type_flag = 3;
    constexpr byte type_val = 4;

    auto write_byte = [&bytes](byte info) { bytes.push_back(info); };
    auto write_string = [&](const std::string& str) {
        if (str.size() > UINT8_MAX) {
            throw std::runtime_error("String too long to serialize");
        }

        write_byte(static_cast<byte>(str.size()));

        bytes.insert(bytes.end(), reinterpret_cast<const byte*>(str.data()),
                     reinterpret_cast<const byte*>(str.data()) + str.size());
    };

    if (auto* arg = dynamic_cast<Object_Arg*>(obj)) {
        write_byte(type_arg);
        write_byte(arg->required ? byte(1) : byte(0));

        write_byte(byte(0));
        write_string(arg->name);

        write_byte(arg->block != nullptr ? byte(1) : byte(0));
        if (arg->block)
            write_object(arg->block.get(), bytes);
    } else if (auto* flg = dynamic_cast<Object_Flag*>(obj)) {
        write_byte(type_flag);
        write_byte(flg->required ? byte(1) : byte(0));

        write_string(flg->flag_text);
        write_string(flg->name);

        write_byte(flg->block != nullptr ? byte(1) : byte(0));
        if (flg->block)
            write_object(flg->block.get(), bytes);
    } else if (auto* val = dynamic_cast<Object_Val*>(obj)) {
        write_byte(type_val);
        write_byte(val->required ? byte(1) : byte(0));

        write_string(val->prefix_text);
        write_string(val->name);

        write_byte(val->block != nullptr ? byte(1) : byte(0));
        if (val->block)
            write_object(val->block.get(), bytes);
    } else if (auto* blck = dynamic_cast<Object_Block*>(obj)) {
        write_byte(type_block);
        write_byte(byte(0));

        write_byte(byte(0));
        write_byte(byte(0));

        byte info = blck->is_ordered ? byte(1 << 1) : byte(0 << 1);
        write_byte(info);
        for (auto& child : blck->children) {
            write_object(child.get(), bytes);
        }
    }
}
void argwc_comp::write_objects() {
    /*
    ==================================== FORMAT ===================================
    | Object type (0->invalid, 1->block, 2->arg, 3->flag, 4->val)         1 byte  |
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
    std::vector<byte> bytes;

    for (auto& obj : objects) {
        write_object(obj.get(), bytes);
    }

    std::ofstream outfile(output_filepath);
    if (!outfile) {
        throw std::runtime_error("Couldn't open output file \"" + output_filepath +
                                 "\" after compilation.");
    }

    outfile << "#ifndef ARGUE_WITH_CPP_COMPILER_143613\n"
               "#define ARGUE_WITH_CPP_COMPILER_143613\n"
               "#include <array>\n"
               "\n"
               "namespace argfile {\n"
               "    std::array<unsigned char, ";
    outfile << std::to_string(bytes.size());
    outfile << "> data = {\n        ";

    for (int i = 0; i < bytes.size(); i++) {
        auto bt = bytes[i];
        int toint = bt;
        std::cout << std::left << std::setw(10) << toint << "    ";
        std::cout << bt << "\n";
        outfile << std::to_string(toint);
        if (i != bytes.size() - 1) {
            outfile << ", ";
        }
    }
    outfile << "\n    };\n"
               "}\n"
               "#endif";

    if (!outfile) {
        throw std::runtime_error("Failed to write to \"" + output_filepath + "\"");
    }
    outfile.close();
}