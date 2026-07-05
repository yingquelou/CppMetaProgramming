#include "sort.hpp"
#include <tuple>
#include <type_traits>

template <typename L, typename R>
using size_less = std::integral_constant<bool, sizeof(L) < sizeof(R)>;

namespace test_comparator_adapter {
using Adapter = types::sort::ComparatorAdapter<size_less>;

static_assert(Adapter::template equal<int, int>::value, "equal same types");
static_assert(!Adapter::template equal<int, double>::value, "equal different types");

static_assert(!Adapter::template less<int, int>::value, "less same types");
static_assert(Adapter::template less<char, int>::value, "less char < int");
static_assert(!Adapter::template less<int, char>::value, "less int not < char");

static_assert(Adapter::template less_equal<int, int>::value, "less_equal same types");
static_assert(Adapter::template less_equal<char, int>::value, "less_equal char <= int");
static_assert(!Adapter::template less_equal<int, char>::value, "less_equal int not <= char");

static_assert(!Adapter::template greater<int, int>::value, "greater same types");
static_assert(!Adapter::template greater<char, int>::value, "greater char not > int");
static_assert(Adapter::template greater<int, char>::value, "greater int > char");

static_assert(Adapter::template greater_equal<int, int>::value, "greater_equal same types");
static_assert(!Adapter::template greater_equal<char, int>::value, "greater_equal char not >= int");
static_assert(Adapter::template greater_equal<int, char>::value, "greater_equal int >= char");
}

namespace test_unique {
using Sorted = std::tuple<char, char, short, short, int, int, long long, long long>;
using Uniq = types::sort::unique<Sorted, size_less>::type;
static_assert(std::tuple_size<Uniq>::value == 4, "unique groups equal elements");

using Mixed = std::tuple<char, short, char, short>;
using Grouped = types::sort::unique<Mixed, size_less>::type;
static_assert(std::tuple_size<Grouped>::value == 2, "unique groups all equal elements together");

static_assert(std::tuple_size<types::sort::unique<std::tuple<>, size_less>::type>::value == 0, "unique empty");
static_assert(std::tuple_size<types::sort::unique<std::tuple<int>, size_less>::type>::value == 1, "unique single");
}

namespace test_is_sorted {
using Sorted = std::tuple<char, short, int, long>;
using Unsorted = std::tuple<int, char, long, short>;

static_assert(types::sort::is_sorted<Sorted, size_less>::value, "is_sorted sorted tuple");
static_assert(!types::sort::is_sorted<Unsorted, size_less>::value, "is_sorted unsorted tuple");
static_assert(types::sort::is_sorted<std::tuple<int, int, int>, size_less>::value, "is_sorted all same");
static_assert(types::sort::is_sorted<std::tuple<char, short>, size_less>::value, "is_sorted pair");
static_assert(!types::sort::is_sorted<std::tuple<short, char>, size_less>::value, "is_sorted unsorted pair");
static_assert(types::sort::is_sorted<std::tuple<char, short, int>, size_less>::value, "is_sorted triple");
static_assert(!types::sort::is_sorted<std::tuple<int, short, char>, size_less>::value, "is_sorted unsorted triple");
}

namespace test_sort_insert {
using Unsorted = std::tuple<double, bool, unsigned short, short, void*, unsigned char, char, unsigned int, long, unsigned long, long long, unsigned long long, long double, int, float>;
using Sorted = types::sort::sort_insert<Unsorted, size_less>::type;
static_assert(types::sort::is_sorted<Sorted, size_less>::value, "sort_insert produces sorted result");

static_assert(std::is_same<types::sort::sort_insert<std::tuple<int>, size_less>::type, std::tuple<int>>::value, "sort_insert single");
}

namespace test_sort_bubble {
using Unsorted = std::tuple<double, bool, unsigned short, short, void*, unsigned char, char, unsigned int, long, unsigned long, long long, unsigned long long, long double, int, float>;
using Sorted = types::sort::sort_bubble<Unsorted, size_less>::type;
static_assert(types::sort::is_sorted<Sorted, size_less>::value, "sort_bubble produces sorted result");

static_assert(std::is_same<types::sort::sort_bubble<std::tuple<int>, size_less>::type, std::tuple<int>>::value, "sort_bubble single");
}

