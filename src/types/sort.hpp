/**
 * \file sort.hpp
 * \brief 类型集合元素排序
 * \author yingquelou (2695193723@qq.com)
 */
#ifndef HYBRID_TYPE_SORT_HPP
#define HYBRID_TYPE_SORT_HPP 1
#include "types.hpp"
namespace types {
/*!
  \brief 类型集合排序与判定命名空间

  提供多种基于 `std::tuple` 的编译期排序算法（插入、冒泡、计数、分组、
  归并、堆排序、分区快速排序）、相邻重复元素去除（`unique`）以及有序性判定
  （`is_sorted`）。所有算法均接受一个比较器模板作为排序准则。
 */
namespace sort {
/*!
 * \brief 关系适配器：将仅提供 `<`（或 `<=`）语义的比较器扩展为完整的
 * 五序关系（`equal`、`less`、`less_equal`、`greater`、`greater_equal`）。
 *
 * 给定一个二元谓词 `Comparator<L, R>`，其语义为 "L < R"（或 "L <= R"），
 * 本适配器通过反转比较器及异或非等规则推导出所有五种关系。
 *
 * \tparam Comparator 一个模板模板参数，接受两个类型参数，返回一个可转为
 *                    `bool` 的类型（通常是 `std::integral_constant<bool,
 * ...>`）。
 */
template <template <typename, typename> class Comparator>
class ComparatorAdapter {
  template <typename L, typename R> using _less = Comparator<L, R>;
  template <typename L, typename R> using _greater = Comparator<R, L>;

public:
  /// 相等：`!(L < R) && !(R < L)`
  template <typename L, typename R>
  using equal =
      std::integral_constant<bool, !(static_cast<bool>(_less<L, R>::value) ^
                                     static_cast<bool>(_greater<L, R>::value))>;
  /// 小于：`(L < R) && !(L == R)`
  template <typename L, typename R>
  using less =
      std::integral_constant<bool, _less<L, R>::value && !equal<L, R>::value>;
  /// 小于等于：`(L < R) || (L == R)`
  template <typename L, typename R>
  using less_equal =
      std::integral_constant<bool, _less<L, R>::value || equal<L, R>::value>;
  /// 大于：`(R < L) && !(L == R)`
  template <typename L, typename R>
  using greater = std::integral_constant<bool, _greater<L, R>::value &&
                                                   !equal<L, R>::value>;
  /// 大于等于：`(R < L) || (L == R)`
  template <typename L, typename R>
  using greater_equal =
      std::integral_constant<bool, _greater<L, R>::value || equal<L, R>::value>;
};

namespace __impl_unique {
/// 去除 `Tuple` 中相邻的重复元素（基于 `Comparator` 判等）
template <typename Tuple, template <typename, typename> class Comparator>
class unique {
  using cmp = ComparatorAdapter<Comparator>;
  template <typename CurTuple> class helper {
  public:
    using type = CurTuple;
  };
  template <typename T, typename... Ts> class helper<std::tuple<T, Ts...>> {
    template <typename _T, std::size_t, typename>
    using equal = typename cmp::template equal<_T, T>;
    using splited = typename filter<equal, std::tuple<Ts...>>::type;
    using equal_tuples =
        typename concat<std::tuple<T>, typename splited::first_type>::type;

  public:
    using type = typename concat<
        std::tuple<equal_tuples>,
        typename helper<typename splited::second_type>::type>::type;
  };

public:
  /// 去重后的元组类型
  using type = typename helper<Tuple>::type;
};
} // namespace __impl_unique
/// 基于 `Comparator` 判等去除 `Tuple` 中相邻重复元素
using __impl_unique::unique;

// 以不同方式实现了排序

namespace __impl_insert {
/// \brief 插入排序实现
///
/// 对 `Tuple` 进行插入排序：每次取出首元素，使用 `Comparator` 的 `less_equal`
/// 将其插入到已排序尾部的合适位置。
///
/// \tparam Tuple      待排序的 `std::tuple<...>` 类型
/// \tparam Comparator 比较器模板，提供 `<` 或 `<=` 语义
template <typename Tuple, template <typename, typename> class Comparator>
class sort_insert {
  template <typename CurTuple> struct helper {
    using type = CurTuple;
  };
  template <typename T> struct helper<std::tuple<T>> {
    using type = std::tuple<T>;
  };
  template <typename L, typename R>
  using cmp = typename ComparatorAdapter<Comparator>::template less_equal<L, R>;
  template <typename T, typename... Ts> struct helper<std::tuple<T, Ts...>> {
  private:
    // 插入单个元素到已排序的元组中
    template <typename Elem, typename SortedTuple> struct inserter;
    template <typename Elem> struct inserter<Elem, std::tuple<>> {
      using type = std::tuple<Elem>;
    };
    template <typename Elem, typename First, typename... Rest>
    struct inserter<Elem, std::tuple<First, Rest...>> {
      using type = typename std::conditional<
          cmp<Elem, First>::value, std::tuple<Elem, First, Rest...>,
          decltype(std::tuple_cat(
              std::tuple<First>{},
              typename inserter<Elem, std::tuple<Rest...>>::type{}))>::type;
    };
    using rest_sorted = typename helper<std::tuple<Ts...>>::type;

  public:
    using type = typename inserter<T, rest_sorted>::type;
  };

public:
  /// 插入排序的结果元组类型
  using type = typename helper<Tuple>::type;
};
} // namespace __impl_insert
/// 插入排序：将 `Tuple` 按 `Comparator` 顺序排序
/// @tparam Tuple      待排序的 `std::tuple<...>`
/// @tparam Comparator 比较器（提供 `<` 或 `<=` 语义）
using __impl_insert::sort_insert;
/// \cond
namespace __impl_bubble {
/// \brief 冒泡排序实现
///
/// 通过 `Pass` 次遍历元组，每次比较相邻元素并按 `less_equal` 交换，
/// 最终得到按 `Comparator` 排序的元组类型。
template <typename Tuple, template <typename, typename> class Comparator>
struct sort_bubble {
private:
  template <typename L, typename R>
  using cmp = typename ComparatorAdapter<Comparator>::template less_equal<L, R>;
  // 单次冒泡
  template <std::size_t, typename CurrentTuple, typename = void>
  struct bubble_pass {
    typedef CurrentTuple type;
  };
  /// 单次冒泡（递归步骤，Pass > 1）
  template <std::size_t Pass, typename First, typename Second, typename... Rest>
  struct bubble_pass<Pass, std::tuple<First, Second, Rest...>,
                     typename std::enable_if<(Pass > 1)>::type> {
  private:
    using after_compare =
        typename std::conditional<cmp<First, Second>::value,
                                  std::pair<First, Second>,
                                  std::pair<Second, First>>::type;
    using rest_sorted = typename bubble_pass<
        Pass - 1,
        std::tuple<typename after_compare::second_type, Rest...>>::type;

  public:
    using type = decltype(std::tuple_cat(
        std::tuple<typename after_compare::first_type>{}, rest_sorted{}));
  };
  // 执行固定次数的冒泡排序
  template <std::size_t RemainingPasses, typename CurrentTuple>
  struct bubble_sort_impl {
  private:
    using after_pass =
        typename bubble_pass<RemainingPasses, CurrentTuple>::type;

