#ifndef VIOLET_TupleUtilities_H
#define VIOLET_TupleUtilities_H

#include "engine/template/TemplateUtilities.h"

#include <functional>
#include <ostream>
#include <tuple>

namespace Violet
{
	/*
	 * has_type
	 */
	
	namespace detail
	{
		// Recursive helper
		template <typename Tuple, typename DesiredType, unsigned Width>
		struct has_type : bool_constant<
			bool_if_else<equal<DesiredType, typename std::tuple_element<Width - 1, Tuple>::type>::value,
			true,
			has_type<Tuple, DesiredType, Width - 1>::value>::value> {};

		// Base helper
		template <typename Tuple, typename DesiredType>
		struct has_type<Tuple, DesiredType, 0> : false_type {};
	}

	/*
	 * Defines whether or not a given std::tuple type Tuple has a value with type DesiredType
	 */
	template <typename Tuple, typename DesiredType>
	struct has_type : bool_constant<detail::has_type<Tuple, DesiredType, std::tuple_size<Tuple>::value>::value> {};




	/*
	 * is_unique
	 */

	namespace detail
	{
		// Recursive helper
		template <typename Tuple, unsigned Width>
		struct is_unique
		{
			typedef typename std::tuple_element<Width - 1, Tuple>::type ElementType;
			enum { value =    !detail::has_type<Tuple, ElementType, Width - 1>::value
							&& is_unique<Tuple, Width - 1>::value };
		};

		// Base helper
		template <typename Tuple>
		struct is_unique<Tuple, 0> : true_type {};
	}
	
	/*
	 * Defines whether a std::tuple of type Tuple has any values with matching types
	 */
	template <typename Tuple>
	struct is_unique : bool_constant<detail::is_unique<Tuple, std::tuple_size<Tuple>::value>::value> {};
	



	/*
	 * index_of
	 */

	namespace detail
	{
		// Recursive helper
		template <typename ReturnType, typename Tuple, unsigned Width>
		struct index_of : unsigned_if_else<
			equal<ReturnType, typename std::tuple_element<Width - 1, Tuple>::type>::value,
			Width - 1,
			index_of<ReturnType, Tuple, Width - 1>::value> {};

		// Base helper
		template <typename ReturnType, typename Tuple>
		struct index_of<ReturnType, Tuple, 0> : unsigned_constant<0> {};
	}

	/*
	 * Defines the index of value in Tuple that has a given type
	 * Fails if the tuple has more than one value of the given type or if it is not unique
	 */
	template <typename ReturnType, typename Tuple>
	struct index_of : unsigned_constant<
		detail::index_of<ReturnType, Tuple, std::tuple_size<Tuple>::value>::value>
	{
		static_assert(has_type<Tuple, ReturnType>::value, "Tuple doesn't have type");
		static_assert(is_unique<Tuple>::value, "Tuple is not unique");
	};




	/*
	 * get
	 */

	/*
	 * Implementation of C++14's std::get<typename>(Tuple&)
	 */
	template <typename ReturnType, typename Tuple>
	ReturnType & get(Tuple & tuple)
	{
		return std::get<detail::index_of<ReturnType, Tuple, std::tuple_size<Tuple>::value>::value>(tuple);
	}

	/*
	 * Implementation of C++14's std::get<typename>(const Tuple&)
	 */
	template <typename ReturnType, typename Tuple>
	const ReturnType & get(const Tuple & tuple)
	{
		return std::get<detail::index_of<ReturnType, Tuple, std::tuple_size<Tuple>::value>::value>(tuple);
	}




	/*
	 * extract
	 */

	namespace detail
	{
		// Recursive helper
		template <typename InTuple, typename OutTuple, unsigned OutWidth>
		struct extractor
		{
			static void extract(const InTuple & inTuple, OutTuple & outTuple)
			{
				extractor<InTuple, OutTuple, OutWidth - 1>::extract(inTuple, outTuple);
				std::get<OutWidth - 1>(outTuple) = Violet::get<typename std::tuple_element<OutWidth - 1, OutTuple>::type>(inTuple);
			}
		};

		// Base helper
		template <typename InTuple, typename OutTuple>
		struct extractor<InTuple, OutTuple, 0>
		{
			static void extract(const InTuple & inTuple, OutTuple & outTuple)
			{
			}
		};
	}


	/*
	 * Copies all the values from in into out that match a type in out
	 */
	template <typename InTuple, typename OutTuple>
	void extract(const InTuple & in, OutTuple & out)
	{
		detail::extractor<InTuple, OutTuple, std::tuple_size<OutTuple>::value>::extract(in, out);
	}




	/*
	 * print
	 */

	namespace detail
	{
		// Recursive helper
		template <typename Tuple, unsigned Index>
		struct PrintHelper
		{
			static void print(std::ostream & stream, const Tuple & tuple, const char * separator)
			{
				PrintHelper<Tuple, Index - 1>::print(stream, tuple, separator);
				stream << separator << std::get<Index>(tuple);
			}
		};

		// Base helper
		template <typename Tuple>
		struct PrintHelper<Tuple, 0>
		{
			static void print(std::ostream & stream, const Tuple & tuple, const char * /*separator*/)
			{
				stream << std::get<0>(tuple);
			}
		};
	}

