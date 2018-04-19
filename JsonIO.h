#pragma once

#include <ios>

namespace json {
	class Value;
	
	std::istream& operator>>(std::istream& in, Value& value);
	std::ostream& operator<<(std::ostream& out, const Value& value);
}