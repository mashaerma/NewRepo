#include "../include/date_parser.h" 
#include <fstream> //для записи в файл
#include <iostream> //для вывода на экран
#include <string> //для работы со строками
#include <random> //для генерации случайных чисел

using namespace std;

void generateJSON(int count, const string& filename) {
    random_device rd; //получение рандомных чисел
    mt19937 gen(rd()); //основной генератор случайных чисел
    
    //диапозоны для рандома
    uniform_int_distribution<> year_dist(2000, 2030); //диапозоны года
    uniform_int_distribution<> month_dist(1, 12); //месяцы
    uniform_int_distribution<> format_dist(0, 3); //формат даты: 0,1,2,3
    uniform_int_distribution<> error_dist(0, 9); //для 10% ошибок

    //массив с названиями месяцев для создаия дат в текстовом формате 
    const char* months[] = {"january", "february", "march", "april", "may", "june",
                            "july", "august", "september", "october", "november", "december"};


    //открываем файл для записи
    ofstream file(filename); //открывает один файл
    file << "[\n"; //начало JSON-массива


    //основной цикл генерации количество дат, создается одна случайная дата
    for (int i = 0; i < count; i++) {
        string date; 
        
        //вероятность 10% получить ошибочную дату
        if (error_dist(gen) == 0) { //если выпало 0(шанс 1 к 10)
            //массив с неправильными датами
            const char* errors[] = {"invalid date", "", "2025-13-40", "32-13-2025"};
            //выьираем случайную ошибку из четырех
            date = errors[error_dist(gen) % 4]; //выбор случайной ошибки(одна из 4 строк)
        } else {
            
            //генерация случайного года, месяца
            int year = year_dist(gen);
            int month = month_dist(gen);
            
            //учитываем кол-во дней в месяце(и високосный код(1 если високосный,0 если нет))
            int daysInMonth[] = { 31, 28 + isLeapYear(year), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
            //генерация дня от 1 до максимального в месяце
            uniform_int_distribution<> day_dist(1, daysInMonth[month - 1]);
            int day = day_dist(gen); 

            
            //выбор формата
            int format = format_dist(gen); //случайный выбор формата(0,1,2,3)
            
            //формируем дату в разных форматах
            switch (format) {
                case 0: // YYYY-MM-DD (ISO)
                    date = to_string(year) + "-" + (month < 10 ? "0" : "") + to_string(month) + "-" + (day < 10 ? "0" : "") + to_string(day);
                    break;
                case 1: // DD-MM-YYYY (Европейский)
                    date = (day < 10 ? "0" : "") + to_string(day) + "-" + (month < 10 ? "0" : "") + to_string(month) + "-" + to_string(year);
                    break;
                case 2: // MM/DD/YYYY (Американский)
                    date = (month < 10 ? "0" : "") + to_string(month) + "/" + (day < 10 ? "0" : "") + to_string(day) + "/" + to_string(year);
                    break;
                case 3: // DD month YYYY (Текстовый)
                    date = to_string(day) + " " + months[month - 1] + " " + to_string(year);
                    break;
            }
        }
        //записываем  готовую дату в файл
        file << "  {\"raw_date\": \"" << date << "\"}";
        //если это не последняя дата, то ставим запятую
        if (i < count - 1) file << ",";
        file << "\n";
    }

    file << "]\n"; //закрываем JSON-файл
    file.close(); //закрываем файл
    cout << "Generated " << count << " dates in " << filename << " (10% errors)\n";
}