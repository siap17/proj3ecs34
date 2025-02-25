#include <gtest/gtest.h>
#include "XMLWriter.h"
#include "XMLEntity.h"
#include "StringDataSink.h"
#include <memory>
#include <string>

class XMLWriterTest : public ::testing::Test {
protected:
    std::shared_ptr<CStringDataSink> DataSink;
    std::unique_ptr<CXMLWriter> Writer;
    
    void SetUp() override {
        DataSink = std::make_shared<CStringDataSink>();
        Writer = std::make_unique<CXMLWriter>(DataSink);
    }
};

TEST_F(XMLWriterTest, SimpleElement) {
    SXMLEntity Element;
    Element.DType = SXMLEntity::EType::StartElement;
    Element.DNameData = "cat";
    EXPECT_TRUE(Writer->WriteEntity(Element));
    
    Element.DType = SXMLEntity::EType::EndElement;
    EXPECT_TRUE(Writer->WriteEntity(Element));
    EXPECT_TRUE(Writer->Flush());
    
    EXPECT_EQ(DataSink->String(), "<cat></cat>");
}

TEST_F(XMLWriterTest, ElementWithAttribute) {
    SXMLEntity Element;
    Element.DType = SXMLEntity::EType::StartElement;
    Element.DNameData = "need";
    Element.DAttributes.push_back({"cat", "dog"});
    EXPECT_TRUE(Writer->WriteEntity(Element));
    
    Element.DType = SXMLEntity::EType::EndElement;
    Element.DAttributes.clear();
    EXPECT_TRUE(Writer->WriteEntity(Element));
    EXPECT_TRUE(Writer->Flush());
    
    EXPECT_EQ(DataSink->String(), "<need cat=\"dog\"></need>");
}

TEST_F(XMLWriterTest, CompleteElement) {
    SXMLEntity Element;
    Element.DType = SXMLEntity::EType::CompleteElement;
    Element.DNameData = "need";
    Element.DAttributes.push_back({"cat", "dog"});
    EXPECT_TRUE(Writer->WriteEntity(Element));
    EXPECT_TRUE(Writer->Flush());
    
    EXPECT_EQ(DataSink->String(), "<need cat=\"dog\"/>");
}

TEST_F(XMLWriterTest, CharacterData) {
    SXMLEntity Element;
    Element.DType = SXMLEntity::EType::StartElement;
    Element.DNameData = "test";
    EXPECT_TRUE(Writer->WriteEntity(Element));
    
    Element.DType = SXMLEntity::EType::CharData;
    Element.DNameData = "Hi World";
    EXPECT_TRUE(Writer->WriteEntity(Element));
    
    Element.DType = SXMLEntity::EType::EndElement;
    Element.DNameData = "test";
    EXPECT_TRUE(Writer->WriteEntity(Element));
    EXPECT_TRUE(Writer->Flush());
    
    EXPECT_EQ(DataSink->String(), "<test>Hi World</test>");
}

TEST_F(XMLWriterTest, EscapedCharacters) {
    SXMLEntity Element;
    Element.DType = SXMLEntity::EType::StartElement;
    Element.DNameData = "test";
    EXPECT_TRUE(Writer->WriteEntity(Element));
    
    Element.DType = SXMLEntity::EType::CharData;
    Element.DNameData = "<Hi & World>";
    EXPECT_TRUE(Writer->WriteEntity(Element));
    
    Element.DType = SXMLEntity::EType::EndElement;
    Element.DNameData = "test";
    EXPECT_TRUE(Writer->WriteEntity(Element));
    EXPECT_TRUE(Writer->Flush());
    
    EXPECT_EQ(DataSink->String(), "<test>&lt;Hi &amp; World&gt;</test>");
}

TEST_F(XMLWriterTest, NestedElements) {
    SXMLEntity Element;
    // outer element start
    Element.DType = SXMLEntity::EType::StartElement;
    Element.DNameData = "outer";
    EXPECT_TRUE(Writer->WriteEntity(Element));
    
    // inner element
    Element.DNameData = "inner";
    EXPECT_TRUE(Writer->WriteEntity(Element));
    
    // inner element end
    Element.DType = SXMLEntity::EType::EndElement;
    EXPECT_TRUE(Writer->WriteEntity(Element));
    
    // outer element end
    Element.DNameData = "outer";
    EXPECT_TRUE(Writer->WriteEntity(Element));
    EXPECT_TRUE(Writer->Flush());
    
    EXPECT_EQ(DataSink->String(), "<outer><inner></inner></outer>");
}

TEST_F(XMLWriterTest, MultipleAttributes) {
    SXMLEntity Element;
    Element.DType = SXMLEntity::EType::StartElement;
    Element.DNameData = "test";
    Element.DAttributes.push_back({"key1", "value1"});
    Element.DAttributes.push_back({"key2", "value2"});
    EXPECT_TRUE(Writer->WriteEntity(Element));
    
    Element.DType = SXMLEntity::EType::EndElement;
    Element.DAttributes.clear();
    EXPECT_TRUE(Writer->WriteEntity(Element));
    EXPECT_TRUE(Writer->Flush());
    
    EXPECT_EQ(DataSink->String(), "<test key1=\"value1\" key2=\"value2\"></test>");
}

TEST_F(XMLWriterTest, AttributeEscaping) {
    SXMLEntity Element;
    Element.DType = SXMLEntity::EType::StartElement;
    Element.DNameData = "test";
    Element.DAttributes.push_back({"key", "value\"with'quotes"});
    EXPECT_TRUE(Writer->WriteEntity(Element));
    
    Element.DType = SXMLEntity::EType::EndElement;
    Element.DAttributes.clear();
    EXPECT_TRUE(Writer->WriteEntity(Element));
    EXPECT_TRUE(Writer->Flush());
    
    EXPECT_EQ(DataSink->String(), "<test key=\"value&quot;with&apos;quotes\"></test>");
}

TEST_F(XMLWriterTest, AutoClosingTags) {
    SXMLEntity Element;
    Element.DType = SXMLEntity::EType::StartElement;
    Element.DNameData = "parent";
    EXPECT_TRUE(Writer->WriteEntity(Element));
    
    Element.DNameData = "child";
    EXPECT_TRUE(Writer->WriteEntity(Element));
    
    EXPECT_TRUE(Writer->Flush());
    
    EXPECT_EQ(DataSink->String(), "<parent><child></child></parent>");
}
