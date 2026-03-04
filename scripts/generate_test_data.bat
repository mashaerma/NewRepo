@echo off
echo === ГЕНЕРАТОР ТЕСТОВЫХ ДАННЫХ ===
echo.
echo Генерирую 100 дат...
..\build\DateConverter.exe --generate 100
echo.
echo Генерирую 1000 дат...
..\build\DateConverter.exe --generate 1000
echo.
echo Генерирую 10000 дат...
..\build\DateConverter.exe --generate 10000
echo.
echo Готово!
pause