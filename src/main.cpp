#include <termios.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include "color_console.hpp" //https://github.com/memoryInject/color-console
#include <math.h>
#include <stdio.h>

#define ESC_KEY 27
#define ENTER_KEY 10

namespace console
{
    static struct termios old, current;
    void initTermios(int echo)
    {
        tcgetattr(0, &old);
        current = old;
        current.c_lflag &= ~ICANON;
        if (echo)
        {
            current.c_lflag |= ECHO;
        }
        else
        {
            current.c_lflag &= ~ECHO;
        }
        tcsetattr(0, TCSANOW, &current);
    }
    void resetTermios(void)
    {
        tcsetattr(0, TCSANOW, &old);
    }
    char getch_(int echo)
    {
        char ch;
        initTermios(echo);
        ch = getchar();
        resetTermios();
        return ch;
    }
    char getch(void)
    {
        return getch_(0);
    }
    char getche(void)
    {
        return getch_(1);
    }
}

class app
{
private:
    std::vector<std::string> file_names;
    ColorConsole console;
    std::string str;
    std::string file_im_searching;

public:
    void print_highlighted(std::string str)
    {
        std::cout << console.get(str, {console.bg_black, console.bold, console.bg_white});
    }
    void print_highlighted(char ch)
    {
        std::string _str;
        _str.push_back(ch);
        print_highlighted(_str);
    }
    bool isSubstring(std::string str1, std::string str2)//implemented KMP algorithm, time complexity O(n + m)
    {
        int m = str1.length();
        int n = str2.length();

        std::vector<int> lps(m, 0);
        int len = 0;

        for (int i = 1; i < m;)
        {
            if (str1[i] == str1[len])
            {
                len++;
                lps[i] = len;
                i++;
            }
            else
            {
                if (len != 0)
                {
                    len = lps[len - 1];
                }
                else
                {
                    lps[i] = 0;
                    i++;
                }
            }
        }

        int i = 0;
        int j = 0;

        while (j < n)
        {
            if (str1[i] == str2[j])
            {
                i++;
                j++;
                if (i == m)
                {
                    return true;
                }
            }
            else
            {
                if (i != 0)
                {
                    i = lps[i - 1];
                }
                else
                {
                    j++;
                }
            }
        }

        return false;
    }

    void start()
    {
        system("clear");
        std::cout << "type the name of the file you are searching: ";
        std::cin >> file_im_searching;
        system("sudo chmod +x files.bash");
        system("sudo ./files.bash > /dev/null 2>&1");
    }
    app()
    {
        start();
        std::ifstream fin("file_list.txt");

        int page_nr = 1;
        int selected_index = 0;
        while (fin >> str)
        {
            if (isSubstring(file_im_searching, str))
            {
                file_names.push_back(str);
            }
        }
        remove("file_list.txt");
        int enter_count = 0;
        if (file_names.size() == 0)
        {
            print_highlighted("haven't found any files with this name!!\n");
            exit(0);
        }
        while (true)
        {
            system("clear");
            print_highlighted("press w/s to go up/down | press a/d to go next/previous page | press enter to open file | press esc to exit\n\n");

            // page_nr is the current page number
            // we have 10 lines per page
            // selected_index is the current highlighted filepath

            for (int i = (page_nr * 10 - 10); i < 10 * page_nr - 1; ++i)
            {
                if (i == selected_index)
                {
                    print_highlighted(file_names[i]);
                }
                else
                {
                    std::cout << file_names[i];
                }
                std::cout << "\n";
            }

            char ch = console::getch();
            switch (ch)
            {
            case ESC_KEY:
                exit(0);
                break;

            case ENTER_KEY:
                enter_count++;
                if (enter_count != 1)
                    system(("sudo nautilus " + file_names[selected_index] + "> /dev/null 2>&1 &").c_str());
                break;

            case 'w': // moves selected_index up,if it's already at the edge nothing happens
                if (selected_index)
                    selected_index--;
                if (selected_index / 10 + 1 != page_nr)
                    selected_index++;
                break;
            case 's': // moves selected_index down,if it's already at the edge nothing happens
                selected_index++;
                if (selected_index / 10 + 1 != page_nr)
                    selected_index -= 2;
                break;
            case 'a': // changes page
                if (page_nr > 1)
                {
                    page_nr--;
                    selected_index -= 10;
                }
                break;
            case 'd': // changes page
                if (((page_nr + 1) * 10) < file_names.size())
                {
                    page_nr++;
                    selected_index += 10;
                    selected_index %= file_names.size();
                }
                break;
            }
        }
    }
};
int main()
{
    app a;

    return 0;
}