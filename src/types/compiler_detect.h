/**
 * \file compiler_detect.h
 * \brief 编译器工具链环境检查
 * \author yingquelou (2695193723@qq.com)
 */
#ifndef COMPILER_DETECT_H
#define COMPILER_DETECT_H

/// 未知编译器
#define COMPILER_UNKNOWN 0
/// Clang 编译器
#define COMPILER_CLANG 1
/// GCC 编译器
#define COMPILER_GCC 2
/// MSVC 编译器
#define COMPILER_MSVC 3
/// Intel ICC 编译器
#define COMPILER_INTEL 4
/// MinGW 编译器
#define COMPILER_MINGW 5
/// Borland 编译器
#define COMPILER_BORLAND 6
/// Embarcadero CodeGear 编译器
#define COMPILER_CODEGEAR 7
/// Digital Mars 编译器
#define COMPILER_DMC 8
/// Tiny C 编译器
#define COMPILER_TINYC 9
/// Watcom 编译器
#define COMPILER_WATCOM 10

// 编译器版本号宏（主版本和次版本）
// 注意：这些宏在编译器检测后定义
// #define COMPILER_VERSION_MAJOR 0
// #define COMPILER_VERSION_MINOR 0

// 编译器检测
#if defined(__clang__)
/// 当前编译器类型（编译期检测）
#define COMPILER COMPILER_CLANG
/// 编译器主版本号
#define COMPILER_VERSION_MAJOR __clang_major__
/// 编译器次版本号
#define COMPILER_VERSION_MINOR __clang_minor__
/// 编译器修订号
#define COMPILER_VERSION_PATCH __clang_patchlevel__
#elif defined(__INTEL_COMPILER)
#define COMPILER COMPILER_INTEL
#define COMPILER_VERSION_MAJOR (__INTEL_COMPILER / 100)
#define COMPILER_VERSION_MINOR (__INTEL_COMPILER % 100)
#define COMPILER_VERSION_PATCH 0
#elif defined(__MINGW64__) || defined(__MINGW32__)
#define COMPILER COMPILER_MINGW
#define COMPILER_VERSION_MAJOR __GNUC__
#define COMPILER_VERSION_MINOR __GNUC_MINOR__
#define COMPILER_VERSION_PATCH __GNUC_PATCHLEVEL__
#elif defined(__GNUC__)
#define COMPILER COMPILER_GCC
#define COMPILER_VERSION_MAJOR __GNUC__
#define COMPILER_VERSION_MINOR __GNUC_MINOR__
#define COMPILER_VERSION_PATCH __GNUC_PATCHLEVEL__
#elif defined(_MSC_VER)
#define COMPILER COMPILER_MSVC
#define COMPILER_VERSION_MAJOR (_MSC_VER / 100)
#define COMPILER_VERSION_MINOR (_MSC_VER % 100)
#define COMPILER_VERSION_PATCH 0
#elif defined(__BORLANDC__)
#define COMPILER COMPILER_BORLAND
#define COMPILER_VERSION_MAJOR (__BORLANDC__ / 0x100)
#define COMPILER_VERSION_MINOR (__BORLANDC__ % 0x100)
#define COMPILER_VERSION_PATCH 0
#elif defined(__CODEGEARC__)
#define COMPILER COMPILER_CODEGEAR
#define COMPILER_VERSION_MAJOR (__CODEGEARC__ / 100)
#define COMPILER_VERSION_MINOR (__CODEGEARC__ % 100)
#define COMPILER_VERSION_PATCH 0
#elif defined(__DMC__)
#define COMPILER COMPILER_DMC
#define COMPILER_VERSION_MAJOR (__DMC__ / 0x100)
#define COMPILER_VERSION_MINOR (__DMC__ % 0x100)
#define COMPILER_VERSION_PATCH 0
#elif defined(__TINYC__)
#define COMPILER COMPILER_TINYC
#define COMPILER_VERSION_MAJOR __TINYC__
#define COMPILER_VERSION_MINOR 0
#define COMPILER_VERSION_PATCH 0
#elif defined(__WATCOMC__)
#define COMPILER COMPILER_WATCOM
#define COMPILER_VERSION_MAJOR (__WATCOMC__ / 100)
#define COMPILER_VERSION_MINOR (__WATCOMC__ % 100)
#define COMPILER_VERSION_PATCH 0
#else
/// 当前编译器类型（未知）
#define COMPILER COMPILER_UNKNOWN
/// 编译器主版本号（未知时为 0）
#define COMPILER_VERSION_MAJOR 0
/// 编译器次版本号（未知时为 0）
#define COMPILER_VERSION_MINOR 0
/// 编译器修订号（未知时为 0）
#define COMPILER_VERSION_PATCH 0
#endif

