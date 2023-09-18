#ifndef ___SKIP_LIST_HPP
#define ___SKIP_LIST_HPP

#include <cmath> // for log2
#include <string>
#include <vector>
#include "runtimeexcept.hpp"
#include <iostream>

/**
 * flipCoin -- NOTE: Only read if you are interested in how the
 * coin flipping works.
 * 
 * @brief Uses the bitwise representation of the key to simulate
 * the flipping of a deterministic coin.
 * 
 * This function looks at the bitwise representation to determine
 * how many layers it occupies in the skip list. It takes the bitwise
 * XOR of each byte in a 32-bit unsigned number and uses the index `previousFlips`
 * to determine the truth value. It's best to look at an example and
 * to start with values that fit into one byte.
 * 
 * Let's start with the function call `flipCoin(0, 0)`. This call is
 * asking whether or not we should insert the key `0` into layer `1`
 * (because `previousFlips` represents the number of previous flips).
 * 
 * This function will get the bitwise representation of 0: 
 * 
 *        00000000 00000000 00000000 00000000
 * 
 * All bytes are XOR'd together: 
 * 
 * 
 *  c = 0 ^ 0 ^ 0 ^ 0
 * 
 * Then the index `previousFlips` is used to obtain the bit in the `previousFlips % 8` position.
 * 
 * So the return value is `0 & 1 = 0`, since the value in the zeroth position 
 * (obtained by bitmasking the value (1 << 0) at the bottom of the function
 * is 0.
 * Thus, this value `0` should never get added beyond the bottom-most layer.
 * 
 * Before:
 * 
 * S_1: -inf ----> inf
 * S_0: -inf ----> inf
 * 
 * And after 0 is inserted
 * 
 * S_1: -inf --------> inf
 * S_0: -inf --> 0 --> inf
 * 
 * Let's look at something more interesting, like the call `flipCoin(5, 0)`.
 * Remember the binary representation for 5 is 00000101.
 * 
 * c = 0 ^ 0 ^ 0 ^ 00000101 = 00000101
 * 
 * Now we get the bit at 0th position (from `previousFlips`).
 * 
 * 00000101 & 1 = 1. Thus, `5` DOES get inserted into the next layer,
 * layer 1.
 * 
 * So the skip list before this insertion might look like:
 * 
 * S_1: -inf ----> inf
 * S_0: -inf ----> inf
 * 
 * And after it would look like:
 * 
 * S_2: -inf --------> inf
 * S_1: -inf --> 5 --> inf
 * S_0: -inf --> 5 --> inf
 * 
 * Observe that a new layer had to get created, because there should always
 * be an empty layer at the top.
 * 
 * Since 5 got inserted into the next layer, we need to flip again to see if
 * it should get propagated once more.
 * 
 * `flipCoin(5, 1)` this will produce a result of `0`, since 00000101 & (1 << 1) = 0.
 * 
 * Thus, `5` will not be pushed up to the next layer.
 * 
 * @param key The integer key which will be added to the skip list
 * @param previousFlips The number of previous flips for this key
 * @return true simulates a "heads" from a coin flip
 * @return false simulates a "tails" from a coin flip
 */
inline bool flipCoin(unsigned key, unsigned previousFlips)
{
    char c;
    unsigned first8Bits = (key & 0xFF000000) / 0x01000000 ;
    unsigned next8Bits =  (key & 0x00FF0000) / 0x00010000;
    unsigned andThen =	 (key & 0x0000FF00) / 0x00000100;
    unsigned lastBits =   (key & 0x000000FF);
    c = first8Bits ^ next8Bits ^ andThen ^ lastBits;
    previousFlips = previousFlips % 8;
    return ( c & (1 << previousFlips) ) != 0;
}

/**
 * @brief Works the same as the integer version above, except
 * it XORs chars in a string instead of the first four
 * bytes of an integer.
 * 
 * @param key key that will be inserted into the skip list
 * @param previousFlips number of previous flips for this key 
 * @return true simulates a "heads" from a coin flip
 * @return false simulates a "tails" from a coin flip
 */
