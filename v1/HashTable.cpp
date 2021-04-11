#include "HashTable.hpp"

#include <climits>
#include <immintrin.h>

HashTable::HashTable(unsigned long long (*const hashFunc)(const HashTable::String &))
: hashFunc_(hashFunc), arr_{}
{
}

std::size_t HashTable::hashFuncModulusWrapper(const HashTable::String &str) const
{
    return hashFunc_(str) % sz_;
}

void HashTable::insert(const HashTable::Entry &entry)
{
    insert(*entry.key, *entry.val);
}

void HashTable::insert(const HashTable::String &key, const HashTable::String &val)
{
    DoublyLinkedArrayList<HashTable::Entry> &list = arr_[hashFuncModulusWrapper(key)];

    Entry entry{.key = &key, .val = &val};
    auto node = list.findNodeByValue(entry);
    if (node != nullptr) return;

    list.insertAfterHead(entry);
}

bool HashTable::remove(const HashTable::String &key)
{
    auto &list = arr_[hashFuncModulusWrapper(key)];

    auto node = list.findNodeByValue(Entry{.key = &key, .val = nullptr});
    if (node == nullptr) return false;

    list.deleteFromPhysicalPos(node->curr);

    return true;
}

const HashTable::String *HashTable::find(const HashTable::String &key) const
{
    auto node = arr_[hashFuncModulusWrapper(key)].findNodeByValue(Entry{.key = &key, .val = nullptr});
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

unsigned long long crc32Hash(const HashTable::String &str)
{
    return _mm_crc32_u64(0, *reinterpret_cast<const unsigned long long *>(str));
}
