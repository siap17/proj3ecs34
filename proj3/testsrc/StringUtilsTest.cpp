#include <gtest/gtest.h>
#include "StringUtils.h"
#include <algorithm>

TEST(StringUtilsTest, SliceTest) {
    std::string str = "new york";
    EXPECT_EQ(StringUtils::Slice(str, 0, 3), "new");
}

TEST(StringUtilsTest, Capitalize) {
    std::string str = "cats";
    EXPECT_EQ(StringUtils::Capitalize(str), "Cats");
}

TEST(StringUtilsTest, Upper) {
    std::string str = "hola";
    EXPECT_EQ(StringUtils::Upper(str), "HOLA");
}

TEST(StringUtilsTest, Lower) {
    std::string str = "Hola";
    EXPECT_EQ(StringUtils::Lower(str), "hola");
}

TEST(StringUtilsTest, LStrip) {
    std::string str = "   cats";
    EXPECT_EQ(StringUtils::LStrip(str), "cats");
}

TEST(StringUtilsTest, RStrip) {
    std::string str = "cats   ";
    EXPECT_EQ(StringUtils::RStrip(str), "cats");
}

TEST(StringUtilsTest, Strip) {
    std::string str = "   cats   ";
    EXPECT_EQ(StringUtils::Strip(str), "cats");
}

TEST(StringUtilsTest, Center) {
    std::string str = "cats";
    EXPECT_EQ(StringUtils::Center(str, 10), "   cats   ");
}

TEST(StringUtilsTest, LJust) {
    std::string str = "cats";
    EXPECT_EQ(StringUtils::LJust(str, 10), "cats      ");
}

TEST(StringUtilsTest, RJust) {
    std::string str = "cats";
    EXPECT_EQ(StringUtils::RJust(str, 10), "      cats");
}

TEST(StringUtilsTest, Replace) {
    std::string str = "new york";
    EXPECT_EQ(StringUtils::Replace(str, "york", "cat"), "new cat");
}

TEST(StringUtilsTest, Split) {
    std::string str = "pasta is yummy";
    std::vector<std::string> result = StringUtils::Split(str, " ");
    std::vector<std::string> expected = {"pasta", "is", "yummy"};
    EXPECT_EQ(result, expected);
}

TEST(StringUtilsTest, Join) {
    std::vector<std::string> vec = {"pasta", "is", "yummy"};
    std::string result = StringUtils::Join("", vec);
    EXPECT_EQ(result, "pastaisyummy");
}

TEST(StringUtilsTest, ExpandTabs){
    EXPECT_EQ(StringUtils::ExpandTabs("\t", 4), "    ");
    EXPECT_EQ(StringUtils::ExpandTabs("hello\t", 8), "hello   "); 
}

TEST(StringUtilsTest, EditDistance) {
    std::string str1 = "kitten";
    std::string str2 = "sitting";
    EXPECT_EQ(StringUtils::EditDistance(str1, str2), 3);
}