  public:
    using type =
        typename bubble_sort_impl<RemainingPasses - 1, after_pass>::type;
  };
  template <typename CurrentTuple> struct bubble_sort_impl<0, CurrentTuple> {
    typedef CurrentTuple type;
  };

public:
  /// 冒泡排序的结果元组类型
  using type =
      typename bubble_sort_impl<std::tuple_size<Tuple>::value, Tuple>::type;
};
} // namespace __impl_bubble
/// \endcond
/// 冒泡排序：将 `Tuple` 按 `Comparator` 顺序排序
/// @tparam Tuple      待排序的 `std::tuple<...>`
/// @tparam Comparator 比较器（提供 `<` 或 `<=` 语义）
using __impl_bubble::sort_bubble;
namespace __impl_counting {
/// \brief 计数排序实现
///
/// 对 `Tuple` 中每个元素统计"小于它的元素个数"作为其排位，
/// 再按排位依次收集元素得到有序元组。稳定性取决于 `filter` 的实现。
template <typename Tuple, template <typename, typename> class Comparator>
struct sort_count {
private:
  // 元素:元素计数
  template <typename T, std::size_t Ct>
  struct entry : private std::integral_constant<std::size_t, Ct> {
    // 元素
    using type = T;
    // 元素计数
    using std::integral_constant<std::size_t, Ct>::value;
  };
  template <typename T, std::size_t Index, typename CurTuple>
  class count_smaller {
    template <typename _T, std::size_t _Index, typename>
    using is_true = std::integral_constant<bool, Index != _Index &&
                                                     Comparator<_T, T>::value>;
    using trues = typename filter_include<is_true, CurTuple>::type;

