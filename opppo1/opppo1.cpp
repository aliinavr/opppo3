
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <exception>
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

    static bool compareTextField(
        const Film& film,
        const string& field,
        const string& value)
    {
        if (field == "type")
        {
            return film.getType() == value;
        }

        if (field == "director")
        {
            return film.getDirector() == value;
        }

        if (field == "method")
        {
            return film.getCreationMethod() == value;
        }

        if (field == "title")
        {
            string expectedTitle = value;

            if (expectedTitle.size() >= 2 &&
                expectedTitle.front() == '"' &&
                expectedTitle.back() == '"')
            {
                expectedTitle = expectedTitle.substr(
                    1, expectedTitle.size() - 2
                );
            }

            return film.getTitle() == expectedTitle;
        }

        return false;
    }

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
        const string& comparisonOperator,
        int value)
    {
        if (episodes == -1)
        {
            return false;
        }

        if (comparisonOperator == ">=")
        {
            return episodes >= value;
        }

        if (comparisonOperator == "<=")
        {
            return episodes <= value;
        }

        if (comparisonOperator == "==")
        {
            return episodes == value;
        }

        if (comparisonOperator == ">")
        {
            return episodes > value;
        }

        if (comparisonOperator == "<")
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

        if (op == "==" && field != "episodes")
        {
            return compareTextField(film, field, value);
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


class FilmFactory
{
public:
    static unique_ptr<Film> create(
        const vector<string>& parts)
    {
        if (parts.empty())
        {
            return nullptr;
        }

        if (parts[0] == "FEATURE")
        {
            if (parts.size() != 3)
            {
                return nullptr;
            }

            return make_unique<FeatureFilm>(
                parts[1],
                parts[2]
            );
        }

        if (parts[0] == "CARTOON")
        {
            if (parts.size() != 3)
            {
                return nullptr;
            }

            CreationMethod method =
                stringToCreationMethod(parts[2]);

            return make_unique<Cartoon>(
                parts[1],
                method
            );
        }

        if (parts[0] == "SERIES")
        {
            if (parts.size() != 4)
            {
                return nullptr;
            }

            try
            {
                size_t parsed = 0;

                int episodes = stoi(parts[3], &parsed);

                if (parsed != parts[3].size() ||
                    episodes <= 0)
                {
                    return nullptr;
                }

                return make_unique<Series>(
                    parts[1],
                    parts[2],
                    episodes
                );
            }
            catch (const invalid_argument&)
            {
                return nullptr;
            }
            catch (const out_of_range&)
            {
                return nullptr;
            }
        }

        return nullptr;
    }
};




void processAdd(
    const string& data,
    FilmContainer& container)
{
    vector<string> parts = split(data, ';');

    unique_ptr<Film> film =
        FilmFactory::create(parts);

    if (!film)
    {
        cout << "Ошибка параметров ADD." << endl;
        return;
    }

    string type = film->getType();
    string title = film->getTitle();

    container.add(std::move(film));

    cout << "Добавлен объект: "
        << type << " - "
        << title << endl;
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

    try
    {
        processFile("commands.txt");
    }
    catch (const std::exception& error)
    {
        cerr << "Ошибка выполнения: "
            << error.what() << endl;

        return 1;
    }

    return 0;
}