@echo off
:: Вмикаємо підтримку UTF8 та ANSI
chcp 65001 >nul
powershell -NoProfile -ExecutionPolicy Bypass -Command "[Console]::OutputEncoding = [System.Text.Encoding]::UTF8; cls; ./logger_app.exe; pause"