//
// Created by 赵先生 on 2021/2/27.
//
#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>

namespace myContainer {
    template<class T>
    class List {
    public:
        struct Node {
            Node *before;
            Node *next;
            T *data;
            Node() : before(nullptr), next(nullptr), data(nullptr) {}
            Node(Node *_before, Node *_next, T *_data) : before(_before), next(_next), data(_data) {}
            ~Node(){
                if(data != nullptr) delete data;
            }
        };

        Node *head;
        Node *tail;
        int elemNum;
    public:
        List():head(nullptr),tail(nullptr),elemNum(0){}

        List & operator=(const List & o){
            if(this == &o) return *this;
            this->clear();
            if(o.elemNum == 0) return *this;
            int i = o.elemNum;
            while(i--){
                this->push_front((new T(*(o[i].data)))); // require T to have copy construction
            }
            this->elemNum = o.elemNum;
            return *this;
        }

        ~List(){
            this->clear();
        }

        void clear(){
            while(head != nullptr){
                Node* tmp = head;
                head = head->next;
                delete tmp;
            }
            head = nullptr;tail = nullptr;
            this->elemNum = 0;
        }

        void push_back( T*  o){
            if(!elemNum){
                elemNum++;
                head = new Node(nullptr, nullptr,o);
                tail = head;
            }else{
                tail->next = new Node(tail, nullptr,o);
                elemNum++;
                tail = tail->next;
            }
        }
        void push_front( T*  o){
            if(!elemNum){
                elemNum++;
                head = new Node(nullptr, nullptr,o);
                tail = head;
            }else{
                head->before = new Node(nullptr,head,o);
                elemNum++;
                head = head->before;
            }
        }

        Node & operator[](int pos) const { // can't use for modifying tail
            if(pos < 0 || pos >= elemNum) throw sjtu::index_out_of_bound();
            Node * temp1 = head;
            if(pos == 0) return *head;
            if(pos == elemNum - 1) return *tail;
            while(pos--){
                temp1 = temp1->next;
            }
            return *temp1;
        }

        Node & back() const{
            return *tail;
        }

        void insert(int pos, T*  o){
            if(elemNum == 0) push_front(o);
            if(pos >= elemNum) return;
            if(elemNum == 1){
                head->before = new Node(nullptr,head,0);
                head = head->before;
                elemNum++;
            }else{
                if(pos == 0){push_front(o);return;}
                Node * temp = &((*this)[pos]);
                (temp->before)->next = new Node(temp->before,temp,o);
                temp->before = temp->before->next;
                elemNum++;
                return;
            };
        }

        void erase(int pos){
            if(pos < 0 || pos >= elemNum) return;
            if(elemNum == 1){
                delete head;
                head = nullptr;
                tail = nullptr;
                elemNum--;
                return;
            }
            if(pos == 0){
                Node * temp = head;
                head = head->next;
                head->before = nullptr;
                elemNum--;
                delete temp;
                return;
            }
            if(pos == elemNum - 1){
                Node * temp = tail;
                tail = tail->before;
                tail->next = nullptr;
                elemNum--;
                delete temp;
                return;
            }
            Node * temp = &((*this)[pos]);
            temp->before->next = temp->next;
            temp->next->before = temp->before;
            elemNum--;
            delete temp;
        }

        int& size() {
            return elemNum;
        }
    };
}
namespace sjtu {

