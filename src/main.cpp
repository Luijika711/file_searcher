#include <termios.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <fstream>
#include "color_console.hpp" //https://github.com/memoryInject/color-console
#include <unordered_map>
#include <thread>
#include <mutex>
#include <math.h>
#include <stdio.h>

namespace console // getch function for linux
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
    bool isSubstring(const std::string &str1, const std::string &str2)
    {
        const int MOD = INT32_MAX - 1;
        int len1 = str1.length();
        int len2 = str2.length();

        int hashStr1 = 0;
        for (char c : str1)
        {
            hashStr1 = (hashStr1 * 31 + c) % MOD;
        }

        int currentHash = 0;
        for (int i = 0; i < len1; ++i)
        {
            currentHash = (currentHash * 31 + str2[i]) % MOD;
        }

        for (int i = 0; i <= len2 - len1; ++i)
        {
            if (hashStr1 == currentHash)
            {
                bool match = true;
                for (int j = 0; j < len1; ++j)
                {
                    if (str2[i + j] != str1[j])
                    {
                        match = false;
                        break;
                    }
                }
                if (match)
                {
                    return true;
                }
            }
            if (i < len2 - len1)
            {
                currentHash = (currentHash - str2[i] * static_cast<int>(std::pow(31, len1 - 1))) % MOD;
                if (currentHash < 0)
                {
                    currentHash += MOD;
                }
                currentHash = (currentHash * 31 + str2[i + len1]) % MOD;
            }
        }

        return false;
    }
    app()
    {
        int page_nr = 1;

        system("clear");
        std::cout << "type the name of the file you are searching: ";
        std::cin >> file_im_searching;
        system("sudo chmod +x files.bash");
        system("sudo ./files.bash > /dev/null 2>&1");
        std::ifstream fin("file_list.txt");

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
            if (ch == 27) // esc key
            {
                exit(0);
            }
            else if (ch == 10)
            {
                enter_count++;
                if (enter_count != 1)
                    system(("sudo nautilus " + file_names[selected_index] + "> /dev/null 2>&1 &").c_str());
            }
            else if (ch == 'w' && selected_index > 0)
            {
                selected_index--;
                if (selected_index / 10 + 1 != page_nr)
                    selected_index++;
            }
            else if (ch == 's')
            {
                selected_index++;
                if (selected_index / 10 + 1 != page_nr)
                    selected_index -= 2;
            }
            if (ch == 'a')
            {
                if (page_nr > 1)
                {
                    page_nr--;
                    selected_index -= 10;
                }
            }
            else if (ch == 'd')
            {
                if (((page_nr + 1) * 10) < file_names.size())
                {
                    page_nr++;
                    selected_index += 10;
                    selected_index %= file_names.size();
                }
            }
        }
    }
};
int main()
{
    app a;

    return 0;
}