namespace test_sort_count {
using Unsorted = std::tuple<double, bool, unsigned short, short, void*, unsigned char, char, unsigned int, long, unsigned long, long long, unsigned long long, long double, int, float>;
using Sorted = types::sort::sort_count<Unsorted, size_less>::type;
static_assert(types::sort::is_sorted<Sorted, size_less>::value, "sort_count produces sorted result");

static_assert(std::is_same<types::sort::sort_count<std::tuple<int>, size_less>::type, std::tuple<int>>::value, "sort_count single");
}

namespace test_sort_group {
using Unsorted = std::tuple<double, bool, unsigned short, short, void*, unsigned char, char, unsigned int, long, unsigned long, long long, unsigned long long, long double, int, float>;
using Sorted = types::sort::sort_group<Unsorted, size_less>::type;
static_assert(types::sort::is_sorted<Sorted, size_less>::value, "sort_group produces sorted result");

static_assert(std::is_same<types::sort::sort_group<std::tuple<int>, size_less>::type, std::tuple<int>>::value, "sort_group single");
}

namespace test_sort_merge {
using Unsorted = std::tuple<double, bool, unsigned short, short, void*, unsigned char, char, unsigned int, long, unsigned long, long long, unsigned long long, long double, int, float>;
using Sorted = types::sort::sort_merge<Unsorted, size_less>::type;
static_assert(types::sort::is_sorted<Sorted, size_less>::value, "sort_merge produces sorted result");

static_assert(std::is_same<types::sort::sort_merge<std::tuple<int>, size_less>::type, std::tuple<int>>::value, "sort_merge single");
}

namespace test_sort_partition {
using Unsorted = std::tuple<double, bool, unsigned short, short, void*, unsigned char, char, unsigned int, long, unsigned long, long long, unsigned long long, long double, int, float>;
using Sorted = types::sort::sort_partition<Unsorted, size_less>::type;
static_assert(types::sort::is_sorted<Sorted, size_less>::value, "sort_partition produces sorted result");

static_assert(std::is_same<types::sort::sort_partition<std::tuple<int>, size_less>::type, std::tuple<int>>::value, "sort_partition single");
}

namespace test_sort_heap {
using Unsorted = std::tuple<double, bool, unsigned short, short, void*, unsigned char, char, unsigned int, long, unsigned long, long long, unsigned long long, long double, int, float>;
using Sorted = types::sort::sort_heap<Unsorted, size_less>::type;
static_assert(types::sort::is_sorted<Sorted, size_less>::value, "sort_heap produces sorted result");

static_assert(std::is_same<types::sort::sort_heap<std::tuple<int>, size_less>::type, std::tuple<int>>::value, "sort_heap single");
}

namespace test_sort_consistency {
using Unsorted = std::tuple<double, bool, unsigned short, short, void*, unsigned char, char, unsigned int, long, unsigned long, long long, unsigned long long, long double, int, float>;

using Insert = types::sort::sort_insert<Unsorted, size_less>::type;
using Bubble = types::sort::sort_bubble<Unsorted, size_less>::type;
using Count = types::sort::sort_count<Unsorted, size_less>::type;
using Group = types::sort::sort_group<Unsorted, size_less>::type;
using Merge = types::sort::sort_merge<Unsorted, size_less>::type;
using Partition = types::sort::sort_partition<Unsorted, size_less>::type;
using Heap = types::sort::sort_heap<Unsorted, size_less>::type;

static_assert(types::sort::is_sorted<Insert, size_less>::value, "insert sorted");
static_assert(types::sort::is_sorted<Bubble, size_less>::value, "bubble sorted");
static_assert(types::sort::is_sorted<Count, size_less>::value, "count sorted");
static_assert(types::sort::is_sorted<Group, size_less>::value, "group sorted");
static_assert(types::sort::is_sorted<Merge, size_less>::value, "merge sorted");
static_assert(types::sort::is_sorted<Partition, size_less>::value, "partition sorted");
static_assert(types::sort::is_sorted<Heap, size_less>::value, "heap sorted");

static_assert(std::is_same<Insert, Bubble>::value, "insert == bubble");
static_assert(std::is_same<Bubble, Group>::value, "bubble == group");
static_assert(std::is_same<Group, Merge>::value, "group == merge");
}

int main() {
    return 0;
}