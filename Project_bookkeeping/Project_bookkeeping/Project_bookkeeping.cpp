#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <limits>
#include <locale>  // Для поддержки UTF-8 в Linux

#ifdef _WIN32
#define NOMINMAX // Исключаем макросы min и max из windows.h из-за конфликтов с STL
#include <windows.h> // Только для Windows
#endif

using namespace std;

void initConsoleEncoding() {
#ifdef _WIN32
    SetConsoleOutputCP(1251); // UTF-8 в Windows
    SetConsoleCP(1251);
#else
    setlocale(LC_ALL, "en_US.UTF-8"); // UTF-8 в Linux
#endif
}



// Структура для хранения данных о книге
struct Book {
    string name;        // Название книги
    string author;      // Автор
    string genre;       // Жанр
    int year;           // Год издания
    bool isAvailable;   // Доступна ли книга (не списана)

};

// Класс для управления коллекцией книг
class Library {
private:
    vector<Book> books;
    const string filename = "library.txt";

public:
    // Добавление книги
    void addBook(const Book& book) {
        books.push_back(book);
    }

    // Удаление (списание) книги по названию
    bool removeBook(const string& name) {
        auto it = find_if(books.begin(), books.end(),
            [&name](const Book& b) { return b.name == name; });

        if (it != books.end()) {
            it->isAvailable = false;  // Помечаем как списанную
            return true;
        }
        return false;
    }

    // Поиск книг по критериям (название/автор/жанр)
    vector<Book> findBooks(const string& keyword, const string& searchBy = "name") const {
        vector<Book> result;
        for (const auto& book : books) {
            bool match = false;
            if (searchBy == "name" && book.name.find(keyword) != string::npos) {
                match = true;
            }
            else if (searchBy == "author" && book.author.find(keyword) != string::npos) {
                match = true;
            }
            else if (searchBy == "genre" && book.genre.find(keyword) != string::npos) {
                match = true;
            }
            if (match) {
                result.push_back(book);
            }
        }
        return result;
    }

    // Сортировка книг по году издания
    vector<Book> getBooksSortedByYear() const {
        vector<Book> sortedBooks = books;
        sort(sortedBooks.begin(), sortedBooks.end(),
            [](const Book& a, const Book& b) { return a.year < b.year; });
        return sortedBooks;
    }

    // Сохранение данных в файл
    bool saveToFile() const {
        ofstream outFile(filename);
        if (!outFile) return false;

        for (const auto& book : books) {
            outFile << book.name << ","
                << book.author << ","
                << book.genre << ","
                << book.year << ","
                << book.isAvailable << "\n";
        }
        outFile.close();
        return true;
    }

    // Загрузка данных из файла
    bool loadFromFile() {
        ifstream inFile(filename);
        if (!inFile) return false;

        books.clear();
        string line;
        while (getline(inFile, line)) {
            stringstream ss(line);
            Book book;
            getline(ss, book.name, ',');
            getline(ss, book.author, ',');
            getline(ss, book.genre, ',');

            string yearStr, availableStr;
            getline(ss, yearStr, ',');
            getline(ss, availableStr);

            book.year = stoi(yearStr);
            book.isAvailable = (availableStr == "1");

            books.push_back(book);
        }
        inFile.close();
        return true;
    }

    // Печать всех книг (с фильтром по жанру, если указан)
    void printAllBooks(const string& genreFilter = "") const {
        if (books.empty()) {
            cout << "Библиотека пуста.\n";
            return;
        }

        cout << "\nСписок книг:\n";
        cout << "--------------------------------------------------------------------------\n";
        cout << setw(20) << left << "Название"
            << setw(20) << "Автор"
            << setw(15) << "Жанр"
            << setw(10) << "Год"
            << "Статус\n";
        cout << "--------------------------------------------------------------------------\n";

        for (const auto& book : books) {
            if (genreFilter.empty() || book.genre == genreFilter) {
                cout << setw(20) << left << book.name
                    << setw(20) << book.author
                    << setw(15) << book.genre
                    << setw(10) << book.year
                    << setw(10) << (book.isAvailable ? "Доступна" : "Списана") << "\n";
            }
        }
    }
};

