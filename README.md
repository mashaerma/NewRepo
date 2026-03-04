# Конвертер дат — Лабораторная работа по C++

## Описание проекта
Консольное приложение для конвертации дат из различных форматов в ISO и обратно.  


**Основные возможности:**
- Загрузка данных из JSON-файлов
- Парсинг 4 форматов дат:
  - YYYY-MM-DD
  - DD-MM-YYYY
  - MM/DD/YYYY
  - DD month YYYY (например, "30 november 2025")
- Валидация дат (високосные годы, корректность дней/месяцев)
- Конвертация в русский длинный формат ("30 ноября 2025")
- Генерация тестовых JSON-файлов с датами
- Замер производительности
- Сохранение результатов в JSON


## Запуск программы
# Перейти в папку с программой
C:\Users\Мария\OneDrive\Desktop\date_converter_lab\DateConverterLab\x64\Debug

#Генерация дат
DateConverterLab.exe --generate 10

#Конвератция в ISO
DateConverterLab.exe --input generated.json --output result.json --to iso

#Бенчмарк(замер скорости)
DateConverterLab.exe --bench generated.json

#Конвертация в русский язык
DateConvertedLab.exe --input generated.json --output result.json --to ru_long

#Посмотреть что получилось
type generated.json

## Сруктура проекта
date_converter_lab/
├── src/ # Исходный код
│ ├── main.cpp
│ ├── date_parser.cpp
│ └── json_generator.cpp
├── include/ # Заголовочные файлы
│ └── date_parser.h
├── tests/ # Тесты
│ └── run_tests.cpp
├── data/ # Примеры JSON
│ ├── sample.json
│ └── generated.json
├── docs/ # Документация
│ ├── Implementation_Plan.md
│ └── bench.md
└── scripts/ # Вспомогательные скрипты
└── run_tests.bat


**Узкое место:** Парсинг текстовых названий месяцев(линейный поиск)
**Рекомендация:** Заменить на 'std::unordered_map' для ускорения


## Автор 
**Ермаченкова Мария**
Группа: МОАИС-О-25/1
Вариант: 5
