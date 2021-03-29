//
// Created by 赵先生 on 2021/3/12.
//

#ifndef MAP_AVLTREE_HPP
#define MAP_AVLTREE_HPP
#include "utility.hpp"
namespace myContainer {
    template<class keyType,class dataType,class Compare = std::less<keyType>>
    class AVLtree {
    public: // because of the friend's question, I use the public
//        friend class sjtu:
        typedef sjtu::pair<const keyType,dataType> T;
        Compare function;
        struct Node {
            T * value; // pair<const Key, T>
            int height = 0;
            Node *leftson;
            Node *rightson;
            Node *parent;

            Node() :value(nullptr),leftson(nullptr), rightson(nullptr), parent(nullptr), height(0) {}

            Node(const T &_value, Node *_parent):parent(_parent), leftson(nullptr),rightson(nullptr),height(0){
                value = new T(_value);
            }
            Node(T * _value,Node * _parent,int _height):parent(_parent),leftson(nullptr),rightson(nullptr),height(_height){
                if(_value == nullptr) value = nullptr;
                else value = new T(*_value);
            }
            Node(const Node &o):height(o.height),leftson(o.leftson),rightson(o.rightson),parent(o.parent){
                value = new T(*o.value);
            }
            Node & operator = (const Node & o){
                if(this == &o) return *this;
                if(value) delete value;
                height = o.height;leftson = o.leftson;rightson = o.rightson;parent = o.parent;
                if(o.value == nullptr){value = nullptr;return *this;}
                value = new T(*o.value);
                return *this;
            }
            ~Node(){if(value) delete value;}
            inline int max(int x, int y) { return x > y ? x : y; }

            void setHeight() {
                int lh = this->leftson == nullptr ? -1 : this->leftson->height;
                int rh = this->rightson == nullptr ? -1 : this->rightson->height;
                this->height = max(lh, rh) + 1;
            }

            int getBalanceSize(){
                int lh = this->leftson == nullptr ? -1 : this->leftson->height;
                int rh = this->rightson == nullptr ? -1 : this->rightson->height;
                return lh - rh;
            }

            void ll_rotation(){ // can't manage root proble
                Node * _parent = this->parent;                //  check for nullptr
                Node * _newroot_rs = this->leftson->rightson;      //  check for nullptr
                Node * _newroot = this->leftson;
                if(_parent){
                    if (_parent->rightson == this) _parent->rightson = _newroot;
                    else _parent->leftson = _newroot;
                }
                _newroot->parent = _parent;
                _newroot->rightson = this;
                this->parent = _newroot;
                this->leftson = _newroot_rs;
                if(_newroot_rs) _newroot_rs->parent = this;
                this->setHeight(); _newroot->setHeight();
            }

            void rr_rotation(){
                Node * _parent = this->parent;                //  check for nullptr
                Node * _newroot_ls = this->rightson->leftson;      //  check for nullptr
                Node * _newroot = this->rightson;
                if(_parent){
                    if (_parent->rightson == this) _parent->rightson = _newroot;
                    else _parent->leftson = _newroot;
                }
                _newroot->parent = _parent;
                _newroot->leftson = this;
                this->parent = _newroot;
                this->rightson = _newroot_ls;
                if(_newroot_ls) _newroot_ls->parent = this;
                this->setHeight(); _newroot->setHeight();
            }

            void lr_rotation(){
                this->leftson->rr_rotation();
                this->ll_rotation();
            }

            void rl_rotation(){
                this->rightson->ll_rotation();
                this->rr_rotation();
            }
        };

        Node *root;
        int nodeNum;

        Node *getMax(Node *_root) const{ return _root->rightson == nullptr ? _root : getMax(_root->rightson);}

        Node *getMin(Node *_root) const{return _root->leftson == nullptr ? _root : getMin(_root->leftson);}

