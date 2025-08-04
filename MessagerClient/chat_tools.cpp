#include "chat_tools.h"
#include <sstream>

// ��������� ������� ������ �� ������ �� '\n'
void splitLines(std::vector<std::string>& lines, const std::string& input) {
    std::stringstream ss(input);
    std::string line;

    while (std::getline(ss, line)) {
        if (!line.empty()) {
            lines.push_back(line);
        }
    }
}

// ������������ ���� ������: number, firstWord, rest (rest ��������� ������������ �������)
void processLine(const std::string& line, int& id, std::string& owner, std::string& name) {
    std::stringstream ss(line);
    ss >> id >> owner;

    // ������ ����� ��������� ������� name
    // ������� ����� ����� + ������ + ������ �����
    size_t pos = 0;

    // ���������� �����
    while (pos < line.size() && line[pos] != ' ') pos++;
    pos++; // ���������� ������ ����� �����

    // ���������� ������ �����
    while (pos < line.size() && line[pos] != ' ') pos++;
    pos++; // ���������� ������ ����� ������� �����

    // ��, ��� ��������, - ��� name (� ����������� ���� ��������)
    name = line.substr(pos);
}