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
                  make_flag(&help, {"-h", "--help", "--Help", "-help", "-H"}, "Shows this message"),
                  make_flag(&verbose, {"--verbose"})
            },
            {
                  make_option(&inputString, {"-i", "--input"}, "input string"),
                  make_option(&inputInteger, {"-n", "--number"}, "input integer"), 
                  make_option(&inputDouble, {"-d", "--double"}, "input double")
            }
      );

      if (help) {
            std::cout << "Usage:" << space(12 - 6) << "example [Flags/Options]" << std::endl;
            pars.printAll();
      }
}
```

### Advanced
With one possible command. Of course this can be used for more than just this.
```cpp
#include "libcmd.hpp"

enum class Mode {None, CMD1, CMD2, CMD3};

int main(int argc, char* argv[]) {

      Mode mode = Mode::None;
      int frontOffset = 0;
      int backOffset = 0;

      try{
            if(argc > 1){
                  switch (std::string(*(argv + 1))) {
                        case "command1":
                              mode = Mode::CMD1;
                              ++frontOffset;
                              break;
                        case "command2":
                              mode = Mode::CMD2;
                              ++frontOffset;
                              break;
                        case "command3":
                              mode = Mode::CMD3;
                              ++frontOffset;
                              break;
                  }
            }
      } catch(std::logic_error const&){}

      bool help = false;
      bool verbose = false;
      std::string inputString = "";
      int inputInteger = 0;
      double inputDouble = 0.0;

      try {            
            cmdParser pars (
                  argc - frontOffset - backOffset,
                  argv + frontOffset,
                  {
                        make_flag(&help, {"-h", "--help", "--Help", "-help", "-H"}, "Shows this message"),
                        make_flag(&verbose, {"--verbose"})
                  },
                  {
                        make_option(&inputString, {"-i", "--input"}, "input string"),
                        make_option(&inputInteger, {"-n", "--number"}, "input integer"), 
                        make_option(&inputDouble, {"-d", "--double"}, "input double")
                  }
            );

            if (help || pars.isEmpty()) {
                  std::cout << "Usage:" << space(12 - 6) << "example [Flags/Options]" << std::endl;
                  pars.printAll();
                  exit(0);
            }
      } catch (const std::invalid_argument& e) {
            std::cout << e.what() << std::endl;
            exit(0);
      }

      if (mode == Mode::CMD1) {
            //TODO
      } else if (mode == Mode::CMD2) {
            //TODO
      } else if (mode == Mode::CMD3) {
            //TODO
      } else {
            //TODO
      }
}
```
With front and back offsets it is possible to have programms of such schema:
> program [command_1] [command_2] ... [command_n] [flags and options] [filepath_1] [filepath_2] ... [filepath_n]

For another (actually the same) example take a look at "example.cpp".

## Licensing
The file "libcmd.hpp" is licensed under the [MPL 2.0 license](https://mozilla.org/MPL/2.0/).
The file "example.cpp" and the examples above are under the terms of [CC0 1.0](https://creativecommons.org/publicdomain/zero/1.0/).