#include <cstdlib>
#include <ctime>

#include <sys/stat.h>

#include "HashTable.hpp"

std::size_t fileSize(const char *name);
std::size_t lineCount(const char *strg);
void readLinesFromStorage(char *strg, const char *lines[]);

signed main()
{
    const std::size_t nLookUps = 100000000;
    std::srand(time(nullptr));

    const std::size_t fileSz = fileSize("words.txt");
    std::FILE *wordStream = std::fopen("words.txt", "rb");
    auto strg = new char[fileSz + 1]{};
    std::fread(strg, sizeof(*strg), fileSz, wordStream);
    std::fclose(wordStream);

    const std::size_t nLines = lineCount(strg);
    auto lines = new const char *[nLines]{};
    readLinesFromStorage(strg, lines);

    HashTable hashTable{elfHash};

    for (std::size_t i = 0; i < nLines; ++i) hashTable.insert(lines[i], "");

    for (std::size_t i = 0; i < nLookUps; ++i) hashTable.find(lines[std::rand() % nLines]);

    delete[] lines;
    delete[] strg;

    return EXIT_SUCCESS;
}

std::size_t fileSize(const char *const name)
{
    struct stat buf{};
    stat(name, &buf);

    return buf.st_size;
}

std::size_t lineCount(const char *strg)
{
    if (*strg == '\0') return 0;

    std::size_t nLines = 0;
    while (*strg != '\0') {
        if (*strg++ == '\n') ++nLines;
    }

    return nLines + 1;
}

void readLinesFromStorage(char *strg, const char *lines[])
{
    const char *currLine = strg;
    std::size_t idx = 0;
    while (*strg != '\0') {
        if (*strg++ == '\n') {
            strg[-1] = '\0';
            lines[idx++] = currLine;
            currLine = strg;
        }
    }

    lines[idx] = currLine;
}
