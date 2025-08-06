#include "process_tools.h"
#include <sstream>

// ��������� ������� ������ �� ������ �� '\n'
void splitLines(std::vector<std::string>& lines, const std::string& input) {
    if (input[0] == '\0') return;

    std::stringstream ss(input);
    std::string line;

    while (std::getline(ss, line)) {
        if (!line.empty()) {
            lines.push_back(line);
        }
    }
}

// ������������ ���� ������: number, firstWord, rest (rest ��������� ������������ �������)
void processLineChat(const std::string& line, int& id, std::string& owner, std::string& name) {
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

void processLineMessage(const std::string& line, int& id, std::string& tag, std::string& date, std::string& content)
{
    static std::string buffer;

    std::stringstream ss(line);
    ss >> id;
    ss >> tag;
    ss >> buffer;
    ss >> date;
    date = buffer + ' ' + date;

    size_t pos = 0;

    // ���������� id
    while (pos < line.size() && line[pos] != ' ') pos++;
    pos++;

    // ���������� tag
    while (pos < line.size() && line[pos] != ' ') pos++;
    pos++;

    // ���������� ���������� ������ ����� date
    while (pos < line.size() && line[pos] != ' ') pos++;
    pos++;

    // ���������� ���������� ������ ����� date
    while (pos < line.size() && line[pos] != ' ') pos++;
    pos++;

    // ��, ��� ��������, - ��� name (� ����������� ���� ��������)
    content = line.substr(pos);
}