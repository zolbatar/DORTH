#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "../Types/Types.h"

void Strings_Clear();
void Strings_Summary();

extern "C" {
void Strings_AddPermanent(const char *s);
};

/*
void Strings_Free(T_SI index);
T_SI Strings_Add(T_S s);
T_S &Strings_Get(T_SI idx);
extern "C" void Strings_ClearTemporary();
extern "C" void Strings_MakePermanent(T_SI idx, T_SI idx_prev);
extern "C" void Strings_FreePermanent(T_SI idx, T_SI idx_prev);
void Strings_SetConstant(T_SI idx);
size_t Strings_GetCount();
size_t Strings_GetCountTemporary();
void Strings_LeaveLocal();
*/