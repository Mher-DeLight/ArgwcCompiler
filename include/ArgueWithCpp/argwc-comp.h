#pragma once
#include "Common.h"
#include "ErrorHandler.h"
#include <fstream>
#include <map>
#include <sstream>

class argwc_comp {
private:
    std::string config_file = "";
    std::vector<std::unique_ptr<Object>> objects;

    void read_config();

public:
    argwc_comp(int argc_, char** argv_, const std::string& filepath);
};