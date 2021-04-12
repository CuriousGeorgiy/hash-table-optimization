#ifndef HASHTABLE_HPP
#define HASHTABLE_HPP

#include <cstring>
#include <climits>

#include "DoublyLinkedArrayList.hpp"

class HashTable {
public:
    static const std::size_t StringSize = CHAR_BIT * sizeof(unsigned long long);
    typedef char String[StringSize];

    struct Entry {
        friend bool operator==(const Entry &a, const Entry &b)
        {
            return ((a.key != nullptr) && (b.key != nullptr)) &&
            (*reinterpret_cast<const unsigned long long *>(*a.key) ==
             *reinterpret_cast<const unsigned long long *>(*b.key));
        }

        const String *key;
        const String *val;
    };

    HashTable() = delete;
    explicit HashTable(unsigned long long (*hashFunc)(const String &));

    void validate() const;

    void insert(const Entry &entry);
    void insert(const String &key, const String &val);
    bool remove(const String &key);
    const String *find(const String &key) const;

    void clear();

private:
    std::size_t hashFuncModulusWrapper(const String &str) const;

    static const std::size_t sz_ = 1009;
    DoublyLinkedArrayList<Entry> arr_[sz_];

    unsigned long long (*hashFunc_)(const String &);
};

std::size_t elfHash(const char *str);
unsigned long long crc32Hash(const HashTable::String &str);

#endif /* HASHTABLE_HPP */
