#include "RomanNumerals.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
static void __RomanFromNumber(const RomanContext *ctx, char *buffer,
                              size_t *pos, size_t num) {
  if (num < 1) {
    return;
  }
  size_t Y = floor(log10(num));
  size_t offset = Y * 2;
  size_t n1 = pow(10, Y), opt;
  char c1 = ctx->str->data[offset];
  char c5 = ctx->str->data[offset + 1];
  char c10 = ctx->str->data[offset + 2];
  switch (opt = num / n1) {
  case 9:
    buffer[(*pos)++] = c1;
    buffer[(*pos)++] = c10;
    break;
  case 8:
    buffer[(*pos)++] = c5;
    buffer[(*pos)++] = c1;
  case 7:
    if (opt == 7)
      buffer[(*pos)++] = c5;
    buffer[(*pos)++] = c1;
  case 6:
    if (opt == 6)
      buffer[(*pos)++] = c5;
    buffer[(*pos)++] = c1;
    break;
  case 5:
  case 4:
    if (opt == 4)
      buffer[(*pos)++] = c1;
    buffer[(*pos)++] = c5;
    break;
  case 3:
    buffer[(*pos)++] = c1;
  case 2:
    buffer[(*pos)++] = c1;
  case 1:
    buffer[(*pos)++] = c1;
    break;
  }
  num -= n1 * opt;
  __RomanFromNumber(ctx, buffer, pos, num);
}
RomanContext *RomanContextInit(const char *roman, size_t sz) {
  RomanString *str = RomanStringAlloc(roman, sz);
  if (!str) {
    return NULL;
  }
  RomanContext *ctx = malloc(sizeof(RomanContext));
  if (ctx) {
    ctx->str = str;
    div_t dt = div(str->sz, 2);
    size_t m = pow(10, dt.quot);
    ctx->bsz = dt.quot * 4;
    if (dt.rem) {
      ctx->max = 4 * m;
      ctx->bsz += 3;
    } else {
      ctx->max = m / 10 * 9;
    }
  } else {
    RomanStringRelease(str);
  }
  return ctx;
}
void RomanContextDestroy(RomanContext *ctx) {
  ctx->max = 0;
  ctx->bsz = 0;
  RomanStringRelease(ctx->str);
  ctx->str = NULL;
  free(ctx);
}
RomanString *RomanFromNumber(const RomanContext *ctx, size_t num) {
  RomanString *str = NULL;
  if (num && num < ctx->max) {
    str = RomanStringAlloc(NULL, ctx->bsz);
    if (str) {
      size_t pos = 0;
      __RomanFromNumber(ctx, str->data, &pos, num);
      str = realloc(str, sizeof(RomanString) + pos);
      str->sz = pos;
    }
  } else {
    fprintf(stderr,
            "Roman numerals defined by '%.*s' "
            "can represent decimal numbers in the open interval (0, %zd), "
            "so the decimal number '%zd' cannot be represented.\n",
            (int)ctx->str->sz, ctx->str->data, ctx->max, num);
  }
  return str;
}
RomanString *RomanStringAlloc(const char *roman, size_t sz) {
  if (!sz || sz == -1) {
    if (roman)
      sz = strlen(roman);
    else
      sz = 0;
  }

  RomanString *str = NULL;
  if (sz) {
    str = malloc(sizeof(RomanString) + sz);
    if (str) {
      str->sz = sz;
      if (roman)
        memcpy(str->data, roman, sz);
    }
  }
  return str;
}
void RomanStringRelease(RomanString *str) {
  if (str) {
    str->sz = 0;
    free(str);
  }
}