        Node *getSuccessor(Node *_root) const{
            if (_root == getMax(this->root)) return nullptr;
            if (_root->rightson != nullptr) return getMin(_root->rightson);
            else {
                Node *p = _root->parent;
                Node *q = _root;
                while (p != nullptr && p->rightson == q) {
                    q = p;p = p->parent;
                }
                return p;
            }
        }

        Node *getPredecessor(Node *_root) const{
            if (_root == getMin(this->root)) return nullptr;
            if (_root->leftson != nullptr) return getMax(_root->leftson);
            else {
                Node *p = _root->parent, *q = _root;
                while (p != nullptr && p->leftson == q) {
                    q = p, p = p->parent;
                }
                return p;
            }
        }

        void clear(Node *_root) {
            if (_root == nullptr) return;
            if (_root->leftson != nullptr) clear(_root->leftson);
            if (_root->rightson != nullptr) clear(_root->rightson);
            delete _root;
        }

        // usage: root = copy(root,o.root);
        Node *copy(Node *duplicate, Node *_parent = nullptr) {
            if (duplicate == nullptr) return duplicate;
            Node *newNode = new Node(duplicate->value, _parent,duplicate->height);
            newNode->leftson = copy(duplicate->leftson, newNode);
            newNode->rightson = copy(duplicate->rightson, newNode);
            return newNode;
        }

        Node *search(Node *_root, const keyType &_key) const{
            if (_root == nullptr) return nullptr;
            if (!(function(_root->value->first , _key) || function(_key,_root->value->first))) return _root; // maybe trouble
            if (function(_key , _root->value->first)) return search(_root->leftson, _key);
            if (function(_root->value->first , _key)) return search(_root->rightson, _key);
//            return _root;
        }

        void assistant_insert(Node *_root, const T &_value) {
            if (function(_root->value->first , _value.first)) {
                if (_root->rightson == nullptr) _root->rightson = new Node(_value, _root);
                else assistant_insert(_root->rightson, _value);
            } else {
                if (_root->leftson == nullptr) _root->leftson = new Node(_value, _root);
                else assistant_insert(_root->leftson, _value);
            }
            _root->setHeight();
            this->balance(_root);
        }

        void assistant_height(Node *_leaf) {
            while (_leaf != nullptr) {
                _leaf->setHeight();
                Node * tmp = _leaf;
                _leaf = _leaf->parent;
                this->balance(tmp);
            }
        }

        void balance(Node * _root){
            bool isRoot = (_root == this->root);
            if(_root->getBalanceSize() == 2){
                if(_root->leftson->getBalanceSize() >= 0){ // 为什么删除时左右相等要单旋转？
                    if(isRoot) this->root = _root->leftson;
                    _root->ll_rotation();
                }else{
                    if (isRoot) this->root = _root->leftson->rightson;
                    _root->lr_rotation();
                }
            }else if(_root->getBalanceSize() == -2){
                if(_root->rightson->getBalanceSize() <= 0){
                    if(isRoot) this->root = _root->rightson;
                    _root->rr_rotation();
                }
                else{
                    if(isRoot) this->root = _root->rightson->leftson;
                    _root->rl_rotation();
                }
            }
        }
    public:
        AVLtree() : root(nullptr), nodeNum(0) {}

        AVLtree(const AVLtree &o) {
            this->nodeNum = o.nodeNum;
            this->root = copy(o.root);
        }

        ~AVLtree() { clear(this->root); }

        AVLtree &operator=(const AVLtree &o) {
            if (this == &o) return *this;
            this->nodeNum = o.nodeNum;
            clear(this->root);
            this->root = copy(o.root);
            return *this;
        }

        bool insert(const T &_value) {
            if (search(this->root, _value.first) != nullptr) return false;
            if (!nodeNum) this->root = new Node(_value, nullptr);
            else {
                assistant_insert(this->root, _value);
            }
            nodeNum++;
            return true;
        }

