#include "HashTable.hpp"

#include <climits>

HashTable::HashTable(std::size_t (*const hashFunc)(const char *))
: hashFunc_(hashFunc), arr_{}
{
}

std::size_t HashTable::hashFuncModulusWrapper(const char *const str) const
{
    return hashFunc_(str) % sz_;
}

void HashTable::insert(HashTable::Entry entry)
{
    insert(entry.key, entry.val);
}

void HashTable::insert(const char *const key, const char *const val)
{
    Entry entry{.key = key, .val = val};
    auto &list = arr_[hashFuncModulusWrapper(key)];

    auto node = list.findNodeByValue(entry);
    if (node != nullptr) return;

    list.insertAfterHead(entry);
}

bool HashTable::remove(const char *const key)
{
    auto &list = arr_[hashFuncModulusWrapper(key)];

    auto node = list.findNodeByValue(Entry{.key = key});
    if (node == nullptr) return false;

    list.deleteFromPhysicalPos(node->curr);

    return true;
}

const char *HashTable::find(const char *const key) const
{
    auto node = arr_[hashFuncModulusWrapper(key)].findNodeByValue(Entry{.key = key});
    if (node == nullptr) return nullptr;

    return node->data.val;
}

void HashTable::clear()
{
    for (auto &list: arr_) list.clear();
}

void HashTable::validate() const
{
    for (auto &list: arr_) list.validate();
}

std::size_t zeroHash(const char *const str)
{
    return 0;
}

std::size_t firstCharASCIICodeHash(const char *const str)
{
    return str[0];
}

std::size_t lenHash(const char *const str)
{
    return std::strlen(str);
}

std::size_t asciiCodeSumHash(const char *str)
{
    std::size_t sum = 0;
    while (*str != '\0') sum += *str++;

    return sum;
}

std::size_t rolHash(const char *str)
{
    if (*str++ == '\0') return 0;

    std::size_t hash = 0;
    while (*str != '\0') hash = ((hash >> (sizeof(hash) * CHAR_BIT - 1)) | (hash << 1)) ^ *str++;

    return hash;
}

std::size_t elfHash(const char *str)
{
    std::size_t hash = 0;
    std::size_t highBits = 0;
    while (*str != '\0') {
        hash = (hash << 4) + *str++;
        highBits = hash & (static_cast<std::size_t>(0xF) << (sizeof(hash) - 1) * CHAR_BIT);
        if (highBits) hash ^= highBits >> (sizeof(hash) - 1) * CHAR_BIT;
        hash &= ~highBits;
    }

    return hash;
}
