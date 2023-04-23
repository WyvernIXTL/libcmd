
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

struct Flag
{
    bool* pointer;
    std::vector<std::string> hands;
    std::string description;
};

Flag make_flag(bool* pointer, std::vector<std::string> hands, std::string description){
    Flag flaggy;
    flaggy.pointer = pointer;
    flaggy.hands = hands;
    flaggy.description = description;
    return flaggy;
}

Flag make_flag(bool* pointer, std::vector<std::string> hands){
    return make_flag(pointer, hands, "");
}


struct Option
{
    std::string* pointer;
    int* pointerInt;
    double* pointerDouble;
    std::vector<std::string> hands;
    std::string description;
    int type;
};


Option make_option(std::string* pointer, std::vector<std::string> hands, std::string description){
    Option flaggy;
    flaggy.pointer = pointer;
    flaggy.hands = hands;
    flaggy.description = description;
    flaggy.type = 1;
    return flaggy;
}

Option make_option(std::string* pointer, std::vector<std::string> hands){
    return make_option(pointer, hands, "");
}


Option make_option(int* pointer, std::vector<std::string> hands, std::string description){
    Option flaggy;
    flaggy.pointerInt = pointer;
    flaggy.hands = hands;
    flaggy.description = description;
    flaggy.type = 2;
    return flaggy;
}

Option make_option(int* pointer, std::vector<std::string> hands){
    return make_option(pointer, hands, "");
}


Option make_option(double* pointer, std::vector<std::string> hands, std::string description){
    Option flaggy;
    flaggy.pointerDouble = pointer;
    flaggy.hands = hands;
    flaggy.description = description;
    flaggy.type = 3;
    return flaggy;
}

Option make_option(double* pointer, std::vector<std::string> hands){
    return make_option(pointer, hands, "");
}


/* ============================================================================================================================== */

class cmdParser
{
private:
    int _argc;
    char** _argv;
    std::vector<Flag> _flags; //&bool, Shorthand, Long, description 
    std::vector<Option> _options; //&options, Shorthand, Long, description 

    bool _flagExists(char** begin, char** end, const std::string flag);
    std::string _retrieveOption(char** begin, char** end, const std::string option);

public:
    cmdParser(int argc, char* argv[],
        std::vector<Flag> flags,
        std::vector<Option> options);

    void digest();
    bool isEmpty();
    void printFlags();
    void printFlags(int spaces);
    void printOptions();
    void printOptions(int spaces);
    void printAll();
    void printAll(int spaces);
};


/* ============================================================================================================================== */

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


bool cmdParser::isEmpty() {
    if(_argc == 1)
        return true;
    else if(_argc < 1)
        throw std::invalid_argument( "ERROR: invalid number of arguments (argv <= 0)" );
    else if(!_argv)
        throw std::invalid_argument( "ERROR: nullptr given instead of char**" );
    return false;
}


cmdParser::cmdParser(int argc, char* argv[],
        std::vector<Flag> flags,
        std::vector<Option> options)
    : _argc(argc), _argv(argv), _flags(flags), _options(options)
{
    digest();
}

std::string cmdParser::_retrieveOption(char** begin, char** end, const std::string option)
{
    char** itr = std::find(begin, end, option);
    if(itr != end && itr + 1 != end) {
        return *(itr+1);
    }
    return "";
}

bool cmdParser::_flagExists(char** begin, char** end, const std::string flag)
{
    return std::find(begin, end, flag) != end;
}

std::string space(int n) {
    if(n <= 0){
        return "";
    }
    std::ostringstream os;
    for(int i = 0; i < n; i++)
        os << " ";
    return os.str();
}

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

void cmdParser::printFlags(int spaces) {
    int amountOfFlags = getHandCount(_flags);
    for(std::size_t i = 0; i < _flags.size(); ++i) {
        if(i == 0)
            std::cout << "FLAGS:" << space(spaces-6) << makeHandToString(_flags[i].hands, amountOfFlags, spaces) << _flags[i].description << std::endl;
        else
            std::cout << space(spaces) << makeHandToString(_flags[i].hands, amountOfFlags, spaces) << _flags[i].description << std::endl;
    }
}

void cmdParser::printFlags() {
    printFlags(SPACES);
}

void cmdParser::printOptions(int spaces) {
    int amountOfFlags = getHandCount(_options);
    for(std::size_t i = 0; i < _options.size(); ++i) {
        if(i == 0)
            std::cout << "OPTIONS:" << space(spaces-8) << makeHandToString(_options[i].hands, amountOfFlags, spaces) << _options[i].description << std::endl;
        else
            std::cout << space(spaces) << makeHandToString(_options[i].hands, amountOfFlags, spaces) << _options[i].description << std::endl;
    }
}

void cmdParser::printOptions() {
    printOptions(SPACES);
}

void cmdParser::printAll(int spaces) {
    printFlags(spaces);
    std::cout << " " << std::endl;
    printOptions(spaces);
}

void cmdParser::printAll() {
    printAll(SPACES);
}

#endif