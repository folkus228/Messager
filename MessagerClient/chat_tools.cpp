#include "chat_tools.h"
#include <sstream>

// –азбивает большую строку на строки по '\n'
void splitLines(std::vector<std::string>& lines, const std::string& input) {
    std::stringstream ss(input);
    std::string line;

    while (std::getline(ss, line)) {
        if (!line.empty()) {
            lines.push_back(line);
        }
    }
}

// ќбрабатывает одну строку: number, firstWord, rest (rest сохран€ет оригинальные пробелы)
void processLine(const std::string& line, int& id, std::string& owner, std::string& name) {
    std::stringstream ss(line);
    ss >> id >> owner;

    // “еперь нужно вычислить позицию name
    // —читаем длину числа + пробел + первое слово
    size_t pos = 0;

    // ѕропускаем число
    while (pos < line.size() && line[pos] != ' ') pos++;
    pos++; // пропускаем пробел после числа

    // ѕропускаем первое слово
    while (pos < line.size() && line[pos] != ' ') pos++;
    pos++; // пропускаем пробел после первого слова

    // ¬сЄ, что осталось, - это name (с сохранением всех пробелов)
    name = line.substr(pos);
}