# libcmd
A simple, portable library for parsing command line arguments in C++.

## Examples
### Very Simple

```cpp
#include "libcmd.hpp"

int main(int argc, char* argv[]) {

      bool help = false;
      bool verbose = false;
      std::string inputString = "";
      int inputInteger = 0;
      double inputDouble = 0.0;

      cmdParser pars (
            argc,
            argv,
            {     
                  Option(&help, {"-h", "--help", "--Help", "-help", "-H"}, "Shows this message"),
                  Option(&verbose, {"--verbose"}),

                  Option(&inputString, {"-i", "--input"}, "input string"),
                  Option(&inputInteger, {"-n", "--number"}, "input integer"), 
                  Option(&inputDouble, {"-d", "--double"}, "input double")
            }
      );
      
      pars.comfortDigest();

      if (help || pars.isEmpty()) {
            std::cout << "Usage:" << space(12 - 6) << "example [Flags/Options]" << std::endl;
            pars.printAll();
            exit(0);
      }
}
```

### Advanced
Have a look at "example.cpp".

## Licensing
* The file "libcmd.hpp" is licensed under the [MPL 2.0 license](https://mozilla.org/MPL/2.0/).
* The file "example.cpp" and the examples above are under the terms of [CC0 1.0](https://creativecommons.org/publicdomain/zero/1.0/).