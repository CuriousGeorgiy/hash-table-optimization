#ifndef HASHTABLE_HPP
#define HASHTABLE_HPP

#include <cstring>

#include "DoublyLinkedArrayList.hpp"

class HashTable {
public:
    struct Entry {
        friend bool operator==(const Entry &a, const Entry &b)
        {
            return ((a.key != nullptr) && (b.key != nullptr)) && (std::strcmp(a.key, b.key) != 0);
        }

        const char *key;
        const char *val;
    };

    HashTable() = delete;
    explicit HashTable(std::size_t (*hashFunc)(const char *));

    void validate() const;

    void insert(Entry entry);
    void insert(const char *key, const char *val);
    bool remove(const char *key);
    const char *find(const char *key) const;

    void clear();

private:
    std::size_t hashFuncModulusWrapper(const char *str) const;

    static const std::size_t sz_ = 1009;
    DoublyLinkedArrayList<Entry> arr_[sz_];

    std::size_t (*hashFunc_)(const char *);
};

std::size_t elfHash(const char *str);

#endif /* HASHTABLE_HPP */
