

/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */


#include <iostream>
#include <vector>

#include "../libcmd.hpp"

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
    bool verbose = false;
    std::string input = "";
    int printCycles = 1;
    double doublingEfforts = 1.0;


    // The command line flags and options you wish

    cmdParser pars (
        argc - frontOffset - backOffset,
        argv + frontOffset,
        {
            Option(&verbose, {"--verbose"}), //description omited 

            Option(&input, {"-i", "--input"}, "input string which is then printed"),
            Option(&printCycles, {"-cycles", "/Cycles"}, "the number of times input is printed", {"-c"}), 
            Option(&doublingEfforts, {"-d", "--double"}, "Double which acts as multiplier for Cycles")
        },
        [](){
            std::cout << "\nThis example program is licensed under CC0 1.0 apart from the used libraries\n" << LICENSENOTICE << std::endl; 
            exit(0);
        },
        [](){
            std::cout << "\nExample Program by Adam McKellar\n" << std::endl;
            std::cout << "Usage:" << space(12 - 6) << "example [Command] [Flags] [Input]" << std::endl;
            std::cout << "Example:" << space(12 - 8) << "example Print -i abc -cycles 10 -d 0.5\n" << std::endl;
        }
    );

    pars.comfortDigest();


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