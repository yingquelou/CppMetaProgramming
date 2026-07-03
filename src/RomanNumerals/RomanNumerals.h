#ifndef MODERNROMANNUMERALS_H
#define MODERNROMANNUMERALS_H 1
#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
typedef struct RomanString {
  size_t sz;
  char data[0];
} RomanString;
typedef struct RomanContext {
  /// Roman元字符序列所能表示的最大正整数是max-1
  size_t max;
  /// 建议的预分配缓冲区大小
  size_t bsz;
  /// Roman元字符序列
  RomanString *str;
} RomanContext;
RomanString *RomanStringAlloc(const char *roman, size_t sz);
void RomanStringRelease(RomanString *str);
/// Modern Roman numerals
/// |I|V|X|L|C|D|M|...|
/// |-|-|-|-|-|-|-|-|
/// |1|5|10|50|100|500|1000|...|
RomanContext *RomanContextInit(const char *roman, size_t sz);
void RomanContextDestroy(RomanContext *ctx);
/// The return value's memory is allocated on the heap.
/// Please release it yourself when it is no longer needed.
RomanString *RomanFromNumber(const RomanContext *ctx, size_t num);
#ifdef __cplusplus
}
#endif
#endif