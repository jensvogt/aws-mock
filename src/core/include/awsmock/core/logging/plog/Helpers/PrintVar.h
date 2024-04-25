#pragma once

#define PLOG_IMPL_PRINT_VAR_1(a1) #a1 ": " << a1
#define PLOG_IMPL_PRINT_VAR_2(a1, a2) PLOG_IMPL_PRINT_VAR_1(a1) PLOG_IMPL_PRINT_VAR_TAIL(a2)
#define PLOG_IMPL_PRINT_VAR_3(a1, a2, a3) PLOG_IMPL_PRINT_VAR_2(a1, a2) PLOG_IMPL_PRINT_VAR_TAIL(a3)
#define PLOG_IMPL_PRINT_VAR_4(a1, a2, a3, a4) PLOG_IMPL_PRINT_VAR_3(a1, a2, a3) PLOG_IMPL_PRINT_VAR_TAIL(a4)
#define PLOG_IMPL_PRINT_VAR_5(a1, a2, a3, a4, a5) PLOG_IMPL_PRINT_VAR_4(a1, a2, a3, a4) PLOG_IMPL_PRINT_VAR_TAIL(a5)
#define PLOG_IMPL_PRINT_VAR_6(a1, a2, a3, a4, a5, a6) PLOG_IMPL_PRINT_VAR_5(a1, a2, a3, a4, a5) PLOG_IMPL_PRINT_VAR_TAIL(a6)
#define PLOG_IMPL_PRINT_VAR_7(a1, a2, a3, a4, a5, a6, a7) PLOG_IMPL_PRINT_VAR_6(a1, a2, a3, a4, a5, a6) PLOG_IMPL_PRINT_VAR_TAIL(a7)
#define PLOG_IMPL_PRINT_VAR_8(a1, a2, a3, a4, a5, a6, a7, a8) PLOG_IMPL_PRINT_VAR_7(a1, a2, a3, a4, a5, a6, a7) PLOG_IMPL_PRINT_VAR_TAIL(a8)
#define PLOG_IMPL_PRINT_VAR_9(a1, a2, a3, a4, a5, a6, a7, a8, a9) PLOG_IMPL_PRINT_VAR_8(a1, a2, a3, a4, a5, a6, a7, a8) PLOG_IMPL_PRINT_VAR_TAIL(a9)
#define PLOG_IMPL_PRINT_VAR_TAIL(a) << ", " PLOG_IMPL_PRINT_VAR_1(a)

#define PLOG_IMPL_PRINT_VAR_EXPAND(x) x

#ifdef __GNUC__
#pragma GCC system_header// disable warning: variadic macros are a C99 feature
#endif

#define PLOG_IMPL_PRINT_VAR_GET_MACRO(x1, x2, x3, x4, x5, x6, x7, x8, x9, NAME, ...) NAME

#define PLOG_PRINT_VAR(...) PLOG_IMPL_PRINT_VAR_EXPAND(PLOG_IMPL_PRINT_VAR_GET_MACRO(__VA_ARGS__,                                                                                                       \
                                                                                     PLOG_IMPL_PRINT_VAR_9, PLOG_IMPL_PRINT_VAR_8, PLOG_IMPL_PRINT_VAR_7, PLOG_IMPL_PRINT_VAR_6, PLOG_IMPL_PRINT_VAR_5, \
                                                                                     PLOG_IMPL_PRINT_VAR_4, PLOG_IMPL_PRINT_VAR_3, PLOG_IMPL_PRINT_VAR_2, PLOG_IMPL_PRINT_VAR_1, UNUSED)(__VA_ARGS__))
