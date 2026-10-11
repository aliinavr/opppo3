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
        return CreationMethod::DRAWN;
    }

    if (value == "CLAY")
    {
        return CreationMethod::DRAWN;
    }

    if(value == "COMPUTER")
    {
        return CreationMethod::DRAWN;
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
                    if (condition.find("type ==") == 0)
                    {
                        string value =
                            condition.substr(7);

                        while (!value.empty() &&
                            value.front() == ' ')
                        {
                            value.erase(value.begin());
                        }

                        return film->getType() == value;
                    }

                    if (condition.find("title ==") == 0)
                    {
                        string value =
                            condition.substr(8);

                        while (!value.empty() &&
                            value.front() == ' ')
                        {
                            value.erase(value.begin());
                        }

                        if (!value.empty() &&
                            value.front() == '"')
                        {
                            value.erase(value.begin());
                        }

                        if (!value.empty() &&
                            value.back() == '"')
                        {
                            value.pop_back();
                        }

                        return film->getTitle() == value;
                    }

                    if (condition.find("director ==") == 0)
                    {
                        string value =
                            condition.substr(11);

                        while (!value.empty() &&
                            value.front() == ' ')
                        {
                            value.erase(value.begin());
                        }

                        return film->getDirector() == value;
                    }

                    if (condition.find("method ==") == 0)
                    {
                        string value =
                            condition.substr(9);

                        while (!value.empty() &&
                            value.front() == ' ')
                        {
                            value.erase(value.begin());
                        }

                        return film->getCreationMethod() == value;
                    }

                    if (condition.find("episodes >=") == 0)
                    {
                        int value =
                            stoi(condition.substr(11));

                        int episodes =
                            film->getEpisodes();

                        return episodes != -1 &&
                            episodes >= value;
                    }

                    if (condition.find("episodes <=") == 0)
                    {
                        int value =
                            stoi(condition.substr(11));

                        int episodes =
                            film->getEpisodes();

                        return episodes != -1 &&
                            episodes <= value;
                    }

                    if (condition.find("episodes ==") == 0)
                    {
                        int value =
                            stoi(condition.substr(11));

                        int episodes =
                            film->getEpisodes();

                        return episodes != -1 &&
                            episodes == value;
                    }

                    if (condition.find("episodes >") == 0)
                    {
                        int value =
                            stoi(condition.substr(10));

                        int episodes =
                            film->getEpisodes();

                        return episodes != -1 &&
                            episodes > value;
                    }

                    if (condition.find("episodes <") == 0)
                    {
                        int value =
                            stoi(condition.substr(10));

                        int episodes =
                            film->getEpisodes();

                        return episodes != -1 &&
                            episodes < value;
                    }

                    return false;
                }
            ),

            films.end()
        );

        size_t removedCount =
            oldSize - films.size();

        cout << "Удалено объектов: "
            << removedCount
            << endl;
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