#include <cstdlib>
#include <ctime>

#include <sys/stat.h>

#include "HashTable.hpp"

std::size_t fileSize(const char *name);
std::size_t cntLines(const char *strg);
void readLinesFromStorage(const char *strg, HashTable::String *lines[]);

signed main()
{
    const std::size_t nLookUps = 100000000;
    std::srand(time(nullptr));

    const size_t fileSz = fileSize("words.txt");
    std::FILE *wordStream = std::fopen("words.txt", "rb");
    auto strg = new char[fileSz + 1]{};
    std::fread(strg, sizeof(*strg), fileSz, wordStream);
    std::fclose(wordStream);

    const std::size_t nLines = cntLines(strg);
    auto lines = new HashTable::String *[nLines]{};
    readLinesFromStorage(strg, lines);
    delete[] strg;

    HashTable hashTable{crc32Hash};

    HashTable::String placeholder{""};
    for (std::size_t i = 0; i < nLines; ++i) {
        hashTable.insert(*lines[i], placeholder);
    }

    for (std::size_t i = 0; i < nLookUps; ++i) hashTable.find(*lines[std::rand() % nLines]);

    for (size_t i = 0; i < nLines; ++i) std::free(lines[i]);
    delete[] lines;

    return EXIT_SUCCESS;
}

std::size_t fileSize(const char *const name)
{
    struct stat buf{};
    stat(name, &buf);

    return buf.st_size;
}

std::size_t cntLines(const char *strg)
{
    if (*strg == '\0') return 0;

    std::size_t nLines = 0;
    while (*strg != '\0') {
        if (*strg++ == '\n') ++nLines;
    }

    return nLines + 1;
}

void readLinesFromStorage(const char *strg, HashTable::String *lines[])
{
    const char *currLine = strg;
    size_t lineLen = 0;
    size_t idx = 0;
    while (*strg != '\0') {
        if (*strg++ == '\n') {
            lines[idx] = static_cast<HashTable::String *>(std::calloc(1, sizeof(HashTable::String)));

            lineLen = strg - 1 - currLine;
            std::strncpy(static_cast<char *>(*lines[idx++]), currLine,
                         (lineLen > HashTable::StringSize - 1) ? HashTable::StringSize - 1 : lineLen);
            currLine = strg;
        }
    }

    lines[idx] = static_cast<HashTable::String *>(std::calloc(1, sizeof(HashTable::String)));
    lineLen = (strg == currLine) ? 0 : strg - currLine;
    std::strncpy(static_cast<char *>(*lines[idx]), currLine,
                 (lineLen > HashTable::StringSize - 1) ? HashTable::StringSize - 1 : lineLen);
}
