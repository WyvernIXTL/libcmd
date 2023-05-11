
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

/* ============================================================================================================================== */

int SPACES = 12;

/* ============================================================================================================================== */

/// @brief Struct for holding a Flag, its hands and description.
struct Flag
{
    bool* pointer;
    std::vector<std::string> hands;
    std::string description;
};

/**
 * @brief Construct a struct including necessary components for parsing and pretty printing.
 * 
 * @param[out] pointer Pointer to bool argument to be overwritten.
 * @param[in] hands Hands of Flag. Example: "-h", "--help", "-H".
 * @param[in] description Description of Flag. Example: "show this message".
 * @return Flag Struct with this information.
 */
Flag make_flag(bool* pointer, std::vector<std::string> hands, std::string description){
    Flag flaggy;
    flaggy.pointer = pointer;
    flaggy.hands = hands;
    flaggy.description = description;
    return flaggy;
}

/**
 * @brief Construct a struct including necessary components for parsing.
 * 
 * @overload
 */
Flag make_flag(bool* pointer, std::vector<std::string> hands){
    return make_flag(pointer, hands, "");
}

/// @brief Struct for holding an Option, its hands and description.
struct Option
{
    std::string* pointer;
    int* pointerInt;
    double* pointerDouble;
    std::vector<std::string> hands;
    std::string description;
    int type;
};

/**
 * @brief Construct a struct including necessary components for parsing and pretty printing.
 * 
 * @param[out] pointer Pointer to Object to be overwritten.
 * @param[in] hands Hands of Flag. Example: "-h", "--help", "-H".
 * @param[in] description Description of Flag. Example: "show this message".
 * @return Option Struct with this information.
 */
Option make_option(std::string* pointer, std::vector<std::string> hands, std::string description){
    Option flaggy;
    flaggy.pointer = pointer;
    flaggy.hands = hands;
    flaggy.description = description;
    flaggy.type = 1;
    return flaggy;
}

/**
 * @brief Construct a struct including necessary components for parsing.
 * 
 * @overload
 */
Option make_option(std::string* pointer, std::vector<std::string> hands){
    return make_option(pointer, hands, "");
}

/**
 * @brief Construct a struct including necessary components for parsing and pretty printing.
 * 
 * @overload
 */
Option make_option(int* pointer, std::vector<std::string> hands, std::string description){
    Option flaggy;
    flaggy.pointerInt = pointer;
    flaggy.hands = hands;
    flaggy.description = description;
    flaggy.type = 2;
    return flaggy;
}

/**
 * @brief Construct a struct including necessary components for parsing.
 * 
 * @overload
 */
Option make_option(int* pointer, std::vector<std::string> hands){
    return make_option(pointer, hands, "");
}

/**
 * @brief Construct a struct including necessary components for parsing and pretty printing.
 * 
 * @overload
 */
Option make_option(double* pointer, std::vector<std::string> hands, std::string description){
    Option flaggy;
    flaggy.pointerDouble = pointer;
    flaggy.hands = hands;
    flaggy.description = description;
    flaggy.type = 3;
    return flaggy;
}

/**
 * @brief Construct a struct including necessary components for parsing.
 * 
 * @overload
 */
