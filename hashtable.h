#ifndef HASHTABLE_H
#define HASHTABLE_H

// Project Identifier: 2C4A3C53CD5AD45A7BEA3AE5356A3B1622C9D04B

// INSTRUCTIONS:
// fill out the methods in the class below.

// You may assume that the key and value types can be copied and have default
// constructors.

// You can also assume that the key type has (==) and (!=) methods.

// You may assume that Hasher is a functor type with a
// size_t operator()(const Key&) overload.

// The key/value types aren't guaranteed to support any other operations.

// Do not add, remove, or change any of the class's member variables.
// The num_deleted counter is *optional*. You might find it helpful, but it
// is not required to pass the lab assignment.

// Do not change the Bucket type.

// SUBMISSION INSTRUCTIONS:
// Submit this file, by itself, in a .tar.gz.
// Other files will be ignored.

#include <cstdint>
#include <functional> // where std::hash lives
#include <vector>
#include <cassert> // useful for debugging!

// A bucket's status tells you whether it's empty, occupied, 
// or contains a deleted element.
enum class Status : uint8_t {
    Empty,
    Occupied,
    Deleted
};

template<typename K, typename V, typename Hasher = std::hash<K>>
class HashTable {
    // used by the autograder; do not change/remove.
    friend class Verifier;
public:
    // A bucket has a status, a key, and a value.
    struct Bucket {
        // Do not modify Bucket.
        Status status = Status::Empty;
        K key;
        V val;
    };

    HashTable() {
        buckets.resize(20);
    }

    size_t size() const {
        return num_elements;
    }

    // returns a reference to the value in the bucket with the key, if it
    // already exists. Otherwise, insert it with a default value, and return
    // a reference to the resulting bucket.
    V& operator[](const K& key) {

        V default_val;
        insert(key, default_val);

        return find(key)->val;
        
    }

    // insert returns whether inserted successfully
    // (if the key already exists in the table, do nothing and return false).
    bool insert(const K& key, const V& val) {
        auto loc = find(key);

        if (loc->status != Status::Empty) {
            return false; // fail to insert fully find even past deleted elts
        }

        // find insertion
        Hasher hasher;
        size_t desired_bucket = hasher(key) % buckets.size();
        size_t index = desired_bucket;

        while (buckets[index].status == Status::Occupied) {
            index = (index + 1) % buckets.size();
        }

        buckets[index] = {Status::Occupied, key, val};
        ++num_elements;

        if (num_elements * 2 > buckets.size()) {
            rehash_and_grow();
        }

        return true;
        
    }
    // erase returns the number of items remove (0 or 1)
    size_t erase(const K& key) {
        auto loc = find(key);

        if (loc->status == Status::Empty) {
            return 0;
        }

        loc->status = Status::Deleted;
        --num_elements;

        return 1;
    }

private:
    size_t num_elements = 0;
    size_t num_deleted = 0; // OPTIONAL: you don't need to use num_deleted to pass
    std::vector<Bucket> buckets;

    void swap(HashTable& other) noexcept {
        buckets.swap(other.buckets);
        std::swap(num_elements, other.num_elements);
    }

    void set_size(const int size) {
        buckets.resize(size);
    }

    void rehash_and_grow() {
        HashTable<K, V, Hasher> new_table;
        new_table.set_size(buckets.size() * 2);

        for (Bucket& b : buckets) {
            if (b.status == Status::Occupied) {
                new_table.insert(b.key, b.val);
            }
        }

        swap(new_table);
    }

    typename std::vector<Bucket>::iterator find(const K& key) {
        Hasher hasher;
        size_t desired_bucket = hasher(key) % buckets.size();
        size_t index = desired_bucket;
        Bucket& bucket = buckets[index];

        while (bucket.status != Status::Empty) {
            if (bucket.status != Status::Deleted && key == bucket.key) {
                return buckets.begin() + index;
            }

            index = (index + 1) % buckets.size();
            bucket = buckets[index]; 
        }

        return buckets.begin() + index;
    }
};

#endif // HASHTABLE_H