inline bool flipCoin(std::string key, unsigned previousFlips)
{
    char c = key[0];
    for(unsigned j = 1; j < key.length(); j++)
    {
        c = c ^ key[j];
    }
    previousFlips = previousFlips % 8;
    return ( c & (1 << previousFlips) ) != 0;
}


template<typename Key, typename Value>
class Node
{
public:
    // Node Data
    Key key;
    Value value;
    Node<Key, Value>* next;
    Node<Key, Value>* left;
    Node<Key, Value>* up;
    Node<Key, Value>* down;
    bool isInfinity;

    // Node Constructor
    Node() {
        this->key = 0;
        this->value = 0;
        this->next = nullptr;
        this->left = nullptr;
        this->up = nullptr;
        this->down = nullptr;
        isInfinity = false;
    };
    Node(const Key& key, const Value& value) {
        this->key = key;
        this->value = value;
        this->next = nullptr;
        this->left = nullptr;
        this->up = nullptr;
        this->down = nullptr;
        isInfinity = false;
    };
    Node(bool infinity_bool) {
        this->next = nullptr;
        this->left = nullptr;
        this->up = nullptr;
        this->down = nullptr;
        isInfinity = infinity_bool;
    };
};



template<typename Key, typename Value>
class SkipList
{


private:
    // private variables go here.
    int s_len;
    unsigned layers;

    Node<Key, Value> *top_lvl;
    Node<Key, Value> *insert_lvl;
    Node<Key, Value> *insert_end;
    Node<Key, Value> *top_end;


public:

    SkipList();

    // You DO NOT need to implement a copy constructor or an assignment operator.

    ~SkipList();

    // How many distinct keys are in the skip list?
    size_t size() const noexcept;

    // Does the Skip List contain zero keys?
    bool isEmpty() const noexcept;

    // How many layers are in the skip list?
    // Note that an empty Skip List has two layers by default,
    // the "base" layer S_0 and the top layer S_1.
    //
    // [S_1: Top layer]    -inf ------> inf
    // [S_0: Bottom layer] -inf ------> inf
    //
    // This "empty" Skip List has two layers and a height of one.
    unsigned numLayers() const noexcept;

    // What is the height of this key, assuming the "base" layer S_0
    // contains keys with a height of 1?
    // For example, "0" has a height of 1 in the following skip list.
    //
    // [S_1]  -inf --------> inf
    // [S_0]  -inf --> 0 --> inf
    //
    // Throw an exception if this key is not in the Skip List.
    unsigned height(const Key & k) const;

    // If this key is in the SkipList and there is a next largest key
    // return the next largest key.
    // This function should throw a RuntimeException if either the key doesn't exist
    // or there is no subsequent key.
    // A consequence of this is that this function will
    // throw a RuntimeException if *k* is the *largest* key in the Skip List.
    Key nextKey(const Key & k) const;

    // If this key is in the SkipList and a next smallest key exists,
    // return the next smallest key.
    // This function should throw a RuntimeException if either the key doesn't exist
    // or there is no previous key.
    // A consequence of this is that this function will
    // throw a RuntimeException if *k* is the *smallest* key in the Skip List.
    Key previousKey(const Key & k) const;

    // These return the value associated with the given key.
    // Throw a RuntimeException if the key does not exist.
    Value & find(const Key & k);
    const Value & find(Key k) const;

    // Return true if this key/value pair is successfully inserted, false otherwise.
    // See the project write-up for conditions under which the key should be "bubbled up"
    // to the next layer.
    // If the key already exists, do not insert one -- return false.
    bool insert(const Key & k, const Value & v);

    // Return a vector containing all inserted keys in increasing order.
    std::vector<Key> allKeysInOrder() const;

    // Is this the smallest key in the SkipList? Throw a RuntimeException
    // if the key *k* does not exist in the Skip List.
    bool isSmallestKey(const Key & k) const;

