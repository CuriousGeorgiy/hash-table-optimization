#ifndef DOUBLYLINKEDARRAYLIST_HPP
#define DOUBLYLINKEDARRAYLIST_HPP

#include <cstdlib>
#include <cstdio>

#include <type_traits>

#ifndef NDEBUG
#define VALIDATE_LIST validate()
#else
#define VALIDATE_LIST
#endif

template<typename T>
class DoublyLinkedArrayList {
public:
    struct Node {
        T data;
        size_t next;
        size_t curr;
        size_t prev;
    };

    bool sorted;
    size_t size;
    size_t capacity;

    DoublyLinkedArrayList();
    explicit DoublyLinkedArrayList(size_t capacity);

    DoublyLinkedArrayList(const DoublyLinkedArrayList<T> &) = delete;
    DoublyLinkedArrayList(DoublyLinkedArrayList<T> &&) = delete;

    DoublyLinkedArrayList<T> &operator=(const DoublyLinkedArrayList<T> &) = delete;
    DoublyLinkedArrayList<T> &operator=(DoublyLinkedArrayList<T> &&) = delete;

    ~DoublyLinkedArrayList();

    size_t insertAfterPhysicalPos(size_t physicalPos, T val);
    size_t insertBeforePhysicalPos(size_t physicalPos, T val);
    size_t insertBeforeTail(T val);
    size_t insertAfterHead(T val);

    void deleteFromPhysicalPos(size_t physicalPos);
    void deleteHead();
    void deleteTail();

    void sort();
    static int compareNodesByNext(const void *arg1, const void *arg2);

    Node *nodeAfterPhysicalPos(size_t physicalPos) const;
    Node *nodeBeforePhysicalPos(size_t physicalPos) const;
    Node *tailNode() const;
    Node *headNode() const;

    void dump(const char *msg) const;
    bool isValid() const;
    void validate() const;
    void clear();

    Node *findNodeByValue(T val) const;
    Node *findNodeByLogicalPos(size_t logicalPos) const;

private:
    static const size_t growCoeff = 16;

    Node *nodes;

    size_t headPos;
    size_t tailPos;
    size_t freeListHeadPos;

    size_t findFreePos();
    void addToFree(size_t physicalPos);

    void grow();

    void createFreePosList(size_t freeListNewHeadPos);

    bool checkInsertPhysicalPosCorrectness(size_t physicalPos) const;
    bool checkDeletePhysicalPosCorrectness(size_t physicalPos) const;
    bool checkFindPhysicalPosCorrectness(size_t physicalPos) const;
};

template<typename T>
DoublyLinkedArrayList<T>::DoublyLinkedArrayList(size_t capacity)
: sorted(false), size(0), capacity(capacity), nodes((Node *) std::calloc(capacity + 1, sizeof(*nodes))), headPos(0), tailPos(0),
  freeListHeadPos(0)
{
    createFreePosList(1);

    VALIDATE_LIST;
}

template<typename T>
DoublyLinkedArrayList<T>::~DoublyLinkedArrayList()
{
    VALIDATE_LIST;

    std::free(nodes);
}

template<typename T>
size_t DoublyLinkedArrayList<T>::insertAfterPhysicalPos(size_t physicalPos, T val)
{
    VALIDATE_LIST;

    if (!checkInsertPhysicalPosCorrectness(physicalPos)) return 0;

    if (size == capacity) grow();

    if (physicalPos == headPos) {
        VALIDATE_LIST;
        return insertAfterHead(val);
    }

    size_t freePhysicalPos = findFreePos();
    if (freePhysicalPos == 0) {
        VALIDATE_LIST;
        return 0;
    }

    nodes[freePhysicalPos].curr = freePhysicalPos;
    nodes[freePhysicalPos].next = nodes[physicalPos].next;
    nodes[physicalPos].next = freePhysicalPos;
    nodes[nodes[freePhysicalPos].next].prev = freePhysicalPos;
    nodes[freePhysicalPos].prev = physicalPos;
    nodes[freePhysicalPos].data = val;
    sorted = false;
    ++size;

    VALIDATE_LIST;
    return freePhysicalPos;
}

