#include <termios.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <algorithm>
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
namespace levenshtein_sort // sorts std::vector<std::string> by levenshtein distance
{
    int levenshteinDistance(const std::string &word1, const std::string &word2)
    {
        int m = word1.length();
        int n = word2.length();

        std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));

        for (int i = 1; i <= m; ++i)
        {
            dp[i][0] = i;
        }

        for (int j = 1; j <= n; ++j)
        {
            dp[0][j] = j;
        }

        for (int i = 1; i <= m; ++i)
        {
            for (int j = 1; j <= n; ++j)
            {
                if (word1[i - 1] == word2[j - 1])
                {
                    dp[i][j] = dp[i - 1][j - 1];
                }
                else
                {
                    dp[i][j] = 1 + std::min({dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]});
                }
            }
        }

        return dp[m][n];
    }

    bool cmp(const std::string &word1, const std::string &word2, const std::string &target)
    {
        return levenshteinDistance(word1, target) > levenshteinDistance(word2, target);
    }

    void sort(std::string target, std::vector<std::string> &words)
    {
        std::sort(words.begin(), words.end(), [&](const std::string &a, const std::string &b)
                  { return cmp(a, b, target); });
    }
};
int main()
{

    return 0;
}