#include "Json.h"
#include "JsonIO.h"

#include <iostream>
#include <sstream>

json::Value parseJson(const std::string& text)
{
	json::Value result;
	std::istringstream(text) >> result;
	return result;
}

void testNumber()
{
	std::cout << "* Parse and output number" << std::endl;
	auto value = parseJson("123");
	std::cout << "Number = " << value.as<json::Number>() << std::endl;
}

void testString()
{
	std::cout << "* Parse and output string" << std::endl;
	auto value = parseJson("\"4\\t56\"");
	std::cout << "String = " << value.as<json::String>() << std::endl;
}

void testArray()
{
	std::cout << "* Parse and output array" << std::endl;
	auto value = parseJson(R"X([456, 1, 5, "world"])X");
	std::cout << "Array[3] = " << *value.as<json::Array>()[3] << std::endl;
}

void testObject()
{
	std::cout << "* Parse and output object" << std::endl;
	auto value = parseJson(R"X({ "array1": [456, 1, 5, "wor\tld", [1,2,3], {"test":"ok", "test2": 1}], "num2": 67834, "str3": "hello world", "obj4": {}, "emptyarr": [{},{},[]] })X");

	auto n456 = value.as<json::Object>()["array1"]->as<json::Array>()[0]->as<json::Number>();

	std::cout << "Object[\"array1\"][0] = " << n456 << std::endl;
	std::cout << "Object = " << value << std::endl;
}


void testEmptyObject()
{
	std::cout << "* Parse and output empty object" << std::endl;
	auto value = parseJson("{}");
	std::cout << "Object = " << value << std::endl;
}

void testBigJson()
{
	std::cout << "* Parse and extract value from big json" << std::endl;

	auto value = parseJson(R"X({
	"glossary": {
		"title": "example glossary",
		"GlossDiv": {
			"title": "S",
			"GlossList": {
				"GlossEntry": {
					"ID": "SGML-\"test\"",
					"SortAs": "SGML",
					"GlossTerm": "Standard Generalized Markup Language",
					"Acronym": "SGML",
					"Abbrev": "ISO 8879:1986",
					"GlossDef": {
						"para": "A meta-markup language, used to create markup languages such as DocBook.",
						"GlossSeeAlso": ["GML", "XML"]
					},
					"GlossSee": "markup"
				}
			}
		}
	}
	})X");

	auto glossEntryId = 
		value.as<json::Object>()["glossary"]
			-> as<json::Object>()["GlossDiv"]
			-> as<json::Object>()["GlossList"]
			-> as<json::Object>()["GlossEntry"]
			-> as<json::Object>()["ID"] -> as<json::String>();

	std::cout << "GlossEntry.ID = " << glossEntryId << std::endl;
	std::cout << "Glossary JSON = " << value << std::endl;
}


int main()
{
	testNumber();
	testString();
	testArray();
	testObject();
	testEmptyObject();
	testBigJson();

	return 0;
}
