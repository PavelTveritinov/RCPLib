// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <stdlib.h>

#include <memory>

#if __cplusplus >= 202011L
#   define DEPRECATED_FROM_CPP23  [[deprecated("Use std versions for this functionality")]]
#else
#   define DEPRECATED_FROM_CPP23
#endif

#if __cplusplus >= 201703L
#   define DEPRECATED_FROM_CPP20  [[deprecated("Use std versions for this functionality")]]
#else
#   define DEPRECATED_FROM_CPP20
#endif

// NOTE: Based on chromium error subsystem
//
// A wrapper around `__has_attribute`, similar to HAS_CPP_ATTRIBUTE.
#if defined( __has_attribute )
#    define HAS_ATTRIBUTE( x ) __has_attribute( x )
#else
#    define HAS_ATTRIBUTE( x ) 0
#endif

#ifdef __APPLE__
#   define ALWAYS_INLINE    [[clang::always_inline]]
#else
#   define ALWAYS_INLINE    __forceinline
#endif

#if defined( __clang__ ) && HAS_ATTRIBUTE( noinline )
#   define NOINLINE [[clang::noinline]]
#elif defined(_MSC_VER)
#   define NOINLINE __declspec(noinline)
#else
#   error "Unsupported platform"
#endif

//
//-----------------------------------------------------------------------------
// Annotate a function indicating it should never be tail called. Useful to make
// sure callers of the annotated function are never omitted from call-stacks.
// To provide the complementary behavior (prevent the annotated function from
// being omitted) look at NOINLINE. Also note that this doesn't prevent code
// folding of multiple identical caller functions into a single signature. To
// prevent code folding, see NO_CODE_FOLDING() in base/debug/alias.h.
// Use like:
//   NOT_TAIL_CALLED void FooBar();
//-----------------------------------------------------------------------------

#if defined( __clang__ ) && HAS_ATTRIBUTE( not_tail_called )
#   define NOT_TAIL_CALLED [[clang::not_tail_called]]
#else
#   define NOT_TAIL_CALLED
#endif

// Use nomerge attribute to disable optimization of merging multiple same calls.
#if defined( __clang__ ) && HAS_ATTRIBUTE( nomerge )
#   define NOMERGE [[clang::nomerge]]
#else
#   define NOMERGE
#endif


// The ANALYZER_ASSUME_TRUE(bool arg) macro adds compiler-specific hints
// to Clang which control what code paths are statically analyzed,
// and is meant to be used in conjunction with assert & assert-like functions.
// The expression is passed straight through if analysis isn't enabled.
//
// ANALYZER_SKIP_THIS_PATH() suppresses static analysis for the current
// codepath and any other branching codepaths that might follow.
#if defined(__clang_analyzer__)

inline constexpr bool AnalyzerNoReturn() __attribute__((analyzer_noreturn)) {
  return false;
}

inline constexpr bool AnalyzerAssumeTrue(bool arg) {
  // AnalyzerNoReturn() is invoked and analysis is terminated if |arg| is
  // false.
  return arg || AnalyzerNoReturn();
}

#define ANALYZER_ASSUME_TRUE(arg) ::AnalyzerAssumeTrue(!!(arg))
#define ANALYZER_SKIP_THIS_PATH() static_cast<void>(::AnalyzerNoReturn())

#else  // !defined(__clang_analyzer__)

#define ANALYZER_ASSUME_TRUE(arg) (arg)
#define ANALYZER_SKIP_THIS_PATH()

#endif  // defined(__clang_analyzer__)

#if !defined(LIKELY)
#   if defined(COMPILER_GCC) || defined(__clang__)
#      define LIKELY(x) __builtin_expect(!!(x), 1)
#   else
#      define LIKELY(x) (x)
#   endif  // defined(COMPILER_GCC)
#endif  // !defined(LIKELY)


// The arraysize(arr) macro returns the # of elements in an array arr.
// The expression is a compile-time constant, and therefore can be
// used in defining new arrays, for example.  If you use arraysize on
// a pointer by mistake, you will get a compile-time error.
#define arraysize(array) (sizeof(ArraySizeHelper(array)))


// This template function declaration is used in defining arraysize.
// Note that the function doesn't need an implementation, as we only
// use its type.
template <typename T, size_t N>
char (&ArraySizeHelper(T (&array)[N]))[N];


#if !defined(_MSC_VER)
// That gcc wants both of these prototypes seems mysterious. VC, for
// its part, can't decide which to use (another mystery). Matching of
// template overloads: the final frontier.
template <typename T, size_t N>
char (&ArraySizeHelper(const T (&array)[N]))[N];
#endif


namespace base {
// Function object which invokes 'free' on its parameter, which must be
// a pointer. Can be used to store malloc-allocated pointers in std::unique_ptr:
//
// std::unique_ptr<int, base::FreeDeleter> foo_ptr(
//     static_cast<int*>(malloc(sizeof(int))));
struct FreeDeleter {
  inline void operator()(void* ptr) const { std::free(ptr); }
};

}  // namespace base