template<typename T>
size_t DoublyLinkedArrayList<T>::insertBeforePhysicalPos(size_t physicalPos, const T val)
{
    VALIDATE_LIST;

    if (!checkInsertPhysicalPosCorrectness(physicalPos)) return 0;

    if (size == capacity) grow();

    if (physicalPos == tailPos) {
        VALIDATE_LIST;
        return insertBeforeTail(val);
    }

    size_t freePhysicalPos = findFreePos();
    if (freePhysicalPos == 0) {
        VALIDATE_LIST;
        return 0;
    }

    nodes[freePhysicalPos].curr = freePhysicalPos;
    nodes[freePhysicalPos].next = physicalPos;
    nodes[freePhysicalPos].prev = nodes[physicalPos].prev;
    nodes[physicalPos].prev = freePhysicalPos;
    nodes[nodes[freePhysicalPos].prev].next = freePhysicalPos;
    nodes[freePhysicalPos].data = val;
    sorted = false;
    ++size;

    VALIDATE_LIST;
    return freePhysicalPos;
}

template<typename T>
size_t DoublyLinkedArrayList<T>::insertBeforeTail(const T val)
{
    VALIDATE_LIST;

    if (size == capacity) grow();

    size_t freePhysicalPos = findFreePos();
    if (freePhysicalPos == 0) {
        VALIDATE_LIST;
        return 0;
    }

    if (tailPos != 0) {
        nodes[tailPos].prev = freePhysicalPos;
    } else {
        headPos = freePhysicalPos;
    }

    nodes[freePhysicalPos].curr = freePhysicalPos;
    nodes[freePhysicalPos].next = tailPos;
    nodes[freePhysicalPos].prev = 0;
    nodes[freePhysicalPos].data = val;
    tailPos = freePhysicalPos;
    sorted = false;
    ++size;

    VALIDATE_LIST;
    return freePhysicalPos;
}

template<typename T>
size_t DoublyLinkedArrayList<T>::insertAfterHead(const T val)
{
    VALIDATE_LIST;

    if (size == capacity) grow();

    size_t freePhysicalPos = findFreePos();
    if (freePhysicalPos == 0) {
        VALIDATE_LIST;
        return 0;
    }

    if (headPos != 0) {
        nodes[headPos].next = freePhysicalPos;
    } else {
        tailPos = freePhysicalPos;
    }

    nodes[freePhysicalPos].curr = freePhysicalPos;
    nodes[freePhysicalPos].next = 0;
    nodes[freePhysicalPos].prev = headPos;
    auto &data = const_cast<typename std::remove_const<T>::type &>(nodes[freePhysicalPos].data);
    data = val;
    headPos = freePhysicalPos;
    ++size;

    VALIDATE_LIST;
    return freePhysicalPos;
}

template<typename T>
size_t DoublyLinkedArrayList<T>::findFreePos()
{
    VALIDATE_LIST;

    if (freeListHeadPos == 0) return 0;

    size_t foundFree = freeListHeadPos;
    freeListHeadPos = nodes[freeListHeadPos].next;

    return foundFree;
}

template<typename T>
void DoublyLinkedArrayList<T>::deleteFromPhysicalPos(size_t physicalPos)
{
    VALIDATE_LIST;

    if (!checkDeletePhysicalPosCorrectness(physicalPos)) return;

    if (physicalPos == headPos) {
        deleteHead();
        VALIDATE_LIST;
        return;
    }

    if (physicalPos == tailPos) {
        deleteTail();
        VALIDATE_LIST;
        return;
    }

    nodes[nodes[physicalPos].next].prev = nodes[physicalPos].prev;
    nodes[nodes[physicalPos].prev].next = nodes[physicalPos].next;
    addToFree(physicalPos);
    sorted = false;
    --size;

    VALIDATE_LIST;
}

template<typename T>
void DoublyLinkedArrayList<T>::deleteHead()
{
    VALIDATE_LIST;

    nodes[nodes[headPos].prev].next = 0;
    size_t deletedHeadPos = headPos;
    headPos = nodes[headPos].prev;
    addToFree(deletedHeadPos);
    --size;

    VALIDATE_LIST;
}

template<typename T>
void DoublyLinkedArrayList<T>::deleteTail()
{
    VALIDATE_LIST;

    nodes[nodes[tailPos].next].prev = 0;
    size_t deletedTailPos = tailPos;
    tailPos = nodes[tailPos].next;
    addToFree(deletedTailPos);
    --size;

    VALIDATE_LIST;
}

template<typename T>
void DoublyLinkedArrayList<T>::addToFree(size_t physicalPos)
{
    nodes[physicalPos].next = freeListHeadPos;
    nodes[physicalPos].prev = 0;
    freeListHeadPos = physicalPos;
}

