#include "catch.hpp"
#include "csv_parser.h"
#include <string>
#include <vector>

using namespace csv_parser;

TEST_CASE(
    "Calculating Statistics from Direct Input",
    "[read_csv_stat_direct]" ) {
    // Header on first row
    CSVStat reader(',', '"', -1);
    reader.set_col_names({"A", "B", "C"});
    std::string int_str;
    std::string int_list;
    
    for (int i = 1; i < 101; i++) {
        int_str = std::to_string(i);
        int_list = int_str + "," + int_str + "," + int_str + "\r\n";
        reader.feed(int_list);
    }
    
    reader.end_feed();
    reader.calc();
    
    // Expected Results
    std::vector<long double> means = { 50.5, 50.5, 50.5 };
    std::vector<long double> mins = { 1, 1, 1 };
    std::vector<long double> maxes = { 100, 100, 100 };

    REQUIRE( reader.get_mins() == mins );
    REQUIRE( reader.get_maxes() == maxes );
    REQUIRE( reader.get_mean() == means );
    REQUIRE( ceil(reader.get_variance()[0]) == 842 );
    
    // Make sure all integers between 1 and 100 have a count of 1
    for (int i = 1; i < 101; i++) {
        REQUIRE( reader.get_counts()[0][std::to_string(i)] == 1 );
    }
    
    // Confirm column at pos 0 has 100 integers (type 2)
    REQUIRE( reader.get_dtypes()[0][2] == 100 );
}

TEST_CASE( "Statistics - ints.csv", "[read_csv_stat]" ) {
    // Header on first row
    CSVStat reader(',', '"', 0);
    reader.calc_csv("./tests/data/fake_data/ints.csv");
    
    // Expected Results
    std::vector<long double> means = {
        50.5, 50.5, 50.5, 50.5, 50.5,
        50.5, 50.5, 50.5, 50.5, 50.5
    };
    
    REQUIRE( reader.get_mean() == means );
    REQUIRE( reader.get_mins()[0] == 1 );
    REQUIRE( reader.get_maxes()[0] == 100 );
    REQUIRE( ceil(reader.get_variance()[0]) == 842 );
}

TEST_CASE( "Statistics (Line Feed Record-Separated)",
    "[read_csv_stat2]" ) {
    // Header on first row
    CSVStat reader(',', '"', 0);
    reader.calc_csv("./tests/data/fake_data/ints_newline_sep.csv");
    std::vector<long double> expected_means = {
        50.5, 50.5, 50.5, 50.5, 50.5,
        50.5, 50.5, 50.5, 50.5, 50.5
    };
    
    REQUIRE( reader.get_mean() == expected_means );
    REQUIRE( reader.get_mins()[0] == 1 );
    REQUIRE( reader.get_maxes()[0] == 100 );
    REQUIRE( ceil(reader.get_variance()[0]) == 842 );
}

TEST_CASE( "Statistics - persons.csv", "[test_stat_person]" ) {
    // Header on first row
    CSVStat reader(',', '"', 0);
    reader.calc_csv("./tests/data/mimesis_data/persons.csv");
    REQUIRE( ceil(reader.get_mean()[1]) == 42 );
}