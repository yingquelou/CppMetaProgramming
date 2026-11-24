/**
 * \file types.hpp
 * \brief 类型集合运算
 * \author yingquelou (2695193723@qq.com)
 */
#pragma once
#ifndef TYPES_HPP
/// TYPES_HPP 头文件保护宏
#define TYPES_HPP 1
#include "compiler_detect.h"
#include <tuple>
#include <type_traits>
#include <utility>
/**
  \brief types 命名空间约定：
  - `UnaryTypeTrait`: 约定对应的类符合 C++ UnaryTypeTrait
  - `type`: 约定对应的类存在公有类型成员 `type`, 作为结果
  本命名空间提供类型集合（`std::tuple`）相关的编译期操作：
  基本运算（`in`、`concat`、`transfer`）、集合运算（`types`、`n`、`x`、`u`）、
  变换（`map`、`filter`、`reversal`）以及在 C++11/14 下缺失的标准库等价实现。
 */
namespace types {
#if HAS_CXX17
/// 直接使用标准库中的 `std::conjunction`
using std::conjunction;
/// 直接使用标准库中的 `std::disjunction`
using std::disjunction;
#else
/// \internal
namespace __impl_logical {
namespace _1 {
template <typename, typename _B1, typename... _Bn> struct __disjunction {
  using type = _B1;
};
template <typename _B1, typename _B2, typename... _Bn>
struct __disjunction<typename std::enable_if<!_B1::value>::type, _B1, _B2,
                     _Bn...> {
  using type = typename __disjunction<void, _B2, _Bn...>::type;
};
template <typename, typename _B1, typename... _Bn> struct __conjunction {
  using type = _B1;
};
template <typename _B1, typename _B2, typename... _Bn>
struct __conjunction<typename std::enable_if<_B1::value>::type, _B1, _B2,
                     _Bn...> {
  using type = typename __conjunction<void, _B2, _Bn...>::type;
};
/// `UnaryTypeTrait`: 析取/或
template <typename... _Bn>
struct disjunction : __disjunction<void, _Bn...>::type {};
template <> struct disjunction<> : std::false_type {};
/// `UnaryTypeTrait`: 合取/与
template <typename... _Bn>
struct conjunction : __conjunction<void, _Bn...>::type {};
template <> struct conjunction<> : std::true_type {};
} // namespace _1
namespace _2 {
/// `UnaryTypeTrait`: 析取/或
template <typename... _Bn> struct disjunction : std::false_type {};
template <typename _B> struct disjunction<_B> : _B {};
template <typename _B1, typename... _Bn>
struct disjunction<_B1, _Bn...>
    : std::conditional<_B1::value, _B1, disjunction<_Bn...>>::type {};
/// `UnaryTypeTrait`: 合取/与
template <typename... _Bn> struct conjunction : std::true_type {};
template <typename _B> struct conjunction<_B> : _B {};
template <typename _B1, typename... _Bn>
struct conjunction<_B1, _Bn...>
    : std::conditional<_B1::value, conjunction<_Bn...>, _B1>::type {};
} // namespace _2
using _1::conjunction;
using _1::disjunction;
// using _2::conjunction;
// using _2::disjunction;
} // namespace __impl_logical
/// `UnaryTypeTrait`: 合取（与），所有条件均为真时 `value` 为真
using __impl_logical::conjunction;
/// `UnaryTypeTrait`: 析取（或），任一条件为真时 `value` 为真
using __impl_logical::disjunction;
#endif
namespace __impl_transfer {
/**
  `type`: 转发模板参数到另一个模板
  - 例如: 欲将 `std::variant<...>` 转为 `std::tuple<...>`,
    则 `using type = typename transfer<std::tuple, std::variant<...>>::type`
  - 支持任意具有 `Ts...` 参数包的容器模板
 */
template <template <typename...> class Container, typename Source>
struct transfer;
template <template <typename...> class Container1,
          template <typename...> class Container2, typename... Ts>
struct transfer<Container1, Container2<Ts...>> {
  using type = Container1<Ts...>;
};
} // namespace __impl_transfer
/// 转发 `Source` 模板的参数包到 `Container` 模板
using __impl_transfer::transfer;
#if HAS_CXX14
/// 直接使用标准库中的 `std::index_sequence`
using std::index_sequence;
/// 直接使用标准库中的 `std::index_sequence_for`
using std::index_sequence_for;
/// 直接使用标准库中的 `std::integer_sequence`
using std::integer_sequence;
/// 直接使用标准库中的 `std::make_index_sequence`
using std::make_index_sequence;
/// 直接使用标准库中的 `std::make_integer_sequence`
using std::make_integer_sequence;
#else
/// C++14标准前兼容的编译时的整数序列
template <typename T, T... Ints> struct integer_sequence {
  /// 值类型
  using value_type = T;
  static_assert(std::is_integral<T>::value, "T must be an integral type");
  /// 序列大小
  static constexpr std::size_t size() noexcept { return sizeof...(Ints); }
};
namespace __impl_integer_sequence {
namespace __1 {
template <typename T, T N> class __make_integer_sequence {
  template <T CurInt, T... Ints> struct helper {
    using type = typename helper<CurInt - 1, CurInt - 1, Ints...>::type;
  };
  template <T... Ints> struct helper<0, Ints...> {
    using type = integer_sequence<T, Ints...>;
  };

public:
  using type = typename helper<N>::type;
};
} // namespace __1
template <typename, typename> struct __sequence_concat;
template <typename T, T... Is1, T... Is2>
struct __sequence_concat<integer_sequence<T, Is1...>,
                         integer_sequence<T, Is2...>> {
  using type = integer_sequence<T, Is1..., (Is2 + sizeof...(Is1))...>;
};
/// \cond
namespace __3 {
/// 整数序列生成器实现（基于二分递归）
/// 主模板 - 默认不定义，强制使用特化
template <typename T, T N, typename Enable = void>
struct __make_integer_sequence;

/// 特化：N = 0
template <typename T, T N>
struct __make_integer_sequence<T, N, typename std::enable_if<N == 0>::type> {
  using type = integer_sequence<T>;
};

/// 特化：N = 1
template <typename T, T N>
struct __make_integer_sequence<T, N, typename std::enable_if<N == 1>::type> {
  using type = integer_sequence<T, 0>;
};

/// 特化：N >= 2
template <typename T, T N>
struct __make_integer_sequence<T, N, typename std::enable_if<(N >= 2)>::type> {
private:
  static constexpr T half = N / 2;

