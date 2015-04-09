#pragma once
#include<vector>

enum event_type
{
	INFORMATION,
	CONFLICT,
	WARNING,
	VERBOSE
};

enum event_message
{
	NONE,
	CTR_FILE_NOT_FOUND,
	WRONG_PASSWORD,
	MISSING_ENC_FILES,
	DECRYPTION_ERROR,
	WRONG_ARGUMENTS,
	SUCC,
	SYNCHRONIZING,
	FINISHED_SYNCHRONIZING,
	NO_WARNING,
	CLOSING,
	ADD_FILE,
	UPDATE_FILE,
	DELETE_FILE,
	EXTRACT_FILE,
	EXTRACT_FILES,
	INIT_CONTAINER
};

struct container_event
{
	event_type type;
	event_message message;

	std::vector<char> _data_;
};