Option make_option(double* pointer, std::vector<std::string> hands){
    return make_option(pointer, hands, "");
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
    std::vector<Flag> _flags;
    std::vector<Option> _options;

public:
    cmdParser(int argc, char* argv[],
        std::vector<Flag> flags,
        std::vector<Option> options);

    void digest();
    void comfortDigest();
    bool isEmpty();
    void printFlags();
    void printFlags(int spaces);
    void printOptions();
    void printOptions(int spaces);
    void printAll();
    void printAll(int spaces);
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

    std::unordered_map<std::string, std::tuple<Flag*, Option*>> opt;
    for(auto& elem : _flags) {
        for(auto& hand : elem.hands) {
            opt.insert({hand, std::make_tuple(&elem, nullptr)});
        }
    }
    for (auto& elem : _options) {
        for(auto& hand : elem.hands) {
            opt.insert({hand, std::make_tuple(nullptr, &elem)});
        }
    }
    

    for (char** itr = _argv + 1; itr != _argv + _argc; ++itr) {
        auto hand = opt.find(std::string(*itr));
        if (hand != opt.end()) {
            if (std::get<Flag*>(hand->second)) {
                *((*(std::get<Flag*>(hand->second))).pointer) = true;
            } else {
                if (itr + 1 != _argv + _argc) {
                    if (opt.find(*(itr + 1)) == opt.end()) {
                        auto elem = (*(std::get<Option*>(hand->second)));
                        if (elem.type == 1) {
                            *(elem.pointer) = *(itr + 1);
                        } else if (elem.type == 2) {
                            try{
                                *(elem.pointerInt) = std::stoi(*(itr + 1));
                            } catch (...) {
                                std::ostringstream oserr;
                                oserr << "ERROR: Expected type >>int<<, but got: " << *(itr + 1);
                                throw std::invalid_argument( oserr.str() );
                            }
                        } else if (elem.type == 3) {
                            try{
                                *(elem.pointerDouble) = std::stod(*(itr + 1));
                            } catch (...){
                                std::ostringstream oserr;
                                oserr << "ERROR: Expected type >>double<<, but got: " << *(itr + 1);
                                throw std::invalid_argument( oserr.str() );
                            }
                        } else {
                            throw std::invalid_argument( "ERROR: unknown parsing from string to <type>" );
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
        std::vector<Flag> flags,
        std::vector<Option> options)
    : _argc(argc), _argv(argv), _flags(flags), _options(options)
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
 * @tparam T Flag or Option.
 * @param hands unintuitively a vector of Flags or a vector of Options.
 * @return int Example: If the flag Help has the hands "-help", "--help", "-h", "-H" and has the most hands returns 4.
 */
template<typename T>
int getHandCount(std::vector<T> hands) {
    auto max = [](auto a, auto b) {
        if(a > b)
            return a;
        return b;
    };
    int highest = 0;
    for(auto elem : hands) {
        int count = 0;
        for(auto hand : elem.hands) {
            count++;
        }
        highest = max(highest, count);
    }
    return highest;
}

/**
 * @brief Pretty print all hands of all flags and the description of said flags.
 * 
 * @param spaces the amount of spaces between hands of flags.
 */
void cmdParser::printFlags(int spaces) {
    int amountOfFlags = getHandCount(_flags);
    for(std::size_t i = 0; i < _flags.size(); ++i) {
        if(i == 0)
            std::cout << "FLAGS:" << space(spaces-6) << makeHandToString(_flags[i].hands, amountOfFlags, spaces) << _flags[i].description << std::endl;
        else
            std::cout << space(spaces) << makeHandToString(_flags[i].hands, amountOfFlags, spaces) << _flags[i].description << std::endl;
    }
}

/// @brief Pretty print all hands of all flags and the description of said flags.
/// @overload
void cmdParser::printFlags() {
    printFlags(SPACES);
}

/**
 * @brief Pretty print all hands of all options and description of said options.
 * 
 * @param spaces the amount of spaces between hands of options.
 */
void cmdParser::printOptions(int spaces) {
    int amountOfFlags = getHandCount(_options);
    for(std::size_t i = 0; i < _options.size(); ++i) {
        if(i == 0)
            std::cout << "OPTIONS:" << space(spaces-8) << makeHandToString(_options[i].hands, amountOfFlags, spaces) << _options[i].description << std::endl;
        else
            std::cout << space(spaces) << makeHandToString(_options[i].hands, amountOfFlags, spaces) << _options[i].description << std::endl;
    }
}

/// @brief Pretty print all hands of all options and description of said options.
/// @overload
void cmdParser::printOptions() {
    printOptions(SPACES);
}

/**
 * @brief Pretty print all hands of all flags and options and description of said structs.
 * 
 * @param spaces the amount of spaces between hands.
 */
void cmdParser::printAll(int spaces) {
    printFlags(spaces);
    std::cout << " " << std::endl;
    printOptions(spaces);
}

/// @brief Pretty print all hands of all flags and options and description of said structs.
/// @overload
void cmdParser::printAll() {
    printAll(SPACES);
}

#endif