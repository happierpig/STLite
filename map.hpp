/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"
#include "AVLtree.hpp"

namespace sjtu {

    template<
            class Key,
            class T,
            class Compare = std::less<Key>
    > class map {
    private:
        myContainer::AVLtree<const Key,T,Compare> theTree;
    public:
        typedef pair<const Key, T> value_type;
        typedef typename myContainer::AVLtree<const Key,T,Compare>::Node * Nodeptr;
        class const_iterator;
        class iterator {
            friend class map;
        private:
            map * toMap;
            Nodeptr toNode;
            bool checkBegin() const{return toNode == toMap->theTree.getMin(toMap->theTree.root);}
            bool checkMax() const {return toNode == toMap->theTree.getMax(toMap->theTree.root);}
            bool isValid;bool isEnd;
        public:
            iterator():toMap(nullptr),toNode(nullptr),isValid(false),isEnd(false){}
            iterator(map * _toMap,Nodeptr _toNode,bool _isValid,bool _isEnd):toMap(_toMap),toNode(_toNode),isValid(_isValid),isEnd(_isEnd){}
            iterator(const iterator &other) = default;
            iterator operator++(int) {
                iterator tmp(*this);
                if(isValid){
                    if(isEnd) {throw invalid_iterator();}
                    else{
                        if(checkMax()){
                            isEnd = true;toNode = nullptr;
                        }else toNode = toMap->theTree.getSuccessor(toNode);
                    }
                }
                return tmp;
            }
            iterator & operator++() {
                if(isValid){
                    if(isEnd) { throw invalid_iterator(); }
                    else{
                        if(checkMax()){
                            isEnd = true;toNode = nullptr;
                        }else toNode = toMap->theTree.getSuccessor(toNode);
                    }
                }
                return *this;
            }
            iterator operator--(int) {
                iterator tmp(*this);
                if(isValid){
                    if(checkBegin()){
                        throw invalid_iterator();
                    }else{
                        if(isEnd){
                            isEnd = false;toNode = toMap->theTree.getMax(toMap->theTree.getRoot());
                        }else toNode = toMap->theTree.getPredecessor(toNode);
                    }
                }
                return tmp;
            }
            iterator & operator--() {
                if(isValid){
                    if(checkBegin()){
                        throw invalid_iterator();
                    }else{
                        if(isEnd){
                            isEnd = false;toNode = toMap->theTree.getMax(toMap->theTree.getRoot());
                        }else toNode = toMap->theTree.getPredecessor(toNode);
                    }
                }
                return *this;
            }
            value_type & operator*() const {
                if(!isValid || isEnd) throw invalid_iterator();
                return *(toNode->value);
            }
            bool operator==(const iterator &rhs) const {return (toMap == rhs.toMap) && (toNode == rhs.toNode) && (isEnd == rhs.isEnd);}
            bool operator==(const const_iterator &rhs) const {return (toMap == rhs.toMap) && (toNode == rhs.toNode) && (isEnd == rhs.isEnd);}
            bool operator!=(const iterator &rhs) const {return !((toMap == rhs.toMap) && (toNode == rhs.toNode) && (isEnd == rhs.isEnd));}
            bool operator!=(const const_iterator &rhs) const {return !((toMap == rhs.toMap) && (toNode == rhs.toNode) && (isEnd == rhs.isEnd));}
            value_type* operator->() const noexcept {
                if(!isValid || isEnd) throw invalid_iterator();
                return toNode->value;
            }
        };
        class const_iterator { //attention: used by const map
            friend class map;
        private:
            const map * toMap;
            Nodeptr toNode;
            bool checkBegin() const{return toNode == toMap->theTree.getMin(toMap->theTree.root);}
            bool checkMax() const {return toNode == toMap->theTree.getMax(toMap->theTree.root);}
            bool isValid;bool isEnd;
        public:
            const_iterator():toMap(nullptr),toNode(nullptr),isValid(false),isEnd(false){}
            const_iterator(const map * _toMap,Nodeptr _toNode,bool _isValid,bool _isEnd):toMap(_toMap),toNode(_toNode),isValid(_isValid),isEnd(_isEnd){}
            const_iterator(const const_iterator &other) = default;
            const_iterator(const iterator &other):toMap(other.toMap),toNode(other.toNode),isValid(other.isValid),isEnd(other.isEnd){}
            const_iterator operator++(int) {
                const_iterator tmp(*this);
                if(isValid){
                    if(isEnd) {throw invalid_iterator();}
                    else{
                        if(checkMax()){
                            isEnd = true;toNode = nullptr;
                        }else toNode = toMap->theTree.getSuccessor(toNode);
                    }
                }
                return tmp;
            }
            const_iterator & operator++() {
                if(isValid){
                    if(isEnd) { throw invalid_iterator(); }
                    else{
                        if(checkMax()){
                            isEnd = true;toNode = nullptr;
                        }else toNode = toMap->theTree.getSuccessor(toNode);
                    }
                }
                return *this;
            }
            const_iterator operator--(int) {
                const_iterator tmp(*this);
                if(isValid){
                    if(checkBegin()){
                        throw invalid_iterator();
                    }else{
                        if(isEnd){
                            isEnd = false;toNode = toMap->theTree.getMax(toMap->theTree.getRoot());
                        }else toNode = toMap->theTree.getPredecessor(toNode);
                    }
                }
                return tmp;
            }
            const_iterator & operator--() {
                if(isValid){
                    if(checkBegin()){
                        throw invalid_iterator();
                    }else{
                        if(isEnd){
                            isEnd = false;toNode = toMap->theTree.getMax(toMap->theTree.getRoot());
                        }else toNode = toMap->theTree.getPredecessor(toNode);
                    }
                }
                return *this;
            }
            const value_type & operator*() const {
                if(!isValid || isEnd) throw invalid_iterator();
                return *toNode->value;
            }
            bool operator==(const iterator &rhs) const {return (toMap == rhs.toMap) && (toNode == rhs.toNode) && (isEnd == rhs.isEnd);}
            bool operator==(const const_iterator &rhs) const {return (toMap == rhs.toMap) && (toNode == rhs.toNode) && (isEnd == rhs.isEnd);}
            bool operator!=(const iterator &rhs) const {return !((toMap == rhs.toMap) && (toNode == rhs.toNode) && (isEnd == rhs.isEnd));}
            bool operator!=(const const_iterator &rhs) const {return !((toMap == rhs.toMap) && (toNode == rhs.toNode) && (isEnd == rhs.isEnd));}
            const value_type* operator->() const noexcept {
                if(!isValid || isEnd) throw invalid_iterator();
                return toNode->value;
            }
        };
        map() {}

        map(const map &other):theTree(other.theTree) {}

        map & operator=(const map &other) {
            this->theTree = other.theTree;
            return *this;
        }

        ~map() {}

        T & at(const Key &key) {
            Nodeptr tmp = theTree.search(theTree.getRoot(),key);
            if(!tmp) throw index_out_of_bound();
            return tmp->value->second;
        }

        const T & at(const Key &key) const {
            Nodeptr tmp = theTree.search(theTree.getRoot(),key);
            if(!tmp) throw index_out_of_bound();
            return tmp->value->second;
        }

        T & operator[](const Key &key) {
            Nodeptr tmp = theTree.search(theTree.getRoot(),key);
            if(tmp) return tmp->value->second;
            else{
                theTree.insert(pair<Key,T>(key,T()));
                return theTree.search(theTree.getRoot(),key)->value->second;
            }
        }

        const T & operator[](const Key &key) const {
            Nodeptr tmp = theTree.search(theTree.getRoot(),key);
            if(!tmp) throw index_out_of_bound();
            return tmp->value->second;
        }

        iterator begin() {
            if(size() == 0) return iterator(this, nullptr,true,true);
            return iterator(this,theTree.getMin(theTree.getRoot()),true,false);
        }

        const_iterator cbegin() const {
            if(size() == 0) return const_iterator(this, nullptr,true,true);
            return const_iterator(this,theTree.getMin(theTree.getRoot()),true,false);
        }

        iterator end() {return iterator(this, nullptr,true,true);}

        const_iterator cend() const {return const_iterator(this, nullptr,true,true);}

        bool empty() const {return theTree.size() == 0;}

        size_t size() const {return theTree.size();}

        void clear() {theTree.clear();}

        pair<iterator, bool> insert(const value_type &value) {
            Nodeptr tmpPtr = theTree.search(theTree.getRoot(),value.first);
            if(tmpPtr) return pair<iterator,bool>(iterator(this,tmpPtr,true,false),false);
            else{
                theTree.insert(value);
                tmpPtr = theTree.search(theTree.getRoot(),value.first);
                return pair<iterator,bool>(iterator(this,tmpPtr,true,false),true);
            }
        }

        void erase(iterator pos) {
            if(!pos.isValid || pos.isEnd) throw invalid_iterator();
            theTree.erase(pos.toNode->value->first,pos.toNode);
        }

        size_t count(const Key &key) const {
            if(theTree.search(theTree.getRoot(),key) == nullptr) return 0;
            else return 1;
        }

        iterator find(const Key &key) {
            Nodeptr tmpPtr = theTree.search(theTree.getRoot(),key);
            if(!tmpPtr) return end();
            else return iterator(this,tmpPtr,true, false);
        }

        const_iterator find(const Key &key) const {
            Nodeptr tmpPtr = theTree.search(theTree.getRoot(),key);
            if(!tmpPtr) return cend();
            else return const_iterator(this,tmpPtr,true, false);
        }
    };

}

#endif
