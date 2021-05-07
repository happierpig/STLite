#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>

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
	class const_iterator;
	class iterator {
	    friend vector<T>;
	    friend const_iterator;
	private:
//	     wrong: invalid when doublespace();
//	     T ** ptr = nullptr;
//       right: the space used by vector is constant;
         vector<T> * ptr; // using position of vector
		 int pos;

	public:
	    iterator() = default;
	    iterator(const iterator & o) = default;
	    iterator(T ** _ptr,int _pos):ptr(_ptr),pos(_pos){}
	    iterator & operator =(const iterator & o) = default;
		iterator operator+(const int &n) const {
			return iterator(this->ptr,this->pos + n);
		}
		iterator operator-(const int &n) const {
            return iterator(this->ptr,this->pos - n);
        }
		int operator-(const iterator &rhs) const {
			if(this->ptr != rhs.ptr) throw invalid_iterator();
			int dis = this->pos - rhs.pos;
			return (dis > 0) ? dis : (-dis);
		}
		iterator& operator+=(const int &n) {
			this->pos += n;
            return *this;
		}
		iterator& operator-=(const int &n) {
			this->pos -= n;
            return *this;
		}
		iterator operator++(int) {
		    iterator it(*this);
		    this->pos++;
		    return it;
		}
		iterator& operator++() {
		    this->pos++;
            return *this;
		}
		iterator operator--(int) {
		    iterator it(*this);
		    this->pos--;
            return it;
		}
		iterator& operator--() {
		    this->pos--;
            return *this;
		}
		T& operator*() const{
		    return *(ptr[pos]);
		}
		bool operator==(const iterator &rhs) const {
            if(this->ptr == rhs.ptr && this->pos == rhs.pos) return true;
            else return false;
		}
		bool operator==(const const_iterator &rhs) const {
            if(this->ptr == rhs.ptr && this->pos == rhs.pos) return true;
            else return false;
		}
		bool operator!=(const iterator &rhs) const {
            if(this->ptr == rhs.ptr && this->pos == rhs.pos) return false;
            else return true;
		}
		bool operator!=(const const_iterator &rhs) const {
            if(this->ptr == rhs.ptr && this->pos == rhs.pos) return false;
            else return true;
		}
	};
	class const_iterator {
	    friend vector<T>;
    private:
//	     wrong: invalid when doublespace();
//	     T ** ptr = nullptr;
//       right: the space used by vector is constant;
        vector<T> * ptr;
        int pos;
    public:
	    const_iterator() = default;
        const_iterator(const iterator & o):ptr(o.ptr),pos(o.pos){}
        const_iterator(const const_iterator & o) = default;
	    const_iterator(T ** _ptr,int _pos):ptr(_ptr),pos(_pos){}
	    const_iterator & operator = (const const_iterator & o) = default;
        const_iterator operator+(const int &n) const {
            return const_iterator(this->ptr,this->pos + n);
        }
        const_iterator operator-(const int &n) const {
            return const_iterator(this->ptr,this->pos - n);
        }
        int operator-(const const_iterator &rhs) const {
            if(this->ptr != rhs.ptr) throw invalid_iterator();
            int dis = this->pos - rhs.pos;
            return (dis > 0) ? dis : (-dis);
        }
        const_iterator& operator+=(const int &n) {
            this->pos += n;
            return *this;
        }
        const_iterator& operator-=(const int &n) {
            this->pos -= n;
            return *this;
        }
        const_iterator operator++(int) {
            const_iterator it(*this);
            this->pos++;
            return it;
        }
        const_iterator& operator++() {
            this->pos++;
            return *this;
        }
        const_iterator operator--(int) {
            iterator it(*this);
            this->pos--;
            return it;
        }
        const_iterator& operator--() {
            this->pos--;
            return *this;
        }
        const T& operator*() const{
            return *(ptr[pos]);
        }
        bool operator==(const const_iterator &rhs) const {
            if(this->ptr == rhs.ptr && this->pos == rhs.pos) return true;
            else return false;
        }
        bool operator==(const iterator &rhs) const {
            if(this->ptr == rhs.ptr && this->pos == rhs.pos) return true;
            else return false;
        }
        bool operator!=(const iterator &rhs) const {
            if(this->ptr == rhs.ptr && this->pos == rhs.pos) return false;
            else return true;
        }
        bool operator!=(const const_iterator &rhs) const {
            if(this->ptr == rhs.ptr && this->pos == rhs.pos) return false;
            else return true;
        }
    };
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
	    if(pos < 0 || pos >= curElem) throw index_out_of_bound();
        return *(data[pos]);
	}
	const T & at(const size_t &pos) const {
        if(pos < 0 || pos >= curElem) throw index_out_of_bound();
        return *(data[pos]);
	}
	T & operator[](const size_t &pos) {
        if(pos < 0 || pos >= curElem) throw index_out_of_bound();
        return *(data[pos]);
	}
	const T & operator[](const size_t &pos) const {
        if(pos < 0 || pos >= curElem) throw index_out_of_bound();
        return *(data[pos]);
	}
	const T & front() const {
	    if(this->curElem == 0) throw container_is_empty();
	    return *(data[0]);
	}
	const T & back() const {
        if(this->curElem == 0) throw container_is_empty();
        return *(data[curElem-1]);
	}
	iterator begin() {
        return iterator(this->data,0);
	}
	const_iterator cbegin() const {
        return const_iterator(this->data,0);
	}
	iterator end() {
	    return iterator(this->data,curElem);
	}
	const_iterator cend() const {
        return const_iterator(this->data,curElem);
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
	iterator insert(iterator pos, const T &value) {
	    for(int i = this->curElem;i > pos.pos;--i){
	        data[i] = data[i-1];
	    }
	    data[pos.pos] = new T(value);
	    curElem++;
	    if(curElem == maxElem) doubleSpace();
        return iterator(this->data,pos.pos);
	}
	iterator insert(const size_t &ind, const T &value) {
	    if(ind > this->curElem) throw index_out_of_bound();
        for(int i = this->curElem;i > ind;--i){
            data[i] = data[i-1];
        }
        data[ind] = new T(value);
        curElem++;
        if(curElem == maxElem) doubleSpace();
        return iterator(this->data,ind);
	}
	iterator erase(iterator pos) {
	    delete data[pos.pos];
	    for(int i = pos.pos;i < this->curElem - 1;++i){
	        data[i] = data[i+1];
	    }
	    data[curElem-1] = nullptr;
	    curElem--;
        return iterator(this->data,pos.pos);
	}
	iterator erase(const size_t &ind) {
	    if(ind >= this->curElem || ind < 0) throw index_out_of_bound();
	    delete data[ind];
        for(int i = ind;i < this->curElem - 1;++i){
            data[i] = data[i+1];
        }
        data[curElem-1] = nullptr;
        curElem--;
        return iterator(this->data,ind);
	}
	void push_back(const T &value) {
        data[this->curElem++] = new T(value);
        if (this->curElem == this->maxElem) doubleSpace();
    }
	void pop_back() {
	    if(this->curElem == 0) throw container_is_empty();
	    delete data[curElem-1];data[curElem-1] = nullptr;
	    this->curElem--;
	}
};
}

#endif