        bool erase(const keyType &_key, Node *_target = nullptr) {
            Node *target = (_target == nullptr) ? search(this->root, _key) : _target;
            if (target == nullptr) return false;
            Node *_parent = target->parent;
            bool isRoot = (target == this->root);
            //case1:target don't own either ls nor rs;
            if (!target->leftson && !target->rightson) {
                if (_parent) {
                    if (_parent->rightson == target) _parent->rightson = nullptr;
                    else _parent->leftson = nullptr;
                }
                delete target;
                nodeNum--;
                if (isRoot) this->root = nullptr;
                assistant_height(_parent);
                return true;
            }
            //case2:target only owns one child node
            if (!target->leftson) {
                if (_parent) {
                    if (_parent->rightson == target) _parent->rightson = target->rightson;
                    else _parent->leftson = target->rightson;
                }
                target->rightson->parent = _parent;
                if (isRoot) this->root = target->rightson;
                assistant_height(_parent);
                delete target;
                nodeNum--;
                return true;
            }
            if (!target->rightson) {
                if (_parent) {
                    if (_parent->rightson == target) _parent->rightson = target->leftson;
                    else _parent->leftson = target->leftson;
                }
                target->leftson->parent = _parent;
                if (isRoot) this->root = target->leftson;
                assistant_height(_parent);
                delete target;
                nodeNum--;
                return true;
            }
            //case3:target owns two sons;has some problem;
            if (target->rightson && target->leftson) {
                Node *_successor = getSuccessor(target);
                nodeNum--;
                if(isRoot) this->root = _successor;
                if(target->rightson != _successor) {
                    if (_parent) {
                        if (_parent->rightson == target) _parent->rightson = _successor;
                        else _parent->leftson = _successor;
                    }
                    Node * _successor_parent = _successor->parent;
                    target->leftson->parent = _successor;
                    target->rightson->parent = _successor;
                    if (_successor->rightson) {
                        _successor->rightson->parent = _successor->parent;
                    }
                    _successor->parent->leftson = _successor->rightson;
                    _successor->parent = _parent;
                    _successor->leftson = target->leftson;
                    _successor->rightson = target->rightson;
                    delete target;
                    assistant_height(_successor_parent);
                    return true;
                }else{
                    if(_parent){
                        if (_parent->rightson == target) _parent->rightson = _successor;
                        else _parent->leftson = _successor;
                    }
                    target->leftson->parent = _successor;
                    _successor->leftson = target->leftson;
                    _successor->parent = _parent;
                    delete target;
                    assistant_height(_successor);
                    return true;
                }
//                     wrong writing:
//                Node *_successor = getSuccessor(target);
//                T tmpValue(*_successor->value);
//                erase(tmpValue.first, _successor);
//                delete target->value;
//                target->value = new T(tmpValue);
//                return true;
            }
        }

        void clear(){
            clear(this->root);
            root = nullptr;
            this->nodeNum = 0;
        }

        size_t size() const {return this->nodeNum;}

        Node * getRoot()const {return root;}
//         for test
//        void print() {
//            Node *tmp = getMin(this->root);
//            while (tmp != nullptr) {
//                std::cout << "key : " << tmp->value.first << "data : " << tmp->value.second << " height : " << tmp->height << std::endl;
//                tmp = getSuccessor(tmp);
//            }
//            inorderTravel(root);
//        }
//        void inorderTravel(Node * _root){
//            if(_root == nullptr) return;
//            inorderTravel(_root->leftson);
//            std::cout << "key : " << _root->value->first << " data : " << _root->value->second << " height : " << _root->height << std::endl;
//            inorderTravel(_root->rightson);
//        }
//        void preorderTravel(Node * _root){
//            if(_root == nullptr) return;
//            std::cout << "key : " << _root->value->first << " data : " << _root->value->second << " height : " << _root->height << std::endl;
//            preorderTravel(_root->leftson);
//            preorderTravel(_root->rightson);
//        }
    };
}
#endif //MAP_AVLTREE_HPP
