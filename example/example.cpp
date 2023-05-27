

/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */


#include <iostream>

#include "../libcmd.hpp"


int main(int argc, char* argv[]){

    // control flow variables
    // is only true if command was called
    // example =>               !bakingIceCreamInOven && !print
    // example print =>         !bakingIceCreamInOven &&  print
    // example print bakeice =>  bakingIceCreamInOven && !print
    bool print = false;
    bool bakingIceCreamInOven = false;

    // variables you'll change
    bool hi = false;

    bool verbose = false;
    std::string input = "";
    int printCycles = 1;
    double doublingEfforts = 1.0;

    int bakingIceNtimes = 1;


    // The command line flags and options you wish

    CmdParser pars (
        argc,
        argv,
        {
            Option(&hi, {"--hi"}) //description omited 

        },
        "example",
        // Custom usage header for every subcommand
        "\nExample Program by Adam McKellar",
        // Custom usage header for main program only.
        "\nExample Program by Adam McKellar \n\nUsage:      example [Command] [Flags] [Input] \nExample:    example print -i abc -cycles 10 -d 0.5\n\nexample [subcommands] --help for more\n",

        // Custom license notice.
        "\nThis example program is licensed under CC0 1.0 apart from the used libraries\n",

        { // subcommands
            CmdParser(
                {   
                    Option(&verbose, {"--verbose"}),
                    Option(&input, {"-i", "--input"}, "input string which is then printed"),
                    Option(&printCycles, {"-cycles", "/Cycles"}, "the number of times input is printed", {"-c"}), 
                    Option(&doublingEfforts, {"-d", "--double"}, "Double which acts as multiplier for Cycles")
                },
                "print",
                &print,
                {   // subsubcommands
                    CmdParser(
                        {
                            Option(&bakingIceNtimes, {"-n", "--num"}, "baking my iceee n times", {"-times", "--times"})
                        },
                        "bakeice",
                        &bakingIceCreamInOven,
                        {},
                        "This is baking ice."
                    )
                },
                "Print string you input multiple times."
            )
        }
    );

    pars.comfortDigest();


    if(print) {
        if(input != ""){
            if(verbose)
                std::cout << "VERBOSE: -i --input option in process, equals: " << input << " number of cycles: " << printCycles << " * " << doublingEfforts << " = " << printCycles * doublingEfforts << std::endl;
                      
            for(int i = 0; i < printCycles * doublingEfforts; ++i){
                std::cout << input << std::endl;
            }
            
        } else {
            std::cout << "-i not specified    please look at >> example print --help";
        }

    } else if(bakingIceCreamInOven) {
        std::cout << "Baking " << bakingIceNtimes << " in oven." << std::endl;
    } else {
        std::cout << "You forgot a command." << std::endl;
        if (hi) {
            std::cout << "Hi! I'm actually hungry now :/";
        }
    }
}