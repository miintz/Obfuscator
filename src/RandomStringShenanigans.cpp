#include "RandomStringShenanigans.h"
#include <random>
#include <string>

std::string RandomStringShenanigans::gen_random(const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
	
	char s[100] = "";

    for (int i = 0; i < len; ++i) {
       s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    s[len] = 0;
	std::string str(s);

	return str;
 }