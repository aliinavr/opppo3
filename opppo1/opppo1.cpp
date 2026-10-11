
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <windows.h>

using namespace std;

enum class CreationMethod
{
    DRAWN,
    PUPPET,
    CLAY,
    COMPUTER,
    OTHER
};

string creationMethodToString(CreationMethod method)
{
    switch (method)
    {
    case CreationMethod::DRAWN:
        return "DRAWN";

    case CreationMethod::PUPPET:
        return "PUPPET";

    case CreationMethod::CLAY:
        return "CLAY";

    case CreationMethod::COMPUTER:
        return "COMPUTER";

    default:
        return "OTHER";
    }
}

CreationMethod stringToCreationMethod(string value)
{
    if (value == "DRAWN")
    {
        return CreationMethod::DRAWN;
    }

    if (value == "PUPPET")
    {
        return CreationMethod::PUPPET;
    }

    if (value == "CLAY")
    {
        return CreationMethod::CLAY;
    }

    if (value == "COMPUTER")
    {
        return CreationMethod::COMPUTER;
    }

    return CreationMethod::OTHER;
}

class Film
{
protected:
    string title;

public:
    Film(const string& title)
        : title(title)
    {
    }

    virtual ~Film() = default;

    string getTitle() const
    {
        return title;
    }

    virtual string getType() const = 0;

    virtual void print() const = 0;

    virtual string getDirector() const
    {
        return "";
    }

    virtual int getEpisodes() const
    {
        return -1;
    }

    virtual string getCreationMethod() const
    {
        return "";
    }
};

class FeatureFilm : public Film
{
private:
    string director;

public:
    FeatureFilm(const string& title, const string& director)
        : Film(title), director(director)
    {
    }

    string getType() const override
    {
        return "FEATURE";
    }

    string getDirector() const override
    {
        return director;
    }

    void print() const override
    {
        cout << "Игровой фильм: "
            << "Название = " << title
            << ", режиссер = " << director
            << endl;
    }
};

class Cartoon : public Film
{
private:
    CreationMethod creationMethod;

public:
    Cartoon(const string& title, CreationMethod method)
        : Film(title), creationMethod(method)
    {
    }

    string getType() const override
    {
        return "CARTOON";
    }

    string getCreationMethod() const override
    {
        return creationMethodToString(creationMethod);
    }

    void print() const override
    {
        cout << "Мультфильм: "
            << "Название = " << title
            << ", способ создания = "
            << creationMethodToString(creationMethod)
            << endl;
    }
};

class Series : public Film
{
private:
    string director;
    int episodes;

public:
    Series(const string& title,
        const string& director,
        int episodes)
        : Film(title),
        director(director),
        episodes(episodes)
    {
    }

    string getType() const override
    {
        return "SERIES";
    }

    string getDirector() const override
    {
        return director;
    }

    int getEpisodes() const override
    {
        return episodes;
    }

    void print() const override
    {
        cout << "Сериал: "
            << "Название = " << title
            << ", режиссер = " << director
            << ", количество серий = " << episodes
            << endl;
    }
};

class ConditionParser
{
private:
    static string trim(const string& value)
    {
        size_t first = value.find_first_not_of(' ');

        if (first == string::npos)
        {
            return "";
        }

        size_t last = value.find_last_not_of(' ');

        return value.substr(first, last - first + 1);
    }

    static bool compareEpisodes(
        int episodes,
        const string& op,
        int value)
    {
        if (episodes == -1)
        {
            return false;
        }

        if (op == ">=")
        {
            return episodes >= value;
        }

        if (op == "<=")
        {
            return episodes <= value;
        }

        if (op == "==")
        {
            return episodes == value;
        }

        if (op == ">")
        {
            return episodes > value;
        }

        if (op == "<")
        {
            return episodes < value;
        }

        return false;
    }

public:
    static bool matches(
        const Film& film,
        const string& condition)
    {
        size_t position = condition.find(' ');

        if (position == string::npos)
        {
            return false;
        }

        string field = condition.substr(0, position);

        string rest = trim(condition.substr(position + 1));

        string op;

        if (rest.rfind(">=", 0) == 0)
        {
            op = ">=";
        }
        else if (rest.rfind("<=", 0) == 0)
        {
            op = "<=";
        }
        else if (rest.rfind("==", 0) == 0)
        {
            op = "==";
        }
        else if (rest.rfind(">", 0) == 0)
        {
            op = ">";
        }
        else if (rest.rfind("<", 0) == 0)
        {
            op = "<";
        }
        else
        {
            return false;
        }

        string value = trim(rest.substr(op.size()));

        if (value.empty())
        {
            return false;
        }

        if (field == "type" && op == "==")
        {
            return film.getType() == value;
        }

        if (field == "title" && op == "==")
        {
            if (value.size() >= 2 &&
                value.front() == '"' &&
                value.back() == '"')
            {
                value = value.substr(1, value.size() - 2);
            }

            return film.getTitle() == value;
        }

        if (field == "director" && op == "==")
        {
            return film.getDirector() == value;
        }

        if (field == "method" && op == "==")
        {
            return film.getCreationMethod() == value;
        }

        if (field == "episodes")
        {
            try
            {
                size_t parsed = 0;

                int number = stoi(value, &parsed);

                if (parsed != value.size())
                {
                    return false;
                }

                return compareEpisodes(
                    film.getEpisodes(),
                    op,
                    number
                );
            }
            catch (const invalid_argument&)
            {
                return false;
            }
            catch (const out_of_range&)
            {
                return false;
            }
        }

        return false;
    }
};

