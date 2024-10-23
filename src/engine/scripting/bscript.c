#include "bscript.h"

#include <string.h>

#include "puli_str.h"

struct BscStructField {
	uint64_t key;
	BscValue value;
};

struct BscScriptEngine {
	BscValue stack[BSC_MAX_STACK_COUNT];
	uint32_t stackP;
	BscValue A, B, C;
	BscStruct globals;
};

static void BscFreeValue(BscValue* value) {
	if (value != NULL) {
		if (value->type == BSC_T_STRING) {
			free(value->p_value);
			memset(value, 0, sizeof(BscValue));
		}
	}
}

static BscStructField* BscFindField(BscStruct* s, const char* name) {
	if (s == NULL) {
		return NULL;
	}

	size_t key = pStrHash(name);
	if (s->fields == NULL) {
		return NULL;
	}

	for (size_t i = 0; i < s->num_fields; i++) {
		if (s->fields[i].key == key) {
			return &s->fields[i];
		}
	}
	return NULL;
}

BscScriptEngine* BscInitialize() {
	BscScriptEngine* s = (BscScriptEngine*)malloc(sizeof(BscScriptEngine));
	if (s != NULL) {
		memset(s, 0, sizeof(BscScriptEngine));
	}
	return s;
}
void BscFree(BscScriptEngine** s) {
	if (s != NULL && *s != NULL) {
		free(*s);
		*s = NULL;
	}
}

BscError BscPush(BscScriptEngine* s, BscValue value) {
	if (s == NULL) {
		return BSC_ERROR_NULLPTR;
	}

	if (s->stackP >= BSC_MAX_STACK_COUNT) {
		return BSC_ERROR_STACK_OVERFLOW;
	}
	s->stack[s->stackP++] = value;
	return BSC_OK;
}
BscError BscPushNone(BscScriptEngine* s) {
	return BscPush(s, { 0 });
}
BscError BscPushBool(BscScriptEngine* s, bool value) {
	return BscPush(s, {
		.b_value = value,
		.type =  BSC_T_BOOL
	});
}
BscError BscPushInt(BscScriptEngine* s, int32_t value) {
	return BscPush(s, {
		.i_value = value,
		.type =  BSC_T_INT
	});
}
BscError BscPushUInt(BscScriptEngine* s, uint32_t value) {
	return BscPush(s, {
		.u_value = value,
		.type =  BSC_T_UINT
	});
}
BscError BscPushFloat(BscScriptEngine* s, float value) {
	return BscPush(s, {
		.f_value = value,
		.type =  BSC_T_FLOAT
	});
}
BscError BscPushCFunc(BscScriptEngine* s, BscCFunction value) {
	return BscPush(s, {
		.fn_value = (BscFunction){
			.c_fn = value,
			.is_native = true
		},
		.type =  BSC_T_FUNCTION
	});
}
BscError BscPushVector(BscScriptEngine* s, float vector[3]) {
	BscValue value = { .type = BSC_T_VECTOR };
	value.vec_value[0] = vector[0];
	value.vec_value[1] = vector[1];
	value.vec_value[2] = vector[2];
	return BscPush(s, value);
}
BscError BscPushColor(BscScriptEngine* s, float color[4]) {
	BscValue value = { .type = BSC_T_COLOR };
	value.vec_value[0] = color[0];
	value.vec_value[1] = color[1];
	value.vec_value[2] = color[2];
	value.vec_value[3] = color[3];
	return BscPush(s, value);
}
BscError BscPop(BscScriptEngine* s) {
	if (s->stackP == 0) {
		return BSC_ERROR_STACK_UNDERFLOW;
	}
	BscFreeValue(&s->stack[--s->stackP]);
	return BSC_OK;
}

bool BscGetBool(BscScriptEngine* s, int32_t idx) {

}
int32_t BscGetInt(BscScriptEngine* s, int32_t idx) {

}
uint32_t BscGetUInt(BscScriptEngine* s, int32_t idx) {

}
float BscGetFloat(BscScriptEngine* s, int32_t idx) {

}
BscFunction BscGetFunction(BscScriptEngine* s, int32_t idx) {

}
void BscGetVector(BscScriptEngine* s, int32_t idx, float vector[3]) {

}
void BscGetColor(BscScriptEngine* s, int32_t idx, float vector[4]) {

}

BscError BscSetGlobal(BscScriptEngine* s, const char* name) {
	if (s == NULL) {
		return BSC_ERROR_NULLPTR;
	}
	if (s->stackP == 0) {
		return BSC_ERROR_NULLPTR;
	}

	BscStructField* field = BscFindField(&s->globals, name);
	if (field == NULL) {
		BscStructField* fields = (BscStructField*)realloc(s->globals.fields, (s->globals.num_fields + 1) * sizeof(BscStructField));
		if (fields == NULL) {
			return BSC_ERROR_OUT_OF_MEMORY;
		}
		s->globals.fields = fields;
		field = &s->globals.fields[s->globals.num_fields++];
	}

	field->key = pStrHash(name);
	field->value = s->stack[--s->stackP];
	memset(&s->stack[s->stackP], 0, sizeof(BscValue));

	return BSC_OK;
}
BscError BscGetGlobal(BscScriptEngine* s, const char* name) {
	if (s == NULL) {
		return BSC_ERROR_NULLPTR;
	}

	const BscStructField* field = BscFindField(&s->globals, name);
	if (field == NULL) {
		return BSC_ERROR_UNDEFINED;
	}

	return BscPush(s, field->value);
}

BscError BscDeclareStruct(BscScriptEngine* s, const char* name, const BscStructFieldDef* fields, size_t field_count) {
}
BscError BscCreateStructInstance(BscScriptEngine* s, const char* name) {
}

BscError BscCall(BscScriptEngine* s, size_t argc, size_t retc) {
}

BscError BscGetField(BscScriptEngine* s, int32_t idx, const char* name) {
}
BscError BscSetField(BscScriptEngine* s, int32_t idx, const char* name) {
}
BscError BscGetI(BscScriptEngine* s, int32_t idx, size_t item_idx) {
}
BscError BscSetI(BscScriptEngine* s, int32_t idx, size_t item_idx) {
}

BscError BscLoad(BscScriptEngine* s, const uint8_t* data, size_t size) {
}
BscError BscLoadString(BscScriptEngine* s, const char* str) {
}
BscError BscLoadFile(BscScriptEngine* s, const char* filename) {
	FILE* f = fopen(filename, "r");
	if (f == NULL) {
		return BSC_ERROR_FILE_UNABLE_TO_OPEN;
	}

}
BscError BscRun(BscScriptEngine* s, const uint8_t* data, size_t size) {
	BscError err;
	if ((err = BscLoad(s, data, size)) != BSC_OK) {
		return err;
	}
	if ((err = BscGetGlobal(s, "main")) != BSC_OK) {
		return err;
	}
	return BscCall(s, 0, 0);
}
BscError BscRunString(BscScriptEngine* s, const char* str) {
	BscError err;
	if ((err = BscLoadString(s, str)) != BSC_OK) {
		return err;
	}
	if ((err = BscGetGlobal(s, "main")) != BSC_OK) {
		return err;
	}
	return BscCall(s, 0, 0);
}
BscError BscRunFile(BscScriptEngine* s, const char* filename) {
	BscError err;
	if ((err = BscLoadFile(s, filename)) != BSC_OK) {
		return err;
	}
	if ((err = BscGetGlobal(s, "main")) != BSC_OK) {
		return err;
	}
	return BscCall(s, 0, 0);
}