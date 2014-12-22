#ifdef _WIN32

#pragma once

#include <vector>
#include <string>
#include <functional>
#include <Windows.h>

class folder_watcher
{
public:
	enum filter
	{
		WRITES_ONLY,
		ALL
	};

	folder_watcher(const std::string& dir="");
	~folder_watcher();

	bool is_running();
	void run();
	void stop();
	void force_stop();

	void set_directory(const std::string& dir);
	void set_callback(std::function<void()> callback);

	void set_filter(filter f);
	DWORD get_filter();

	std::string get_directory();
	void call_callback();

private:
	bool runs_;
	std::string dir_;
	std::function<void()> callback_;
	HANDLE thread_;
	DWORD filter_;
};

#endif