  public:
    using type = entry<T, std::tuple_size<trues>::value>;
  };
  // 递归构建最终元组
  template <std::size_t Rank, typename CurEntries> class build {
    template <typename T, std::size_t, typename>
    using is_equal = std::integral_constant<bool, (Rank == T::value)>;
    // 分离具有指定计数的元素
    using splited = typename filter<is_equal, CurEntries>::type;
    template <typename T, std::size_t, typename>
    using extract_type = as_type<typename T::type>;
    using first_type =
        typename map<extract_type, typename splited::first_type>::type;

  public:
    using type = typename concat<
        first_type,
        typename build<Rank + 1, typename splited::second_type>::type>::type;
  };
  template <std::size_t Rank> class build<Rank, std::tuple<>> {
  public:
    using type = std::tuple<>;
  };

public:
  /// 计数排序的结果元组类型
  using type =
      typename build<0, typename map<count_smaller, Tuple>::type>::type;
};
} // namespace __impl_counting
/// 计数排序（排位排序）：将 `Tuple` 按 `Comparator` 顺序排序
/// @tparam Tuple      待排序的 `std::tuple<...>`
/// @tparam Comparator 比较器（提供 `<` 或 `<=` 语义）
using __impl_counting::sort_count;
namespace __impl_group {
/// \brief 分组（选择）排序实现
///
/// 每次从待排序组中选出最小元素，与剩余元素分离，再递归对剩余部分排序。
/// 使用 `unique` 预先去除重复元素以简化实现。
template <typename Tuple, template <typename, typename> class Comparator>
struct sort_group {
private:
  template <typename CurTuple, typename OtherTuple> class find_min_type;
  template <typename First, typename Second, typename... Ts1, typename... Ts2>
  class find_min_type<std::tuple<First, Second, Ts1...>, std::tuple<Ts2...>> {
    using cmp = Comparator<typename std::tuple_element<0, First>::type,
                           typename std::tuple_element<0, Second>::type>;
    using cmp_result =
        typename std::conditional<cmp::value, std::pair<First, Second>,
                                  std::pair<Second, First>>::type;

