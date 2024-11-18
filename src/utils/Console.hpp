#pragma once

#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>

enum class Category
{
    PROMPT,
    SUCCESS,
    FAILURE,
    NOTICE,
    INCOMING,
    OUTGOING,
};

enum class Color
{
    BLACK = 0,
    RED = 1,
    GREEN = 2,
    YELLOW = 3,
    BLUE = 4,
    MAGENTA = 5,
    CYAN = 6,
    WHITE = 7,
};

struct Console
{
public:
    Console(std::string title = " ")
    : title(title)
    {
    }

    std::string prompt(std::string text)
    {
        std::cout << format(Category::PROMPT, text) << std::endl;
        std::cout << format(Category::PROMPT, "");
        return in();
    }

    void incoming(std::string content, std::string label = "")
    {
        std::string text = "\"" + content + "\"";

        if (label.size() > 0)
        {
            text = "(" + label + ") " + text;
        }

        out(format(Category::INCOMING, text));
    }

    void outgoing(std::string content, std::string label = "")
    {
        std::string text = "\"" + content + "\"";

        if (label.size() > 0)
        {
            text = "(" + label + ") " + text;
        }

        out(format(Category::OUTGOING, text));
    }

    void success(std::string text)
    {
        out(format(Category::SUCCESS, text));
    }

    void failure(std::string text)
    {
        out(format(Category::FAILURE, text));
    }

    void exception(std::exception const& e)
    {
        std::string text = "exception: ";
        text += e.what();
        out(format(Category::FAILURE, text));
    }

    void notice(std::string text)
    {
        out(format(Category::NOTICE, text));
    }

    void error(std::string text)
    {
        err(text);
        exit(EXIT_FAILURE);
    }

private:
    std::string format(Category category, std::string text)
    {
        std::string label;
        std::string icon; // TODO: design better icons
        Color color;

        switch (category)
        {
            case Category::PROMPT:
            label = "prompt";
            icon = "???";
            color = Color::YELLOW;
            break;

            case Category::SUCCESS:
            label = "success";
            icon = "(*)";
            color = Color::GREEN;
            break;

            case Category::FAILURE:
            label = "failure";
            icon = "/!\\";
            color = Color::RED;
            break;

            case Category::NOTICE:
            label = "notice";
            icon = "   ";
            color = Color::CYAN;
            break;

            case Category::INCOMING:
            label = "message";
            icon = "<--";
            color = Color::MAGENTA;
            break;

            case Category::OUTGOING:
            label = "message";
            icon = "-->";
            color = Color::MAGENTA;
            break;

            default:
            label = "~~~";
            icon = "~~~";
            color = Color::WHITE;
            break;
        }

        std::stringstream buffer;
        //buffer << "\033[3" << static_cast<int>(Color::BLUE) << "m";
        buffer << "[" << title << "]";
        //buffer << "\033[0m";
        //buffer << " | ";
        buffer << "\033[3" << static_cast<int>(color) << "m";
        //buffer << std::left << std::setw(7) << label;
        buffer << " " << icon;
        buffer << "\033[0m";
        buffer << " | ";
        buffer << text;
        return buffer.str();
    }

    std::string in()
    {
        std::lock_guard guard(mutex);
        std::string text;
        std::getline(std::cin, text);
        return text;
    }

    void out(std::string text)
    {
        std::lock_guard guard(mutex);
        std::cout << text << std::endl;
    }

    void err(std::string text)
    {
        std::lock_guard guard(mutex);
        std::cerr << text << std::endl;
    }

    std::string title;
    std::mutex mutex;
};
