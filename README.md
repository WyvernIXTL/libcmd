# libcmd
A simple, portable library for parsing command line arguments in C++.

```cpp
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
```