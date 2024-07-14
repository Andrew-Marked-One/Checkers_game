#pragma once
#include <concepts>
#include <type_traits>
#include <string>
#include <vector>
#include <variant>


template <typename T>
concept isString = std::is_same_v<std::decay_t<T>, std::string>;

template <typename T>
concept isArray = std::is_array_v<std::remove_reference_t<T>>;

template <typename T>
concept isContainingChar = std::is_same_v<std::remove_cv_t<std::remove_extent_t<std::remove_reference_t<T>>>, char>;

template <typename T>
concept isCharArray = isArray<T> && isContainingChar<T>;

template <typename T>
concept isStringLike = isString<T> || isCharArray<T>;

template <typename T>
concept isStringVec = std::is_same_v<std::decay_t<T>, std::vector<std::string>>;


template <typename T, typename U>
constexpr bool is_type_in_tuple = std::is_same_v<T, U>;

template <typename T>
constexpr bool is_type_in_tuple<T, std::tuple<>> = false;

template <typename T, typename FirstT, typename... RestTs>
constexpr bool is_type_in_tuple<T, std::tuple<FirstT, RestTs...>> = is_type_in_tuple<T, FirstT> || is_type_in_tuple<T, std::tuple<RestTs...>>;

template <typename T, typename Tuple>
concept isTypeInUniquePtrTuple = is_type_in_tuple<std::unique_ptr<T>, Tuple>;

template <typename T1, typename T2>
concept isSameWithoutConstRef = std::is_same_v<std::remove_cvref_t<T1>, T2>;
