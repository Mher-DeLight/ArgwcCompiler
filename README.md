# ArgueWithCpp Compiler (argwc-comp)
argwc-comp is a program to convert .awc file to C++ files such that they can be directly included in the program and packaged alongside the executable. After extracting or downloading from source, put it in your `~/.bin`, `~/.local/bin`, or, if you have root access, `/user/local/bin/` directory. Then invoke with `argwc-comp sourcefile.awc argfile.hpp`. You can optionally add a `--watch` flag only at the end, which will automatically detect changes made to the source file and run the compiler. Then in C++, you can do:
```cpp
#include "argfile.hpp"
#include <argwc.h>

int main(int argc, char** argv) {
    argwc mngr (argc, argv, argfile().text);
}
```