template<typename T>
void DoublyLinkedArrayList<T>::dump(const char *msg) const
{
    std::FILE *dumpFile = std::fopen("DoublyLinkedArrayList.dot", "w");
    std::fprintf(dumpFile, "digraph G {\n"
                           "\trankdir=LR\n");

    for (size_t i = 0; i <= capacity; ++i) {
        std::fprintf(dumpFile, "\t%zu[", i);
        if (i == 0) {
            std::fprintf(dumpFile,
                         "shape=record, label=\"physical pos\\n %zu | <prev> prev\\n %zu | <data> data\\n %d | <next> next\\n %zu\"];\n",
                         i, nodes[i].prev, 0, nodes[i].next);
            continue;
        }

        if ((nodes[i].prev == 0) && (i != tailPos)) {
            std::fprintf(dumpFile, R"(fillcolor=green, color=darkgreen, fontcolor=darkgreen, label="<free> free | )");
        } else if ((i == tailPos) || (i == headPos)) {
            std::fprintf(dumpFile, R"(fillcolor="#ffc0cb", color="#462759", fontcolor="#462759", label=")");
        } else {
            std::fprintf(dumpFile, R"(fillcolor="#add8e6", color="#00008b", fontcolor="#00008b", label=")");
        }
        std::fprintf(dumpFile,
                     "physical pos\\n %zu | <prev> prev\\n %zu | <data> data\\n %d | <next> next\\n %zu\","
                     " style=filled,"
                     " shape=record];\n",
                     i, nodes[i].prev, 0, nodes[i].next);
    }

    for (size_t i = 1; i <= capacity; ++i) {
        if ((nodes[i].prev == 0) && (i != tailPos)) continue;
        std::fprintf(dumpFile, "\t%zu:<next>->%zu:<data>;\n"
                               "\t%zu:<prev>->%zu:<data>;\n", i, nodes[i].next, i, nodes[i].prev);
    }

    size_t freePos = freeListHeadPos;
    while (nodes[freePos].next != 0) {
        std::fprintf(dumpFile, "\t%zu:<free>->%zu:<free>;\n", freePos, nodes[freePos].next);
        freePos = nodes[freePos].next;
    }

    std::fprintf(dumpFile, "\tlabelloc=\"t\"\n"
                           "\tlabel=\"%s\"\n", msg);

    std::fprintf(dumpFile, "}\n");
    std::fclose(dumpFile);

    std::system("dot -Tpng DoublyLinkedArrayList.dot > DoublyLinkedArrayList.png");
    std::system("start DoublyLinkedArrayList.png");
    std::system("pause");
}

template<typename T>
typename DoublyLinkedArrayList<T>::Node *DoublyLinkedArrayList<T>::tailNode() const
{
    VALIDATE_LIST;

    return nodes + tailPos;
}

template<typename T>
typename DoublyLinkedArrayList<T>::Node *DoublyLinkedArrayList<T>::headNode() const
{
    VALIDATE_LIST;

    return nodes + headPos;
}

template<typename T>
void DoublyLinkedArrayList<T>::sort()
{
    VALIDATE_LIST;

    for (size_t physicalPos = headPos, logicalPos = size; physicalPos != 0; physicalPos = nodes[physicalPos].prev, --logicalPos) {
        nodes[physicalPos].next = logicalPos;
    }

    size_t freePos = freeListHeadPos;
    while (freePos != 0) {
        size_t nextFreePos = nodes[freePos].next;
        nodes[freePos].next = size + 1;
        freePos = nextFreePos;
    }

    qsort(nodes + 1, capacity, sizeof(*nodes), compareNodesByNext);

    tailPos = 1;
    headPos = size;
    nodes[headPos].prev = size - 1;
    nodes[headPos].next = 0;

    for (size_t physicalPos = tailPos; physicalPos < headPos; ++physicalPos) {
        nodes[physicalPos].next = physicalPos + 1;
        nodes[physicalPos].prev = physicalPos - 1;
    }

    createFreePosList(size + 1);

    sorted = true;

    VALIDATE_LIST;
}

template<typename T>
bool DoublyLinkedArrayList<T>::checkInsertPhysicalPosCorrectness(size_t physicalPos) const
{
    VALIDATE_LIST;

    return (physicalPos <= capacity) && ((nodes[physicalPos].prev != 0) || (physicalPos == tailPos)) &&
           ((physicalPos != 0) || ((tailPos == 0) && (headPos == 0)));
}

template<typename T>
bool DoublyLinkedArrayList<T>::checkDeletePhysicalPosCorrectness(size_t physicalPos) const
{
    VALIDATE_LIST;

    return (physicalPos <= capacity) && ((nodes[physicalPos].prev != 0) || (physicalPos == tailPos)) && (physicalPos != 0);
}

template<typename T>
bool DoublyLinkedArrayList<T>::checkFindPhysicalPosCorrectness(size_t physicalPos) const
{
    VALIDATE_LIST;

    return checkDeletePhysicalPosCorrectness(physicalPos);
}

template<typename T>
typename DoublyLinkedArrayList<T>::Node *DoublyLinkedArrayList<T>::nodeAfterPhysicalPos(size_t physicalPos) const
{
    VALIDATE_LIST;

    if (!checkFindPhysicalPosCorrectness(physicalPos)) return nullptr;

    return nodes + nodes[physicalPos].next;
}

template<typename T>
typename DoublyLinkedArrayList<T>::Node *DoublyLinkedArrayList<T>::nodeBeforePhysicalPos(size_t physicalPos) const
{
    VALIDATE_LIST;

    if (!checkFindPhysicalPosCorrectness(physicalPos)) return nullptr;

    return nodes + nodes[physicalPos].prev;
}

template<typename T>
typename DoublyLinkedArrayList<T>::Node *DoublyLinkedArrayList<T>::findNodeByValue(T val) const
{
    VALIDATE_LIST;

    for (size_t i = 0; i < capacity; ++i) {
        if (nodes[i].data == val) {
            return nodes + i;
        }
    }

    return nullptr;
}

template<typename T>
void DoublyLinkedArrayList<T>::clear()
{
    VALIDATE_LIST;

    size = 0;
    headPos = 0;
    tailPos = 0;
    sorted = false;
    createFreePosList(1);

    VALIDATE_LIST;
}

template<typename T>
typename DoublyLinkedArrayList<T>::Node *DoublyLinkedArrayList<T>::findNodeByLogicalPos(size_t logicalPos) const
{
    VALIDATE_LIST;

    if ((logicalPos == 0) || (logicalPos > size)) return nullptr;

    if (sorted) return nodes + tailPos + logicalPos - 1;

    size_t physicalPos = tailPos;
    for (size_t i = 1; i < logicalPos; ++i) physicalPos = nodes[physicalPos].next;

    return nodes + physicalPos;
}

template<typename T>
bool DoublyLinkedArrayList<T>::isValid() const
{
    if (size > capacity) return false;

    if ((size == capacity) && (freeListHeadPos != 0)) return false;

    size_t freePos = freeListHeadPos;
    for (size_t i = 0; i < capacity - size; freePos = nodes[freePos].next, ++i) {
        if (nodes[freePos].prev != 0) return false;
    }
    if (freePos != 0) return false;

    if (size == 0) {
        if ((tailPos != 0) || (headPos != 0)) return false;

        for (size_t i = 1; i <= capacity; ++i) {
            if (nodes[i].prev != 0) return false;
        }

        return true;
    }

    if ((tailPos == 0) || (headPos == 0) || (nodes[tailPos].prev != 0) || (nodes[headPos].next != 0)) return false;

    size_t physicalPos = tailPos;
    for (size_t i = 1; i <= size; physicalPos = nodes[physicalPos].next, ++i) {
        if ((nodes[physicalPos].prev == 0) && (physicalPos != tailPos)) return false;
    }
    if (physicalPos != 0) return false;

    physicalPos = headPos;
    for (size_t i = size; i > 0; physicalPos = nodes[physicalPos].prev, --i);
    if (physicalPos != 0) return false;

    return true;
}

template<typename T>
void DoublyLinkedArrayList<T>::createFreePosList(size_t freeListNewHeadPos)
{
    freeListHeadPos = freeListNewHeadPos;
    for (size_t i = freeListHeadPos; i <= capacity; ++i) {
        nodes[i].next = i + 1;
        nodes[i].prev = 0;
    }
    nodes[capacity].next = 0;

    VALIDATE_LIST;
}

template<typename T>
int DoublyLinkedArrayList<T>::compareNodesByNext(const void *arg1, const void *arg2)
{
    auto node1 = (Node *) arg1;
    auto node2 = (Node *) arg2;

    return node1->next - node2->next;
}

template<typename T>
void DoublyLinkedArrayList<T>::grow()
{
    VALIDATE_LIST;

    capacity *= growCoeff;
    nodes = (Node *) std::realloc(nodes, sizeof(Node) * (capacity + 1));

    createFreePosList(size + 1);

    VALIDATE_LIST;
}

template<typename T>
void DoublyLinkedArrayList<T>::validate() const
{
    if (!isValid()) {
        dump("Invalid list");
        exit(EXIT_FAILURE);
    }
}

template<typename T>
DoublyLinkedArrayList<T>::DoublyLinkedArrayList()
: DoublyLinkedArrayList(8)
{

}

#undef VALIDATE_LIST

#endif /* DOUBLYLINKEDARRAYLIST_HPP */
