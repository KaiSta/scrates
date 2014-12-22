#pragma once

#include <string>
#include <stdint.h>

static int64_t s_to_int64(const std::string &s)
{
	if (s.size() == 0)
		std::runtime_error error("invalid argument");
	int64_t v = 0;
	size_t i = 0;
	char sign = (s[0] == '-' || s[0] == '+') ? (++i, s[0]) : '+';
	for (; i < s.size(); ++i)
	{
		if (s[i] < '0' || s[i] > '9')
			throw std::runtime_error("invalid argument");
		v = v * 10 + s[i] - '0';
	}
	return sign == '-' ? -v : v;
}