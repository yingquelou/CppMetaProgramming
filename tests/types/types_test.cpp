#include "types.hpp"
#include <tuple>
#include <type_traits>
#include <utility>

template <typename L, typename R>
using size_less = std::integral_constant<bool, sizeof(L) < sizeof(R)>;

namespace test_conjunction_disjunction {
static_assert(types::conjunction<>::value, "conjunction<> should be true");
static_assert(types::conjunction<std::true_type>::value, "conjunction<true> should be true");
static_assert(types::conjunction<std::true_type, std::true_type>::value, "conjunction<true,true> should be true");
static_assert(!types::conjunction<std::true_type, std::false_type>::value, "conjunction<true,false> should be false");
static_assert(!types::conjunction<std::false_type, std::true_type>::value, "conjunction<false,true> should be false");
static_assert(!types::conjunction<std::false_type, std::false_type>::value, "conjunction<false,false> should be false");
static_assert(!types::conjunction<std::true_type, std::true_type, std::false_type>::value, "conjunction with false should be false");

static_assert(!types::disjunction<>::value, "disjunction<> should be false");
static_assert(types::disjunction<std::true_type>::value, "disjunction<true> should be true");
static_assert(!types::disjunction<std::false_type>::value, "disjunction<false> should be false");
static_assert(types::disjunction<std::true_type, std::false_type>::value, "disjunction<true,false> should be true");
static_assert(types::disjunction<std::false_type, std::true_type>::value, "disjunction<false,true> should be true");
static_assert(!types::disjunction<std::false_type, std::false_type>::value, "disjunction<false,false> should be false");
static_assert(types::disjunction<std::false_type, std::false_type, std::true_type>::value, "disjunction with true should be true");
}

namespace test_transfer {
static_assert(std::is_same<types::transfer<std::tuple, std::tuple<int, double>>::type, std::tuple<int, double>>::value, "transfer tuple to tuple");
static_assert(std::is_same<types::transfer<std::pair, std::tuple<int, double>>::type, std::pair<int, double>>::value, "transfer tuple to pair");
static_assert(std::is_same<types::transfer<std::tuple, std::tuple<>>::type, std::tuple<>>::value, "transfer empty tuple");
static_assert(std::is_same<types::transfer<std::tuple, std::tuple<int>>::type, std::tuple<int>>::value, "transfer single element");
}

namespace test_integer_sequence {
static_assert(types::integer_sequence<int, 0, 1, 2>::size() == 3, "integer_sequence size");
static_assert(types::integer_sequence<int>::size() == 0, "empty integer_sequence size");
static_assert(std::is_same<types::integer_sequence<int, 0, 1, 2>::value_type, int>::value, "integer_sequence value_type");

static_assert(types::make_index_sequence<0>::size() == 0, "make_index_sequence<0>");
static_assert(types::make_index_sequence<1>::size() == 1, "make_index_sequence<1>");
static_assert(types::make_index_sequence<5>::size() == 5, "make_index_sequence<5>");

static_assert(types::make_integer_sequence<int, 0>::size() == 0, "make_integer_sequence<int, 0>");
static_assert(types::make_integer_sequence<int, 3>::size() == 3, "make_integer_sequence<int, 3>");

static_assert(types::index_sequence_for<int, double, bool>::size() == 3, "index_sequence_for size");
static_assert(types::index_sequence_for<>::size() == 0, "index_sequence_for empty");
}

namespace test_as_type {
static_assert(std::is_same<types::as_type<int>::type, int>::value, "as_type<int>");
static_assert(std::is_same<types::as_type<double>::type, double>::value, "as_type<double>");
static_assert(std::is_same<types::as_type<std::tuple<int, double>>::type, std::tuple<int, double>>::value, "as_type<tuple>");
}

namespace test_reversal {
static_assert(std::is_same<types::reversal<std::tuple<>>::type, std::tuple<>>::value, "reversal empty");
static_assert(std::is_same<types::reversal<std::tuple<int>>::type, std::tuple<int>>::value, "reversal single");
static_assert(std::is_same<types::reversal<std::tuple<int, double>>::type, std::tuple<double, int>>::value, "reversal pair");
static_assert(std::is_same<types::reversal<std::tuple<int, double, bool>>::type, std::tuple<bool, double, int>>::value, "reversal triple");
static_assert(std::is_same<types::reversal<std::tuple<char, short, int, long>>::type, std::tuple<long, int, short, char>>::value, "reversal four");
}

