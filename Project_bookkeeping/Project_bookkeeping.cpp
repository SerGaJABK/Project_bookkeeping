#include <iostream> // ввод-вывод
#include <fstream> // для работы с файлами
#include <vector>  
#include <algorithm> // алгоритмы STL (find)
#include <iomanip> // форматирование вывода (setw)
#include <sstream> // строковые потоки
#include <limits>   // для numeric_limits
#include <locale>  // Для поддержки UTF-8 в Linux

#ifdef _WIN32   // Только для Windows, кодировка
#define NOMINMAX // Исключаем макросы min и max из windows.h из-за конфликтов с STL
#include <windows.h>  
#endif

using namespace std;  

void initConsoleEncoding() { //инициализация кодировки для Win
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
    const string filename = "library.txt"; //Имя файла для сохранения/загрузки

public:
    // Добавление книги
    void addBook(const Book& book) {
        books.push_back(book);
    }

    // списание книги по названию
    bool removeBook(const string& name) {
        auto it = find_if(books.begin(), books.end(), // Ищем книгу по названию
            [&name](const Book& b) { return b.name == name; });

        if (it != books.end()) { // Если книга найдена
            it->isAvailable = false;  // Помечаем как списанную
            return true;
        }
        return false;
    }

    // Поиск книг по критериям (название/автор/жанр)
    vector<Book> findBooks(const string& keyword, const string& searchBy = "name") const {
        vector<Book> result; // Вектор для хранения найденных книг
        for (const auto& book : books) { // Перебираем все книги
            bool match = false; // Флаг совпадения
            if (searchBy == "name" && book.name.find(keyword) != string::npos) { // Поиск по названию
                match = true;
            }
            else if (searchBy == "author" && book.author.find(keyword) != string::npos) { // Поиск по автору
                match = true;
            }
			else if (searchBy == "genre" && book.genre.find(keyword) != string::npos) { //Поиск по жанру
                match = true;
            }
            if (match) { // Если совпадение найдено
                result.push_back(book); // Добавляем книгу в результат
            }
        }
        return result;
    }

    // Сортировка книг по году издания
    vector<Book> getBooksSortedByYear() const { 
        vector<Book> sortedBooks = books; // Копируем книги
        sort(sortedBooks.begin(), sortedBooks.end(), // Сортируем по году
            [](const Book& a, const Book& b) { return a.year < b.year; });
        return sortedBooks; // Возвращаем отсортированный вектор
    }

    // Сохранение данных в файл
    bool saveToFile() const {
        ofstream outFile(filename); // Открываем файл для записи
        if (!outFile) return false; // Если не удалось открыть, возвращаем false

        for (const auto& book : books) { // Перебираем все книги
            outFile << book.name << "," // Записываем название
                << book.author << "," // Записываем автора
                << book.genre << "," // Записываем жанр
                << book.year << "," // Записываем год
                << book.isAvailable << "\n"; // Записываем статус (1/0)
        }
        outFile.close(); // Закрываем файл
        return true;
    }

    // Загрузка данных из файла
    bool loadFromFile() {
        ifstream inFile(filename); // Открываем файл для чтения
        if (!inFile) return false; // Если не удалось открыть, возвращаем false

        books.clear();  // Очищаем текущий список книг
        string line; // Строка для чтения из файла
        while (getline(inFile, line)) { // Читаем файл построчно
            stringstream ss(line); // Создаём строковый поток
            Book book; // Создаём объект книги
            getline(ss, book.name, ','); // Считываем название
            getline(ss, book.author, ','); // Считываем автора
            getline(ss, book.genre, ','); // Считываем жанр

            string yearStr, availableStr; // Строки для года и статуса
            getline(ss, yearStr, ','); // Считываем год
			getline(ss, availableStr); // Считываем доступна/списана

			book.year = stoi(yearStr); // Преобразуем год в int
			book.isAvailable = (availableStr == "1"); // Преобразуем статус 1 - доступна 0 - списана

			books.push_back(book); // Добавляем книгу в вектор
        }
		inFile.close(); // Закрываем файл
        return true;
    }

    // Печать всех книг (с фильтром по жанру, если указан)
    void printAllBooks(const string& genreFilter = "") const { 
        if (books.empty()) { // Если коллекция пуста
            cout << "Библиотека пуста.\n"; 
            return;
        }

        cout << "\nСписок книг:\n"; // вывод заголовка
        cout << "--------------------------------------------------------------------------\n";
        cout << setw(20) << left << "Название"
            << setw(20) << "Автор"
            << setw(15) << "Жанр"
            << setw(10) << "Год"
            << "Статус\n";
        cout << "--------------------------------------------------------------------------\n";

        for (const auto& book : books) { // Перебираем все книги
            if (genreFilter.empty() || book.genre == genreFilter) { // Фильтр по жанру
                cout << setw(20) << left << book.name
                    << setw(20) << book.author
                    << setw(15) << book.genre
                    << setw(10) << book.year
                    << setw(10) << (book.isAvailable ? "Доступна" : "Списана") << "\n";
            }
        }
    }

    // Редактирование книги по названию
    bool editBook(const string& name) {
        auto it = find_if(books.begin(), books.end(), // Ищем книгу по названию
            [&name](const Book& b) { return b.name == name; });

        if (it != books.end()) { // Если книга найдена
            cout << "Редактирование книги:\n";
			cout << "Текущее название: " << it->name << "\n"; // указатель на текущую книгу
            cout << "Введите новое название (или оставьте пустым для сохранения текущего): ";
            string newName;
            getline(cin, newName);
			if (!newName.empty()) { // Если введено новое название
                it->name = newName; 
            }

            cout << "Текущий автор: " << it->author << "\n";
            cout << "Введите нового автора (или оставьте пустым для сохранения текущего): ";
            string newAuthor;
            getline(cin, newAuthor);
            if (!newAuthor.empty()) {
                it->author = newAuthor;
            }

            cout << "Текущий жанр: " << it->genre << "\n";
            cout << "Введите новый жанр (или оставьте пустым для сохранения текущего): ";
            string newGenre;
            getline(cin, newGenre);
            if (!newGenre.empty()) {
                it->genre = newGenre;
            }

            cout << "Текущий год издания: " << it->year << "\n";
            cout << "Введите новый год издания (или оставьте пустым для сохранения текущего): ";
            string newYearStr;
            getline(cin, newYearStr);
            if (!newYearStr.empty()) {
				it->year = stoi(newYearStr); // Преобразуем строку в int
            }

            return true;
        }
        return false;
    }
};