// Функция для ввода данных о книге с клавиатуры
// Функция для ввода данных о книге с клавиатуры
Book inputBook() {
    Book book;
    while (true) {
        cout << "Введите название книги: ";
        getline(cin, book.name);
        if (book.name.empty()) {
            cout << "Название не может быть пустым. ";
        } else {break;}
    }

    while (true) {
        cout << "Введите автора: ";
        getline(cin, book.author);
        if (book.author.empty()) {
            cout << "Автор не может быть пустым. ";
        } else {break;}
    }

    while (true) {
        cout << "Введите жанр: ";
        getline(cin, book.genre);
        if (book.genre.empty()) {
            cout << "Жанр не может быть пустым. ";
        } else {break;}
    }

    // Проверка корректности ввода года
    while (true) {
        cout << "Введите год издания: ";
        cin >> book.year;
        if (book.genre.empty() || book.year <= 0) {
            cout << "Ошибка: введите корректный числовой год.\n";
        } else {break;}
    }

    book.isAvailable = true;
    return book;
}

// Главное меню программы
void displayMenu() {
    cout << "\n=== Система учета книг в библиотеке ===\n"
        << "1. Добавить книгу\n"
        << "2. Списать книгу\n"
        << "3. Найти книгу\n"
        << "4. Показать все книги (с фильтром по жанру)\n"
        << "5. Сохранить данные\n"
        << "6. Загрузить данные\n"
        << "0. Выход\n"
        << "Выберите действие: ";
}

int main() {
    initConsoleEncoding();

    Library lib;
    int choice;


    if (lib.loadFromFile()) {
        cout << "Данные успешно загружены из файла.\n";
    }
    else {
        cout << "Файл не найден. Начните с пустой библиотеки.\n";
    }

    do {
        displayMenu();
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
        case 1: {
            Book book = inputBook();
            lib.addBook(book);
            cout << "Книга добавлена.\n";
            break;
        }
        case 2: {
            string name;
            cout << "Введите название книги для списания: ";
            getline(cin, name);
            if (lib.removeBook(name)) {
                cout << "Книга списана.\n";
            }
            else {
                cout << "Книга не найдена.\n";
            }
            break;
        }
        case 3: {
            string keyword, searchBy;
            cout << "Введите ключевое слово для поиска: ";
            getline(cin, keyword);
            cout << "Искать по (name/author/genre): ";
            getline(cin, searchBy);

            vector<Book> found = lib.findBooks(keyword, searchBy);
            if (found.empty()) {
                cout << "Книги не найдены.\n";
            }
            else {
                cout << "\nНайдено " << found.size() << " книг:\n";
                for (const auto& book : found) {
                    cout << book.name << " | " << book.author << " | " << book.genre
                        << " | " << book.year << " | " << (book.isAvailable ? "Доступна" : "Списана"); "\n";
                }
            }
            break;
        }
        case 4: {
            string genreFilter;
            cout << "Введите жанр для фильтра (или оставьте пустым): ";
            getline(cin, genreFilter);
            lib.printAllBooks(genreFilter);
            break;
        }
        case 5: {
            if (lib.saveToFile()) {
                cout << "Данные сохранены в файл.\n";
            }
            else {
                cout << "Ошибка при сохранении.\n";
            }
            break;
        }
        case 6: {
            if (lib.loadFromFile()) {
                cout << "Данные загружены.\n";
            }
            else {
                cout << "Ошибка при загрузке.\n";
            }
            break;
        }
        case 0: {
            cout << "Выход из программы.\n";
            break;
        }
        default: {
            cout << "Неверный выбор. Попробуйте снова.\n";
        }
        }
    } while (choice != 0);

    return 0;
}
