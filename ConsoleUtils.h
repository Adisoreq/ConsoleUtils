// ConsoleUtils

#pragma once
#ifndef _ConsoleUtils_
#define _ConsoleUtils_

#include <string>
#include <stdio.h>
#include <fstream>
#include <sstream>

#ifdef _WIN32
#include <conio.h>
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif // _WIN32

#ifndef UINT
#define UINT unsigned int
#endif

namespace ConsoleUtils
{
    struct CursorPosition {
        UINT x, y;
        CursorPosition(UINT x, UINT y) : x(x), y(y) {}
    };
}

class Cursor
{
#ifdef _WIN32
private:
    inline static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
public:
    static void SetVisibility(const bool visible) {
#ifdef _WIN32
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
        cursorInfo.bVisible = (visible ? TRUE : FALSE);
        SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
#else
        std::printf("\033[?25%c", visible ? 'h' : 'l');
#endif
    }
    static void Show() { SetVisibility(true); }
    static void Hide() { SetVisibility(false); }

    static void SetPosition(UINT x, UINT y) {
#ifdef _WIN32
        COORD coord = { static_cast<SHORT>(x), static_cast<SHORT>(y) };
        if (!SetConsoleCursorPosition(hConsole, coord)) {
            std::fprintf(stderr, "Failed to set cursor position.");
        }
#else
        std::printf("\033[%d;%dH", y, x);
#endif
    }

    static ConsoleUtils::CursorPosition GetPosition() {
        UINT x = 0, y = 0;
#ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(hConsole, &csbi);
        x = csbi.dwCursorPosition.X;
        y = csbi.dwCursorPosition.Y;
#else
        std::printf("\033[6n");
        std::getline(std::ws);
        char ch;
        std::string response;
        while (std::cin.get(ch) && ch != 'R') { response += ch; }
        size_t semicolon = response.find(';');
        if (semicolon != std::string::npos) {
            y = std::stoi(response.substr(2, semicolon - 2));
            x = std::stoi(response.substr(semicolon + 1));
        }
#endif
        return ConsoleUtils::CursorPosition(x, y);
    }

    static void ResetPosition() {
#ifdef _WIN32
        SetPosition(1, 1);
#else
        std::printf("\033[H");
#endif
    }
};

namespace Keyboard
{
    class Key {
    private:
        unsigned char _B[2];

    public:
        Key(const unsigned char& B1, const unsigned char& B2 = 0) {
            _B[0] = B1;
            _B[1] = B2;
        }

        inline unsigned char get(const size_t& byte = 0) const noexcept {
            return (byte < 2) ? _B[byte] : 0;
        }

        bool extended() const { return (_B[1] != 0); }

        bool operator==(const Key& other) const {
            return (_B[0] == other.get(0) && _B[1] == other.get(1));
        }

        std::string toString() const {
            if (!extended()) {
                return std::string(1, _B[0]);
            }
            else {
                return "[" + std::string(1, _B[0]) + ", " + std::string(1, _B[1]) + "]";
            }
        }

        bool isDigit() const { return !extended() && (_B[0] >= '0' && _B[0] <= '9'); }
        bool isLetter() const { return isLowerCaseLetter() || isUpperCaseLetter(); }
        bool isUpperCaseLetter() const { return !extended() && (_B[0] >= 'A' && _B[0] <= 'Z'); }
        bool isLowerCaseLetter() const { return !extended() && (_B[0] >= 'a' && _B[0] <= 'z'); }
        bool isSpecialCharacter() const {
            return !extended() && (
                (_B[0] >= '!' && _B[0] <= '/') ||
                (_B[0] >= ':' && _B[0] <= '@') ||
                (_B[0] >= '[' && _B[0] <= '`') ||
                (_B[0] >= '{' && _B[0] <= '~')
                );
        }
        bool isFunctionKey() const { return extended() && (_B[0] == 0 || _B[0] == 224); }
        bool isControlKey() const { return (_B[0] >= 1 && _B[0] <= 26); }
        bool isArrowKey() const { return extended() && (_B[0] == 224) && (_B[1] >= 72 && _B[1] <= 80); }
    };

