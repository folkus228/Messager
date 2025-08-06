#include "process_tools.h"
#include <sstream>

// Разбивает большую строку на строки по '\n'
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

// Обрабатывает одну строку: number, firstWord, rest (rest сохраняет оригинальные пробелы)
void processLineChat(const std::string& line, int& id, std::string& owner, std::string& name) {
    std::stringstream ss(line);
    ss >> id >> owner;

    // Теперь нужно вычислить позицию name
    // Считаем длину числа + пробел + первое слово
    size_t pos = 0;

    // Пропускаем число
    while (pos < line.size() && line[pos] != ' ') pos++;
    pos++; // пропускаем пробел после числа

    // Пропускаем первое слово
    while (pos < line.size() && line[pos] != ' ') pos++;
    pos++; // пропускаем пробел после первого слова

    // Всё, что осталось, - это name (с сохранением всех пробелов)
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

    // Пропускаем id
    while (pos < line.size() && line[pos] != ' ') pos++;
    pos++;

    // Пропускаем tag
    while (pos < line.size() && line[pos] != ' ') pos++;
    pos++;

    // Пропускаем пропускаем первую часть date
    while (pos < line.size() && line[pos] != ' ') pos++;
    pos++;

    // Пропускаем пропускаем вторую часть date
    while (pos < line.size() && line[pos] != ' ') pos++;
    pos++;

    // Всё, что осталось, - это name (с сохранением всех пробелов)
    content = line.substr(pos);
}