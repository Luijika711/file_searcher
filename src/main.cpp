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
#define MOD (INT32_MAX - 1)
#define hash_multiply 31

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
        std::vector
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
    std::ifstream fin;

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
    bool is_match(int i, std::string str1, std::string str2)
    {
        bool match = true;
        for (int j = 0; j < str1.size(); ++j)
        {
            if (str2[i + j] != str1[j])
            {
                return false;
            }
        }
        if (match)
        {
            return true;
        }
    }
    int update_hash(int currentHash, std::string str1, std::string str2, int i)
    {
        int p = static_cast<int>(std::pow(hash_multiply, (int)((str1.size()) - 1)));

        currentHash = (currentHash - str2[i] * p) % MOD;
        if (currentHash < 0)
        {
            currentHash += MOD;
        }

        return (currentHash * hash_multiply + str2[i + (int)str1.size()]) % MOD;
    }
    bool isSubstring(const std::string &str1, const std::string &str2) // function that returns true if str1 is a substring of str2
    {

        int len1 = str1.length();
        int len2 = str2.length();

        int hashStr1 = 0;
        for (char c : str1)
        {
            hashStr1 = (hashStr1 * hash_multiply + c) % MOD;
        }

        int currentHash = 0;
        for (int i = 0; i < len1; ++i)
        {
            currentHash = (currentHash * hash_multiply + str2[i]) % MOD;
        }

        for (int i = 0; i <= len2 - len1; ++i)
        {
            if (hashStr1 == currentHash)
            {
                if (is_match(i, str1, str2))
                {
                    return true;
                }
            }
            if (i < str2.size() - str1.size())
            {
                currentHash = update_hash(currentHash, str1, str2, i);
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
        fin = std::ifstream("file_list.txt");
    }
    app()
    {
        start();
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

            //page_nr is the current page number
            //we have 10 lines per page
            //selected_index is the current highlighted filepath

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

            case 'w'://moves selected_index up,if it's already at the edge nothing happens
                if (selected_index)
                    selected_index--;
                if (selected_index / 10 + 1 != page_nr)
                    selected_index++;
                break;
            case 's'://moves selected_index down,if it's already at the edge nothing happens
                selected_index++;
                if (selected_index / 10 + 1 != page_nr)
                    selected_index -= 2;
                break;
            case 'a'://changes page
                if (page_nr > 1)
                {
                    page_nr--;
                    selected_index -= 10;
                }
                break;
            case 'd'://changes page
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