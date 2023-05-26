
/*
 * Copyright (C) 2023 Adam McKellar
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#ifndef LIB_CMD
#define LIB_CMD

#include <vector>
#include <tuple>
#include <string>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <functional>

/* ============================================================================================================================== */

int SPACES = 12;

/* ============================================================================================================================== */

enum Type {BOOL, STRING, INT, DOUBLE};

/**
 * Class for handling Options.
 * 
 * There are 3 inputs: 
 * 1. The variable you want to be overwritten by the parsed argument.
 * 2. All strings to be identified as option/flag.
 * 3. The description for the help message.
 */
class Option {
private:
    Type _type;
    std::vector<std::string> _hands;
    std::string _description;

public:
    union {
        bool* pointerBool;
        std::string* pointerString;
        int* pointerInt;
        double* pointerDouble;
    };  

    Option (bool* pointer, std::vector<std::string> hands, std::string description = "");
    Option (std::string* pointer, std::vector<std::string> hands, std::string description = "");
    Option (int* pointer, std::vector<std::string> hands, std::string description = "");
    Option (double* pointer, std::vector<std::string> hands, std::string description = "");

    Type getType();
    std::vector<std::string> getHands();
    std::string getDescription();
};


Option::Option (bool* pointer, std::vector<std::string> hands, std::string description)
        : _hands(hands), _description(description), _type(Type::BOOL), pointerBool(pointer) {}

Option::Option (std::string* pointer, std::vector<std::string> hands, std::string description)
        : _hands(hands), _description(description), _type(Type::STRING), pointerString(pointer) {} 

Option::Option (int* pointer, std::vector<std::string> hands, std::string description)
        : _hands(hands), _description(description), _type(Type::INT), pointerInt(pointer) {}

Option::Option (double* pointer, std::vector<std::string> hands, std::string description)
        : _hands(hands), _description(description), _type(Type::DOUBLE), pointerDouble(pointer) {}

Type Option::getType() {
    return _type;
}

std::vector<std::string> Option::getHands() {
    return _hands;
}

std::string Option::getDescription() {
    return _description;
}


/* ============================================================================================================================== */

/**
 * Class for parsing command line arguments.
 * 
 * Two lists are given as inputs: Flags and Options.
 * These lists are then on creation of this class used to parse argc and argv arguments of the main class.
 * This class also includes pretty print capabilities of said Flags and Options.
 */
class cmdParser {
private:
    int _argc;
    char** _argv;
    std::vector<Option> _options;

public:
    cmdParser(int argc, char* argv[],
        std::vector<Option> options);

    void digest();
    void comfortDigest();
    bool isEmpty();
    void printOptions(int spaces = SPACES, std::string prefix = "", std::function<bool(Type)> include = [](Type a){return true;});
    void printAll(int spaces = SPACES);
};


/* ============================================================================================================================== */

/**
 * @brief Parse command line arguments.
 * 
 * @throws std::invalid_argument if invalid integer is parsed for option of integer type.
 * @throws std::invalid_argument if invalid double is parsed for option of double type.
 * @throws std::invalid_argument if invalid type n > 3 OR n < 1 is given in form of an option.
 */
void cmdParser::digest() {
    if (this->isEmpty()) return;

    std::unordered_map<std::string, Option*> opt;
    for (auto& elem : _options) {
        for(auto& hand : elem.getHands()) {
            opt.insert({hand, &elem});
        }
    }
    

    for (char** itr = _argv + 1; itr != _argv + _argc; ++itr) {
        auto hand = opt.find(std::string(*itr));
        if (hand != opt.end()) {
            if (hand->second->getType() == BOOL) {
                *(hand->second->pointerBool) = true;
            } else {
                if (itr + 1 != _argv + _argc) {
                    if (opt.find(*(itr + 1)) == opt.end()) {
                        switch (hand->second->getType()) 
                        {
                        case STRING:
                            *(hand->second->pointerString) = *(itr + 1);
                            break;

                        case INT:
                            try{
                                *(hand->second->pointerInt) = std::stoi(*(itr + 1));
                            } catch (...) {
                                std::ostringstream oserr;
                                oserr << "ERROR: Expected type >>int<<, but got: " << *(itr + 1);
                                throw std::invalid_argument( oserr.str() );
                            }
                            break;
                        
                        case DOUBLE:
                            try{
                                *(hand->second->pointerDouble) = std::stod(*(itr + 1));
                            } catch (...){
                                std::ostringstream oserr;
                                oserr << "ERROR: Expected type >>double<<, but got: " << *(itr + 1);
                                throw std::invalid_argument( oserr.str() );
                            }
                            break;
                        
                        default:
                            throw std::invalid_argument( "ERROR: unknown parsing from string to <type>" );
                            break;
                        }
                        ++itr;
                    }
                }
            }
        } else {
            std::ostringstream oserr;
            oserr << "ERROR: Unkown argument: " << *(itr);
            throw std::invalid_argument( oserr.str() ); 
        }
    }

}

