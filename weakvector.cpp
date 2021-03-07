#include <iostream>
#include <cstring>
using namespace std;
namespace sjtu {
    const static int initSize = 10;
    template<typename T>
    class vector {
    private:
        int maxElem;
        int curElem;
        T ** data;
        void doubleSpace(){
            T ** temp = data;
            maxElem *= 2;
            data = new T*[maxElem];
            for(int i = 0; i < curElem; ++i) data[i] = temp[i]; // unnecessery to new
            for(int i = curElem;i < maxElem;++i) data[i] = nullptr;
            delete [] temp;
        }
    public:
        vector():maxElem(initSize),curElem(0){
            data = new T* [initSize];
            for(int i = 0;i < maxElem;++i) data[i] = nullptr;
        }

        vector(const vector &other) {
            this->maxElem = other.maxElem; this->curElem = other.curElem;
            this->data = new T*[this->maxElem];
            for(int i = 0;i < this->curElem;++i) this->data[i] = new T(*(other.data[i]));
            for(int i = curElem;i < maxElem;++i) this->data[i] = nullptr;
        }
        ~vector() {
            if(this->data != nullptr){
                for(int i = 0;i < maxElem;++i){
                    if(this->data[i] != nullptr) delete data[i];
                }
                delete [] data;
            }
        }
        vector &operator= (const vector &other) {
            if(this == &other) return *this;
            this->clear();
            this->maxElem = other.maxElem;this->curElem = other.curElem;
            this->data = new T*[this->maxElem];
            for(int i = 0;i < this->curElem;++i) this->data[i] = new T(*(other.data[i]));
            for(int i = curElem;i < maxElem;++i) this->data[i] = nullptr;
            return *this;
        }
        T & at(const size_t &pos) {
            return *(data[pos]);
        }
        const T & at(const size_t &pos) const {
            return *(data[pos]);
        }
        T & operator[](const size_t &pos) {
            return *(data[pos]);
        }
        const T & operator[](const size_t &pos) const {
            return *(data[pos]);
        }
        const T & front() const {
            return *(data[0]);
        }
        const T & back() const {
            return *(data[curElem-1]);
        }
        bool empty() const {
            return (curElem == 0);
        }
        size_t size() const {
            return curElem;
        }
        void clear() {
            if(this->data != nullptr){
                for(int i = 0;i < maxElem;++i){
                    if(this->data[i] != nullptr) delete data[i];
                }
                delete [] data;
            }
            this->curElem = 0;this->maxElem = 0;
            data = nullptr;
        }
        void push_back(const T &value) {
            data[this->curElem++] = new T(value);
            if (this->curElem == this->maxElem) doubleSpace();
        }
        void pop_back() {
            delete data[curElem-1];data[curElem-1] = nullptr;
            this->curElem--;
        }
    };
}