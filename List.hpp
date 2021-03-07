//
// Created by 赵先生 on 2021/2/24.
//

#ifndef LIST_LIST_HPP
#define LIST_LIST_HPP
#include <iostream>

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
            elemNum = 0;
            head = nullptr;tail = nullptr;
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

        Node & operator[](int pos) const {
            if(pos < 0 || pos >= elemNum) throw "index_out_of_bound";
            Node * temp1 = head;
            if(pos == 0) return *head;
            if(pos == elemNum - 1) return *tail;
            if(pos < (elemNum / 2)){
                while(pos--){
                    temp1 = temp1->next;
                }
                return *temp1;
            }else{
                pos = elemNum - 1 -pos;
                temp1 = tail;
                while (pos--){
                    temp1 = temp1->before;
                }
                return *temp1;
            }
        }

        Node & back(){
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

        int size(){
            return elemNum;
        }
    };
}

#endif //LIST_LIST_HPP
