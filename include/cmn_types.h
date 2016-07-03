#ifndef CMN_TYPES_H
#define CMN_TYPES_H

#include <stdint.h>

typedef uint8_t  BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef uint64_t QWORD;

typedef int8_t  SBYTE;
typedef int16_t SWORD;
typedef int32_t SDWORD;
typedef int64_t SQWORD;

typedef double  DFWORD;

typedef char* STR;

typedef BYTE reg_op;
typedef BYTE seg_op;
typedef WORD instr_op;

#endif

