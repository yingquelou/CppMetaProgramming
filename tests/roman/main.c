#include <RomanNumerals.h>
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char const *argv[], char const *env[]) {
  RomanContext *ctx = RomanContextInit("IVXLCDM", 7);
  if (ctx) {
    RomanString *str = NULL;
    for (size_t i = 1; i < ctx->max; ++i) {
      str = RomanFromNumber(ctx, i);
      if (str) {
        printf("%zd,%.*s\n", i, (int)str->sz, str->data);
        RomanStringRelease(str);
      }
    }
    RomanContextDestroy(ctx);
  }
  return 0;
}