    // Is this the largest key in the SkipList? Throw a RuntimeException
    // if the key *k* does not exist in the Skip List.
    bool isLargestKey(const Key & k) const;

    // I am not requiring you to implement remove.


};

template<typename Key, typename Value>
SkipList<Key, Value>::SkipList() {
    // TODO - your implementation goes here!
    s_len = 0;
    layers = 2;
    top_lvl = new Node<Key,Value>(true);
    insert_lvl = new Node<Key,Value>(true);

    top_end = new Node<Key,Value>(true);
    top_lvl->next = top_end;
    top_end->left = top_lvl;

    insert_end = new Node<Key,Value>(true);
    insert_lvl->next = insert_end;
    insert_end->left = insert_lvl;

    top_lvl->down = insert_lvl;
    insert_lvl->up = top_lvl;

    top_end->down = insert_end;
    insert_end->up = top_end;
}


template<typename Key, typename Value>
SkipList<Key, Value>::~SkipList() {
    // TODO - your implementation goes here!

    Node<Key, Value> *copy = top_lvl;
    Node<Key, Value> *temp;
    Node<Key, Value> *deleteNode;
    while (copy != nullptr) {
        deleteNode = copy;
        copy = deleteNode->down;
        while(deleteNode != nullptr) {
            temp = deleteNode->next;
            delete deleteNode;
            deleteNode = temp;
        }
    }
}




template<typename Key, typename Value>
size_t SkipList<Key, Value>::size() const noexcept {
    // TODO - your implementation goes here!
    return s_len;
}

template<typename Key, typename Value>
bool SkipList<Key, Value>::isEmpty() const noexcept {
    // TODO - your implementation goes here!
    if (insert_lvl->next->isInfinity == true) {
        return true;
    }
    return false;
}

template<typename Key, typename Value>
unsigned SkipList<Key, Value>::numLayers() const noexcept {
    // TODO - your implementation goes here!
    return layers;
}

template<typename Key, typename Value>
unsigned SkipList<Key, Value>::height(const Key & k) const {
    // TODO - your implementation goes here!
    Node<Key,Value> *copy = top_lvl->down; // down one
    while (copy != nullptr){
        if (copy->next->isInfinity != true && copy->next->key <= k){
            copy = copy->next;
        } else if (copy->key == k && copy->isInfinity != true){
            unsigned h = 0;
            while(copy != nullptr){
                h++;
                copy = copy->down;
            }
            return h;
        } else{
            copy = copy->down;
        }
    }
    throw RuntimeException("Key does not exist");
}

template<typename Key, typename Value>
Key SkipList<Key, Value>::nextKey(const Key & k) const {
    Node<Key,Value> *copy = top_lvl->down;
    while (copy->down != nullptr){
        copy = copy->down;
        while (copy->next->isInfinity != true && copy->next->key <= k){
            copy = copy->next;
        }
    }
    if (copy->key == k && copy->next->isInfinity != true){
        return copy->next->key; //right
    
    }

    throw RuntimeException("Key does not exist");
}

template<typename Key, typename Value>
Key SkipList<Key, Value>::previousKey(const Key & k) const {
    Node<Key,Value> *copy = top_lvl->down;
    while (copy->down != nullptr){
        copy = copy->down;
        while (copy->next->isInfinity != true && copy->next->key <= k){
            copy = copy->next; //left
        }
    }
    if (copy->key == k && copy->left->isInfinity != true){
        return copy->left->key;
        
    }
    throw RuntimeException("Key does not exist");
}

template<typename Key, typename Value>
const Value & SkipList<Key, Value>::find(Key k) const {
    Node<Key, Value> *copy = top_lvl->down;

    while (copy->down != nullptr){
        copy = copy->down;

        while (copy->next->isInfinity != true && copy->next->key <= k){
            copy = copy->next;
        }
    }
    if (copy->key == k){
        return copy->value;
    }
    throw RuntimeException("Key does not exist");
}

