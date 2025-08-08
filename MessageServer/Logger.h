#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <mutex>

class Logger
{
public:

	Logger() = default;

	~Logger();

	void write_log(std::string& request, std::string& answer);

private:

	std::once_flag is_init;

	void init();

	std::ofstream* _file;
	std::mutex _mutex;
};

