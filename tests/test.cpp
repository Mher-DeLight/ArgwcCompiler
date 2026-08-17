#include <ArgueWithCpp/argwc-comp.h>
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 3) {
        throw std::runtime_error(
            "Not enough arguments; pass at least input and output file paths.");
    }
    if (argc > 4) {
        throw std::runtime_error("Too many arguments; pass, at most, input and output file paths, "
                                 "alongside an optional --watch flag at the end");
    }
    if (argc == 4 && argv[3] != "--watch") {
        throw std::runtime_error("Invalid flag \"" + std::string(argv[3]) + "\"");
    }

    argwc_comp comp(argv[1], argv[2]);
    return 0;
}