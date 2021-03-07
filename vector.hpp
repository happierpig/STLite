#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>

namespace sjtu {
    const static int initSize = 10;
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
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
	/**
	 * TODO
	 * a type for actions of the elements of a vector, and you should write
	 *   a class named const_iterator with same interfaces.
	 */
	/**
	 * you can see RandomAccessIterator at CppReference for help.
	 */

	class const_iterator;
	class iterator {
	    friend vector<T>;
	    friend const_iterator;
	private:
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */
		 T ** ptr = nullptr;
		 int pos;

	public:
	    iterator() = default;
	    iterator(const iterator & o) = default;
	    iterator(T ** _ptr,int _pos):ptr(_ptr),pos(_pos){}
	    iterator & operator =(const iterator & o) = default;
		/**
		 * return a new iterator which pointer n-next elements
		 * as well as operator-
		 */
		iterator operator+(const int &n) const {
			return iterator(this->ptr,this->pos + n);
		}
		iterator operator-(const int &n) const {
            return iterator(this->ptr,this->pos - n);
        }
		// return the distance between two iterators,
		// if these two iterators point to different vectors, throw invaild_iterator.
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
		/**
		 * TODO iter++
		 */
		iterator operator++(int) {
		    iterator it(*this);
		    this->pos++;
		    return it;
		}
		/**
		 * TODO ++iter
		 */
		iterator& operator++() {
		    this->pos++;
            return *this;
		}
		/**
		 * TODO iter--
		 */
		iterator operator--(int) {
		    iterator it(*this);
		    this->pos--;
            return it;
		}
		/**
		 * TODO --iter
		 */
		iterator& operator--() {
		    this->pos--;
            return *this;
		}
		/**
		 * TODO *it
		 */
		T& operator*() const{
		    return *(ptr[pos]);
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory address).
		 */
		bool operator==(const iterator &rhs) const {
            if(this->ptr == rhs.ptr && this->pos == rhs.pos) return true;
            else return false;
		}
		bool operator==(const const_iterator &rhs) const {
            if(this->ptr == rhs.ptr && this->pos == rhs.pos) return true;
            else return false;
		}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {
            if(this->ptr == rhs.ptr && this->pos == rhs.pos) return false;
            else return true;
		}
		bool operator!=(const const_iterator &rhs) const {
            if(this->ptr == rhs.ptr && this->pos == rhs.pos) return false;
            else return true;
		}
	};
	/**
	 * TODO
	 * has same function as iterator, just for a const object.
	 */
	class const_iterator {
	    friend vector<T>;
    private:
        T ** ptr = nullptr;
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
	/**
	 * TODO Constructs
	 * At least two: default constructor, copy constructor
	 */
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

	/**
	 * TODO Destructor
	 */
	~vector() {
	    if(this->data != nullptr){
	        for(int i = 0;i < maxElem;++i){
	            if(this->data[i] != nullptr) delete data[i];
	        }
	        delete [] data;
	    }
	}
	/**
	 * TODO Assignment operator
	 */
	vector &operator= (const vector &other) {
        if(this == &other) return *this;
        this->clear();
        this->maxElem = other.maxElem;this->curElem = other.curElem;
        this->data = new T*[this->maxElem];
        for(int i = 0;i < this->curElem;++i) this->data[i] = new T(*(other.data[i]));
        for(int i = curElem;i < maxElem;++i) this->data[i] = nullptr;
        return *this;
	}
	/**
	 * assigns specified element with bounds checking
	 * throw index_out_of_bound if pos is not in [0, size)
	 */
	T & at(const size_t &pos) {
	    if(pos < 0 || pos >= curElem) throw index_out_of_bound();
        return *(data[pos]);
	}
	const T & at(const size_t &pos) const {
        if(pos < 0 || pos >= curElem) throw index_out_of_bound();
        return *(data[pos]);
	}
	/**
	 * assigns specified element with bounds checking
	 * throw index_out_of_bound if pos is not in [0, size)
	 * !!! Pay attentions
	 *   In STL this operator does not check the boundary but I want you to do.
	 */
	T & operator[](const size_t &pos) {
        if(pos < 0 || pos >= curElem) throw index_out_of_bound();
        return *(data[pos]);
	}
	const T & operator[](const size_t &pos) const {
        if(pos < 0 || pos >= curElem) throw index_out_of_bound();
        return *(data[pos]);
	}
	/**
	 * access the first element.
	 * throw container_is_empty if size == 0
	 */
	const T & front() const {
	    if(this->curElem == 0) throw container_is_empty();
	    return *(data[0]);
	}
	/**
	 * access the last element.
	 * throw container_is_empty if size == 0
	 */
	const T & back() const {
        if(this->curElem == 0) throw container_is_empty();
        return *(data[curElem-1]);
	}
	/**
	 * returns an iterator to the beginning.
	 */
	iterator begin() {
        return iterator(this->data,0);
	}
	const_iterator cbegin() const {
        return const_iterator(this->data,0);
	}
	/**
	 * returns an iterator to the end.
	 */
	iterator end() {
	    return iterator(this->data,curElem);
	}
	const_iterator cend() const {
        return const_iterator(this->data,curElem);
    }
	/**
	 * checks whether the container is empty
	 */
	bool empty() const {
	    return (curElem == 0);
	}
	/**
	 * returns the number of elements
	 */
	size_t size() const {
        return curElem;
	}
	/**
	 * clears the contents
	 */
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
	/**
	 * inserts value before pos
	 * returns an iterator pointing to the inserted value.
	 */
	iterator insert(iterator pos, const T &value) {
	    for(int i = this->curElem;i > pos.pos;--i){
	        data[i] = data[i-1];
	    }
	    data[pos.pos] = new T(value);
	    curElem++;
	    if(curElem == maxElem) doubleSpace();
        return iterator(this->data,pos.pos);
	}
	/**
	 * inserts value at index ind.
	 * after inserting, this->at(ind) == value
	 * returns an iterator pointing to the inserted value.
	 * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
	 */
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
	/**
	 * removes the element at pos.
	 * return an iterator pointing to the following element.
	 * If the iterator pos refers the last element, the end() iterator is returned.
	 */
	iterator erase(iterator pos) {
	    delete data[pos.pos];
	    for(int i = pos.pos;i < this->curElem - 1;++i){
	        data[i] = data[i+1];
	    }
	    data[curElem-1] = nullptr;
	    curElem--;
        return iterator(this->data,pos.pos);
	}
	/**
	 * removes the element with index ind.
	 * return an iterator pointing to the following element.
	 * throw index_out_of_bound if ind >= size
	 */
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
	/**
	 * adds an element to the end.
	 */
	void push_back(const T &value) {
        data[this->curElem++] = new T(value);
        if (this->curElem == this->maxElem) doubleSpace();
    }
	/**
	 * remove the last element from the end.
	 * throw container_is_empty if size() == 0
	 */
	void pop_back() {
	    if(this->curElem == 0) throw container_is_empty();
	    delete data[curElem-1];data[curElem-1] = nullptr;
	    this->curElem--;
	}
};


}

#endif
