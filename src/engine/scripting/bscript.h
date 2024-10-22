#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#ifndef BSC_MAX_STACK_COUNT
	#define BSC_MAX_STACK_COUNT 64
#endif

typedef struct BscScriptEngine BscScriptEngine;
typedef struct BscValue BscValue;
typedef struct BscFunction BscFunction;
typedef struct BscStructField BscStructField;
typedef struct BscStruct BscStruct;

typedef enum ValueType {
	BSC_T_NONE,
	BSC_T_BOOL,
	BSC_T_INT,
	BSC_T_UINT,
	BSC_T_FLOAT,
	BSC_T_STRING,
	BSC_T_FUNCTION,
	BSC_T_ARRAY,
	BSC_T_STRUCT,
	BSC_T_VECTOR,
	BSC_T_COLOR,
	BSC_T_MATRIX
} ValueType;

typedef int (*BscCFunction)(BscScriptEngine* se, int argc);

struct BscFunction {
	union {
		BscCFunction c_fn;
		uint64_t bsc_fn;
	};
	bool is_native;
};

struct BscValue {
	union {
		bool b_value;
		int32_t i_value;
		uint32_t u_value;
		float f_value;
		char* s_value;
		BscFunction fn_value;
		BscStruct* struct_value;
		void* p_value;
	};
	ValueType type;
	ValueType subtype;
};

struct BscStructField {
	uint64_t key;
	BscValue value;
};

struct BscStruct {
	uint64_t id;
	BscStructField* fields;
	size_t num_fields;
};

struct BscScriptEngine {
	BscValue stack[BSC_MAX_STACK_COUNT];
	BscValue A, B, C;
};