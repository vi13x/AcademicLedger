#ifndef LIST_HPP
#define LIST_HPP

#include "exceptionhandler.h"
#include <stdexcept>
#include <QList>
#include <QMap>
#include <QVector>
#include <QString>

template<typename T>
class List {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& value) : data(value), next(nullptr) {}
    };
    Node* head;
    int count;

    Node* getNodeAt(int index) const {
        if (index < 0 || index >= count) {
            throw OutOfRangeException(
                QString("Индекс %1 выходит за границы списка размером %2").arg(index).arg(count)
            );
        }
        Node* current = head;
        for (int i = 0; i < index; i++) {
            current = current->next;
        }
        return current;
    }

public:
    // Конструкторы
    List() : head(nullptr), count(0) {}
    
    List(const List& other) : head(nullptr), count(0) {
        Node* current = other.head;
        while (current) {
            append(current->data);
            current = current->next;
        }
    }
    
    List& operator=(const List& other) {
        if (this != &other) {
            clear();
            Node* current = other.head;
            while (current) {
                append(current->data);
                current = current->next;
            }
        }
        return *this;
    }
    
    ~List() {
        clear();
    }
    
    // Добавление элементов
    void append(const T& value) {
        try {
            Node* newNode = new(std::nothrow) Node(value);
            if (!newNode) {
                throw BadAllocException("Не удалось выделить память для нового узла");
            }
            
            if (!head) {
                head = newNode;
            } else {
                Node* current = head;
                while (current->next) {
                    current = current->next;
                }
                current->next = newNode;
            }
            count++;
        } catch (const std::bad_alloc& e) {
            throw BadAllocException("Ошибка выделения памяти при добавлении элемента");
        }
    }
    
    void push_back(const T& value) {
        append(value);
    }
    
    void push_front(const T& value) {
        try {
            Node* newNode = new(std::nothrow) Node(value);
            if (!newNode) {
                throw BadAllocException("Не удалось выделить память для нового узла");
            }
            newNode->next = head;
            head = newNode;
            count++;
        } catch (const std::bad_alloc& e) {
            throw BadAllocException("Ошибка выделения памяти при добавлении элемента");
        }
    }
    
    void insert(int index, const T& value) {
        if (index < 0 || index > count) {
            throw OutOfRangeException(
                QString("Индекс %1 выходит за границы списка размером %2").arg(index).arg(count)
            );
        }
        
        if (index == 0) {
            push_front(value);
            return;
        }
        
        if (index == count) {
            append(value);
            return;
        }
        
        try {
            Node* newNode = new(std::nothrow) Node(value);
            if (!newNode) {
                throw BadAllocException("Не удалось выделить память для нового узла");
            }
            
            Node* prev = getNodeAt(index - 1);
            newNode->next = prev->next;
            prev->next = newNode;
            count++;
        } catch (const std::bad_alloc& e) {
            throw BadAllocException("Ошибка выделения памяти при вставке элемента");
        }
    }
    
    // Удаление элементов
    void clear() {
        while (head) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
        count = 0;
    }
    
    void removeAt(int index) {
        if (index < 0 || index >= count) {
            throw OutOfRangeException(
                QString("Индекс %1 выходит за границы списка размером %2").arg(index).arg(count)
            );
        }
        
        if (index == 0) {
            Node* temp = head;
            head = head->next;
            delete temp;
        } else {
            Node* prev = getNodeAt(index - 1);
            Node* temp = prev->next;
            prev->next = temp->next;
            delete temp;
        }
        count--;
    }
    
    bool remove(const T& value) {
        Node* current = head;
        Node* prev = nullptr;
        int index = 0;
        
        while (current) {
            if (current->data == value) {
                if (prev) {
                    prev->next = current->next;
                } else {
                    head = current->next;
                }
                delete current;
                count--;
                return true;
            }
            prev = current;
            current = current->next;
            index++;
        }
        return false;
    }
    
    // Доступ к элементам
    T& at(int index) {
        Node* node = getNodeAt(index);
        return node->data;
    }
    
    const T& at(int index) const {
        Node* node = getNodeAt(index);
        return node->data;
    }
    
    T& operator[](int index) {
        return at(index);
    }
    
    const T& operator[](int index) const {
        return at(index);
    }
    
    T first() const {
        if (isEmpty()) {
            throw OutOfRangeException("Список пуст, невозможно получить первый элемент");
        }
        return head->data;
    }
    
    T last() const {
        if (isEmpty()) {
            throw OutOfRangeException("Список пуст, невозможно получить последний элемент");
        }
        Node* current = head;
        while (current->next) {
            current = current->next;
        }
        return current->data;
    }
    
    // Информация о списке
    int size() const {
        return count;
    }
    
    bool isEmpty() const {
        return count == 0;
    }
    
    // Поиск
    bool contains(const T& value) const {
        Node* current = head;
        while (current) {
            if (current->data == value) return true;
            current = current->next;
        }
        return false;
    }
    
    int indexOf(const T& value) const {
        Node* current = head;
        int index = 0;
        while (current) {
            if (current->data == value) return index;
            current = current->next;
            index++;
        }
        return -1;
    }
    
    // Преобразование в QVector для совместимости с Qt
    QVector<T> toVector() const {
        QVector<T> result;
        result.reserve(count);
        Node* current = head;
        while (current) {
            result.append(current->data);
            current = current->next;
        }
        return result;
    }

    // Преобразование в QList/QStringList для совместимости с Qt
    QList<T> toQList() const {
        QList<T> result;
        result.reserve(count);
        Node* current = head;
        while (current) {
            result.append(current->data);
            current = current->next;
        }
        return result;
    }

    // Позволяет не писать toQList() в местах, где ожидается QList<T>
    operator QList<T>() const {
        return toQList();
    }

    // Удаление всех вхождений значения (аналог QList::removeAll)
    int removeAll(const T& value) {
        int removed = 0;
        Node* current = head;
        Node* prev = nullptr;

        while (current) {
            if (current->data == value) {
                Node* toDelete = current;
                if (prev) {
                    prev->next = current->next;
                } else {
                    head = current->next;
                }
                current = current->next;
                delete toDelete;
                count--;
                removed++;
            } else {
                prev = current;
                current = current->next;
            }
        }
        return removed;
    }
    
    // Итератор (простой)
    class Iterator {
    private:
        Node* current;
        
    public:
        Iterator(Node* node) : current(node) {}
        
        Iterator& operator++() {
            if (current) current = current->next;
            return *this;
        }
        
        Iterator operator++(int) {
            Iterator temp = *this;
            ++(*this);
            return temp;
        }
        
        bool operator==(const Iterator& other) const {
            return current == other.current;
        }
        
        bool operator!=(const Iterator& other) const {
            return current != other.current;
        }
        
        T& operator*() {
            if (!current) {
                throw OutOfRangeException("Попытка разыменовать невалидный итератор");
            }
            return current->data;
        }
        
        T* operator->() {
            if (!current) {
                throw OutOfRangeException("Попытка разыменовать невалидный итератор");
            }
            return &current->data;
        }
    };
    
    Iterator begin() const {
        return Iterator(head);
    }
    
    Iterator end() const {
        return Iterator(nullptr);
    }
};

