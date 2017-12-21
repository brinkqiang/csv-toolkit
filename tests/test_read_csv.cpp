#include "catch.hpp"
#include "csv_parser.h"

using namespace csv_parser;
using std::vector;
using std::string;

// guess_delim()
TEST_CASE("col_pos() Test", "[test_col_pos]") {
    int pos = get_col_pos(
        "./tests/data/real_data/2015_StateDepartment.csv",
        "Entity Type");
    REQUIRE(pos == 1);
}

TEST_CASE("guess_delim() Test - Pipe", "[test_guess_pipe]") {
    char delim = guess_delim(
        "./tests/data/real_data/2009PowerStatus.txt");
    REQUIRE(delim == '|');
}

TEST_CASE("guess_delim() Test - Semi-Colon", "[test_guess_scolon]") {
    char delim = guess_delim(
        "./tests/data/real_data/YEAR07_CBSA_NAC3.txt");
    REQUIRE(delim == ';');
}

// get_file_info()
TEST_CASE("get_file_info() Test", "[test_file_info]") {
    CSVFileInfo info = get_file_info(
        "./tests/data/real_data/2009PowerStatus.txt");
        
    REQUIRE(info.delim == '|');
    REQUIRE(info.n_rows == 37960); // Can confirm with Excel
    REQUIRE(info.n_cols == 3);
    REQUIRE(info.col_names == vector<string>({"ReportDt", "Unit", "Power"}));
}

// Test Main Functions
TEST_CASE( "Test Reading CSV From Direct Input", "[read_csv_direct]" ) {
    string csv_string("A,B,C\r\n" // Header row
                      "123,234,345\r\n"
                      "1,2,3\r\n"
                      "1,2,3");

    // Feed Strings
    CSVReader reader;
    vector<string> row;
    reader.feed(csv_string);
    reader.end_feed();
    
    // Expected Results
    reader.read_row(row);
    vector<string> first_row = {"123", "234", "345"};
    REQUIRE( row == first_row );
}

TEST_CASE( "Test Escaped Comma", "[read_csv_comma]" ) {
    string csv_string = ("A,B,C\r\n" // Header row
                         "123,\"234,345\",456\r\n"
                         "1,2,3\r\n"
                         "1,2,3");
    CSVReader reader;
    vector<string> row;
    reader.feed(csv_string);
    reader.end_feed();
    
    // Expected Results
    reader.read_row(row);
    vector<string> first_row = {"123", "234,345", "456"};
    REQUIRE( row == first_row );
}

TEST_CASE( "Test Escaped Newline", "[read_csv_newline]" ) {
    string csv_string = ("A,B,C\r\n" // Header row
                              "123,\"234\n,345\",456\r\n"
                              "1,2,3\r\n"
                              "1,2,3");
    CSVReader reader;
    vector<string> row;
    reader.feed(csv_string);
    reader.end_feed();
    
    // Expected Results
    reader.read_row(row);
    vector<string> first_row = {"123", "234\n,345", "456"};
    REQUIRE( row == first_row );
}

TEST_CASE( "Test Empty Field", "[read_empty_field]" ) {
    // Per RFC 1480, escaped quotes should be doubled up
    string csv_string = ("A,B,C\r\n" // Header row
                              "123,\"\",456\r\n");
    
    CSVReader reader;
    vector<string> row;
    reader.feed(csv_string);
    reader.end_feed();
    
    // Expected Results
    reader.read_row(row);
    vector<string> correct_row = {"123", "", "456"};
    REQUIRE( row == correct_row );
}

TEST_CASE( "Test Escaped Quote", "[read_csv_quote]" ) {
    // Per RFC 1480, escaped quotes should be doubled up
    string csv_string = (
        "A,B,C\r\n" // Header row
        "123,\"234\"\"345\",456\r\n"
        // Only a single quote --> Not valid but correct it
        "123,\"234\"345\",456\r\n");
    
    CSVReader reader;
    vector<string> row;
    reader.feed(csv_string);
    reader.end_feed();
    
    // Expected Results: Double " is an escape for a single "
    vector<string> correct_row = {"123", "234\"345", "456"};

    // First Row
    reader.read_row(row);
    REQUIRE( row == correct_row );

    // Second Row
    reader.read_row(row);
    REQUIRE( row == correct_row );
}

TEST_CASE( "Test Read CSV with Header Row", "[read_csv_header]" ) {
    // Header on first row
    CSVReader reader;
    reader.read_csv("./tests/data/real_data/2015_StateDepartment.csv");

    vector<string> row;
    reader.read_row(row); // Populate row with first line
    
    // Expected Results
    vector<string> col_names = {
        "Year", "Entity Type", "Entity Group", "Entity Name",
        "Department / Subdivision", "Position", "Elected Official",
        "Judicial", "Other Positions", "Min Classification Salary",
        "Max Classification Salary", "Reported Base Wage", "Regular Pay",
        "Overtime Pay", "Lump-Sum Pay", "Other Pay", "Total Wages",
        "Defined Benefit Plan Contribution", "Employees Retirement Cost Covered",
        "Deferred Compensation Plan", "Health Dental Vision",
        "Total Retirement and Health Cost", "Pension Formula",
        "Entity URL", "Entity Population", "Last Updated",
        "Entity County", "Special District Activities"
    };
    
    vector<string> first_row = {
        "2015","State Department","","Administrative Law, Office of","",
        "Assistant Chief Counsel","False","False","","112044","129780",""
        ,"133020.06","0","2551.59","2434.8","138006.45","34128.65","0","0"
        ,"15273.97","49402.62","2.00% @ 55","http://www.spb.ca.gov/","",
        "08/02/2016","",""
    };
    REQUIRE( row == first_row );
    REQUIRE( reader.get_col_names() == col_names );
    
    // Can confirm with MS Excel, etc...
    REQUIRE( reader.row_num == 246498 );
}