/** 
 *  Check if arguments are empty.
 * 
 *  @return Return true if there is only one element and false if there are more elements.
 *  @throws std::invalid_argument if there are zero or less arguments.
 *  @throws std::invalid_argument if the input is a nullptr.
 */
bool cmdParser::isEmpty() {
    if(_argc == 1)
        return true;
    else if(_argc < 1)
        throw std::invalid_argument( "ERROR: invalid number of arguments (argv <= 0)" );
    else if(!_argv)
        throw std::invalid_argument( "ERROR: nullptr given instead of char**" );
    return false;
}

/**
 * @brief Digest which does not throw exceptions but rather prints them and exits the program.
 * 
 */
void cmdParser::comfortDigest() {
    try {
        digest();
    } catch(const std::invalid_argument& e) {
        std::cout << e.what() << std::endl;
        exit(1);
    }
}

/**
 * @brief Construct a new cmd Parser::cmd Parser object and parse right after.
 * 
 * @param argc Argument count of your main function (probably: "argc").
 * @param argv Array of arguments given to your main function (probably: "argv").
 * @param flags Array or Vector of Flag structs given to parse argv.
 * @param options Array or Vector of Option structs given to parse argv.
 */
cmdParser::cmdParser(int argc, char* argv[],
        std::vector<Option> options)
    : _argc(argc), _argv(argv), _options(options)
{}

/**
 * @brief Return n white spaces.
 * 
 * @param n number of spaces.
 * @return std::string with n spaces " ".
 */
std::string space(int n) {
    if(n <= 0){
        return "";
    }
    std::ostringstream os;
    for(int i = 0; i < n; i++)
        os << " ";
    return os.str();
}

/**
 * @brief Return pretty printable string of multiple hands.
 * 
 * @param hand the hands to be printed.
 * @param handsAmount the number of hands to be printed.
 * @param spaces the amount of spaces between the hands.
 * @return std::string single line string of hands dinstanced by handsAmount spaces.
 */
std::string makeHandToString(std::vector<std::string> hand, int handsAmount, int spaces) {
    std::ostringstream os;
    auto makeRoom = [spaces](std::string& hand) {
        return space(spaces - int(hand.length()));
    };
    for(auto& elem : hand) {
        os << elem << makeRoom(elem);
    }
    for(int i = 0; i < handsAmount - int(hand.size()); ++i) {
        os << space(spaces);
    }
    return os.str();
}

/**
 * @brief Return the maximum of count of hands of a single Flag or Option of all Flag or Options.
 * 
 * @param options a vector of Options.
 * @param include lambda or function deciding which Types to inlcude.
 * @return int Example: If the flag Help has the hands "-help", "--help", "-h", "-H" and has the most hands returns 4.
 */
int getHandCount(std::vector<Option> options, std::function<bool(Type)> inlcude = [](Type a){return true;}) {
    auto max = [](auto a, auto b) {
        if(a > b)
            return a;
        return b;
    };
    int highest = 0;
    for(auto option : options) {
        if (!inlcude(option.getType())) continue;        
        int count = 0;
        for(auto hand : option.getHands()) {
            count++;
        }
        highest = max(highest, count);
    }
    return highest;
}

/**
 * @brief Pretty print options.
 * 
 * @param spaces The size of the tabs. Make larger for long options.
 * @param prefix First line of print gets a prefix.
 * @param include The data Type of Option you want to include, like BOOL for flags.
 */
void cmdParser::printOptions(int spaces, std::string prefix, std::function<bool(Type)> include) {
    int amountOfFlags = getHandCount(_options, include );
    bool firstLine = true;
    for(auto& opt : _options) {
        if (include(opt.getType())) {
            if (firstLine) {
                firstLine = false;
                std::cout << prefix << space(spaces - int(prefix.length()));
            } else {
                std::cout << space(spaces);
            }
            std::cout << makeHandToString(opt.getHands(), amountOfFlags, spaces) << opt.getDescription() << std::endl;
        }
    }
}


/**
 * @brief Pretty print all hands of all flags and options and description of said structs.
 * 
 * @param spaces the amount of spaces between hands.
 */
void cmdParser::printAll(int spaces) {
    printOptions(spaces, "FLAGS:", [](Type a){return a == BOOL;});
    std::cout << " " << std::endl;
    printOptions(spaces, "OPTIONS:", [](Type a){return a != BOOL;});
}


#endif