    const static int MaxBlock = 400;
    const static int HalfBlock = 200;
    const static int MergeBoundary = 250;
    const static int MinBlock = 50;

template<class T>
class deque {
private:
    struct Block;
    typedef typename myContainer::List<Block>::Node* nodePointer;
    struct Block{
        myContainer::List<T> dataSet;
        nodePointer inController;
        Block():inController(nullptr){}
        explicit Block(nodePointer o):inController(o){}
        Block(const Block &o){
            this->inController = nullptr;
            this->dataSet = o.dataSet;
        }
        void splitBlock(){ // ++elemNum shoule be run in deque;tail shoule be modified
            nodePointer tmp = this->inController;
            auto newBlock = new Block();
            auto newNode = new typename myContainer::List<Block>::Node(tmp,tmp->next,newBlock);
            newBlock->inController = newNode;
            // node
            if(tmp->next != nullptr) tmp->next->before = newNode;
            tmp->next = newNode;
            //Block
            newBlock->dataSet.head = &(this->dataSet[HalfBlock]);newBlock->dataSet.tail = &(this->dataSet[this->dataSet.elemNum - 1]);
            this->dataSet.tail = &(this->dataSet[HalfBlock-1]);
            this->dataSet.tail->next = nullptr;newBlock->dataSet.head->before = nullptr;
            newBlock->dataSet.elemNum = this->dataSet.elemNum - HalfBlock;this->dataSet.elemNum = HalfBlock;
        }
        void mergeBlock(){ // --elemNum should be run in deque;tail should be modified
            nodePointer former = this->inController;nodePointer latter = former->next;
            //Block
            this->dataSet.back().next = latter->data->dataSet.head;latter->data->dataSet.head->before = this->dataSet.tail;
            this->dataSet.tail = latter->data->dataSet.tail;
            this->dataSet.elemNum += latter->data->dataSet.size();
            latter->data->dataSet.head = nullptr;latter->data->dataSet.elemNum = 0;
            //node
            if(latter->next != nullptr) latter->next->before = former;
            former->next = latter->next;
            delete latter;
        }
    };
    int totalNum = 0;
    myContainer::List<Block> Controller;       //Controller.size()为缓冲区数目
public:
	class const_iterator;
	class iterator {
	    friend deque<T>;
	private:
	    bool validity = true;
        deque<T>* theQueue;
        nodePointer nodeptr;
        typename myContainer::List<T>::Node* dataptr;
	    // 0-base
	    int countInAll;
	    // end():true;this;nullptr,nullptr,totalnumer; //
	public:
	    iterator():theQueue(nullptr),countInAll(-1),nodeptr(nullptr),dataptr(nullptr),validity(true){}
	    iterator(const iterator & o) = default;
	    iterator( deque<T>*  _ptr,const int _all,nodePointer _nodeptr,typename myContainer::List<T>::Node* _dataptr,bool _validity = true):theQueue(_ptr),countInAll(_all),nodeptr(_nodeptr),dataptr(_dataptr),validity(_validity){}
	    void setInvalid(){validity = false;}

		iterator operator+(const int &n) const {
	        if(n == 0) return iterator(*this);
			if(countInAll + n > theQueue->totalNum){
                iterator temp(*this);
			    temp.setInvalid();
			    return temp;
			}
			if(countInAll + n ==  theQueue->totalNum){
                return iterator(theQueue,theQueue->totalNum, nullptr, nullptr,true);//end();
			}
			if(theQueue->totalNum == 0){
                return iterator(theQueue,theQueue->totalNum, nullptr, nullptr,true);//end();
            }
			int _countInAll = countInAll + n;nodePointer _nodeptr = nullptr;typename myContainer::List<T>::Node* _dataptr = nullptr;
			theQueue->getPosition(_countInAll,_nodeptr,_dataptr);
			return iterator(theQueue,_countInAll,_nodeptr,_dataptr,true);
		}
		iterator operator-(const int &n) const {
            if(n == 0) return iterator(*this);
            if(countInAll - n < 0){
                iterator temp(*this);
                temp.setInvalid();
                return temp;
            }
            if(theQueue->totalNum == 0){
                return iterator(theQueue,theQueue->totalNum, nullptr, nullptr,true);//end();
            }
            int _countInAll = countInAll - n;nodePointer _nodeptr = nullptr;typename myContainer::List<T>::Node* _dataptr = nullptr;
            theQueue->getPosition(_countInAll,_nodeptr,_dataptr);
            return iterator(theQueue,_countInAll,_nodeptr,_dataptr,true);
		}
		int operator-(const iterator &rhs) const {
			if(theQueue != rhs.theQueue) throw invalid_iterator();
			int dis = countInAll - rhs.countInAll;
			return dis;
		}
		iterator& operator+=(const int &n) {
	        if(n == 0) return *this;
            if(countInAll + n > theQueue->totalNum){
                this->setInvalid();
                return *this;
            }
            if(countInAll + n ==  theQueue->totalNum){
                //end
                this->countInAll = theQueue->totalNum; this->nodeptr = nullptr;this->dataptr = nullptr;
                return *this;
            }
            if(theQueue->totalNum == 0){
                this->countInAll = theQueue->totalNum;this->nodeptr = nullptr;this->dataptr = nullptr;
                return *this;
            }
            int _countInAll = countInAll + n;nodePointer _nodeptr = nullptr;typename myContainer::List<T>::Node* _dataptr = nullptr;
            theQueue->getPosition(_countInAll,_nodeptr,_dataptr);
            countInAll = _countInAll;nodeptr = _nodeptr;dataptr = _dataptr;
            return *this;
		}
		iterator& operator-=(const int &n) {
            if(n == 0) return *this;
            if(countInAll - n < 0){
                this->setInvalid();
                return *this;
            }
            if(theQueue->totalNum == 0){
                this->countInAll = theQueue->totalNum;this->nodeptr = nullptr;this->dataptr = nullptr;
                return *this;
            }
            int _countInAll = countInAll - n;nodePointer _nodeptr = nullptr;typename myContainer::List<T>::Node* _dataptr = nullptr;
            theQueue->getPosition(_countInAll,_nodeptr,_dataptr);
            countInAll = _countInAll;nodeptr = _nodeptr;dataptr = _dataptr;
            return *this;
		}

