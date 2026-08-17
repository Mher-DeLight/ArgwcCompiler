#pragma once
#include "Common.h"
#include "ErrorHandler.h"
#include <fstream>
#include <map>
#include <sstream>

class argwc_comp {
private:
    std::string config_file = "";
    std::string output_filepath = "";
    std::vector<std::unique_ptr<Object>> objects;

    void read_config();
    void write_objects();
    void write_object(Object* obj, std::vector<uint8_t>& bytes);

public:
    argwc_comp(const std::string& infile, const std::string& outfile);
};