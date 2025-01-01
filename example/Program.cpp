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