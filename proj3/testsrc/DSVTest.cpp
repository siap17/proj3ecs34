#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>
#include "DSVWriter.h"
#include "DSVReader.h"
#include "StringDataSink.h"
#include "StringDataSource.h"


// Unit tests for CDSVWriter and CDSVReader

// Test fixture for DSV tests
class DSVTest : public ::testing::Test {
protected:
    std::shared_ptr<CStringDataSink> Sink;
    std::shared_ptr<CStringDataSource> Source;
    std::unique_ptr<CDSVWriter> Writer;
    std::unique_ptr<CDSVReader> Reader;

    void SetUp() override {
        Sink = std::make_shared<CStringDataSink>();
        Writer = std::make_unique<CDSVWriter>(Sink, ',');  // Writer with comma delimiter
    }

    void InitializeReader() {
        Source = std::make_shared<CStringDataSource>(Sink->String());  // Read from written data
        Reader = std::make_unique<CDSVReader>(Source, ',');
    }
};

// **CDSVWriter Tests**

// Test that WriteRow fails initially (before implementation)
//TEST_F(DSVTest, WriteRowFailsInitially) {
   // std::vector<std::string> row = {"Cat", "17", "New York"};
  //  EXPECT_FALSE(Writer->WriteRow(row));  // Should fail at first
//}

// Test writing a single row
TEST_F(DSVTest, WriteSingleRow) {
    std::vector<std::string> row = {"Aerie", "27", "Los Angeles"};
    EXPECT_TRUE(Writer->WriteRow(row));  // Expect success after implementation
    EXPECT_EQ(Sink->String(), "Aerie,27,Los Angeles\n");
}

// Test writing multiple rows
TEST_F(DSVTest, WriteMultipleRows) {
    Writer->WriteRow({"Name", "Age", "City"});
    Writer->WriteRow({"Aerie", "27", "Los Angeles"});
    EXPECT_EQ(Sink->String(), "Name,Age,City\nAerie,27,Los Angeles\n");
}

// Test handling of quotes
TEST_F(DSVTest, WriteRowWithQuotes) {
    Writer->WriteRow({"Cat, Woman", "30", "New York"});
    EXPECT_EQ(Sink->String(), "\"Cat, Woman\",30,New York\n");  // Quoted correctly
}

// **CDSVReader Tests**

// Test reading an empty source
TEST_F(DSVTest, ReadEmptySource) {
    InitializeReader();
    std::vector<std::string> row;
    EXPECT_FALSE(Reader->ReadRow(row));  // Should fail with no data
}

// Test reading a valid DSV file
TEST_F(DSVTest, ReadValidDSV) {
    Writer->WriteRow({"Name", "Age", "City"});
    Writer->WriteRow({"Aerie", "27", "Los Angeles"});
    InitializeReader();

    std::vector<std::string> row;
    EXPECT_TRUE(Reader->ReadRow(row));
    EXPECT_EQ(row, std::vector<std::string>({"Name", "Age", "City"}));

    EXPECT_TRUE(Reader->ReadRow(row));
    EXPECT_EQ(row, std::vector<std::string>({"Aerie", "27", "Los Angeles"}));

    EXPECT_FALSE(Reader->ReadRow(row));  // No more data
}

// Test handling of quoted values
TEST_F(DSVTest, ReadRowWithQuotes) {
    Writer->WriteRow({"Cat, Woman", "30", "New York"});
    InitializeReader();

    std::vector<std::string> row;
    EXPECT_TRUE(Reader->ReadRow(row));
    EXPECT_EQ(row, std::vector<std::string>({"Cat, Woman", "30", "New York"}));
}

// Test End() function
TEST_F(DSVTest, EndFunction) {
    InitializeReader();
    EXPECT_TRUE(Reader->End());  // No data, should be at end
}