TEST_CASE( "Test CSV Subsetting", "[read_csv_subset]" ) {
    vector<int> subset = {0, 1, 2, 3, 4};
    vector<string> row;
    CSVReader reader(',', '"', 0, subset);
    reader.read_csv("./tests/data/real_data/2015_StateDepartment.csv");
    
    // Expected Results
    vector<string> first_row = {
        "2015","State Department","","Administrative Law, Office of", "" };
    vector<string> col_names = {
        "Year","Entity Type","Entity Group","Entity Name","Department / Subdivision"
    };
    
    reader.read_row(row);
    REQUIRE( row == first_row );
    REQUIRE( reader.get_col_names() == col_names);
    REQUIRE( reader.row_num == 246498 );
}

TEST_CASE( "Test JSON Output", "[csv_to_json]") {
    string csv_string("I,Like,Turtles\r\n");
    vector<string> col_names = {"A", "B", "C"};
    
    // Feed Strings
    CSVReader reader(',', '"', -1);
    reader.set_col_names(col_names);
    reader.feed(csv_string);
    reader.end_feed();
    reader.to_json("./tests/temp/test.ndjson");
    
    // Expected Results
    std::ifstream test_file("./tests/temp/test.ndjson");
    string first_line;
    std::getline(test_file, first_line, '\n');
    REQUIRE( first_line == "{\"A\":\"I\",\"B\":\"Like\",\"C\":\"Turtles\"}" );
}

TEST_CASE( "Test JSON Output (Memory)", "[csv_to_json_mem]") {
    string csv_string(
        "A,B,C,D\r\n" // Header row
        "I,Like,Turtles,1\r\n"
        "I,Like,Turtles,2\r\n");
    vector<string> turtles;
    
    // Feed Strings
    CSVReader reader;
    reader.feed(csv_string);
    reader.end_feed();
    turtles = reader.to_json();
    
    // Expected Results
    REQUIRE( turtles[0] == "{\"A\":\"I\",\"B\":\"Like\",\"C\":\"Turtles\",\"D\":1}");
    REQUIRE( turtles[1] == "{\"A\":\"I\",\"B\":\"Like\",\"C\":\"Turtles\",\"D\":2}");
}

TEST_CASE( "Test JSON Escape", "[csv_to_json_escape]") {
    string csv_string(
        "A,B,C,D\r\n" // Header row
        "I,\"Like\"\"\",Turtles,1\r\n" // Quote escape test
        "I,\"Like\\\",Turtles,1\r\n"   // Backslash escape test
        "I,\"Like\r\n\",Turtles,1\r\n" // Newline escape test
        "I,\"Like\t\",Turtles,1\r\n"   // Tab escape test
        "I,\"Like/\",Turtles,1\r\n"    // Slash escape test
        );
    vector<string> turtles;
    
    // Feed Strings
    CSVReader reader;
    reader.feed(csv_string);
    reader.end_feed();
    turtles = reader.to_json();
    
    // Expected Results
    REQUIRE( turtles[0] == "{\"A\":\"I\",\"B\":\"Like\\\"\",\"C\":\"Turtles\",\"D\":1}");
    REQUIRE( turtles[1] == "{\"A\":\"I\",\"B\":\"Like\\\\\",\"C\":\"Turtles\",\"D\":1}");
    REQUIRE( turtles[2] == "{\"A\":\"I\",\"B\":\"Like\\\r\\\n\",\"C\":\"Turtles\",\"D\":1}");
    REQUIRE( turtles[3] == "{\"A\":\"I\",\"B\":\"Like\\\t\",\"C\":\"Turtles\",\"D\":1}");
    REQUIRE( turtles[4] == "{\"A\":\"I\",\"B\":\"Like\\/\",\"C\":\"Turtles\",\"D\":1}");
}

// read_row()
TEST_CASE("Test read_row() void* - Easy", "[read_row_void1]") {
    // Test that integers are type-casted properly
    CSVReader reader("./tests/data/fake_data/ints.csv");
    vector<CSVField> row;

    while (reader.read_row(row)) {
        for (size_t i = 0; i < row.size(); i++) {
            REQUIRE( row[i].get_int() <= 100 );
            REQUIRE( row[i].dtype == _int );
        }
    }
}

TEST_CASE("Test read_row() CSVField - Power Status", "[read_row_csvfield]") {
    CSVReader reader("./tests/data/real_data/2009PowerStatus.txt");
    vector<CSVField> row;
    int i = 0;
    bool caught_error = false;

    while (reader.read_row(row)) {
        // Assert correct types
        REQUIRE(row[0].is_string());
        REQUIRE(row[1].is_string());
        REQUIRE(row[2].is_int() == 1);

        // Spot check
        if (i == 2) {
            REQUIRE(row[2].get_int() == 100);
            REQUIRE(row[0].get_string() == "12/31/2009");
            REQUIRE(row[1].get_string() == "Beaver Valley 1");

            // Assert misusing API throws the appropriate errors
            try {
                row[0].get_int();
            }
            catch (std::runtime_error) {
                caught_error = true;
            }

            REQUIRE(caught_error);
            caught_error = false;

            try {
                row[0].get_float();
            }
            catch (std::runtime_error) {
                caught_error = true;
            }

            REQUIRE(caught_error);
        }

        i++;
    }
}