		iterator operator++(int) {
		    iterator temp(*this);
		    if(countInAll == theQueue->totalNum - 1){ // end();
		        this->countInAll ++;nodeptr = nullptr;dataptr = nullptr;
		        return temp;
		    }
		    if(dataptr->next == nullptr){
		        nodeptr = nodeptr->next;
		        dataptr = nodeptr->data->dataSet.head;
		    }else{
		        dataptr = dataptr->next;
		    }
		    countInAll++;
		    return temp;
		}
		iterator& operator++() {
            if(countInAll == theQueue->totalNum - 1){
                this->countInAll ++;nodeptr = nullptr;dataptr = nullptr;
                return *this;
            }
            if(nodeptr == nullptr && dataptr == nullptr && validity){
                this->setInvalid();
                return *this;
            }
            if(dataptr->next == nullptr){
                nodeptr = nodeptr->next;
                dataptr = nodeptr->data->dataSet.head;
            }else{
                dataptr = dataptr->next;
            }
            countInAll++;
            return *this;
		}
		iterator operator--(int) {
		    iterator temp(*this);
		    if(countInAll == 0){
                this->countInAll ++;nodeptr = nullptr;dataptr = nullptr;this->setInvalid();
                return temp;
		    }
		    if(dataptr == nullptr && nodeptr == nullptr && validity){
		        countInAll--;
		        nodeptr = theQueue->Controller.tail;dataptr = nodeptr->data->dataSet.tail;
                return temp;
		    }
		    if(dataptr->before == nullptr){
		        nodeptr = nodeptr->before;
		        dataptr = nodeptr->data->dataSet.tail;
		    }else{
		        dataptr = dataptr->before;
		    }
		    countInAll--;
		    return temp;
		}
		iterator& operator--() {
            if(countInAll == 0){
                this->countInAll ++;nodeptr = nullptr;dataptr = nullptr;this->setInvalid();
                return *this;
            }
            if(countInAll == theQueue->totalNum){
                this->countInAll--;this->nodeptr = theQueue->Controller.tail;this->dataptr = theQueue->Controller.tail->data->dataSet.tail;
                return *this;
            }
            if(dataptr->before == nullptr){
                nodeptr = nodeptr->before;
                dataptr = nodeptr->data->dataSet.tail;
            }else{
                dataptr = dataptr->before;
            }
            countInAll--;
            return *this;
		}
		T& operator*() const {
		    if((!validity) || nodeptr == nullptr) throw invalid_iterator();
		    return *(dataptr->data);
		}
		T* operator->() const noexcept {
            if((!validity) || nodeptr == nullptr) throw invalid_iterator();
            return dataptr->data;
		}
		bool operator==(const iterator &rhs) const {
            return (theQueue == rhs.theQueue && countInAll == rhs.countInAll && nodeptr == rhs.nodeptr && validity == rhs.validity);
		}
		bool operator==(const const_iterator &rhs) const {
            return (theQueue == rhs.theQueue && countInAll == rhs.countInAll && nodeptr == rhs.nodeptr && validity == rhs.validity);
        }
		bool operator!=(const iterator &rhs) const {
            return !(theQueue == rhs.theQueue && countInAll == rhs.countInAll && nodeptr == rhs.nodeptr && validity == rhs.validity);
		}
		bool operator!=(const const_iterator &rhs) const {
            return !(theQueue == rhs.theQueue && countInAll == rhs.countInAll && nodeptr == rhs.nodeptr && validity == rhs.validity);
        }
	};
	class const_iterator {
        friend deque<T>;
    private:
        bool validity = true;
        const deque<T> * theQueue;
        // 0-base
        int index;
        int countInAll;
        int countInBlock;
	public:
	    const_iterator():theQueue(nullptr),countInAll(-1),countInBlock(-1),index(-1),validity(true){}
	    const_iterator(const const_iterator & o) = default;
	    const_iterator(const deque<T>*  _ptr,const int _all,const int _block,const int _index,bool _validity = true):theQueue(_ptr),countInAll(_all),countInBlock(_block),index(_index),validity(_validity){}
	    explicit const_iterator(const iterator & o):theQueue(o.theQueue),countInAll(o.countInAll),countInBlock(o.countInBlock),index(o.index),validity(o.validity){}
	    void setInvalid(){validity = false;}
        const_iterator operator+(const int &n) const {
            if(countInAll + n >= theQueue->totalNum){
                const_iterator temp(theQueue,theQueue->totalNum,-1,-1,false);
                return temp;
            }
            int _countInAll = countInAll + n,_index = 0;
            int temp = _countInAll;
            while(temp >= (theQueue->Controller[_index].data)->dataSet.size()){
                temp -= (theQueue->Controller[_index].data)->dataSet.size();
                _index++;
            }
            return const_iterator(theQueue,_countInAll,temp,_index,true);
        }
        const_iterator operator-(const int &n) const {
            if(countInAll - n < 0){
                const_iterator temp(theQueue,countInAll,-1,-1,false);
                return temp;
            }
            int _countInAll = countInAll - n,_index = 0;
            int temp = _countInAll;
            while(temp >= (theQueue->Controller[_index].data)->dataSet.size()){
                temp -= (theQueue->Controller[_index].data)->dataSet.size();
                _index++;
            }
            return const_iterator(theQueue,_countInAll,temp,_index);
        }
        int operator-(const const_iterator &rhs) const {
            if(theQueue != rhs.theQueue) throw invalid_iterator();
            int dis = countInAll - rhs.countInAll;
            return dis;
        }
        const_iterator& operator+=(const int &n) {
            if(countInAll + n >= theQueue->totalNum){
                this->countInBlock = -1;this->index = -1;
                this->setInvalid();
                return *this;
            }
            int _countInAll = countInAll + n,_index = 0;
            int temp = _countInAll;
            while(temp >= (theQueue->Controller[_index].data)->dataSet.size()){
                temp -= (theQueue->Controller[_index].data)->dataSet.size();
                _index++;
            }
            countInAll = _countInAll;countInBlock = temp;index = _index;
            return *this;
        }
        const_iterator& operator-=(const int &n) {
            if(countInAll - n < 0){
                this->setInvalid();this->countInBlock = -1;this->index = -1;
                return *this;
            }
            int _countInAll = countInAll - n,_index = 0;
            int temp = _countInAll;
            while(temp >= (theQueue->Controller[_index].data)->dataSet.size()){
                temp -= (theQueue->Controller[_index].data)->dataSet.size();
                _index++;
            }
            countInAll = _countInAll;countInBlock = temp;index = _index;
            return *this;
        }
        const_iterator operator++(int) {
            const_iterator temp(*this);
            if(countInAll == theQueue->totalNum - 1){
                this->countInAll ++;this->countInBlock = -1;this->index = -1;this->setInvalid();
                return temp;
            }
            int boundary = (theQueue->Controller[index].data)->dataSet.size();
            if(countInBlock == boundary - 1){
                this->countInAll ++;this->countInBlock = 0;this->index++;
                return temp;
            }
            this->countInAll ++;this->countInBlock++;
            return temp;
        }
        const_iterator& operator++() {
            if(countInAll == theQueue->totalNum - 1){
                this->countInAll ++;this->countInBlock = -1;this->index = -1;this->setInvalid(); //end();
                return *this;
            }
            int boundary = (theQueue->Controller[index].data)->dataSet.size();
            if(countInBlock == boundary - 1){
                this->countInAll ++;this->countInBlock = 0;this->index++;
                return *this;
            }
            this->countInAll ++;this->countInBlock++;
            return *this;
        }
        const_iterator operator--(int) {
            const_iterator temp(*this);
            if(countInAll == 0){
                this->countInAll = -1;this->countInBlock = -1;this->index = -1;this->setInvalid();
                return temp;
            }
            if(countInBlock == 0){
                this->countInAll--;this->index--;this->countInBlock = (theQueue->Controller[index].data)->dataSet.size() - 1;
                return temp;
            }
            this->countInAll--;this->countInBlock--;
            return temp;
        }
        const_iterator& operator--() {
            if(countInAll == 0){
                this->countInAll = -1;this->countInBlock = -1;this->index = -1;this->setInvalid();
                return *this;
            }
            if((!validity) && index == -1 && countInBlock == -1){
                this->validity = true;this->countInAll = theQueue->totalNum-1;
                int temp = countInAll;int _index = 0;
                while(temp >= (theQueue->Controller[_index].data)->dataSet.size()){
                    temp -= (theQueue->Controller[_index].data)->dataSet.size();
                    _index++;
                }
                countInBlock = temp;index = _index;
                return *this;
            }
            if(countInBlock == 0){
                this->countInAll--;this->index--;this->countInBlock = (theQueue->Controller[index].data)->dataSet.size() - 1;
                return *this;
            }
            this->countInAll--;this->countInBlock--;
            return *this;
        }
        T& operator*() const {
            if(!validity) throw invalid_iterator();
            return *(theQueue->Controller[index].data->dataSet[countInBlock].data);
        }
        T* operator->() const noexcept {
            if(!validity) throw invalid_iterator();
            return (theQueue->Controller[index].data->dataSet[countInBlock].data);
        }
        bool operator==(const iterator &rhs) const {
            return (theQueue == rhs.theQueue && countInAll == rhs.countInAll && countInBlock == rhs.countInBlock && validity == rhs.validity);

        }
        bool operator==(const const_iterator &rhs) const {
            return (theQueue == rhs.theQueue && countInAll == rhs.countInAll && countInBlock == rhs.countInBlock && validity == rhs.validity);
        }
        bool operator!=(const iterator &rhs) const {
            return !(theQueue == rhs.theQueue && countInAll == rhs.countInAll && countInBlock == rhs.countInBlock && validity == rhs.validity);
        }
        bool operator!=(const const_iterator &rhs) const {
            return !(theQueue == rhs.theQueue && countInAll == rhs.countInAll && countInBlock == rhs.countInBlock && validity == rhs.validity);
        }
	};
	deque():totalNum(0) {}
	deque(const deque &other) {
        if(!other.Controller.elemNum){ this->totalNum = 0;return;}
	    this->Controller = other.Controller; this->totalNum = other.totalNum;
	    int i = this->Controller.size();
	    while(i--){
            (this->Controller[i].data)->inController = &(Controller[i]);
	    }
	}
	~deque() {
	    this->clear();
	}
	deque &operator=(const deque &other) {
        if(this == &other) return *this;
        this->clear();
        this->totalNum = other.totalNum;
        if(totalNum == 0) return *this;
        this->Controller = other.Controller;
        int i = this->Controller.size();
        while(i--){
            (this->Controller[i].data)->inController = &(Controller[i]);
        }
        return *this;
	}
	/**
	 * access specified element with bounds checking
	 * throw index_out_of_bound if out of bound.
	 */
	void getPosition(int _countInAll,nodePointer & _nodeptr,typename myContainer::List<T>::Node* & _dataptr) const{
	    _nodeptr = Controller.head;
	    while(_countInAll >= _nodeptr->data->dataSet.size()){
	        _countInAll -= _nodeptr->data->dataSet.size();
	        _nodeptr = _nodeptr->next;
	    }
	    _dataptr = &(_nodeptr->data->dataSet[_countInAll]);
	}

