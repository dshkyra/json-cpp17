#include "JsonIO.h"
#include "Json.h"

#include <iterator>
#include <istream>
#include <cctype>
#include <stdexcept>
#include <sstream>
#include <algorithm>

namespace json {
	namespace detail {
		char processControlChar(std::istream_iterator<char>& start, const std::istream_iterator<char> end)
		{
			if (*start == '\\') // special processing of control chars
			{
				++start;
				if (start == end) throw std::runtime_error("unexpected end of stream");
				switch (*start)
				{
				case '"': return '"';
				case '\\': return '\\';
				case '/': return '/';
				case 't': return '\t';
				case 'n': return '\n';
				case 'r': return '\r';
				case 'b': return '\b';
				case 'f': return '\f';
				default: throw std::runtime_error("unexpected escaped char");
				}
			}
			else
			{
				return *start;
			}
		}

		std::string escapeControlChars(const std::string& str)
		{
			std::ostringstream result;
			for (auto c : str)
			{
				switch (c)
				{
				case '"': result << "\\\""; break;
				case '\\': result << "\\\\"; break;
				case '/': result << "/"; break;
				case '\t': result << "\\t"; break;
				case '\n': result << "\\n"; break;
				case '\r': result << "\\r"; break;
				case '\b': result << "\\b"; break;
				case '\f': result << "\\f"; break;
				default: result << c; break;
				}
			}
			return result.str();
		}

		template <class T>
		T parse(std::istream_iterator<char>& start, const std::istream_iterator<char> end);

		template<>
		Value parse(std::istream_iterator<char>& start, const std::istream_iterator<char> end);


		template<>
		Number parse(std::istream_iterator<char>& start, const std::istream_iterator<char> end)
		{
			std::stringstream result;
			result << *start++; // put first char into result and move to the next one
			while (start != end && std::isdigit(*start))
			{
				result << *start;
				++start;
			}

			return Number(std::atof(result.str().c_str()));
		}

		template<>
		String parse(std::istream_iterator<char>& start, const std::istream_iterator<char> end)
		{
			std::stringstream result;
			++start; // skip first '"'
			while (start != end && *start != '"')
			{
				result << processControlChar(start, end);				
				++start;
			}
			if (start == end) throw std::runtime_error("unexpected end of stream");
			++start; // skip last '"'

			return String(result.str());
		}

		template<>
		Array parse(std::istream_iterator<char>& start, const std::istream_iterator<char> end)
		{
			Array result;

			++start; // skip '['
			while (start != end && *start != ']')
			{
				result.push_back(std::make_unique<Value>(parse<Value>(start, end)));
				while (start != end && *start != ']' && *start != ',') ++start;
				if (*start == ',') ++start;
			}
			if (start == end) throw std::runtime_error("unexpected end of stream");
			++start; // skip ']'

			return result;
		}

		template<>
		Object parse(std::istream_iterator<char>& start, const std::istream_iterator<char> end)
		{
			Object result;

			++start; // skip '{'
			while (start != end && *start != '}')
			{
				auto key = parse<String>(start, end);
				while (start != end && *start != '}' && *start != ':') ++start;
				++start; // skip ':'
				result[key] = std::make_unique<Value>(parse<Value>(start, end));

				while (start != end && *start != '}' && *start != ',') ++start;
				if (*start == ',') ++start;
			}
			if (start == end) throw std::runtime_error("unexpected end of stream");
			++start; // skip '}'

			return result;
		}

		template <>
		Value parse(std::istream_iterator<char>& start, const std::istream_iterator<char> end)
		{
			if (*start == '{')
			{
				return Value(parse<Object>(start, end));
			}
			else if (*start == '[')
			{
				return Value(parse<Array>(start, end));
			}
			else if (*start == '"')
			{
				return Value(parse<String>(start, end));
			}
			else if (std::isdigit(*start) || *start == '-')
			{
				return Value(parse<Number>(start, end));
			}
			else
			{
				throw std::runtime_error("unexpected character");
			}
		}

		template <class C>
		std::ostream& outJsonContainer(std::ostream& out, const C& container)
		{
			auto current = std::begin(container);
			auto end = std::end(container);
			if (current != end)
			{
				out << *current;
				while (++current != end)
				{
					out << "," << *current;
				}
			}
			return out;
		}
	}

	std::istream& operator>>(std::istream& in, Value& value)
	{
		auto start = std::istream_iterator<char>(in);
		value = detail::parse<Value>(start, std::istream_iterator<char>());
		return in;
	}

	std::ostream& operator<<(std::ostream& out, const Array::value_type& value)
	{
		return out << *value;
	}

	std::ostream& operator<<(std::ostream& out, const Array& value)
	{
		return detail::outJsonContainer(out, value);
	}

	std::ostream& operator<<(std::ostream& out, const Object::value_type& value)
	{
		return out << value.first << ":" << *value.second;
	}

	std::ostream& operator<<(std::ostream& out, const Object& value)
	{
		return detail::outJsonContainer(out, value);
	}

	std::ostream& operator<<(std::ostream& out, const Value& value)
	{
		if (value.is<Number>())
		{
			out << value.as<Number>();
		}
		else if (value.is<String>())
		{
			out << '"' << detail::escapeControlChars(value.as<String>()) << '"';
		}
		else if (value.is<Array>())
		{
			out << '[' << value.as<Array>() << ']';
		}
		else if (value.is<Object>())
		{
			out << '{' << value.as<Object>() << '}';
		}

		return out;
	}
}
