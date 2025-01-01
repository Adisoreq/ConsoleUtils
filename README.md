# ConsoleUtils

C++ header-only library to automate writing multiplatform console applications. Contains optimized and universal functions to manipulate user's terminal, suited for Windows, Mac and Linux.

## Example:

```cpp

#include "ConsoleUtils.h"
#include "Keys.h"

int main() {
    Console::SetTitle("Key Detector");

    while (true) {
        Console::Clear();

#ifdef _WIN32
        Console::SetColor(Console::Color::Cyan);
#else
        Console::SetColor(Console::Color::LightBlue);
#endif
        Console::Write("Key Detector v1.0.0");
        Console::ResetColor();

        std::string text = Console::ReadFile("path/file.txt"); // Will return empty string if file is empty or invalid

        Console::WriteLine(1); // Inserts 1 line

        Console::Write(text);
        Console::Write("\nInsert key please\n");

        Keyboard::Key k = Keyboard::ReadKey(); // Read the key input

        if (k == Keys::ESC) {
            break; // Shut down application if ESC clicked
        }
        else {
            Console::Clear();
            Console::Write("Selected key: ");
#ifdef _WIN32
            Console::SetColor(Console::Color::Yellow);
#else
            Console::SetColor(Console::Color::LightYellow);
#endif
            Console::Write(k.toString());
            Console::ResetColor();
            Console::Write(" (key code: " + std::to_string(static_cast<int>(k.get(0))) + ',' + std::to_string(static_cast<int>(k.get(1))) + ")\n");
        }
#ifdef _WIN32
        Console::SetColor(Console::Color::DarkGray);
#else
        Console::SetColor(Console::Color::Gray);
#endif
        Console::Write("\nPress any key to continue...\n");
        Console::ResetColor();
        Console::ReadKey(); // Wait for key input to proceed
    }

    return 0;
}

```

## FAQ

#### The library doesn't work on my IDE.

Please use IDE supporting uncommon STD libraries, like Visual Studio.

#### We already have cin, cout, endl, system("cls"/"clear") etc.

Indeed. However, ConsoleUtils functions are optimized to avoid using system commands and extra operations like buffer flushing, making them faster than commonly used STD input-output operations.

## Installing

Download header files from repo and put in your project folder.

## Requirements

- C++ (version 17+)
- valid STD libraries

## Author

[Adrian P. (Adisoreq)](https://github.com/Adisoreq)

## License

[![MIT License](https://img.shields.io/badge/License-MIT-green.svg)](https://choosealicense.com/licenses/mit/)