	T & at(const size_t &pos) {
        if(pos < 0 || pos >= totalNum) throw index_out_of_bound();
        int tempcount = pos;nodePointer nodeptr = nullptr;typename myContainer::List<T>::Node* dataptr = nullptr;
        getPosition(tempcount,nodeptr,dataptr);
        return *(dataptr->data);
	}
	const T & at(const size_t &pos) const {
        if(pos < 0 || pos >= totalNum) throw index_out_of_bound();
        int tempcount = pos;nodePointer nodeptr = nullptr;typename myContainer::List<T>::Node* dataptr = nullptr;
        getPosition(tempcount,nodeptr,dataptr);
        return *(dataptr->data);
	}
	T & operator[](const size_t &pos) {
        if(pos < 0 || pos >= totalNum) throw index_out_of_bound();
        int tempcount = pos;nodePointer nodeptr = nullptr;typename myContainer::List<T>::Node* dataptr = nullptr;
        getPosition(tempcount,nodeptr,dataptr);
        return *(dataptr->data);
	}
	const T & operator[](const size_t &pos) const {
        if(pos < 0 || pos >= totalNum) throw index_out_of_bound();
        int tempcount = pos;nodePointer nodeptr = nullptr;typename myContainer::List<T>::Node* dataptr = nullptr;
        getPosition(tempcount,nodeptr,dataptr);
        return *(dataptr->data);
	}
	/**
	 * access the first element
	 * throw container_is_empty when the container is empty.
	 */
	const T & front() const {
	    if(totalNum == 0) throw container_is_empty();
        return *((Controller[0].data)->dataSet[0].data);
	}
	/**
	 * access the last element
	 * throw container_is_empty when the container is empty.
	 */
	const T & back() const {
        if(totalNum == 0) throw container_is_empty();
        return *((Controller.back().data)->dataSet.back().data);
	}
	/**
	 * returns an iterator to the beginning.
	 */
	iterator begin() {
	    if(totalNum == 0){return end();}
	    return iterator(this,0,Controller.head,Controller.head->data->dataSet.head,true);
	}
	const_iterator cbegin() const {
        return const_iterator(this,0,0,0,true);
    }
	/**
	 * returns an iterator to the end.
	 */
	iterator end() {
	    return iterator(this,this->totalNum, nullptr, nullptr,true);
	}
	const_iterator cend() const {
        return const_iterator(this,this->totalNum,-1,-1,false);
    }
	/**
	 * checks whether the container is empty.
	 */
	bool empty() const {
	    return totalNum == 0;
	}
	/**
	 * returns the number of elements
	 */
	size_t size() const {
	    return totalNum;
	}
	/**
	 * clears the contents
	 */
	void clear() {
        this->Controller.clear();
        totalNum = 0;
    }
	/**
	 * inserts elements at the specified locat on in the container.
	 * inserts value before pos
	 * returns an iterator pointing to the inserted value
	 *     throw if the iterator is invalid or it point to a wrong place.
	 */
	iterator insert(iterator pos, const T &value) {
	    if(totalNum == 0 && pos == end()){
	        this->push_back(value);
            return iterator(this,totalNum-1,Controller.tail,Controller.tail->data->dataSet.tail,true);
        }
	    if(!pos.validity) throw invalid_iterator();
	    if(pos == end()){
	        this->push_back(value);
            return iterator(this,totalNum-1,Controller.tail,Controller.tail->data->dataSet.tail,true);
	    }
	    if(pos.theQueue != this || pos.countInAll >= totalNum) throw invalid_iterator(); //todo:can't insert to totalnumber
	    if(pos.countInAll == 0){
	        this->push_front(value);
            return begin();
	    }
	    nodePointer _nodeptr = pos.nodeptr;typename myContainer::List<T>::Node* _dataptr = pos.dataptr;
	    int countInBlock = 0;typename myContainer::List<T>::Node* _tempptr = _nodeptr->data->dataSet.head;
	    while(_tempptr != _dataptr){
	        countInBlock++;
	        _tempptr = _tempptr->next;
	    }
	    _nodeptr->data->dataSet.insert(countInBlock,new T(value));totalNum++;
	    if(_nodeptr->data->dataSet.size() >= MaxBlock){
	        if(_nodeptr->next == nullptr){
	            _nodeptr->data->splitBlock();
	            Controller.size()++;
	            Controller.tail = _nodeptr->next;
	        }else{
                _nodeptr->data->splitBlock();
                Controller.size()++;
	        }
	    }
	    getPosition(pos.countInAll,_nodeptr,_dataptr);
        return iterator(this,pos.countInAll,_nodeptr,_dataptr);
	}
	/**
	 * removes specified element at pos.
	 * removes the element at pos.
	 * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
	 * throw if the container is empty, the iterator is invalid or it points to a wrong place.
	 */
	iterator erase(iterator pos) {
        if(pos.theQueue != this || pos.countInAll >= totalNum) throw invalid_iterator();
	    if(this->totalNum == 0) throw container_is_empty();
	    if(!pos.validity) throw invalid_iterator();
        if(pos.countInAll == 0){
            this->pop_front();
            return begin();
        }
        if(pos.countInAll == totalNum - 1){
            this->pop_back();
            return end();
        }
        nodePointer _nodeptr = pos.nodeptr;typename myContainer::List<T>::Node* _dataptr = pos.dataptr;
        int countInBlock = 0;typename myContainer::List<T>::Node* _tempptr = _nodeptr->data->dataSet.head;
        while(_tempptr != _dataptr){
            countInBlock++;
            _tempptr = _tempptr->next;
        }
        _nodeptr->data->dataSet.erase(countInBlock);totalNum--;
        if(_nodeptr->next != nullptr && _nodeptr->data->dataSet.size() < MinBlock){
            _nodeptr->data->mergeBlock();
            if(_nodeptr->next == nullptr){Controller.tail = _nodeptr;}
            Controller.size()--;
        }
        getPosition(pos.countInAll,_nodeptr,_dataptr);
        return iterator(this,pos.countInAll,_nodeptr,_dataptr);
	}
	/**
	 * adds an element to the end
	 */
	void push_back(const T &value) {
        if(totalNum == 0){
            Controller.push_front(new Block());
            (Controller[0].data)->inController = Controller.head;
            (Controller[0].data)->dataSet.push_front(new T(value));
            totalNum++;
        }else{
            (Controller.back().data)->dataSet.push_back(new T(value));
            totalNum++;
            if((Controller.back().data)->dataSet.size() >= MaxBlock){
                (Controller.back().data)->splitBlock();
                Controller.size()++;
                Controller.tail = Controller.tail->next;
            }
        }
	}
	/**
	 * removes the last element
	 *     throw when the container is empty.
	 */
	void pop_back() {
        if(!totalNum) throw container_is_empty();
        (Controller.back().data)->dataSet.erase((Controller.back().data)->dataSet.size() - 1);
        totalNum--;
        if((Controller.back().data)->dataSet.size() == 0){
            Controller.erase(Controller.size() - 1);
        }
	}
	/**
	 * inserts an element to the beginning.
	 */
	void push_front(const T &value) {
	    if(totalNum == 0){
	        Controller.push_front(new Block());
            (Controller[0].data)->inController = Controller.head;
            (Controller[0].data)->dataSet.push_front(new T(value));
	        totalNum++;
	    }else{
            (Controller[0].data)->dataSet.push_front(new T(value));
            totalNum++;
	        if((Controller[0].data)->dataSet.size() >= MaxBlock){
                (Controller[0].data)->splitBlock();
	            // attention :: list[] use tail to get offset;
	            if(Controller.tail == &(Controller[0])){Controller.tail = Controller[0].next;}
                Controller.size()++;
            }
	    }
	}
	/**
	 * removes the first element.
	 *     throw when the container is empty.
	 */
	void pop_front() {
	    if(!totalNum) throw container_is_empty();
        (Controller[0].data)->dataSet.erase(0);
	    totalNum--;
        if(!totalNum){Controller.erase(0);return;}
        if(Controller[0].next != nullptr && (Controller[0].data)->dataSet.size() < MinBlock){
            (Controller[0].data)->mergeBlock();
            if(Controller[0].next == nullptr){Controller.tail = Controller.head;}
            Controller.size()--;
            return;
        }
	}
};
}

#endif