  public:
    using type = typename find_min_type<
        std::tuple<typename cmp_result::first_type, Ts1...>,
        std::tuple<Ts2..., typename cmp_result::second_type>>::type;
  };
  template <typename T, typename OtherTuple>
  class find_min_type<std::tuple<T>, OtherTuple> {
  public:
    using type = std::pair<T, OtherTuple>;
  };
  // 构建排序后的元组类型
  template <typename CurTuple,
            bool Empty = (std::tuple_size<CurTuple>::value == 0)>
  class build;
  template <typename CurTuple> class build<CurTuple, true> {
  public:
    using type = std::tuple<>;
  };
  template <typename CurTuple> class build<CurTuple, false> {
    using splited = typename find_min_type<CurTuple, std::tuple<>>::type;

  public:
    using type = typename concat<
        typename splited::first_type,
        typename build<typename splited::second_type>::type>::type;
  };

public:
  /// 分组排序的结果元组类型
  using type = typename build<
      typename __impl_unique::unique<Tuple, Comparator>::type>::type;
};
} // namespace __impl_group
/// 分组（选择）排序：每次选最小元素递归排序
/// @tparam Tuple      待排序的 `std::tuple<...>`
/// @tparam Comparator 比较器（提供 `<` 或 `<=` 语义）
using __impl_group::sort_group;
namespace __impl_merge {
/// 将类型列表交替拆分为两半
template <typename List> struct split;
template <> struct split<std::tuple<>> {
  using first = std::tuple<>;
  using second = std::tuple<>;
};
template <typename T> struct split<std::tuple<T>> {
  using first = std::tuple<T>;
  using second = std::tuple<>;
};
template <typename T1, typename T2, typename... Ts>
struct split<std::tuple<T1, T2, Ts...>> {
private:
  using RestSplit = split<std::tuple<Ts...>>;

public:
  using first =
      typename concat<std::tuple<T1>, typename RestSplit::second>::type;
  using second =
      typename concat<std::tuple<T2>, typename RestSplit::first>::type;
};
template <typename List1, typename List2,
          template <typename, typename> class Comparator>
class list_merge {
  template <typename L1, typename L2, typename = void> struct helper;
  template <typename Dummy> struct helper<std::tuple<>, std::tuple<>, Dummy> {
    using type = std::tuple<>;
  };
  template <typename... Ts, typename Dummy>
  struct helper<std::tuple<Ts...>, std::tuple<>, Dummy> {
    using type = std::tuple<Ts...>;
  };
  template <typename... Ts, typename Dummy>
  struct helper<std::tuple<>, std::tuple<Ts...>, Dummy> {
    using type = std::tuple<Ts...>;
  };
  template <typename Head1, typename... Tail1, typename Head2,
            typename... Tail2, typename Dummy>
  struct helper<std::tuple<Head1, Tail1...>, std::tuple<Head2, Tail2...>, Dummy> {
    using type = typename std::conditional<
        Comparator<Head1, Head2>::value,
        typename concat<
            std::tuple<Head1>,
            typename helper<std::tuple<Tail1...>,
                            std::tuple<Head2, Tail2...>>::type>::type,
        typename concat<std::tuple<Head2>,
                        typename helper<std::tuple<Head1, Tail1...>,
                                        std::tuple<Tail2...>>::type>::type>::
        type;
  };

public:
  using type = typename helper<List1, List2>::type;
};
/// \brief 归并排序实现
///
/// 通过 `split` 将 `Tuple` 交替拆分为两半，分别递归排序后再用 `list_merge`
/// 归并。
template <typename Tuple, template <typename, typename> class Comparator>
class sort_merge {
  template <typename L, typename R>
  using cmp = Comparator<typename std::tuple_element<0, L>::type,
                         typename std::tuple_element<0, R>::type>;
  template <typename List, typename = void> struct helper;
  template <typename Dummy> struct helper<std::tuple<>, Dummy> {
    using type = std::tuple<>;
  };
  template <typename T, typename Dummy> struct helper<std::tuple<T>, Dummy> {
    using type = std::tuple<T>;
  };
  template <typename... Ts, typename Dummy> struct helper<std::tuple<Ts...>, Dummy> {
  private:
    using SplitLists = split<std::tuple<Ts...>>;
    using SortedFirst = typename helper<typename SplitLists::first>::type;
    using SortedSecond = typename helper<typename SplitLists::second>::type;

  public:
    using type = typename list_merge<SortedFirst, SortedSecond, cmp>::type;
  };

public:
  /// 归并排序的结果元组类型
  using type =
      typename transfer<concat,
                        typename helper<typename __impl_unique::unique<
                            Tuple, Comparator>::type>::type>::type::type;
};
} // namespace __impl_merge
/// 归并排序：将 `Tuple` 按 `Comparator` 顺序排序
/// @tparam Tuple      待排序的 `std::tuple<...>`
/// @tparam Comparator 比较器（提供 `<` 或 `<=` 语义）
using __impl_merge::sort_merge;
namespace __impl_is_sorted {
/*!
 * \brief 判断元组是否已按 `Comparator` 有序
 *
 * 递归检查每对相邻元素是否满足 `Comparator`（即 `L <= R`）。
 * 若所有相邻元素均满足，则 `value` 为 `true`，否则为 `false`。
 *
 * \tparam Tuple      待检查的元组类型
 * \tparam Comparator 比较器（提供 `<` 或 `<=` 语义）
 */
template <typename Tuple,
          template <typename, typename> class /* value */ Comparator>
class is_sorted {
  template <typename L, typename R>
  using cmp = typename ComparatorAdapter<Comparator>::template less_equal<L, R>;
  template <typename CurTuple> struct helper;
  template <typename First, typename Second>
  struct helper<std::tuple<First, Second>> : cmp<First, Second> {};
  template <typename First, typename Second, typename... Ts>
  struct helper<std::tuple<First, Second, Ts...>>
      : conjunction<cmp<First, Second>, helper<std::tuple<Second, Ts...>>> {};

public:
  static const bool value = helper<Tuple>::value;
};
} // namespace __impl_is_sorted
/// 判断元组是否已按 `Comparator` 有序
using __impl_is_sorted::is_sorted;
namespace __impl_partition {
/// \brief 分区（快速）排序实现
///
/// 以首元素为枢轴（pivot），将剩余元素按 `Comparator::less` 划分为
/// 小于枢轴和大于等于枢轴两部分，再递归对两部分排序，最终拼接为有序元组。
/// 注：此实现并非堆排序，而是一种分区式（快速）排序。
template <typename Tuple, template <typename, typename> class Comparator>
class sort_partition {
  template <typename L, typename R>
  using cmp = typename ComparatorAdapter<Comparator>::template less<L, R>;
  template <typename CurTuple> class helper {
  public:
    using type = CurTuple;
  };
  template <typename T, typename... Ts> class helper<std::tuple<T, Ts...>> {
    template <typename _T, std::size_t, typename> using is_small = cmp<_T, T>;
    using splited = typename filter<is_small, std::tuple<Ts...>>::type;

  public:
    using type = typename concat<
        typename helper<typename splited::first_type>::type, std::tuple<T>,
        typename helper<typename splited::second_type>::type>::type;
  };

public:
  /// 分区排序的结果元组类型
  using type = typename helper<Tuple>::type;
};
} // namespace __impl_partition
/// 分区（快速）排序：以首元素为枢轴进行分区递归排序
/// @tparam Tuple      待排序的 `std::tuple<...>`
/// @tparam Comparator 比较器（提供 `<` 语义）
using __impl_partition::sort_partition;
namespace __impl_heap {
/// \cond
namespace _detail {

/// 交换 `Tuple` 中索引 `I` 与 `J` 处元素的类型
template <typename Tuple, std::size_t I, std::size_t J> struct swap_indices {
private:
  static constexpr std::size_t N = std::tuple_size<Tuple>::value;
  template <std::size_t... Is>
  static std::tuple<typename std::tuple_element<(Is == I)   ? J
                                                : (Is == J) ? I
                                                            : Is,
                                                Tuple>::type...>
      _build(index_sequence<Is...>);

public:
  using type = decltype(_build(make_index_sequence<N>{}));
};

/// 对 `Tuple` 从 `Root` 起执行 sift-down（大顶堆语义）
///
/// - `Size` 为堆的有效大小，限定 sift-down 的递归范围。
/// - 三种情形通过 SFINAE 区分：无孩子 / 仅左孩子 / 两个孩子。
template <typename Tuple, std::size_t Root, std::size_t Size,
          template <typename, typename> class Comparator, typename = void>
struct sift_down;

/// 基准情形：`Root` 为叶节点（`2*Root + 1 >= Size`），堆性质已满足
template <typename Tuple, std::size_t Root, std::size_t Size,
          template <typename, typename> class Comparator>
struct sift_down<Tuple, Root, Size, Comparator,
                 typename std::enable_if<(2 * Root + 1 >= Size)>::type> {
  using type = Tuple;
};

/// 仅左孩子情形：`2*Root + 1 < Size` 且 `2*Root + 2 >= Size`
template <typename Tuple, std::size_t Root, std::size_t Size,
          template <typename, typename> class Comparator>
struct sift_down<Tuple, Root, Size, Comparator,
                 typename std::enable_if<(2 * Root + 1 < Size &&
                                          2 * Root + 2 >= Size)>::type> {
private:
  static constexpr std::size_t L = 2 * Root + 1;
  using root_t = typename std::tuple_element<Root, Tuple>::type;
  using left_t = typename std::tuple_element<L, Tuple>::type;
  using swapped = typename swap_indices<Tuple, Root, L>::type;
  using cmp = ComparatorAdapter<Comparator>;

public:
  using type = typename std::conditional<
      cmp::template less<root_t, left_t>::value,
      typename sift_down<swapped, L, Size, Comparator>::type, Tuple>::type;
};

/// 两个孩子情形：`2*Root + 2 < Size`
template <typename Tuple, std::size_t Root, std::size_t Size,
          template <typename, typename> class Comparator>
struct sift_down<Tuple, Root, Size, Comparator,
                 typename std::enable_if<(2 * Root + 2 < Size)>::type> {
private:
  static constexpr std::size_t L = 2 * Root + 1;
  static constexpr std::size_t R = 2 * Root + 2;
  using root_t = typename std::tuple_element<Root, Tuple>::type;
  using left_t = typename std::tuple_element<L, Tuple>::type;
  using right_t = typename std::tuple_element<R, Tuple>::type;
  using cmp = ComparatorAdapter<Comparator>;

  // 判定：哪个孩子更大、父节点是否需要下沉
  static constexpr bool left_larger =
      cmp::template less<right_t, left_t>::value;
  static constexpr bool root_smaller = cmp::template less<
      root_t,
      typename std::conditional<left_larger, left_t, right_t>::type>::value;

  // 与较大孩子交换后继续 sift-down
  using swapped_left = typename swap_indices<Tuple, Root, L>::type;
  using swapped_right = typename swap_indices<Tuple, Root, R>::type;
  using sifted_left =
      typename sift_down<swapped_left, L, Size, Comparator>::type;
  using sifted_right =
      typename sift_down<swapped_right, R, Size, Comparator>::type;

public:
  using type = typename std::conditional<
      root_smaller,
      typename std::conditional<left_larger, sifted_left, sifted_right>::type,
      Tuple>::type;
};

/// 建堆辅助：从索引 `Idx` 向上依次 sift-down 到根
template <typename Tuple, std::size_t Idx, std::size_t Size,
          template <typename, typename> class Comparator>
struct build_heap_helper;
template <typename Tuple, std::size_t Size,
          template <typename, typename> class Comparator>
struct build_heap_helper<Tuple, 0, Size, Comparator> {
  using type = typename sift_down<Tuple, 0, Size, Comparator>::type;
};
template <typename Tuple, std::size_t Idx, std::size_t Size,
          template <typename, typename> class Comparator>
struct build_heap_helper {
  using after_sift = typename sift_down<Tuple, Idx, Size, Comparator>::type;
  using type =
      typename build_heap_helper<after_sift, Idx - 1, Size, Comparator>::type;
};

/// 建堆：从最后一个内部节点 `(Size-2)/2` 向上依次 sift-down
template <typename Tuple, std::size_t Size,
          template <typename, typename> class Comparator>
struct build_heap {
private:
  static constexpr std::size_t LastInternal = (Size - 2) / 2;

public:
  using type =
      typename build_heap_helper<Tuple, LastInternal, Size, Comparator>::type;
};
template <typename Tuple, template <typename, typename> class Comparator>
struct build_heap<Tuple, 0, Comparator> {
  using type = Tuple;
};
template <typename Tuple, template <typename, typename> class Comparator>
struct build_heap<Tuple, 1, Comparator> {
  using type = Tuple;
};

/// 堆排序主体：依次交换堆顶与末尾、缩小堆、sift-down
template <typename Heapified, std::size_t Remaining,
          template <typename, typename> class Comparator>
struct heap_sort_impl {
private:
  static constexpr std::size_t Last = Remaining - 1;
  using swapped = typename swap_indices<Heapified, 0, Last>::type;
  using sifted =
      typename sift_down<swapped, 0, Remaining - 1, Comparator>::type;

public:
  using type = typename heap_sort_impl<sifted, Remaining - 1, Comparator>::type;
};
template <typename Tuple, template <typename, typename> class Comparator>
struct heap_sort_impl<Tuple, 1, Comparator> {
  using type = Tuple;
};
} // namespace _detail
/// \endcond

/// \brief 堆排序实现（大顶堆）
///
/// 两阶段流程：
/// 1. `build_heap` —— 自底向上对 `Tuple` 执行 sift-down，建为大顶堆；
/// 2. `heap_sort_impl` —— 依次交换堆顶与末尾，缩小堆后再 sift-down，
///    最终得到按 `Comparator` 升序排列的元组。
template <typename Tuple, template <typename, typename> class Comparator>
class sort_heap {
private:
  static constexpr std::size_t N = std::tuple_size<Tuple>::value;
  using heapified = typename _detail::build_heap<Tuple, N, Comparator>::type;

public:
  /// 堆排序的结果元组类型
  using type = typename _detail::heap_sort_impl<heapified, N, Comparator>::type;
};
} // namespace __impl_heap
/// 堆排序（大顶堆）：将 `Tuple` 按 `Comparator` 顺序排序
using __impl_heap::sort_heap;
} // namespace sort
} // namespace types
#endif
