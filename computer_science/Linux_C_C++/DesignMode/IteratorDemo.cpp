#include <iostream>
#include <vector>

//迭代器接口
template<class T>
class Iterator {
public:
    virtual bool hasNext() = 0;
    virtual T next() = 0;
};

//具体迭代器
template<class T>
class ConcreteIterator : public Iterator<T> {
public:
    ConcreteIterator(std::vector<T>& collection): collection(collection) {}

    bool hasNext() override {
        return index < collection.size();
    }

    T next() override {
        return collection[index++];
    }
private:
    std::vector<T>& collection;
    int index = 0;
};

//聚合接口
template <class T>
class Aggregate {
public:
    virtual Iterator<T>* createIterator() = 0;
};

//具体聚合
template<class T>
class ConcreteAggregate : public Aggregate<T> {
public:
    void add(T item) {
        collection.push_back(item);
    }

    Iterator<T>* createIterator() override {
        return new ConcreteIterator<T>(collection);
    }
private:
    std::vector<T> collection;
};

int main(void) {
    ConcreteAggregate<int>  aggregate;
    aggregate.add(1);
    aggregate.add(2);
    aggregate.add(3);

    Iterator<int>* iterator = aggregate.createIterator();

    while (iterator->hasNext()) {
        std::cout << iterator->next() << " ";
    }

    delete iterator;
    
    return 0;
}