namespace test_concat {
static_assert(std::is_same<types::concat<std::tuple<int>, std::tuple<double>>::type, std::tuple<int, double>>::value, "concat two tuples");
static_assert(std::is_same<types::concat<std::tuple<>, std::tuple<int>>::type, std::tuple<int>>::value, "concat empty and non-empty");
static_assert(std::is_same<types::concat<std::tuple<int>, std::tuple<>>::type, std::tuple<int>>::value, "concat non-empty and empty");
static_assert(std::is_same<types::concat<std::tuple<>, std::tuple<>>::type, std::tuple<>>::value, "concat two empty");
static_assert(std::is_same<types::concat<std::tuple<int>, std::tuple<double>, std::tuple<bool>>::type, std::tuple<int, double, bool>>::value, "concat three tuples");
}

namespace test_map {
template <typename T, std::size_t Index, typename Tuple> struct ToPointer {
    using type = T*;
};
static_assert(std::is_same<types::map<ToPointer, std::tuple<>>::type, std::tuple<>>::value, "map empty");
static_assert(std::is_same<types::map<ToPointer, std::tuple<int>>::type, std::tuple<int*>>::value, "map single");
static_assert(std::is_same<types::map<ToPointer, std::tuple<int, double>>::type, std::tuple<int*, double*>>::value, "map pair");

template <typename T, std::size_t Index, typename Tuple> struct AddIndex {
    using type = std::integral_constant<std::size_t, Index>;
};
using mapped = types::map<AddIndex, std::tuple<int, double, bool>>::type;
static_assert(std::tuple_size<mapped>::value == 3, "map preserves size");
}

namespace test_filter {
template <typename T, std::size_t, typename>
using IsPointer = std::is_pointer<T>;

using Filtered = types::filter<IsPointer, std::tuple<int, int*, double, double*>>::type;
static_assert(std::tuple_size<typename Filtered::first_type>::value == 2, "filter include count");
static_assert(std::tuple_size<typename Filtered::second_type>::value == 2, "filter exclude count");

using Included = types::filter_include<IsPointer, std::tuple<int, int*, double, double*>>::type;
static_assert(std::tuple_size<Included>::value == 2, "filter_include count");

using Excluded = types::filter_exclude<IsPointer, std::tuple<int, int*, double, double*>>::type;
static_assert(std::tuple_size<Excluded>::value == 2, "filter_exclude count");

static_assert(std::tuple_size<types::filter_include<IsPointer, std::tuple<>>::type>::value == 0, "filter_include empty");
static_assert(std::tuple_size<types::filter_exclude<IsPointer, std::tuple<>>::type>::value == 0, "filter_exclude empty");
}

namespace test_in {
static_assert(types::in<int, std::tuple<int, double>>::value, "in found");
static_assert(!types::in<bool, std::tuple<int, double>>::value, "in not found");
static_assert(types::in<int, std::tuple<int>>::value, "in single found");
static_assert(!types::in<int, std::tuple<>>::value, "in empty not found");
static_assert(types::in<double, std::tuple<int, double, bool>>::value, "in middle found");
}

namespace test_types {
using Uniq = types::types<std::tuple<int, int, double, int, double, bool>>::type;
static_assert(std::tuple_size<Uniq>::value == 3, "types deduplicates");

static_assert(std::tuple_size<types::types<std::tuple<>>::type>::value == 0, "types empty");
static_assert(std::tuple_size<types::types<std::tuple<int>>::type>::value == 1, "types single");
static_assert(std::tuple_size<types::types<std::tuple<int, int, int>>::type>::value == 1, "types all duplicates");
}

namespace test_set_operations {
using A = std::tuple<int, double, bool>;
using B = std::tuple<double, bool, char>;

using Intersection = types::n<A, B>::type;
static_assert(std::tuple_size<Intersection>::value == 2, "intersection size");

using Difference = types::x<A, B>::type;
static_assert(std::tuple_size<Difference>::value == 1, "difference size");

using Union = types::u<A, B>::type;
static_assert(std::tuple_size<Union>::value == 4, "union size (deduplicated)");

static_assert(std::tuple_size<types::n<A, std::tuple<>>::type>::value == 0, "intersection with empty");
static_assert(std::tuple_size<types::u<A, std::tuple<>>::type>::value == 3, "union with empty");
static_assert(std::tuple_size<types::x<A, std::tuple<>>::type>::value == 3, "difference with empty");
}

int main() {
    return 0;
}