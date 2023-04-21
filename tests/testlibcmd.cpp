
/*
 * Copyright (C) 2023 Adam McKellar
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#include <catch2/catch_test_macros.hpp>
#include "../libcmd.hpp"


TEST_CASE( "parseEmptyNoFlagsAndOptions", "[empty]" ) {
    auto executor = [](int argc, char** argv) {
        cmdParser pars {
            argc,
            argv,
            {},
            {}
        };
    };

    REQUIRE_THROWS( executor(0, nullptr) );
    REQUIRE_THROWS( executor(2, nullptr) );
    REQUIRE_NOTHROW( executor(1, nullptr) );
}

TEST_CASE( "parseGoodFlags", "[flags]" ) {
    bool help;
    bool verbose;
    bool flaggy;

    std::vector<char*> argv;
    argv.push_back("programm");
    argv.push_back("--help");
    argv.push_back("wasd");
    argv.push_back(nullptr);

    cmdParser pars {
        int(argv.size() - 1),
        argv.data(),
        {
            make_flag(&help, {"-h", "--help", "--Usage"}, "Shows this message"),
            make_flag(&verbose, {"--verbose"}),
            make_flag(&flaggy, {"wasd"})
        },
        {}
    };

    REQUIRE(help);
    REQUIRE(!verbose);
    REQUIRE(flaggy);
}

TEST_CASE( "parseGoodOptions", "[options]" ) {
    std::string inputStr;
    int inputInt = 0;
    double inputDouble = 0.0;

    std::vector<char*> argv;

    SECTION( "string and postive numbers" ) {

        argv.push_back("programm");
        argv.push_back("-s");
        argv.push_back("wasd");
        argv.push_back("-i");
        argv.push_back("345");
        argv.push_back("-d");
        argv.push_back("345.678");
        argv.push_back(nullptr);

        cmdParser pars {
            int(argv.size() - 1),
            argv.data(),
            {},
            {
                make_option(&inputStr, {"-s", "--string"}, "input string"),
                make_option(&inputInt, {"-i", "--int"}, "input int"),
                make_option(&inputDouble, {"-d", "--double"}, "input double"),
            }
        };

        REQUIRE(inputStr == "wasd");
        REQUIRE(inputInt == 345);
        REQUIRE(inputDouble == 345.678);

    }

    SECTION( "negative numbers" ){

        argv.push_back("programm");
        argv.push_back("-i");
        argv.push_back("-345");
        argv.push_back("-d");
        argv.push_back("-345.678");
        argv.push_back(nullptr);

        cmdParser pars {
            int(argv.size() - 1),
            argv.data(),
            {},
            {
                make_option(&inputInt, {"-i", "--int"}, "input int"),
                make_option(&inputDouble, {"-d", "--double"}, "input double"),
            }
        };

        REQUIRE(inputInt == -345);
        REQUIRE(inputDouble == -345.678);

    }

    SECTION( "overwriting options error" ) {

        argv.push_back("programm");
        argv.push_back("-s");
        argv.push_back("wasd");
        argv.push_back("-s");
        argv.push_back("qwertz");
        argv.push_back(nullptr);

        REQUIRE_THROWS(
            [&](){cmdParser pars {
                int(argv.size() - 1),
                argv.data(),
                {},
                {
                    make_option(&inputStr, {"-s", "--string"}, "input string"),
                    make_option(&inputInt, {"-i", "--int"}, "input int"),
                    make_option(&inputDouble, {"-d", "--double"}, "input double"),
                }
            };}()
        );

    }

}

TEST_CASE( "parseBadOptions", "[badoptions]" ) {
    std::vector<char*> argv;
    argv.push_back("programm");

    SECTION( "options without value" ){
        argv.push_back("-s");
        argv.push_back("-s2");
        argv.push_back("qwertz");
        argv.push_back(nullptr);

        std::string inputStr = "NONE";
        std::string inputStr2 = "NONE";

        cmdParser pars {
            int(argv.size() - 1),
            argv.data(),
            {},
            {
                make_option(&inputStr, {"-s", "--string"}, "input string"),
                make_option(&inputStr2, {"-s2", "--string2"}, "input string"),
            }
        };

        REQUIRE(inputStr == "NONE");
        REQUIRE(inputStr2 == "qwertz");
    }

    SECTION( "bad numbers int" ) {
        argv.push_back("-i");
        argv.push_back("abc");
        argv.push_back(nullptr);

        int inputInt = 0;

        REQUIRE_THROWS(
            [&](){cmdParser pars {
                int(argv.size() - 1),
                argv.data(),
                {},
                {
                    make_option(&inputInt, {"-i", "--int"}, "input int"),
                }
            };}()
        );        
    }

    SECTION( "bad numbers double" ) {
        argv.push_back("-d");
        argv.push_back("abc");
        argv.push_back(nullptr);

        double inputDouble = 0.0;

        REQUIRE_THROWS(
            [&](){cmdParser pars {
                int(argv.size() - 1),
                argv.data(),
                {},
                {
                    make_option(&inputDouble, {"-d", "--double"}, "input double"),
                }
            };}()
        );        
    }
}
