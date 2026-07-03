#include "sort.hpp"
#include <iostream>
#include <type_traits>

#ifdef _MSC_VER
template <typename T> void print(T &&) {
  std::cout << typeid(T).name() << std::endl;
}
#else
void print(std::tuple<>) { std::cout << std::endl; }
template <typename T, typename... Ts> void print(std::tuple<T, Ts...> &&) {
  std::cout << sizeof(T) << ':' << typeid(T).name() << '\t';
  print(std::tuple<Ts...>{});
}
#endif
// 检查所有排序结果是否相同
template <typename Tuple, typename... Tuples>
using type_equal = types::conjunction<std::is_same<Tuple, Tuples>...>;
template <typename L, typename R>
using size_less = std::integral_constant<bool, sizeof(L) < sizeof(R)>;
namespace t_sort = types::sort;
// 独立验证每个排序结果都已按 Comparator 有序
template <typename... Tuples>
using all_sorted = types::conjunction<t_sort::is_sorted<Tuples, size_less>...>;
int main(int argc, char const *argv[]) {
  using A = std::tuple<double, bool, unsigned short, short, void *,
                       unsigned char, char, unsigned int, long, unsigned long,
                       long long, unsigned long long, long double, int, float>;
  print(A{});
  // 测试:排序
  using count = typename t_sort::sort_count<A, size_less>::type;
  static_assert(t_sort::is_sorted<count, size_less>::value,
                "counting_sort failed");
  print(count{});
  using heap = typename t_sort::sort_heap<A, size_less>::type;
  static_assert(t_sort::is_sorted<heap, size_less>::value, "sort_heap failed");
  print(heap{});
  using insert = typename t_sort::sort_insert<A, size_less>::type;
  static_assert(t_sort::is_sorted<insert, size_less>::value,
                "insert_sort failed");
  print(insert{});
  using bubble = typename t_sort::sort_bubble<A, size_less>::type;
  static_assert(t_sort::is_sorted<bubble, size_less>::value,
                "bubble_sort failed");
  print(bubble{});
  using group = typename t_sort::sort_group<A, size_less>::type;
  static_assert(t_sort::is_sorted<group, size_less>::value,
                "iterative_sort failed");
  print(group{});
  using merge = typename t_sort::sort_merge<A, size_less>::type;
  static_assert(t_sort::is_sorted<merge, size_less>::value,
                "iterative_sort failed");
  print(merge{});
  // 每种算法均产生按 Comparator 有序的结果
  static_assert(all_sorted<count, heap, insert, bubble, group, merge>::value,
                "one or more sort algorithms produced unsorted output");
  // 插入/冒泡/分组/归并 对相同输入产生相同的稳定排序结果
  static_assert(type_equal<insert, bubble, group, merge>::value,
                "stable sort algorithms produced different output");
  return 0;
}
