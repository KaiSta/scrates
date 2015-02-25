#pragma once
#include<vector>

enum event_type
{
	INFORMATION,
	CONFLICT,
	WARNING
};
enum conflict
{
	NONE,
	CTR_FILE_NOT_FOUND,
	WRONG_PASSWORD,
	DECRYPTION_ERROR,
	WRONG_ARGUMENTS
};
enum information
{
	SUCC,
	SYNCHRONIZING,
	FINISHED_SYNCHRONIZING
};
enum warning
{
	NONE
};

struct container_event
{
	event_type ev_type;
	information inf;
	conflict c;
	warning w;

	std::vector<char> _data_;
};