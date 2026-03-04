#include "../include/date_parser.h"
#include <string> //для работы со строками
#include <sstream> //для разбора строк на части
#include <cctype> //для работы с символами(tolower)
#include <cstdio> //для snprintf(форматированный вывод к строку)
#include <algorithm> //для transform(преобразования строк)

using namespace std;

//Функция для преобразования месяца в число
int parseMonth(const std::string& month) {
    //создаем копию строки и приводим к нижнему регистру
    std::string lower_month = month;
    std::transform(lower_month.begin(), lower_month.end(), lower_month.begin(),
        [](unsigned char c) { return std::tolower(c); });

    if (lower_month == "january" || lower_month == "jan") return 1;
    if (lower_month == "february" || lower_month == "feb") return 2;
    if (lower_month == "march" || lower_month == "mar") return 3;
    if (lower_month == "april" || lower_month == "apr") return 4;
    if (lower_month == "may") return 5;
    if (lower_month == "june" || lower_month == "jun") return 6;
    if (lower_month == "july" || lower_month == "jul") return 7;
    if (lower_month == "august" || lower_month == "aug") return 8;
    if (lower_month == "september" || lower_month == "sep") return 9;
    if (lower_month == "october" || lower_month == "oct") return 10;
    if (lower_month == "november" || lower_month == "nov") return 11;
    if (lower_month == "december" || lower_month == "dec") return 12;

    return -1;
}
//проверка високосности
bool isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}
//проверка существует ли дата в реальности
bool isValidDate(int year, int month, int day) {
    //год должен быть в разумных пределах
    if (year < 1900 || year > 2100) return false;
    //месяц от 1 до 12
    if (month < 1 || month > 12) return false;

    //кол-во дней в месяцах(для обычного года)
    int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    //если год високосный, то в феврале 29 дней
    if (isLeapYear(year)) daysInMonth[1] = 29;

    //день должен быть от 1 до максимального в месяце
    return day >= 1 && day <= daysInMonth[month - 1];
}

//начало и проверка пустой строки
DateRecord parseDate(const std::string& input) {
    DateRecord result; //создаем структуру для результата
    result.raw_date = input; //сохраняем исходную строку

    if (input.empty()) { //если строка пустая
        result.is_valid = false;
        result.error_message = "Empty string";
        return result;
    }

    //ФОРМАТ 1: YYYY-MM-DD
    //проверка что длина строки 10 символов и на позициях 4 и 7 стоят дефисы
    if (input.size() == 10 && input[4] == '-' && input[7] == '-') {
        int year, month, day;
        char dash1, dash2;
        std::istringstream iss(input); //создаем поток из строки

        //пытаемся прочитать: год, дефис, месяц, дефис, день
        if (iss >> year >> dash1 >> month >> dash2 >> day && dash1 == '-' && dash2 == '-') {
            //проверям, существует ли такая дата
            if (isValidDate(year, month, day)) {
                result.is_valid = true;
                result.iso_date = input; //в ISO-формате
                return result;
            }
        }
    }

    //ФОРМАТ 2: DD-MM-YYYY
    if (input.size() == 10 && input[2] == '-' && input[5] == '-') { //проверка дефисов на позициях 3 и 6.
        int day, month, year;
        char dash1, dash2;
        std::istringstream iss(input);

        if (iss >> day >> dash1 >> month >> dash2 >> year && dash1 == '-' && dash2 == '-') {
            if (isValidDate(year, month, day)) {
                result.is_valid = true;
                char buffer[11]; //буфер для новой строки
                //формуруем в YYYY-MM-DD с ведущими нулями
                //%04d - число, минимум 4 цифры, с ведущими нулями (2025)
                //%02d - число, минимум 2 цифры, с ведущими нулямии(05)
                snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d", year, month, day);
                result.iso_date = buffer;
                return result;
            }
        }
    }

    //ФОРМАТ 3: MM/DD/YYYY
    if (input.size() == 10 && input[2] == '/' && input[5] == '/') {
        int month, day, year;
        char slash1, slash2;
        std::istringstream iss(input);

        if (iss >> month >> slash1 >> day >> slash2 >> year && slash1 == '/' && slash2 == '/') {
            if (isValidDate(year, month, day)) {
                result.is_valid = true;
                char buffer[11];
                snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d", year, month, day);
                result.iso_date = buffer;
                return result;
            }
        }
    }

    //ФОРМАТ 4: DD month YYYY
    std::istringstream text_iss(input);
    int day_text, year_text;
    std::string month_text;

    if (text_iss >> day_text >> month_text >> year_text) {
        int month_num = parseMonth(month_text); //преобразование слово в число
        if (month_num != -1 && isValidDate(year_text, month_num, day_text)) {
            result.is_valid = true;
            char buffer[11];
            snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d", year_text, month_num, day_text);
            result.iso_date = buffer;
            return result;
        }
    }
    //если ничего не подошло
    result.is_valid = false;
    result.error_message = "Unknown date format";
    return result;
}

//Функция для конвертации ISO даты в русский формат
std::string isoToRussian(const std::string& isoDate) {
    //проверка, что это ISO-формат
    if (isoDate.size() != 10 || isoDate[4] != '-' || isoDate[7] != '-') {
        return isoDate; //если нет - возвращаем как есть
    }
    //разбираем ISO-дату на части
    int year = std::stoi(isoDate.substr(0, 4));
    int month = std::stoi(isoDate.substr(5, 2));
    int day = std::stoi(isoDate.substr(8, 2));

    //русские названия месяцев
    const char* months[] = {
        "января", "февраля", "марта", "апреля", "мая", "июня",
        "июля", "августа", "сентября", "октября", "ноября", "декабря"
    };

    if (month >= 1 && month <= 12) {
        //собираем строку: день + месяц + год
        return std::to_string(day) + " " + std::string(months[month - 1]) + " " + std::to_string(year);
    }

    return isoDate;
}