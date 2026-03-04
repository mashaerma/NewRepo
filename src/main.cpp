#include "../include/date_parser.h"
#include <iostream> //для вывода на экран 
#include <fstream> //для работы с файлами(чтение/запись)
#include <string> //для работы со строками
#include <vector> //для массивов
#include <iomanip> //для формирования таблиц
#include <chrono> //для замера времени(бенчмарки)

using namespace std;
using namespace chrono;

//Функция справки
void printHelp() {
    cout << "=== DATE CONVERTER ===\n";
    cout << "Usage:\n";
    cout << "  --help                 Show this help\n";
    cout << "  --input FILE           Input JSON file\n";
    cout << "  --output FILE          Output JSON file\n";
    cout << "  --to FORMAT            Output format (iso/ru_long)\n";
    cout << "  --generate N           Generate N test dates\n";
    cout << "  --bench FILE           Benchmark parsing\n";
}

//Функция вывода таблицы
void printTable(const vector<DateRecord>& records, const string& format) {
    cout << "\n" << string(80, '=') << "\n";
    cout << left << setw(25) << "Original Date"
        << setw(12) << "Status"
        << setw(20) << "ISO Format"
        << "Converted\n";
    cout << string(80, '-') << "\n";

    for (const auto& r : records) {
        cout << left << setw(25) << r.raw_date
            << setw(12) << (r.is_valid ? "VALID" : "INVALID")
            << setw(20) << (r.is_valid ? r.iso_date : "");

        if (r.is_valid) {
            if (format == "ru_long") cout << isoToRussian(r.iso_date);
            else cout << r.iso_date;
        }
        else {
            cout << r.error_message;
        }
        cout << "\n";
    }
    cout << string(80, '=') << "\n";
}

//ГЛАВНАЯ ФУНКЦИЯ
int main(int argc, char* argv[]) {
    string inputFile; //имя входного файла
    string outputFile = "result.json"; //имя выходного файла
    string format = "iso"; //формат вывода
    int genCount = 0; //кол-во генерируемых дат
    bool benchMode = false; //режим бенчмарка(выклчюен)
     
    // Разбор аргументов командной строки
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];

        if (arg == "--help") {
            printHelp();
            return 0;
        }
        else if (arg == "--input" && i + 1 < argc) {
            inputFile = argv[++i];
        }
        else if (arg == "--output" && i + 1 < argc) {
            outputFile = argv[++i];
        }
        else if (arg == "--to" && i + 1 < argc) {
            format = argv[++i];
        }
        else if (arg == "--generate" && i + 1 < argc) {
            genCount = stoi(argv[++i]);
        }
        else if (arg == "--bench" && i + 1 < argc) {
            inputFile = argv[++i];
            benchMode = true;
        }
    }

    //Режим генерации
    if (genCount > 0) {
        generateJSON(genCount, "generated.json");
        cout << "File saved to generated.json\n";
        return 0;
    }

    //Если нет входного файла — ошибка
    if (inputFile.empty()) {
        cerr << "Error: No input file. Use --help\n";
        return 1;
    }

    //попытка открыть файл
    ifstream file(inputFile);
    if (!file.is_open()) {
        cerr << "Error: Cannot open " << inputFile << "\n";
        return 1;
    }
    //подготовка к чтению
    vector<DateRecord> records; //массив для всех записей
    string line; //для чтенич строк
    int valid = 0, invalid = 0; //счетчики валидных/невалидных дат

    auto start = high_resolution_clock::now(); //запоминаем время начала

    // Чтение и парсинг файла
    while (getline(file, line)) { //чтение построчно
        size_t p = line.find("\"raw_date\""); //поиск слова raw_date
        if (p == string::npos) continue; //если его нет - пропускаем строку

        size_t s = line.find(':', p); //поиск двоеточия
        size_t q1 = line.find('\"', s); //поиск открывающей кавычки
        size_t q2 = line.find('\"', q1 + 1); //поиск закрывабщей кавычки
        if (q1 == string::npos || q2 == string::npos) continue; //если нет - пропуск
        
        //вырезаем текст между кавычками - это и есть дата
        string date = line.substr(q1 + 1, q2 - q1 - 1);
        
        DateRecord r = parseDate(date); //парсирование даты
        records.push_back(r); //добавляем в массив
        if (r.is_valid) valid++; else invalid++; //подсчет статистики
    }
    file.close();

    //замер времени
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);

    // Режим бенчмарка
    if (benchMode) {
        cout << "\n=== BENCHMARK RESULTS ===\n";
        cout << "File: " << inputFile << "\n";
        cout << "Records: " << records.size() << "\n";
        cout << "Valid: " << valid << "\n";
        cout << "Invalid: " << invalid << "\n";
        cout << "Time: " << duration.count() << " ms\n";
        cout << "Speed: " << (records.size() * 1000.0 / duration.count()) << " records/sec\n";
        return 0;
    }

    //Вывод таблицы
    printTable(records, format);

    //Сохранение результата
    ofstream out(outputFile);
    out << "[\n";
    for (size_t i = 0; i < records.size(); i++) {
        out << "  {\"raw_date\":\"" << records[i].raw_date << "\","
            << "\"iso_date\":\"" << records[i].iso_date << "\",";

        if (records[i].is_valid) {
            if (format == "ru_long") {
                out << "\"converted\":\"" << isoToRussian(records[i].iso_date) << "\",";
            }
            else {
                out << "\"converted\":\"" << records[i].iso_date << "\",";
            }
        }
        else {
            out << "\"converted\":\"\",";
        }

        out << "\"is_valid\":" << (records[i].is_valid ? "true" : "false") << ","
            << "\"error\":\"" << records[i].error_message << "\"}";
        if (i < records.size() - 1) out << ",";
        out << "\n";
    }
    out << "]\n";
    out.close();
    //сообщение куда сохранен файл 
    cout << "\nResults saved to " << outputFile << "\n";
    return 0;
}