// Функция для ввода данных о книге с клавиатуры
Book inputBook() {
    Book book; // Создаём объект книги
    while (true) {
        cout << "Введите название книги: ";
        getline(cin, book.name);
		if (book.name.empty()) { // Проверяем, что название не пустое
            cout << "Название не может быть пустым. ";
        } else {break;}
    }

    while (true) {
        cout << "Введите автора: ";
        getline(cin, book.author);
		if (book.author.empty()) {  // Проверяем, что автор не пустой
            cout << "Автор не может быть пустым. ";
        } else {break;}
    }

    while (true) {
        cout << "Введите жанр: ";
        getline(cin, book.genre);
		if (book.genre.empty()) { // Проверяем, что жанр не пустой
            cout << "Жанр не может быть пустым. ";
        } else {break;}
    }

    // Проверка корректности ввода года
    while (true) {
        cout << "Введите год издания: ";
		cin >> book.year; 
		if (book.genre.empty() || book.year <= 0) { // Проверяем, что год положительный и не путой
            cout << "Ошибка: введите корректный числовой год.\n";
        } else {break;}
    }

    book.isAvailable = true; // Книга по умолчанию доступна
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
        << "7. Редактировать книгу\n"
        << "0. Выход\n"
        << "Выберите действие: ";
}

int main() { 
    initConsoleEncoding(); // Инициализация кодировки консоли

    Library lib; // Создаём объект библиотеки
    int choice; // Переменная для выбора пользователя


    if (lib.loadFromFile()) { // Пытаемся загрузить данные из файла
        cout << "Данные успешно загружены из файла.\n";
    }
    else {
        cout << "Файл не найден. Начните с пустой библиотеки.\n";
    }

    do { // Основной цикл программы
        displayMenu(); // Показываем меню
        cin >> choice; // Считываем выбор пользователя
		cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Очищаем буфер ввода

        switch (choice) { // Обработка выбора пользователя
        case 1: { // Добавить книгу
            Book book = inputBook(); // Вводим данные о книге
            lib.addBook(book); // Добавляем книгу в библиотеку
            cout << "Книга добавлена.\n";
            break;
        }
        case 2: { // Списать книгу
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
		case 3: { // Найти книгу
            string keyword, searchBy; // Переменные для поиска
            cout << "Введите ключевое слово для поиска: ";
            getline(cin, keyword);
            cout << "Искать по (name/author/genre): ";
            getline(cin, searchBy);

            vector<Book> found = lib.findBooks(keyword, searchBy); // Ищем книги
            if (found.empty()) {
                cout << "Книги не найдены.\n";
            }
            else {
                cout << "\nНайдено " << found.size() << " книг:\n";
                for (const auto& book : found) { // Перебираем найденные книги
                    cout << book.name << " | " << book.author << " | " << book.genre
                        << " | " << book.year << " | " << (book.isAvailable ? "Доступна" : "Списана"); "\n";
                }
            }
            break;
        }
        case 4: { // Показать все книги (с фильтром по жанру)
            string genreFilter; // Переменная для фильтра
            cout << "Введите жанр для фильтра (или оставьте пустым): ";
            getline(cin, genreFilter);
            lib.printAllBooks(genreFilter);
            break;
        }
        case 5: { // Сохранить данные
            if (lib.saveToFile()) {
                cout << "Данные сохранены в файл.\n";
            }
            else {
                cout << "Ошибка при сохранении.\n";
            }
            break;
        } 
        case 6: { // Загрузить данные
            if (lib.loadFromFile()) {
                cout << "Данные загружены.\n";
            }
            else {
                cout << "Ошибка при загрузке.\n";
            }
            break;
        }
        case 7: { // Редактировать книгу
            string name;
            cout << "Введите название книги для редактирования: ";
            getline(cin, name);
            if (lib.editBook(name)) {
                cout << "Книга успешно отредактирована.\n";
            } else {
                cout << "Книга не найдена.\n";
            }
            break;
        }
		case 0: { // Выход из программы
            cout << "Выход из программы.\n";
            break;
        }
        default: {
            cout << "Неверный выбор. Попробуйте снова.\n";
        }
        }
    } while (choice != 0); // Повторяем, пока не выбран выход

    return 0;
}
