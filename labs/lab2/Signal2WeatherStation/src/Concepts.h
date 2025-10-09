#ifndef CONCEPTS_H
#define CONCEPTS_H

#include <concepts>
#include <limits>
#include <type_traits>

template <typename T>
concept Arithmetic = requires(T a, T b) {
	requires std::is_arithmetic_v<T>;
	{ a + b } -> std::same_as<T>;
	{ a - b } -> std::same_as<T>;
	{ a * b } -> std::same_as<T>;
	{ a / b } -> std::same_as<T>;
	{ a < b } -> std::convertible_to<bool>;
	{ a > b } -> std::convertible_to<bool>;
	{ a == b } -> std::convertible_to<bool>;
};

template <typename T>
concept HasNumericLimits = requires {
	typename std::numeric_limits<T>;
	{ std::numeric_limits<T>::infinity() } -> std::convertible_to<T>;
	{ std::numeric_limits<T>::max() } -> std::convertible_to<T>;
	{ std::numeric_limits<T>::min() } -> std::convertible_to<T>;
};

template <typename T>
concept SuitableValueType = Arithmetic<T> && HasNumericLimits<T>;

#endif /* CONCEPTS_H */
