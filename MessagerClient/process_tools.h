#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

// –азбивает большую строку на строки по '\n'
void splitLines(std::vector<std::string>& lines, const std::string& input);

// ќбрабатывает одну строку: number, firstWord, rest (rest сохран€ет оригинальные пробелы)
void processLineChat(const std::string& line, int& id, std::string& owner, std::string& name);

void processLineMessage(const std::string& line, int& id, std::string& tag, std::string& date, std::string& content);
