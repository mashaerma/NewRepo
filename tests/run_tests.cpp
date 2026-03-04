#include <iostream> //для вывода на экран
#include <fstream> //для работы с файлами
#include <string> //для работы со строками
#include <vector> //для массивов
#include <chrono> //для замера времени
#include "../include/date_parser.h" 

using namespace std;
using namespace chrono;

//функция для генерации тестового файла
void generateTestFile(const string& filename, int count) {
    ofstream file(filename);
    file << "[\n";
    for (int i = 0; i < count; i++) {
        string date;
        
        //10% ошибочных дат для тестирования
        if (i % 10 == 0) {
            int errorType = i % 4;
            if (errorType == 0) date = "invalid date";
            else if (errorType == 1) date = "";
            else if (errorType == 2) date = "2025-13-40";
            else date = "32-13-2025";
        } else {
            //правильные даты в разных форматах
            int year = 2020 + (i % 10);
            int month = (i % 12) + 1;
            int day = (i % 28) + 1;
            
            // 4 разных формата
            int format = i % 4;
            //формирование даты
            switch (format) {
                case 0: // YYYY-MM-DD
                    date = to_string(year) + "-" + 
                           (month < 10 ? "0" : "") + to_string(month) + "-" + 
                           (day < 10 ? "0" : "") + to_string(day);
                    break;
                case 1: // DD-MM-YYYY
                    date = (day < 10 ? "0" : "") + to_string(day) + "-" + 
                           (month < 10 ? "0" : "") + to_string(month) + "-" + 
                           to_string(year);
                    break;
                case 2: // MM/DD/YYYY
                    date = (month < 10 ? "0" : "") + to_string(month) + "/" + 
                           (day < 10 ? "0" : "") + to_string(day) + "/" + 
                           to_string(year);
                    break;
                case 3: // DD month YYYY
                    const char* months[] = {"january", "february", "march", "april", "may", "june",
                                           "july", "august", "september", "october", "november", "december"};
                    date = to_string(day) + " " + months[month-1] + " " + to_string(year);
                    break;
            }
        }
        
        file << "  {\"raw_date\": \"" << date << "\"}";
        if (i < count - 1) file << ",";
        file << "\n";
    }
    file << "]\n";
    file.close();
    cout << "Generated " << filename << " with " << count << " dates\n";
}

//ТЕСТ 1: Проверка високосности
void testLeapYear() {
    cout << "\n=== TEST 1: Leap Year Check ===\n";
    cout << "2024: " << (isLeapYear(2024) ? "PASS" :  " FAIL") << " (should be leap)\n";
    cout << "2025: " << (!isLeapYear(2025) ? "PASS" : " FAIL") << " (should not be leap)\n";
    cout << "2000: " << (isLeapYear(2000) ? "PASS" : " FAIL") << " (should be leap)\n";
    cout << "1900: " << (!isLeapYear(1900) ? "PASS" : " FAIL") << " (should not be leap)\n";
}

//ТЕСТ 2: Проверка валидности дат
void testValidDate() {
    cout << "\n=== TEST 2: Date Validation ===\n";
    
    struct TestCase { //структура для одного теста
        int year, month, day;
        bool expected;
        string desc;
    };
    
    vector<TestCase> tests = {
        {2025, 11, 30, true, "2025-11-30 (normal date)"},
        {2024, 2, 29, true, "2024-02-29 (leap year)"},
        {2025, 2, 29, false, "2025-02-29 (not leap year)"},
        {2025, 13, 1, false, "2025-13-01 (invalid month)"},
        {2025, 4, 31, false, "2025-04-31 (invalid day)"}
    };
    
    for (const auto& t : tests) {
        bool result = isValidDate(t.year, t.month, t.day);
        cout << t.desc << ": " << (result == t.expected ? "✓ PASS" : "✗ FAIL");
        if (result != t.expected) {
            cout << " (got " << result << ", expected " << t.expected << ")";
        }
        cout << "\n";
    }
}

//ТЕСТ 3: Парсинг разных форматов
void testParseFormats() {
    cout << "\n=== TEST 3: Date Parsing ===\n";
    
    vector<pair<string, bool>> tests = {
        {"2025-11-30", true},
        {"30-11-2025", true},
        {"11/30/2025", true},
        {"30 november 2025", true},
        {"invalid date", false},
        {"", false},
        {"2025-13-40", false},
        {"32-13-2025", false}
    };
    
    for (const auto& t : tests) {
        DateRecord r = parseDate(t.first);
        bool ok = (r.is_valid == t.second);
        cout << "\"" << t.first << "\": " << (ok ? "✓ PASS" : "✗ FAIL");
        if (!ok) {
            cout << " (expected " << (t.second ? "valid" : "invalid") << ")";
        } else if (!r.is_valid) {
            cout << " (error: " << r.error_message << ")";
        }
        cout << "\n";
    }
}

//ТЕСТ 4: Бенчмарк производительности
void testBenchmark() {
    cout << "\n=== TEST 4: Performance Benchmark ===\n";
    cout << "=====================================\n\n";
    
    //Создаём папку data если её нет
    system("if not exist data mkdir data");
    
    vector<int> sizes = {100, 1000, 10000, 50000}; //размеры для теста
    
    for (int size : sizes) {
        string filename = "data/bench_" + to_string(size) + ".json";
        
        //замер времени генерации
        auto gen_start = high_resolution_clock::now();
        generateTestFile(filename, size);
        auto gen_end = high_resolution_clock::now();
        auto gen_time = duration_cast<milliseconds>(gen_end - gen_start);
        
        // Читаем и парсим файл
        ifstream file(filename);
        vector<DateRecord> records;
        string line;
        int valid = 0, invalid = 0;
        
        auto parse_start = high_resolution_clock::now();
        
        while (getline(file, line)) {
            //поиск и парсинг дат
            size_t p = line.find("\"raw_date\"");
            if (p == string::npos) continue;
            size_t s = line.find(':', p);
            size_t q1 = line.find('\"', s);
            size_t q2 = line.find('\"', q1 + 1);
            if (q1 == string::npos || q2 == string::npos) continue;
            
            string date = line.substr(q1 + 1, q2 - q1 - 1);
            DateRecord r = parseDate(date);
            records.push_back(r);
            if (r.is_valid) valid++; else invalid++;
        }
        file.close();
        
        auto parse_end = high_resolution_clock::now();
        auto parse_time = duration_cast<milliseconds>(parse_end - parse_start);
        
        //Выводим результаты
        cout << "SIZE: " << size << " dates\n";
        cout << "   ├─ Generate time: " << gen_time.count() << " ms\n";
        cout << "   ├─ Parse time:    " << parse_time.count() << " ms\n";
        cout << "   ├─ Valid dates:   " << valid << "\n";
        cout << "   ├─ Invalid dates: " << invalid << "\n";
        cout << "   └─ Speed:         " << (size * 1000.0 / parse_time.count()) << " records/sec\n\n";
    }
}

int main() {
    cout << "===========================================\n";
    cout << "      DATE CONVERTER - TEST SUITE      \n";
    cout << "===========================================\n";
    
    testLeapYear(); //тест 1
    testValidDate(); //тест 2
    testParseFormats(); //тест 3
    testBenchmark(); //тест 4
    
    cout << "\n===========================================\n";
    cout << "           ALL TESTS COMPLETED           \n";
    cout << "===========================================\n";
    
    return 0;
}