  using left = typename __make_integer_sequence<T, half>::type;
  using right_base = typename __make_integer_sequence<T, N - half>::type;

public:
  using type = typename __sequence_concat<left, right_base>::type;
};
} // namespace __3
/// \endcond
#if defined __has_builtin && __has_builtin(__make_integer_seq)
template <typename T, T N> struct __make_integer_sequence {
  using type = __make_integer_seq<integer_sequence, T, N>;
};
#else
using __3::__make_integer_sequence;
#endif
} // namespace __impl_integer_sequence
/// 创建`integer_sequence<T, 0, 1, 2, ..., N-1>`的便捷别名
template <typename T, T N>
using make_integer_sequence =
    typename __impl_integer_sequence::__make_integer_sequence<T, N>::type;
/// `integer_sequence`的`size_t`特化版本
template <std::size_t... Ints>
using index_sequence = integer_sequence<std::size_t, Ints...>;
/// 创建`index_sequence<0, 1, 2, ..., N-1>`的便捷别名
template <std::size_t N>
using make_index_sequence = make_integer_sequence<std::size_t, N>;
/// 根据参数包长度创建对应的`index_sequence`
template <typename... Ts>
using index_sequence_for = make_index_sequence<sizeof...(Ts)>;
#endif
} // namespace types
namespace types {
/// 类型包装器，将`T`包装为具有公有成员`type`的结构体
template <typename T> struct as_type {
  /// 被包装的原始类型`T`
  using type = T;
};
/// `type`: 集合反转
template <typename Tuple> class reversal {
  template <typename CurTuple, typename> struct helper {
    using type = CurTuple;
  };
  template <typename... Ts1, typename T2, typename... Ts2>
  struct helper<std::tuple<Ts1...>, std::tuple<T2, Ts2...>> {
    using type =
        typename helper<std::tuple<T2, Ts1...>, std::tuple<Ts2...>>::type;
  };

public:
  /// 反转后的元组类型
  using type = typename helper<std::tuple<>, Tuple>::type;
};
/// `type`: 将多个容器元拼接为一个容器，不限于 `std::tuple`
/// @see types
namespace __impl_concat {
template <typename... Tuples> struct concat {
  /// 拼接后的容器类型
  using type = decltype(std::tuple_cat(
      typename transfer<std::tuple, Tuples>::type{}...));
};
} // namespace __impl_concat
/// 将多个同构容器元拼接成一个容器
using __impl_concat::concat;
/**
 * @brief 集合映射
 * @tparam Mapper 模板参数,其实例满足约定`type`
 * @li @c T 元素
 * @li @c Index 对应元素索引
 * @li @c Tuple 原集合
 * @tparam Tuple 类型参数,原集合
 */
template <template <typename /* T */, std::size_t /* Index */,
                    typename /* Tuple */> class /* type */ Mapper,
          typename Tuple>
class map {
  template <typename, typename> struct helper;
  template <typename... Ts, std::size_t... Is>
  struct helper<index_sequence<Is...>, std::tuple<Ts...>> {
    using type = std::tuple<typename Mapper<Ts, Is, Tuple>::type...>;
  };

public:
  /// 映射后的元组类型
  using type =
      typename helper<make_index_sequence<std::tuple_size<Tuple>::value>,
                      Tuple>::type;
};
/**
 * 过滤
 * - Predicate 实例是一个`UnaryTypeTrait`
 * - Tuple 需要过滤的类型集
 */
template <template <typename /** T */, std::size_t /* Index */,
                    typename /* Tuple */> class /* value */ Predicate,
          typename Tuple>
class filter {
  template <typename T, std::size_t Index, typename CurTuple>
  using pair = std::conditional<Predicate<T, Index, CurTuple>::value,
                                std::pair<std::tuple<T>, std::tuple<>>,
                                std::pair<std::tuple<>, std::tuple<T>>>;
  using pairs = typename map<pair, Tuple>::type;
  template <typename CurTuple> struct splitter;
  template <typename... Ts> struct splitter<std::tuple<Ts...>> {
    using type = std::pair<std::tuple<typename Ts::first_type...>,
                           std::tuple<typename Ts::second_type...>>;
  };
  using splitted = typename splitter<pairs>::type;

public:
  /**
   * - 对`Predicate`求真者在`std::pair::first_type`中
   * - 对`Predicate`求假者在`std::pair::second_type`中
   * - 当真集与假集都需要时,不要分别使用`filter_include`、`filter_exclude`.
   */
  using type = std::pair<
      typename transfer<concat, typename splitted::first_type>::type::type,
      typename transfer<concat, typename splitted::second_type>::type::type>;
};
/// 参见`filter`
template <template <typename /* T */, std::size_t /* Index */,
                    typename /* Tuple */> class /* value */ Predicate,
          typename Tuple>
struct filter_include {
  /// 过滤后保留谓词为真的元组类型
  using type = typename filter<Predicate, Tuple>::type::first_type;
};
/// 参见`filter`
template <template <typename /* T */, std::size_t /* Index */,
                    typename /* Tuple */> class /* value */ Predicate,
          typename Tuple>
struct filter_exclude {
  /// 过滤后保留谓词为假的元组类型
  using type = typename filter<Predicate, Tuple>::type::second_type;
};
} // namespace types

