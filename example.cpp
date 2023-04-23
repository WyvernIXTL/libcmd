
/*
 * Copyright (C) 2023 Adam McKellar
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#include <iostream>
#include <vector>

#include "libcmd.hpp"

//simple Usage message is printed
void Usage(cmdParser& pars){
    std::cout << "\nExample Program by Adam McKellar\n" << std::endl;
    std::cout << "Usage:" << space(12 - 6) << "example [Command] [Flags] [Input]" << std::endl;
    std::cout << "Example:" << space(12 - 8) << "example Print -i abc -cycles 10 -d 0.5\n" << std::endl;
    pars.printAll();
    std::cout << "" << std::endl;
}


int main(int argc, char* argv[]){
    int mode = 0;

    //Example for how to controll what is parsed
    int frontOffset = 0;
    int backOffset = 0;
    
    try{
        if(argc > 1){
            if (std::string(*(argv + 1)) == "Print"){
                ++frontOffset;
                mode = 1;
            }
        }
    } catch(std::logic_error const&){}

    //variables you'll change
    bool help = false;
    bool verbose = false;
    std::string input = "";
    int printCycles = 1;
    double doublingEfforts = 1.0;

    // The command line flags and options you wish
    // do not forget those , , , , 
    cmdParser pars (
        argc - frontOffset - backOffset,
        argv + frontOffset,
        {
            make_flag(&help, {"-h", "--help", "--Usage"}, "Shows this message"),
            make_flag(&verbose, {"--verbose"}) //description omited 
        },
        {
            make_option(&input, {"-i", "--input"}, "input string which is then printed"),
            make_option(&printCycles, {"-cycles", "/Cycles"}, "the number of times input is printed"), 
            make_option(&doublingEfforts, {"-d", "--double"}, "Double which acts as multiplier for Cycles")
        }
    );


    //print Usage when help is necessary
    //You might want to change pars.isEmpty() to argc == 1
    if(help || pars.isEmpty()){
        Usage(pars);
        return 0;
    }

    if(mode == 1) {
        //checks if option "input" exists, if it does, it'll run your logic
        //If you wanted int or double types you would just have to use: int number = std::stoi(input); double number2 = std::stod(input);
        if(input != ""){
            if(verbose)
                std::cout << "VERBOSE: -i --input option in process, equals: " << input << " number of cycles: " << printCycles << " * " << doublingEfforts << " = " << printCycles * doublingEfforts << std::endl;
                      
            for(int i = 0; i < printCycles * doublingEfforts; ++i){
                std::cout << input << std::endl;
            }
            
        } else {
            if(verbose)
                std::cout << "VERBOSE: -i --input option not used" << std::endl;
        }

    } else {
        std::cout << "You forgot a command." << std::endl;
    }
}