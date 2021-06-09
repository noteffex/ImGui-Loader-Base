#pragma once
#include <unordered_map>

class c_globals {
public:
	bool active = true;
	char user_name[255] = "user";
	char pass_word[255] = "pass";
};

inline c_globals globals;