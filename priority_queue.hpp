#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {
template<typename T, class Compare = std::less<T>>
class priority_queue {
private:
    struct Node{
        T data;
        int dis;
        Node * lson;Node * rson;
        Node():dis(0),lson(nullptr),rson(nullptr){}
        Node(const T & _data,int _dis,Node * _lson,Node * _rson):data(_data),dis(_dis),lson(_lson),rson(_rson){}
    };
    int elemNum;Node * root;
    Compare funciton;
    Node * merge(Node * x,Node * y){
        if(x == nullptr) return y;
        if(y == nullptr) return x;
        if (funciton(x->data, y->data)) std::swap(x, y);
        x->rson = merge(x->rson,y);
        if(x->lson == nullptr) std::swap(x->lson,x->rson);
        else if(x->lson->dis < x->rson->dis) std::swap(x->lson,x->rson);
        x->dis = (x->rson == nullptr ? 0 : x->rson->dis) + 1;
        return x;
    }
    void clear(Node * _root){
        if(_root == nullptr) return;
        if(_root->lson != nullptr) clear(_root->lson);
        if(_root->rson != nullptr) clear(_root->rson);
        delete _root;
    }
    void copyTree(Node * & _origin,Node * _copy){
        //clear work need doing before entering funciton
        if(_copy == nullptr) {_origin = nullptr ; return;}
        _origin = new Node(*_copy);
        if(_origin->rson != nullptr) copyTree(_origin->rson,_copy->rson);
        if(_origin->lson != nullptr) copyTree(_origin->lson,_copy->lson);
    }

public:
	priority_queue():elemNum(0),root(nullptr){}
	priority_queue(const priority_queue &other) {
	    elemNum = other.elemNum;funciton = other.funciton;
        if(other.root == nullptr) return;
        copyTree(this->root,other.root);
	}
	~priority_queue() {clear(this->root);}
	priority_queue &operator=(const priority_queue &other) {
        if(this == &other) return *this;
        clear(this->root);
        this->elemNum = other.elemNum;this->funciton = other.funciton;
        if(other.root == nullptr){this->root = nullptr;return *this;}
        copyTree(this->root,other.root);
        return *this;
    }
    const T & top() const {
	    if(root == nullptr || this->elemNum == 0) throw container_is_empty();
	    return this->root->data;
	}
	void push(const T &e) {
	    elemNum++;
//	    Node * tmp = new Node;tmp->data = e;tmp->rson = nullptr;tmp->lson = nullptr;tmp->dis = 0;
        Node * tmp = new Node(e,0, nullptr, nullptr);
	    this->root = merge(this->root,tmp);
	}
	void pop() {
        if(root == nullptr || this->elemNum == 0) throw container_is_empty();
        elemNum--;
        Node * tmp = this->root;
        root = merge(root->lson,root->rson);
        delete tmp;
    }
    size_t size() const {
        return elemNum;
	}
	bool empty() const {
        return elemNum == 0;
	}
	void merge(priority_queue &other) {
	    this->elemNum+=other.elemNum;
	    this->root = merge(this->root,other.root);
	    other.root = nullptr;
	    other.elemNum = 0;
	}
};
}

#endif