    Key ReadKey() {
#ifdef _WIN32
        unsigned char first = _getch();  // Now using unsigned char to handle all codes properly
        if (first == 0 || first == 224) { // Check for extended keys (such as arrow keys)
            unsigned char second = _getch();
            return Key(first, second);
        }
        return Key(first);
#else
        struct termios oldt, newt;
        unsigned char character[2];

        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        character[0] = getchar();
        if (character[0] == 27) { // ESC key, start of an extended sequence
            getchar();  // Skip the [
            character[1] = getchar();
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
            return Key(224, character[1]); // Return extended key code
        }

        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return Key(character[0]);
#endif
    }
}

class Console
{
private:
#ifdef _WIN32
    inline static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
    static void Print(const std::ostringstream& oss) { std::printf("%s", oss.str().c_str()); }
    static void Err(const std::ostringstream& oss) { std::fprintf(stderr, oss.str().c_str()); }

public:
    enum class Color {
#ifdef _WIN32
        Black = 0, DarkBlue = 1, DarkGreen = 2, DarkCyan = 3, DarkRed = 4, DarkMagenta = 5, DarkYellow = 6, Gray = 7,
        DarkGray = 8, Blue = 9, Green = 10, Cyan = 11, Red = 12, Magenta = 13, Yellow = 14, White = 15
#else
        Black = 30, Red = 31, Green = 32, Yellow = 33, Blue = 34, Magenta = 35, Cyan = 36, White = 37,
        Gray = 90, LightRed = 91, LightGreen = 92, LightYellow = 93, LightBlue = 94, LightMagenta = 95, LightCyan = 96, BrightWhite = 97
#endif
    };

    static void SetColor(Color color) {
#ifdef _WIN32
        SetConsoleTextAttribute(hConsole, static_cast<int>(color));
#else
        std::printf("\033[%um", static_cast<int>(color));
#endif
    }

    static void ResetColor() {
#ifdef _WIN32
        SetColor(Color::White);
#else
        std::printf("\033[0m");
#endif
    }

    static Keyboard::Key ReadKey() { return Keyboard::ReadKey(); }

    static std::string ReadFile(const std::string& directory) {
        std::string result;
        std::ifstream file(directory);
        if (file && file.is_open()) {
            std::string line;
            while (std::getline(file, line)) { result += line + '\n'; }
            file.close();
        }
        return result;
    }

    template <typename T> static void Write(const T& data) {
        std::ostringstream oss;
        oss << data;
        Print(oss);
    }

    template <typename T> static void Write(T* data) { Write(*data); }

    static void Write(const char data[]) {
        std::ostringstream oss;
        for (size_t i = 0; i < strlen(data); i++) {
            oss << data[i];
        }
        Print(oss);
    }

    template <typename T> static void Error(const T& data) {
        std::ostringstream oss;
        oss << data;
        Error(oss);
    }
    template <typename T> static void Error(T* data) { Error(*data); }

    static void WriteLine(const size_t& lines = 1) {
        std::ostringstream oss;
        for (int i = 0; i < lines; i++) { oss << '\n'; }
        Print(oss);
    }

    static void Clear() {
#ifdef _WIN32
        COORD topLeft = { 0, 0 };
        DWORD written;
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
            DWORD size = csbi.dwSize.X * csbi.dwSize.Y;
            FillConsoleOutputCharacter(hConsole, ' ', size, topLeft, &written);
            FillConsoleOutputAttribute(hConsole, csbi.wAttributes, size, topLeft, &written);
            SetConsoleCursorPosition(hConsole, topLeft);
        }
#else
        std::printf("\033[2J\033[1;1H");
#endif
    }

    static void SetTitle(const std::wstring& title) {
#ifdef _WIN32
        SetConsoleTitle(title.c_str());
#else
        std::printf("\033]0;%s\007", title.c_str());
#endif
    }
    static void SetTitle(const std::string& title) {
        std::wstring wTitle(title.begin(), title.end());
        SetTitle(wTitle);
    }
};

#endif // !_ConsoleUtils_