// 编译器名称字符串
#if COMPILER == COMPILER_CLANG
/// 编译器名称字符串
#define COMPILER_NAME "Clang"
#elif COMPILER == COMPILER_INTEL
#define COMPILER_NAME "Intel ICC"
#elif COMPILER == COMPILER_MINGW
#define COMPILER_NAME "MinGW GCC"
#elif COMPILER == COMPILER_GCC
#define COMPILER_NAME "GCC"
#elif COMPILER == COMPILER_MSVC
#define COMPILER_NAME "MSVC"
#elif COMPILER == COMPILER_BORLAND
#define COMPILER_NAME "Borland C++"
#elif COMPILER == COMPILER_CODEGEAR
#define COMPILER_NAME "Embarcadero C++"
#elif COMPILER == COMPILER_DMC
#define COMPILER_NAME "Digital Mars"
#elif COMPILER == COMPILER_TINYC
#define COMPILER_NAME "Tiny C Compiler"
#elif COMPILER == COMPILER_WATCOM
#define COMPILER_NAME "Watcom C"
#else
/// 编译器名称字符串（未知时为 "Unknown Compiler"）
#define COMPILER_NAME "Unknown Compiler"
#endif

/// 编译器版本字符串（格式：名称 主.次.修订）
#define COMPILER_VERSION_STRING COMPILER_NAME " " #COMPILER_VERSION_MAJOR "." #COMPILER_VERSION_MINOR "." #COMPILER_VERSION_PATCH

#ifdef __cplusplus
// C++ 标准版本检测
// __cplusplus 宏值示例：
/// C++98 标准版本
#define STD_CXX98 199711L
/// C++11 标准版本
#define STD_CXX11 201103L
/// C++14 标准版本
#define STD_CXX14 201402L
/// C++17 标准版本
#define STD_CXX17 201703L
/// C++20 标准版本
#define STD_CXX20 202002L
/// C++23 标准版本（部分编译器支持）
#define STD_CXX23 202300L
/// C++26 标准版本（预览）
#define STD_CXX26 202600L

// 针对 MSVC 的 C++ 标准支持宏（MSVC 不完全遵守 __cplusplus，需手动开启 /Zc:__cplusplus）
#if defined(__cplusplus)
  #if defined(_MSC_VER)
    #if defined(_MSVC_LANG)
      /// 当前启用的 C++ 标准版本（MSVC 下读取 _MSVC_LANG）
      #define CPP_STANDARD _MSVC_LANG
    #else
      /// 当前启用的 C++ 标准版本
      #define CPP_STANDARD __cplusplus
    #endif
  #else
    /// 当前启用的 C++ 标准版本
    #define CPP_STANDARD __cplusplus
  #endif
#else
  // 非 C++ 模式时，避免未定义行为
  /// 当前启用的 C++ 标准版本（非 C++ 模式下为 0）
  #define CPP_STANDARD 0L
#endif

/// 是否支持 C++98 标准
#define HAS_CXX98 (CPP_STANDARD >= STD_CXX98)
/// 是否支持 C++11 标准
#define HAS_CXX11 (CPP_STANDARD >= STD_CXX11)
/// 是否支持 C++14 标准
#define HAS_CXX14 (CPP_STANDARD >= STD_CXX14)
/// 是否支持 C++17 标准
#define HAS_CXX17 (CPP_STANDARD >= STD_CXX17)
/// 是否支持 C++20 标准
#define HAS_CXX20 (CPP_STANDARD >= STD_CXX20)
/// 是否支持 C++23 标准
#define HAS_CXX23 (CPP_STANDARD >= STD_CXX23)
/// 是否支持 C++26 标准
#define HAS_CXX26 (CPP_STANDARD >= STD_CXX26)

#else

// C 标准版本检测
// __STDC_VERSION__ 宏值示例：
/// C89 标准版本
#define STD_C89 199409L
/// C99 标准版本
#define STD_C99 199901L
/// C11 标准版本
#define STD_C11 201112L
/// C17 标准版本
#define STD_C17 201710L
/// C23 标准版本（部分编译器支持）
#define STD_C23 202300L

// C 标准支持检测宏
#if defined(__STDC_VERSION__)
  /// 当前启用的 C 标准版本
  #define C_STANDARD __STDC_VERSION__
#elif defined(_MSC_VER)
  // MSVC 仍对 C 标准支持有限，使用默认 C89
  #define C_STANDARD STD_C89
#else
  // 未定义 __STDC_VERSION__，保守认为最小兼容 C89
  /// 当前启用的 C 标准版本（默认 C89）
  #define C_STANDARD STD_C89
#endif

/// 是否支持 C89 标准
#define HAS_C89 (C_STANDARD >= STD_C89)
/// 是否支持 C99 标准
#define HAS_C99 (C_STANDARD >= STD_C99)
/// 是否支持 C11 标准
#define HAS_C11 (C_STANDARD >= STD_C11)
/// 是否支持 C17 标准
#define HAS_C17 (C_STANDARD >= STD_C17)
/// 是否支持 C23 标准
#define HAS_C23 (C_STANDARD >= STD_C23)

#endif

#endif // COMPILER_DETECT_H
