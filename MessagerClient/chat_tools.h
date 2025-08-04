#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

// ��������� ������� ������ �� ������ �� '\n'
void splitLines(std::vector<std::string>& lines, const std::string& input);

// ������������ ���� ������: number, firstWord, rest (rest ��������� ������������ �������)
void processLine(const std::string& line, int& id, std::string& owner, std::string& name);
