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

namespace myContainer {
    template<class T>
    class AVLtree {
    private:
        struct Node {
            T value;
            int height = 0;
            Node *leftson;
            Node *rightson;
            Node *parent;

            Node() : leftson(nullptr), rightson(nullptr), parent(nullptr), height(0) {}

            Node(const T &_value, Node *_parent) : value(_value), parent(_parent), leftson(nullptr),
                                                   rightson(nullptr) {}

            Node(const Node &o) = default;

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

        Node *getMax(Node *_root) { return _root->rightson == nullptr ? _root : getMax(_root->rightson);}

        Node *getMin(Node *_root) {return _root->leftson == nullptr ? _root : getMin(_root->leftson);}

        Node *getSuccessor(Node *_root) {
            if (_root == getMax(this->root)) return nullptr;
            if (_root->rightson != nullptr) return getMin(_root->rightson);
            else {
                Node *p = _root->parent;
                Node *q = _root;
                while (p != nullptr && p->rightson == q) {
                    q = p;
                    p = p->parent;
                }
                return p;
            }
        }

        Node *getPredecessor(Node *_root) {
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
            Node *newNode = new Node(duplicate->value, _parent);
            newNode->leftson = copy(duplicate->leftson, newNode);
            newNode->rightson = copy(duplicate->rightson, newNode);
            return newNode;
        }

        Node *search(Node *_root, const T &_value) {
            if (_root == nullptr) return nullptr;
            if (_root->value == _value) return _root;
            if (_root->value > _value) return search(_root->leftson, _value);
            if (_root->value < _value) return search(_root->rightson, _value);
        }

        void assistant_insert(Node *_root, const T &_value) {
            if (_root->value < _value) {
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
//                int tmpheight = _leaf->height;
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
            if (search(this->root, _value) != nullptr) return false;
            if (!nodeNum) this->root = new Node(_value, nullptr);
            else {
                assistant_insert(this->root, _value);
            }
            nodeNum++;
            return true;
        }

        bool erase(const T &_value, Node *_target = nullptr) {
            Node *target = (_target == nullptr) ? search(this->root, _value) : _target;
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
            //case3:target owns two sons
            if (target->rightson && target->leftson) {
                Node *_successor = getSuccessor(target);
                T tmpValue(_successor->value);
                erase(tmpValue, _successor);
                target->value = tmpValue;
                return true;
            }
        }

        void clear(){
            clear(this->root);
            this->nodeNum = 0;
        }
        // for test
        void print() {
            Node *tmp = getMin(this->root);
            while (tmp != nullptr) {
                std::cout << "value : " << tmp->value << " height : " << tmp->height << std::endl;
                tmp = getSuccessor(tmp);
            }
        }
        Node * getRoot(){return root;}
        void inorderTravel(Node * _root){
            if(_root == nullptr) return;
            inorderTravel(_root->leftson);
            std::cout << "value : " << _root->value << " height : " << _root->height << std::endl;
            inorderTravel(_root->rightson);
        }
        void preorderTravel(Node * _root){
            if(_root == nullptr) return;
            std::cout << "value : " << _root->value << " height : " << _root->height << std::endl;
            preorderTravel(_root->leftson);
            preorderTravel(_root->rightson);
        }
    };
}

namespace sjtu {

template<
	class Key,
	class T,
	class Compare = std::less<Key>
> class map {
public:
	/**
	 * the internal type of data.
	 * it should have a default constructor, a copy constructor.
	 * You can use sjtu::map as value_type by typedef.
	 */
	typedef pair<const Key, T> value_type;
	/**
	 * see BidirectionalIterator at CppReference for help.
	 *
	 * if there is anything wrong throw invalid_iterator.
	 *     like it = map.begin(); --it;
	 *       or it = map.end(); ++end();
	 */
	class const_iterator;
	class iterator {
	private:
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */
	public:
		iterator() {
			// TODO
		}
		iterator(const iterator &other) {
			// TODO
		}
		/**
		 * TODO iter++
		 */
		iterator operator++(int) {}
		/**
		 * TODO ++iter
		 */
		iterator & operator++() {}
		/**
		 * TODO iter--
		 */
		iterator operator--(int) {}
		/**
		 * TODO --iter
		 */
		iterator & operator--() {}
		/**
		 * an operator to check whether two iterators are same (pointing to the same memory).
		 */
		value_type & operator*() const {}
		bool operator==(const iterator &rhs) const {}
		bool operator==(const const_iterator &rhs) const {}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {}
		bool operator!=(const const_iterator &rhs) const {}

		/**
		 * for the support of it->first. 
		 * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
		 */
		value_type* operator->() const noexcept {}
	};
	class const_iterator {
		// it should has similar member method as iterator.
		//  and it should be able to construct from an iterator.
		private:
			// data members.
		public:
			const_iterator() {
				// TODO
			}
			const_iterator(const const_iterator &other) {
				// TODO
			}
			const_iterator(const iterator &other) {
				// TODO
			}
			// And other methods in iterator.
			// And other methods in iterator.
			// And other methods in iterator.
	};
	/**
	 * TODO two constructors
	 */
	map() {}
	map(const map &other) {}
	/**
	 * TODO assignment operator
	 */
	map & operator=(const map &other) {}
	/**
	 * TODO Destructors
	 */
	~map() {}
	/**
	 * TODO
	 * access specified element with bounds checking
	 * Returns a reference to the mapped value of the element with key equivalent to key.
	 * If no such element exists, an exception of type `index_out_of_bound'
	 */
	T & at(const Key &key) {}
	const T & at(const Key &key) const {}
	/**
	 * TODO
	 * access specified element 
	 * Returns a reference to the value that is mapped to a key equivalent to key,
	 *   performing an insertion if such key does not already exist.
	 */
	T & operator[](const Key &key) {}
	/**
	 * behave like at() throw index_out_of_bound if such key does not exist.
	 */
	const T & operator[](const Key &key) const {}
	/**
	 * return a iterator to the beginning
	 */
	iterator begin() {}
	const_iterator cbegin() const {}
	/**
	 * return a iterator to the end
	 * in fact, it returns past-the-end.
	 */
	iterator end() {}
	const_iterator cend() const {}
	/**
	 * checks whether the container is empty
	 * return true if empty, otherwise false.
	 */
	bool empty() const {}
	/**
	 * returns the number of elements.
	 */
	size_t size() const {}
	/**
	 * clears the contents
	 */
	void clear() {}
	/**
	 * insert an element.
	 * return a pair, the first of the pair is
	 *   the iterator to the new element (or the element that prevented the insertion), 
	 *   the second one is true if insert successfully, or false.
	 */
	pair<iterator, bool> insert(const value_type &value) {}
	/**
	 * erase the element at pos.
	 *
	 * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
	 */
	void erase(iterator pos) {}
	/**
	 * Returns the number of elements with key 
	 *   that compares equivalent to the specified argument,
	 *   which is either 1 or 0 
	 *     since this container does not allow duplicates.
	 * The default method of check the equivalence is !(a < b || b > a)
	 */
	size_t count(const Key &key) const {}
	/**
	 * Finds an element with key equivalent to key.
	 * key value of the element to search for.
	 * Iterator to an element with key equivalent to key.
	 *   If no such element is found, past-the-end (see end()) iterator is returned.
	 */
	iterator find(const Key &key) {}
	const_iterator find(const Key &key) const {}
};

}

#endif
