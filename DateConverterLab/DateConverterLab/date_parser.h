#ifndef DATE_PARSER_H
#define DATE_PARSER_H

#include <string>
#include <vector>

struct DateRecord {
    std::string raw_date;
    std::string iso_date;
    bool is_valid;
    std::string error_message;
};

int parseMonth(const std::string& month);
bool isLeapYear(int year);
bool isValidDate(int year, int month, int day);
DateRecord parseDate(const std::string& input);
std::string isoToRussian(const std::string& isoDate);
std::string isoToEnglishLong(const std::string& isoDate);
#endif