#include <string>
#include <gtest/gtest.h>
#include "Json.h"

using namespace myJson;
using namespace std;

Json parseOk(const string& strJson) {
    string errMsg;
    Json json = Json::parse(strJson, errMsg);
    EXPECT_EQ(errMsg, "");
    return json;
}

#define testNull(strJson)\
    do {\
        Json json = parseOk(strJson);\
        EXPECT_TRUE(json.isNull());\
    } while (0)

TEST(Str2Json, JsonNull) {
    testNull("null");
    testNull("   null\n\r\t");
}

#define testBool(expect, content)\
    do {\
        Json json = parseOk(content);\
        EXPECT_TRUE(json.isBool());\
        EXPECT_EQ(json.toBool(), expect);\
        json = Json(!expect);\
        EXPECT_EQ(json.toBool(), !expect);\
    } while (0)

TEST(Str2Json, JsonBool) {
    testBool(true, "true");
    testBool(false, "false");
}

#define testNumber(expect, strJson)\
    do {\
        Json json = parseOk(strJson);\
        EXPECT_TRUE(json.isNumber());\
        EXPECT_EQ(json.toNumber(), expect);\
    } while (0)

TEST(Str2Json, JsonNumber) {
    testNumber(0.0, "0");
    testNumber(0.0, "-0");
    testNumber(0.0, "0.0");
    testNumber(1.0, "1");
    testNumber(-1.0, "-1");
    testNumber(1.5, "1.5");
    testNumber(-1.5, "-1.5");
    testNumber(3.1416, "3.1416");
    testNumber(1E10, "1E10");
    testNumber(1e10, "1e10");
    testNumber(1E+10, "1E+10");
    testNumber(1E-10, "1E-10");
    testNumber(-1E10, "-1E10");
    testNumber(-1e10, "-1e10");
    testNumber(-1E+10, "-1E+10");
    testNumber(-1E-10, "-1E-10");
    testNumber(1.234E+10, "1.234E+10");
    testNumber(1.234E-10, "1.234E-10");
    testNumber(5.0E-324, "5e-324");
    testNumber(0, "1e-10000");
    testNumber(1.0000000000000002, "1.0000000000000002");
    testNumber(4.9406564584124654e-324, "4.9406564584124654e-324");
    testNumber(-4.9406564584124654e-324, "-4.9406564584124654e-324");
    testNumber(2.2250738585072009e-308, "2.2250738585072009e-308");
    testNumber(-2.2250738585072009e-308, "-2.2250738585072009e-308");
    testNumber(2.2250738585072014e-308, "2.2250738585072014e-308");
    testNumber(-2.2250738585072014e-308, "-2.2250738585072014e-308");
    testNumber(1.7976931348623157e+308, "1.7976931348623157e+308");
    testNumber(-1.7976931348623157e+308, "-1.7976931348623157e+308");
    string errMsg;
    Json json = Json::parse("1.2e+12", errMsg);
    EXPECT_TRUE(json.isNumber());
    json = Json(3.1415);
    EXPECT_EQ(3.1415, json.toNumber());
}

#define testString(expect, strJson)\
    do {\
        Json json = parseOk(strJson);\
        EXPECT_TRUE(json.isString());\
        EXPECT_EQ(json.toString(), expect);\
    } while (0)

TEST(Str2Json, JsonString) {
    testString("", "\"\"");
    testString("Hello", "\"Hello\"");
    testString("Hello\nWorld", "\"Hello\\nWorld\"");
    testString("\" \\ / \b \f \n \r \t",
               "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");
    string s = "Hello";
    s.push_back('\0');
    s += "World";
    testString(s, "\"Hello\\u0000World\"");
    testString("\x24", "\"\\u0024\"");
    testString("\xC2\xA2", "\"\\u00A2\"");
    testString("\xE2\x82\xAC", "\"\\u20AC\"");
    testString("\xF0\x9D\x84\x9E", "\"\\uD834\\uDD1E\"");
    testString("\xF0\x9D\x84\x9E", "\"\\ud834\\udd1e\"");
    string errMsg;
    Json json = Json::parse("\"something\"", errMsg);
    json = Json("another thing");
    EXPECT_EQ(json.toString(), "another thing");
}

