#ifndef VIOLET_TemplateUtilities_H
#define VIOLET_TemplateUtilities_H

namespace Violet
{
	template <typename T>
	struct type_is
	{
		typedef T type;
	};

	template <typename ValueType, ValueType v>
	struct integral_constant
	{
		static const ValueType value = v;
	};

	template <bool b>
	using bool_constant = integral_constant<bool, b>;
	using true_type = bool_constant<true>;
	using false_type = bool_constant<false>;

	template <unsigned s>
	using unsigned_constant = integral_constant<unsigned, s>;



	template <bool Predicate, typename Then>
	struct enable_if {};

	template <typename Then>
	struct enable_if<true, Then> : type_is<Then> {};

	template <bool Predicate, typename Then, typename Else>
	struct if_else;

	template <typename Then, typename Else>
	struct if_else<true, Then, Else> : type_is<Then> {};

	template <typename Then, typename Else>
	struct if_else<false, Then, Else> : type_is<Else> {};

	template <bool Predicate, typename ValueType, ValueType Then, ValueType Else>
	struct integral_if_else;

	template <typename ValueType, ValueType Then, ValueType Else>
	struct integral_if_else<true, ValueType, Then, Else> : integral_constant<ValueType, Then> {};

	template <typename ValueType, ValueType Then, ValueType Else>
	struct integral_if_else<false, ValueType, Then, Else> : integral_constant<ValueType, Else> {};

	template <bool Predicate, unsigned Then, unsigned Else>
	using unsigned_if_else = integral_if_else<Predicate, unsigned, Then, Else>;

	template <bool Predicate, bool Then, bool Else>
	using bool_if_else = integral_if_else<Predicate, bool, Then, Else>;

	template <typename Type1, typename Type2>
	struct equal : false_type {};

	template <typename Type>
	struct equal<Type, Type> : true_type {};



	template <typename T>
	struct copyable
	{
		typedef T type;
	};

	template <typename T> using copyable_t = typename copyable<T>::type;

	template <typename T>
	typename copyable_t<T> to_copyable(const T & t)
	{
		return static_cast<copyable_t<T>>(t);
	}
}

#endif