/*!
  \brief 类型集合运算命名空间

  - 唯一性：用 `types` 创建的集合会去除重复元素
  - 确定性：相同输入产生相同输出
  - 无序性：集合内元素顺序不保证
 */
namespace types {
namespace __impl_in {
/// `UnaryTypeTrait`: 检查元素 `T` 是否属于集合 `Tuple`
template <typename T, typename Tuple> struct in;
template <typename T, typename... Ts>
struct in<T, std::tuple<Ts...>> : disjunction<std::is_same<T, Ts>...> {};
} // namespace __impl_in
/// 检查类型 `T` 是否在元组 `Tuple` 中
using __impl_in::in;
namespace __impl_types {
template <typename Tuple, typename> struct __uniqueness {
  using type = Tuple;
};
template <typename... Ts1, typename T2, typename... Ts2>
struct __uniqueness<std::tuple<Ts1...>, std::tuple<T2, Ts2...>> {
  using type = typename __uniqueness<
      typename std::conditional<in<T2, std::tuple<Ts1...>>::value,
                                std::tuple<Ts1...>,
                                std::tuple<Ts1..., T2>>::type,
      std::tuple<Ts2...>>::type;
};
} // namespace __impl_types
/// 对 `Tuple` 去除重复元素得到的唯一类型集合
template <typename Tuple>
using types = __impl_types::__uniqueness<std::tuple<>, Tuple>;
namespace __impl_n {
/// 集合交集
template <typename Tuple1, typename Tuple2> class n {
  template <typename T, std::size_t, typename> using helper = in<T, Tuple2>;

public:
  /// `Tuple1` 与 `Tuple2` 的交集（已去重）
  using type =
      typename types<typename filter_include<helper, Tuple1>::type>::type;
};
} // namespace __impl_n
/// 集合交集
using __impl_n::n;
namespace __impl_x {
/// 集合差集
template <typename Tuple1, typename Tuple2> class x {
  template <typename T, std::size_t, typename> using helper = in<T, Tuple2>;

public:
  /// `Tuple1` 与 `Tuple2` 的差集（已去重）
  using type =
      typename types<typename filter_exclude<helper, Tuple1>::type>::type;
};
} // namespace __impl_x
/// 集合差集
using __impl_x::x;
namespace __impl_u {
/// 集合并集
template <typename Tuple1, typename Tuple2> struct u {
  /// `Tuple1` 与 `Tuple2` 的并集（已去重）
  using type = typename types<typename concat<Tuple1, Tuple2>::type>::type;
};
} // namespace __impl_u
/// 集合并集
using __impl_u::u;

} // namespace types
#endif
