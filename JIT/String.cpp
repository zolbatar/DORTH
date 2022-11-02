#include "String.h"
#include <iostream>

typedef const char *strings_key;
#define i_key strings_key
#include "../../PiTubeDirect/stc/cset.h"
cset_strings_key strings;

static bool debug = false;
//T_SI string_index = 1;
/*static std::unordered_map<T_SI, T_S> strings;
static std::unordered_map<T_SI, T_S> permanent_strings;
static std::unordered_set<T_SI> constant_strings;
static std::unordered_set<T_SI> local_strings;*/

void Strings_Clear() {
	strings = cset_strings_key_init();
}

void Strings_Summary() {
	std::cout << "GC summary\n";
	std::cout << "Temporary: " << cset_strings_key_size(&strings) << std::endl;
/*	std::cout << "Constant: " << constant_strings.size() << std::endl;
	std::cout << "Permanent (global): " << permanent_strings.size() - constant_strings.size() - local_strings.size()
			  << std::endl;
	std::cout << "Permanent (local): " << local_strings.size() << std::endl;*/
}

extern "C" void Strings_AddPermanent(const char *s) {

/*	auto i = string_index++;
	if (debug) printf("Create permanent string: '%s'/%lld\n", s.c_str(), i);
	constant_strings.insert(i);
	permanent_strings.insert(std::make_pair(i, std::move(s)));
	return i;*/
}

/*void Strings_Free(T_SI index) {
	strings.erase(index);
}

T_SI Strings_Add(T_S s) {
	auto i = string_index++;
	if (debug) printf("Create string: '%s'/%lld\n", s.c_str(), i);
	strings.insert(std::make_pair(i, std::move(s)));
	return i;
}

extern "C" void Strings_MakePermanent(T_SI idx, T_SI idx_prev) {
	if (strings.contains(idx_prev)) {
		strings.erase(idx_prev);
	}
	if (permanent_strings.contains(idx_prev) && !constant_strings.contains(idx_prev)) {
		permanent_strings.erase(idx_prev);
	}
	if (strings.contains(idx)) {
		auto f = strings.extract(idx);
		permanent_strings.insert(std::move(f));
		if (debug) printf("Make permanent string: %lld\n", idx);
	}
}

extern "C" void Strings_MakePermanent_Local(T_SI idx, T_SI idx_prev) {
	if (strings.contains(idx_prev)) {
		strings.erase(idx_prev);
	}
	if (permanent_strings.contains(idx_prev) && !constant_strings.contains(idx_prev)) {
		permanent_strings.erase(idx_prev);
	}
	if (strings.contains(idx)) {
		auto f = strings.extract(idx);
		permanent_strings.insert(std::move(f));
		local_strings.insert(idx);
		if (debug) printf("Make permanent string (local): %lld\n", idx);
	}
}

void Strings_SetConstant(T_SI idx) {
	constant_strings.insert(idx);
}

void Strings_FreePermanent(T_SI idx) {
	if (constant_strings.count(idx) > 0)
		return;
	auto f = permanent_strings.extract(idx);
	strings.insert(std::move(f));
}

extern "C" void Strings_ClearTemporary() {
	strings.clear();
	if (debug) printf("Clear temporary strings\n");
}

T_S &Strings_Get(T_SI idx) {
	auto f = strings.find(idx);
	if (f == strings.end()) {
		auto f = permanent_strings.find(idx);
		if (f == permanent_strings.end()) {
			printf("Temporary strings:\n");
			for (auto &s : strings) {
				printf("%lld = '%s'\n", s.first, s.second.c_str());
			}
			printf("Permanent strings:\n");
			for (auto &s : permanent_strings) {
				printf("%lld = '%s'\n", s.first, s.second.c_str());
			}
			printf("Invalid string: %lld\n", idx);
			exit(1);
		}
		return f->second;
	}
	return f->second;
}

size_t GetCount() {
	return permanent_strings.size();
}

size_t GetCountTemporary() {
	return strings.size();
}

void Strings_LeaveLocal() {
	int a = 1;
}*/