template<typename Key, typename Value>
class Map {
private:
    struct Pair {
        Key key;
        Value value;
        Pair(const Key& k, const Value& v) : key(k), value(v) {}
        bool operator==(const Pair& other) const {
            return key == other.key;
        }
    };
    
    List<Pair> pairs;
    
    int findIndex(const Key& key) const {
        for (int i = 0; i < pairs.size(); i++) {
            if (pairs[i].key == key) {
                return i;
            }
        }
        return -1;
    }

public:
    Map() {}
    
    void insert(const Key& key, const Value& value) {
        int index = findIndex(key);
        if (index >= 0) {
            pairs[index].value = value;
        } else {
            pairs.append(Pair(key, value));
        }
    }
    
    Value& operator[](const Key& key) {
        int index = findIndex(key);
        if (index >= 0) {
            return pairs[index].value;
        } else {
            pairs.append(Pair(key, Value()));
            return pairs[pairs.size() - 1].value;
        }
    }
    
    const Value& operator[](const Key& key) const {
        int index = findIndex(key);
        if (index >= 0) {
            return pairs[index].value;
        } else {
            throw OutOfRangeException(QString("Ключ не найден в Map"));
        }
    }
    
    Value value(const Key& key, const Value& defaultValue = Value()) const {
        int index = findIndex(key);
        if (index >= 0) {
            return pairs[index].value;
        }
        return defaultValue;
    }
    
    bool contains(const Key& key) const {
        return findIndex(key) >= 0;
    }
    
    void remove(const Key& key) {
        int index = findIndex(key);
        if (index >= 0) {
            pairs.removeAt(index);
        }
    }
    
    void clear() {
        pairs.clear();
    }
    
    int size() const {
        return pairs.size();
    }
    
    bool isEmpty() const {
        return pairs.isEmpty();
    }
    
    List<Key> keys() const {
        List<Key> result;
        for (int i = 0; i < pairs.size(); i++) {
            result.append(pairs[i].key);
        }
        return result;
    }
    
    List<Value> values() const {
        List<Value> result;
        for (int i = 0; i < pairs.size(); i++) {
            result.append(pairs[i].value);
        }
        return result;
    }
    
    // Преобразование в QMap для совместимости
    QMap<Key, Value> toQMap() const {
        QMap<Key, Value> result;
        for (int i = 0; i < pairs.size(); i++) {
            result[pairs[i].key] = pairs[i].value;
        }
        return result;
    }
};

#endif // LIST_HPP

