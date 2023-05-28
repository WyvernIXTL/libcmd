# libcmd
A simple, portable library for parsing command line arguments in C++.

## Examples
### Bare Bone Version
In this example there are only flags/options parsed. There is no help message, no license message and no subcommands.
```cpp
#include "libcmd.hpp"

int main(int argc, char* argv[]) {

      bool verbose = false;
      std::string inputString = "";
      int inputInteger = 0;
      double inputDouble = 0.0;

      CmdParserFrame pars (
            argc,
            argv,
            {     
                  Option(&verbose, {"--verbose"}),

                  Option(&inputString, {"-i", "--input"}, "input string"),
                  Option(&inputInteger, {"-n", "--number"}, "input integer"), 
                  Option(&inputDouble, {"-d", "--double"}, "input double", {"SomeOtherOptionNotShownInUsage", "-R"})
            }
      );
      
      pars.comfortDigest();
}
```
NOTICE: CmdParserFrame is jank... just use the examples below instead.
### With Comfort Functions
```cpp
#include "libcmd.hpp"

int main(int argc, char* argv[]) {

      bool verbose = false;
      std::string inputString = "";
      int inputInteger = 0;
      double inputDouble = 0.0;

      CmdParser pars (
            argc,
            argv,
            {     
                  Option(&verbose, {"--verbose"}),

                  Option(&inputString, {"-i", "--input"}, "input string"),
                  Option(&inputInteger, {"-n", "--number"}, "input integer"), 
                  Option(&inputDouble, {"-d", "--double"}, "input double", {"SomeOtherOptionNotShownInUsage", "-R"})
            },
            "program name",
            "",
            "This is the description people see, when your programs usage is shown.",
            "Here you can input your copyright notice and license information (for example of all libraries used)"
      );
      
      pars.comfortDigest();
}
```
The above code results in:
```
This is the description people see, when your programs usage is shown.

Usage for: program name

Flags:      -h          --help      Show this message.
            --license               Print licenses.
            --verbose

Options:    -i          --input     input string
            -n          --number    input integer
            -d          --double    input double
```

### With Subcommands
```cpp
#include "libcmd.hpp"

int main(int argc, char* argv[]) {

      bool verbose = false;
      std::string inputString = "";
      int inputInteger = 0;
      double inputDouble = 0.0;

      bool wasSubcommandCalled = false;

      CmdParser pars (
            argc,
            argv,
            {     
                  Option(&verbose, {"--verbose"}),

                  Option(&inputString, {"-i", "--input"}, "input string"),
                  Option(&inputInteger, {"-n", "--number"}, "input integer"), 
                  Option(&inputDouble, {"-d", "--double"}, "input double", {"SomeOtherOptionNotShownInUsage", "-R"})
            },
            "program name",
            "Description of program shown above help message of all subcommands. (make it short)",
            "This is the description people see, when your programs usage is shown.",
            "Here you can input your copyright notice and license information (for example of all libraries used)",
            {
                  SubCommand(
                        {   
                              // Options like above
                        },
                        "mysubcommand",
                        &wasSubcommandCalled,
                        {   
                              // sub SubCommands of your SubCommand
                        },
                        "Description of your subcommand (1 line)"
                  ),
                  // more SubCommands
            }
      );
      
      pars.comfortDigest();
}
```
This results in:
```
This is the description people see, when your programs usage is shown.

Usage for: program name

Flags:      -h          --help      Show this message.
            --license               Print licenses.
            --verbose

Options:    -i          --input     input string
            -n          --number    input integer
            -d          --double    input double

Subcmd:     mysubcommand            Description of your subcommand (1 line)

For more help: program name mysubcommand --help
```

Your can have as many SubCommands as you like. SubCommands also can have SubCommands, LABEL: which  also can have SubCommands, goto LABEL;


## Licensing

* The files "libcmd.hpp", "testlibcmd.cpp" are licensed under the [**ISC License**](https://spdx.org/licenses/ISC.html).
* The file "example.cpp" and the examples above are under the terms of [CC0 1.0](https://creativecommons.org/publicdomain/zero/1.0/).

## Contributing

1. Fork it (<https://github.com/WyvernIXTL/libcmd/fork>)
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Add your feature.
4. Lint your code ([cppcheck](https://cppcheck.sourceforge.io/))
5. Head to tests and build via cmake (`cd tests && cmake . -B ./build/ && cd ./build/`)
6. Build the tests and test (on windows `msbuild ./Project.sln && ./Debug/tests.exe`)
7. Commit your changes (`git commit -am -S 'Add some feature'`)
8. Push to the branch (`git push origin my-new-feature`)
9. Create a new Pull Request

## Contributors

- [Adam McKellar](https://github.com/WyvernIXTL) - creator and maintainer