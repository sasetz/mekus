#ifndef BASE_H
#define BASE_H

#include <stdint.h>
#include <string.h>
#include <sys/types.h>

// style guidelines
//
// variables, functions - camelCase
// structures - PascalCase
// functions that are structure's "member" - _camelCase(structure as first arg)
// functions to initialize new structs - _newStructName(args)
// macros - SCREAMING_SNAKE_CASE
// macro functions - same as regular functions if there is no difference
//
// no shorthand names, only full words unless its library structure/function

// some type definistions
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef char* string;
typedef char constString[];

typedef i8 bool;
#define TRUE 1
#define FALSE 0

typedef int descriptor;
typedef pid_t pid;
typedef int exitCode;

// functions and stuff
#define zero(array,length) memset(array,0,length)
#define zeroStruct(STRUCT) memset(&STRUCT,0,sizeof(STRUCT))

#endif /* end of include guard: BASE_H */

// vim: filetype=c