class FilmContainer
{
private:
    vector<unique_ptr<Film>> films;

public:
    void add(unique_ptr<Film> film)
    {
        films.push_back(std::move(film));
    }

    void print() const
    {
        if (films.empty())
        {
            cout << "Контейнер пуст." << endl;
            return;
        }

        cout << endl;
        cout << "Содержимое контейнера:" << endl;

        for (const auto& film : films)
        {
            film->print();
        }

        cout << endl;
    }

    void removeByCondition(const string& condition)
    {
        size_t oldSize = films.size();

        films.erase(
            remove_if(
                films.begin(),
                films.end(),
                [&](const unique_ptr<Film>& film)
                {
                    return ConditionParser::matches(
                        *film,
                        condition
                    );
                }
            ),
            films.end()
        );

        size_t removedCount = oldSize - films.size();

        cout << "Удалено объектов: "
            << removedCount << endl;
    }
};

vector<string> split(const string& line, char delimiter)
{
    vector<string> parts;

    stringstream stream(line);

    string item;

    while (getline(stream, item, delimiter))
    {
        parts.push_back(item);
    }

    return parts;
}

void processAdd(
    const string& data,
    FilmContainer& container)
{
    vector<string> parts =
        split(data, ';');

    if (parts.empty())
    {
        cout << "Ошибка команды ADD." << endl;
        return;
    }

    if (parts[0] == "FEATURE")
    {
        if (parts.size() != 3)
        {
            cout << "Ошибка параметров FEATURE."
                << endl;

            return;
        }

        container.add(
            make_unique<FeatureFilm>(
                parts[1],
                parts[2]
            )
        );

        cout << "Добавлен игровой фильм: "
            << parts[1]
            << endl;
    }

    else if (parts[0] == "CARTOON")
    {
        if (parts.size() != 3)
        {
            cout << "Ошибка параметров CARTOON."
                << endl;

            return;
        }

        CreationMethod method =
            stringToCreationMethod(parts[2]);

        container.add(
            make_unique<Cartoon>(
                parts[1],
                method
            )
        );

        cout << "Добавлен мультфильм: "
            << parts[1]
            << endl;
    }

    else if (parts[0] == "SERIES")
    {
        if (parts.size() != 4)
        {
            cout << "Ошибка параметров SERIES."
                << endl;

            return;
        }

        int episodes =
            stoi(parts[3]);

        container.add(
            make_unique<Series>(
                parts[1],
                parts[2],
                episodes
            )
        );

        cout << "Добавлен сериал: "
            << parts[1]
            << endl;
    }

    else
    {
        cout << "Неизвестный тип фильма."
            << endl;
    }
}

void processFile(const string& fileName)
{
    ifstream file(fileName);

    if (!file.is_open())
    {
        cout << "Не удалось открыть файл: "
            << fileName
            << endl;

        return;
    }

    FilmContainer container;

    string line;

    while (getline(file, line))
    {
        if (line.empty())
        {
            continue;
        }

        if (line.find("ADD ") == 0)
        {
            string data =
                line.substr(4);

            processAdd(
                data,
                container
            );
        }

        else if (line.find("REM ") == 0)
        {
            string condition =
                line.substr(4);

            cout << endl;

            cout << "Выполнение команды: "
                << line
                << endl;

            container.removeByCondition(
                condition
            );
        }

        else if (line == "PRINT")
        {
            container.print();
        }

        else
        {
            cout << "Неизвестная команда: "
                << line
                << endl;
        }
    }

    file.close();
}

int main()
{
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    processFile("commands.txt");

    return 0;
}