TEST(Str2Json, JsonArray) {
    Json json = parseOk("[ ]");
    EXPECT_TRUE(json.isArray());
    EXPECT_EQ(json.size(), 0);

    json = parseOk("[ null , false , true , 123 , \"abc\" ]");
    EXPECT_TRUE(json.isArray());
    EXPECT_EQ(json.size(), 5);
    EXPECT_EQ(json[0], Json(nullptr));
    EXPECT_EQ(json[1], Json(false));
    EXPECT_EQ(json[2], Json(true));
    EXPECT_EQ(json[3], Json(123.0));
    EXPECT_EQ(json[4], Json("abc"));

    json = parseOk("[ [ ] , [ 0 ] , [ 0 , 1 ] , [ 0 , 1 , 2 ] ]");
    EXPECT_TRUE(json.isArray());
    EXPECT_EQ(json.size(), 4);

    EXPECT_TRUE(json[0].isArray());
    EXPECT_EQ(json[0].size(), 0);

    EXPECT_TRUE(json[1].isArray());
    EXPECT_EQ(json[1].size(), 1);
    EXPECT_EQ(json[1][0].toNumber(), 0);

    EXPECT_TRUE(json[2].isArray());
    EXPECT_EQ(json[2].size(), 2);
    EXPECT_EQ(json[2][0].toNumber(), 0);
    EXPECT_EQ(json[2][1].toNumber(), 1);

    EXPECT_TRUE(json[3].isArray());
    EXPECT_EQ(json[3].size(), 3);
    EXPECT_EQ(json[3][0].toNumber(), 0);
    EXPECT_EQ(json[3][1].toNumber(), 1);
    EXPECT_EQ(json[3][2].toNumber(), 2);
}

TEST(Str2Json, JsonObject) {
    Json json = parseOk("{ }");
    EXPECT_TRUE(json.isObject());
    EXPECT_EQ(json.size(), 0);

    json = parseOk(
        " { "
        "\"n\" : null , "
        "\"f\" : false , "
        "\"t\" : true , "
        "\"i\" : 123 , "
        "\"s\" : \"abc\", "
        "\"a\" : [ 1, 2, 3 ],"
        "\"o\" : { \"1\" : 1, \"2\" : 2, \"3\" : 3 }"
        " } ");
    EXPECT_TRUE(json.isObject());
    EXPECT_EQ(json.size(), 7);

    EXPECT_TRUE(json["n"].isNull());

    EXPECT_TRUE(json["f"].isBool());
    EXPECT_EQ(json["f"].toBool(), false);

    EXPECT_TRUE(json["t"].isBool());
    EXPECT_EQ(json["t"].toBool(), true);

    EXPECT_TRUE(json["i"].isNumber());
    EXPECT_EQ(json["i"].toNumber(), 123.0);

    EXPECT_TRUE(json["s"].isString());
    EXPECT_EQ(json["s"].toString(), "abc");

    EXPECT_TRUE(json["a"].isArray());
    EXPECT_EQ(json["a"].size(), 3);

    EXPECT_TRUE(json["o"].isObject());
    EXPECT_EQ(json["o"].size(), 3);
}

#define testRoundtrip(expect)\
    do {\
        Json json = parseOk(expect);\
        string actual = json.serialize();\
        if (json.isNumber())\
            EXPECT_EQ(strtod(actual.c_str(), nullptr),\
                      strtod(expect, nullptr));\
        else\
            EXPECT_EQ(actual, expect);\
    } while (0)

TEST(RoundTrip, literal) {
    testRoundtrip("null");
    testRoundtrip("true");
    testRoundtrip("false");
}

TEST(RoundTrip, JsonNumber) {
    testRoundtrip("0");
    testRoundtrip("-0");
    testRoundtrip("1");
    testRoundtrip("-0");
    testRoundtrip("1.5");
    testRoundtrip("-1.5");
    testRoundtrip("3.25");
    testRoundtrip("1e+20");
    testRoundtrip("1.234e+20");
    testRoundtrip("1.234e-20");
    testRoundtrip("1.0000000000000002");
    testRoundtrip("4.9406564584124654e-324");
    testRoundtrip("-4.9406564584124654e-324");
    testRoundtrip("2.2250738585072009e-308");
    testRoundtrip("-2.2250738585072009e-308");
    testRoundtrip("2.2250738585072014e-308");
    testRoundtrip("-2.2250738585072014e-308");
    testRoundtrip("1.7976931348623157e+308");
    testRoundtrip("-1.7976931348623157e+308");
}

