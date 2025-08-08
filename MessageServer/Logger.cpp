#include "Logger.h"

#include <fstream>
#include <string>
#include <mutex>


Logger::~Logger()
{
	_file->close();
	delete _file;
}

void Logger::write_log(std::string& request, std::string& answer)
{
	std::call_once(is_init, &Logger::init, this);
	_mutex.lock();
	*_file << "REQUEST:\n\n"
		<< request
		<< "\n\n ANSWER:\n\n"
		<< answer
		<< "\n\n\n";
	_mutex.unlock();
}

void Logger::init()
{
	_file = new std::ofstream("log.txt");
}