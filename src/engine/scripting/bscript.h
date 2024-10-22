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
typedef struct BscStructFieldDef BscStructFieldDef;
typedef struct BscStruct BscStruct;

typedef enum BscError {
	BSC_OK = 0,
	BSC_ERROR_NULLPTR,
	BSC_ERROR_STACK_OVERFLOW,
	BSC_ERROR_STACK_UNDERFLOW,
	BSC_ERROR_OUT_OF_MEMORY,
	BSC_ERROR_REDEFINITION,
	BSC_ERROR_UNDEFINED
} BscError;

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
	BSC_T_COLOR
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
		float vec_value[4];
		void* p_value;
	};
	ValueType type;
	ValueType subtype;
};

struct BscStruct {
	size_t id;
	BscStructField* fields;
	size_t num_fields;
};

struct BscStructFieldDef {
	const char* name;
	BscValue value;
};

BscScriptEngine* BscInitialize();
void BscFree(BscScriptEngine** s);

BscError BscPush(BscScriptEngine* s, BscValue value);
BscError BscPushNone(BscScriptEngine* s);
BscError BscPushBool(BscScriptEngine* s, bool value);
BscError BscPushInt(BscScriptEngine* s, int32_t value);
BscError BscPushUInt(BscScriptEngine* s, uint32_t value);
BscError BscPushFloat(BscScriptEngine* s, float value);
BscError BscPushCFunc(BscScriptEngine* s, BscCFunction value);
BscError BscPushVector(BscScriptEngine* s, float vector[3]);
BscError BscPushColor(BscScriptEngine* s, float color[4]);
BscError BscPop(BscScriptEngine* s);

BscError BscSetGlobal(BscScriptEngine* s, const char* name);
BscError BscGetGlobal(BscScriptEngine* s, const char* name);

BscError BscDeclareStruct(BscScriptEngine* s, const char* name, const BscStructFieldDef* fields, size_t field_count);
BscError BscCreateStructInstance(BscScriptEngine* s, const char* name);

