#include <iostream>
#include <map>
#include <vector>

using namespace std;

template <class K, class V>
void PrintMap(map<K, V> &m) {
    for (auto it = m.begin(); it != m.end(); it++)
        cout << it->first << " => " << it->second << endl;
}

class A {
private:
    int a;

public:
    A(const A& other) {
        cout << "A COPY" << endl;
        a = other.a;
    }

    A(int a = 42) {
        cout << "A NORMAL" << endl;
        this->a = a;
    }

    int GetA() { return a; }

    void operator=(const A &a) {
        cout << "A ASSIGNMENT" << endl;
        this->a = a.a;
    }
};

class B : public A {
private:
    int b;

public:
    B(const A &a) : A(a) {
        cout << "B->A COPY" << endl;
    }

    B(int b) : A() {
        cout << "B NORMAL" << endl;
        this->b = b;
    }

    B(int a, int b) : A(a) {
        cout << "B BOTH" << endl;
        this->b = b;
    }

    int GetB() { return b; }
};


class C {
private:
    int c;

public:
    C(const C &other) {
        cout << "COPY" << endl;
        c = other.c;
    }

    C(int c) {
        cout << "PARAM" << endl;
        this->c = c;
    }

    C() {
        cout << "DEFAULT" << endl;
        c = 0;
    }

    int GetC() {
        return c;
    }

    void operator=(const C &other) {
        cout << "ASSIGNMENT" << endl;
        c = other.c;
    }
};

int main(int argc, char **argv) {
    vector<A> as;
    B b(12);
    cout << b.GetB() << endl;
    as.push_back(b);
    //as.push_back(A());
    cout << 0 << endl;

    B &a = *((B*) &as.back());
    cout << 1 << endl;
    cout << a.GetB() << endl;

    /*map<int, int> idCounter;

    int id1 = 123123;
    int id2 = 124124;
    int id3 = 124125;

    idCounter[id1] = 0;
    idCounter[id2] = 0;
    idCounter[id3] = 0;

    map<int, int>::iterator it;

    for (it = idCounter.begin(); it != idCounter.end(); it++)
        it->second++;

    idCounter.erase(id2);

    PrintMap<int, int>(idCounter);*/
}