TEST(RoundTrip, JsonString) {
    testRoundtrip("\"\"");
    testRoundtrip("\"Hello\"");
    testRoundtrip("\"Hello\\nWorld\"");
    testRoundtrip("\"\\\" \\\\ / \\b \\f \\n \\r \\t\"");
    testRoundtrip("\"Hello\\u0000World\"");
}

TEST(RoundTrip, JsonArray) {
    testRoundtrip("[  ]");
    testRoundtrip("[ null, false, true, 123, \"abc\", [ 1, 2, 3 ] ]");
}

TEST(RoundTrip, JsonObject) {
    testRoundtrip("{  }");
    testRoundtrip((R"({ "f": false })"));
    testRoundtrip((R"({ "n": null })"));
    testRoundtrip((R"({ "i": 123 })"));
    testRoundtrip((R"({ "s": "abc" })"));
    testRoundtrip((R"({ "a": [ 1, 2, 3 ] })"));
    testRoundtrip((R"({ "o": { "1": 1 } })"));
    // testRoundtrip((
    //     R"({ "n": null, "f": false, "t": true, "i": 123, "a": [ 1, 2, 3 ], "s": "abc", "o": { "1": 1, "2": 2, "3": 3 } })"));
}


#define testError(expect, strJson)\
    do {\
        string errMsg;\
        Json json = Json::parse(strJson, errMsg);\
        string actual = errMsg;\
        EXPECT_EQ(actual, expect);\
    } while (0)

TEST(Error, ExpectValue) {
    testError("parse expect value", "");
    testError("parse expect value", " ");
}

TEST(Error, InvalidValue) {
    testError("parse invalid value", "nul");
    testError("parse invalid value", "?");
    testError("parse invalid value", "+0");
    testError("parse invalid value", "+1");
    testError("parse invalid value", ".123");
    testError("parse invalid value", "1.");
    testError("parse invalid value", "inf");
    testError("parse invalid value", "INF");
    testError("parse invalid value", "NAN");
    testError("parse invalid value", "nan");
    testError("parse invalid value", "[1,]");
    testError("parse invalid value", "[\"a\", nul]");
}

TEST(Error, RootNotSingular) {
    testError("parse root not singular", "null x");
    testError("parse root not singular", "0123");
    testError("parse root not singular", "0x0");
    testError("parse root not singular", "0x123");
}

TEST(Error, MissQuotationMark) {
    testError("parse miss quotation mark", "\"");
    testError("parse miss quotation mark", "\"abc");
}

TEST(Error, InvalidStringEscape) {
    testError("parse invalid string escape", "\"\\v\"");
    testError("parse invalid string escape", "\"\\'\"");
    testError("parse invalid string escape", "\"\\0\"");
    testError("parse invalid string escape", "\"\\x12\"");
}

TEST(Error, InvalidStringChar) {
    testError("parse invalid string char", "\"\x01\"");
    testError("parse invalid string char", "\"\x1F\"");
}

TEST(Error, InvalidUnicodeHex) {
    testError("parse invalid unicode hex", "\"\\u\"");
    testError("parse invalid unicode hex", "\"\\u0\"");
    testError("parse invalid unicode hex", "\"\\u01\"");
    testError("parse invalid unicode hex", "\"\\u012\"");
    testError("parse invalid unicode hex", "\"\\u/000\"");
    testError("parse invalid unicode hex", "\"\\uG000\"");
    testError("parse invalid unicode hex", "\"\\u0/00\"");
    testError("parse invalid unicode hex", "\"\\u0G00\"");
    testError("parse invalid unicode hex", "\"\\u000/\"");
    testError("parse invalid unicode hex", "\"\\u00G/\"");
    testError("parse invalid unicode hex", "\"\\u 123/\"");
}