	// Simple print method
	template <typename Tuple>
	void print(std::ostream & stream, const Tuple & tuple, const char * separator)
	{
		detail::PrintHelper<Tuple, std::tuple_size<Tuple>::value - 1>::print(stream, tuple, separator);
	}

	template <typename... Elements>
	std::ostream & operator<<(std::ostream & stream, const std::tuple<Elements...> & tuple)
	{
		print(stream, tuple, ", ");
		return stream;
	}

	/*
	 * Print class
	 * Easier for streaming output (i.e. cout << TuplePrinter<decltype(tuple)>(tuple, ", ") << ...;
	 */
	template <typename Tuple>
	class TuplePrinter
	{
	public:

		TuplePrinter(const Tuple & tuple, const char * separator) :
			m_tuple(tuple),
			m_separator(separator)
		{
		}

		friend std::ostream & operator<<(std::ostream & stream, const TuplePrinter & printer)
		{
			detail::PrintHelper<Tuple, std::tuple_size<Tuple>::value - 1>::print(stream, printer.m_tuple, printer.m_separator);
			return stream;
		}

	private:

		const Tuple & m_tuple;
		const char * m_separator;
	};




	/*
	 * sequence
	 */

	template <size_t... Is>
	struct sequence {};

	template <size_t N, size_t... Is>
	struct sequence_generator : sequence_generator<N - 1, N - 1, Is...> {};

	template <size_t... Is>
	struct sequence_generator<0, Is...>
	{
		using type = sequence<Is...>;
	};




	/*
	 * for_all
	 */
	
	namespace detail
	{
		template <typename Predicate, typename... Args, size_t... N>
		std::result_of_t<Predicate(Args...)> for_all(Predicate pr, const std::tuple<Args...> & tup, sequence<N...> seq)
		{
			return pr(std::get<N>(tup)...);
		}
	}

	template <typename Predicate, typename... Args>
	std::result_of_t<Predicate(Args...)> for_all(Predicate pr, const std::tuple<Args...> & tup)
	{
		return detail::for_all(pr, tup, typename sequence_generator<sizeof...(Args)>::type());
	}

	namespace detail
	{
		template <typename Type, typename Predicate, typename... Args, size_t... N>
		std::result_of_t<Predicate(Type, Args...)> for_all(Type & type, Predicate pr, const std::tuple<Args...> & tup, sequence<N...> seq)
		{
			return (type.*pr)(std::get<N>(tup)...);
		}
	}

	template <typename Type, typename Predicate, typename... Args>
	std::result_of_t<Predicate(Type, Args...)> for_all(Type & type, Predicate pr, const std::tuple<Args...> & tup)
	{
		return detail::for_all(type, pr, tup, typename sequence_generator<sizeof...(Args)>::type());
	}




	/*
	* for_each
	*/

	namespace detail
	{
		template <size_t> struct Index {};

		template <typename Predicate, typename... Args>
		void for_each(Predicate pr, std::tuple<Args...> & tup, Index<0>)
		{
		}

		template <typename Predicate, typename... Args, size_t N>
		void for_each(Predicate pr, std::tuple<Args...> & tup, Index<N>)
		{
			for_each(pr, tup, Index<N - 1>());
			pr(std::get<N - 1>(tup));
		}
	}

	template <typename Predicate, typename... Args>
	void for_each(Predicate pr, std::tuple<Args...> & tup)
	{
		detail::for_each(pr, tup, detail::Index<sizeof...(Args)>());
	}




	/*
	* for_each (const)
	*/

	namespace detail
	{
		template <typename Predicate, typename... Args>
		void for_each(Predicate pr, const std::tuple<Args...> & tup, Index<0>)
		{
		}

		template <typename Predicate, typename... Args, size_t N>
		void for_each(Predicate pr, const std::tuple<Args...> & tup, Index<N>)
		{
			for_each(pr, tup, Index<N - 1>());
			pr(std::get<N - 1>(tup));
		}
	}

	template <typename Predicate, typename... Args>
	void for_each(Predicate pr, const std::tuple<Args...> & tup)
	{
		detail::for_each(pr, tup, detail::Index<sizeof...(Args)>());
	}




	/*
	* first
	*/

	namespace detail
	{
		template <typename Predicate, typename... Args>
		size_t first(Predicate pr, std::tuple<Args...> & tup, Index<0>)
		{
			return std::tuple_size<std::tuple<Args...>>::value;
		}

		template <typename Predicate, typename... Args, size_t N>
		size_t first(Predicate pr, std::tuple<Args...> & tup, Index<N>)
		{
			auto currentResult = first(pr, tup, Index<N - 1>());
			return currentResult != std::tuple_size<std::tuple<Args...>>::value ?
				currentResult :
				pr(std::get<N - 1>(tup)) ?
					N - 1 :
					currentResult;
		}
	}

	template <typename Predicate, typename... Args>
	size_t first(Predicate pr, std::tuple<Args...> & tup)
	{
		return detail::first(pr, tup, detail::Index<sizeof...(Args)>());
	}
}

#endif
