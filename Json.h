#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <variant>

namespace json {
	class Value;
	using ValuePtr = std::unique_ptr<Value>;
	
	using Number = double;
	using String = std::string;
	using Array = std::vector<ValuePtr>;
	using Object = std::map<std::string, ValuePtr>;


	class Value {
	public:
		Value() : _data()
		{
		}

		template <class T>
		explicit Value(T x) : _data(std::move(x)) 
		{
		}

		template <class T> 
		const T& as() const
		{
			return std::get<T>(_data);
		}

		template <class T>
		T& as()
		{
			return std::get<T>(_data);
		}

		template <class T>
		bool is() const
		{
			return std::holds_alternative<T>(_data);
		}

		template <class T>
		Value& operator=(T&& x) 
		{
			_data = std::move(x);
			return *this;
		}

	private:
		std::variant<Number, String, Array, Object> _data;
	};
}