template<typename Key, typename Value>
Value & SkipList<Key, Value>::find(const Key & k) {
    Node<Key, Value> *copy = top_lvl->down;
    while (copy->down != nullptr){
        copy = copy->down;
        while (copy->next->isInfinity != true && copy->next->key <= k){
            copy = copy->next;
        }
    }
    if (copy->key == k){
        return copy->value;
    }
    throw RuntimeException("Key does not exist");
}


template<typename Key, typename Value>
bool SkipList<Key, Value>::insert(const Key & k, const Value & v) {
    s_len += 1; // add for new node
    Node<Key,Value> *copy = top_lvl->down;

    std::vector<Key> allElements = this->allKeysInOrder();
    if (std::find(allElements.begin(), allElements.end(), k) != allElements.end()){
        s_len--;
        return false; // if its not theree
    }

    int maxLayers;
    if (s_len <= 16){
        maxLayers = 12;
    } else {
        maxLayers = 3 * ceil(log2(s_len));
    }
    //std::cout << maxLayers << "  " << s_len << std::endl;

    while (copy != nullptr && copy->next->isInfinity == false) {
        if (copy->next->key < k) {
            copy = copy->next;
        } else{
            copy = copy->down;
        }

    
    }
    while (copy->down != nullptr) {
        copy = copy->down;
    }
    while (copy->next->isInfinity != true && copy->next->key < k) {
        copy = copy->next;
    }

    Node<Key, Value> *next_node = copy->next;

    copy->next = new Node<Key, Value>(k, v);
    copy->next->next = next_node;
    copy->next->left = copy;
    next_node->left = copy->next;
    copy = copy->next;

    int current_lvl = 1;
    Node<Key, Value> *prevNode = copy->left;
    
    unsigned prevFlips = 0;
    while(flipCoin(k,prevFlips) && maxLayers > current_lvl){
        prevFlips++;

        while (prevNode->up == nullptr && prevNode->left != nullptr){
            prevNode = prevNode->left;
        }
        prevNode = prevNode->up;
        copy->up = new Node<Key, Value> (k,v);
        copy->up->next = prevNode->next;
        copy->up->down = copy;
        copy->up->left = prevNode;
        prevNode->next = copy->up;
        copy->up->next->left = copy->up;
        copy = copy->up;
        current_lvl++;

        if (current_lvl == layers){
            Node<Key, Value> * temp_top_lvl= new Node<Key, Value>(true);
            top_lvl->up = temp_top_lvl;
            top_lvl->up->down = top_lvl;
            Node<Key, Value> * temp_top_end= new Node<Key, Value>(true);
            top_end->up =temp_top_end;
            top_end->down = top_end;
            top_end->left = top_lvl->up;
            top_end = top_end->up; //link ends

            top_lvl = top_lvl->up;
            top_lvl->next = top_end;

            layers ++;
        }
    }
    return true;
}

template<typename Key, typename Value>
std::vector<Key> SkipList<Key, Value>::allKeysInOrder() const {
    // TODO - your implementation goes here!
    std::vector<Key> res;
    Node<Key, Value> *copy = insert_lvl->next;
    while (copy->isInfinity != true) {
        res.push_back(copy->key);
        copy = copy->next;
    }
    return res;
}

template<typename Key, typename Value>
bool SkipList<Key, Value>::isSmallestKey(const Key & k) const {
    // TODO - your implementation goes here!
    if (insert_lvl->next->isInfinity == true) {
        throw RuntimeException("Key does not exist");
    } else if (insert_lvl->next->key != k) {
        find(k);
        return false;
    }
    return true;
}

template<typename Key, typename Value>
bool SkipList<Key, Value>::isLargestKey(const Key & k) const {
    if (insert_end->left->isInfinity == true ) {
        throw RuntimeException("Key does not exist");
    } else if (insert_end->left->key != k) {
        find(k);
        return false;
    }
    return true;
}



#endif