TEST(Error, InvalidUnicodeSurrogate) {
    testError("parse invalid unicode surrogate", "\"\\uD800\"");
    testError("parse invalid unicode surrogate", "\"\\uDBFF\"");
    testError("parse invalid unicode surrogate", "\"\\uD800\\\\\\");
    testError("parse invalid unicode surrogate", "\"\\uD800\\uDBFF\"");
    testError("parse invalid unicode surrogate", "\"\\uD800\\uE000\"");
}

TEST(Error, MissCommaOrSquareBracket) {
    testError("parse miss comma or square bracket", "[1");
    testError("parse miss comma or square bracket", "[1}");
    testError("parse miss comma or square bracket", "[1 2");
    testError("parse miss comma or square bracket", "[[]");
}

TEST(Error, MissKey) {
    testError("parse miss key", "{:1,");
    testError("parse miss key", "{1:1,");
    testError("parse miss key", "{true:1,");
    testError("parse miss key", "{false:1,");
    testError("parse miss key", "{null:1,");
    testError("parse miss key", "{[]:1,");
    testError("parse miss key", "{{}:1,");
    testError("parse miss key", "{\"a\":1,");
}

TEST(Error, MissColon) {
    testError("parse miss colon", "{\"a\"}");
    testError("parse miss colon", "{\"a\",\"b\"}");
}

TEST(Error, MissCommaOrCurlyBracket) {
    testError("parse miss comma or curly bracket", "{\"a\":1");
    testError("parse miss comma or curly bracket", "{\"a\":1]");
    testError("parse miss comma or curly bracket", "{\"a\":1 \"b\"");
    testError("parse miss comma or curly bracket", "{\"a\":{}");
}

TEST(Json, JsonCtor) {
    {
        Json json;
        EXPECT_TRUE(json.isNull());
    }

    {
        Json json(nullptr);
        EXPECT_TRUE(json.isNull());
    }

    {
        Json json(true);
        EXPECT_TRUE(json.isBool());
        EXPECT_EQ(json.toBool(), true);

        Json json1(false);
        EXPECT_TRUE(json1.isBool());
        EXPECT_EQ(json1.toBool(), false);
    }

    {
        Json json(0);
        EXPECT_TRUE(json.isNumber());
        EXPECT_EQ(json.toNumber(), 0);

        Json json1(100.1);
        EXPECT_TRUE(json1.isNumber());
        EXPECT_EQ(json1.toNumber(), 100.1);
    }

    {
        Json json("hello");
        EXPECT_TRUE(json.isString());
        EXPECT_EQ(json.toString(), "hello");
    }

    {
        vector<Json> arr{Json(nullptr), Json(true), Json(1.2)};
        Json json(arr);
        EXPECT_TRUE(json.isArray());
        EXPECT_TRUE(json[0].isNull());
    }

    {
        unordered_map<string, Json> obj;
        obj.insert({"hello", Json(nullptr)});
        obj.insert({"world", Json("!!")});
        Json json(obj);
        EXPECT_TRUE(json.isObject());
        EXPECT_TRUE(json["world"].isString());
    }

    {
        Json json = Json::object{
            {"key1", "value1"},
            {"key2", false},
            {"key3", Json::array{1, 2, 3}},
        };
        EXPECT_TRUE(json.isObject());
        EXPECT_TRUE(json["key1"].isString());
        EXPECT_EQ(json["key1"].toString(), "value1");
        EXPECT_TRUE(json["key2"].isBool());
        EXPECT_EQ(json["key2"].toBool(), false);
        EXPECT_TRUE(json["key3"].isArray());
        EXPECT_EQ(json["key3"][1].toNumber(), 2);
    }
}

TEST(Json, JsonOperator) {
    Json json = Json::object{
            {"key1", "value1"},
            {"key2", false},
            {"key3", Json::array{1, 2, 3}},
        };
    Json json1 = json;
    const Json json2 = json1;
    Json json3;
    json3 = json2;
    Json json4;
    json4 = Json::object{
            {"key1", "value1"},
            {"key2", false},
            {"key3", Json::array{1, 2, 3}},
        };
    EXPECT_TRUE(json == json1);
    EXPECT_TRUE(json == json2);
    EXPECT_TRUE(json == json3);
    EXPECT_TRUE(json == json4);
    EXPECT_TRUE(json["key1"] == "value1");
    EXPECT_TRUE(json2["key3"][1] == 2);
}
