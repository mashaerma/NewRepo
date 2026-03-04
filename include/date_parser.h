#ifndef DATE_PARSER_H //если этот файл не подключен
#define DATE_PARSER_H //то подключаем его(от повторов)

#include <string>
#include <vector>

//структура дл€ одной даты
struct DateRecord {
    std::string raw_date; //сыра€ дата
    std::string iso_date; //в формате ISO(2025-11-30)
    bool is_valid = false; //валидна ли дата
    std::string error_message; //если нет, то текст ошибки
};
//объ€влени€ функций
bool isLeapYear(int year); //проверка на високосный год
bool isValidDate(int year, int month, int day); //проверка на правильную дату
DateRecord parseDate(const std::string& input); //парсирование даты
std::string isoToRussian(const std::string& isoDate);
void generateJSON(int count, const std::string& filename);

#endif