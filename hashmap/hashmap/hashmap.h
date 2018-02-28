////////////////////////////////////////////////////////
//
// implementation basic hashmap (unordered container)
// Mikhail Nesterenko: adapted from Proc C++
// 4/15/2014
//
////////////////////////////////
//
// Abdel-Hakeem Badran
// 02/27/2018
//
// Implemented:
// insert(), erase(), and rehash()
//
///////////////////

#include <cstddef>
#include <utility>
#include <functional>
#include <vector>
#include <list>

#include <algorithm>

#include <iostream>

using std::vector;
using std::list;
using std::pair;
using std::make_pair;

//////////////////////////////////////////
// hash function implemented as a class
//////////////////////////////////////////

// any Hash Class must provide 
// two methods: hash() and numBuckets().
template <typename T>
class DefaultHash {
public:
	DefaultHash(size_t numBuckets = defaultNumBuckets);
	size_t hash(const T& key) const;
	size_t numBuckets() const { return numBuckets_; }

private:
	static const size_t defaultNumBuckets = 101; // default number of buckets in the hash
	size_t numBuckets_;
};

template <typename T>
DefaultHash<T>::DefaultHash(size_t numBuckets) : numBuckets_(numBuckets) {}


// uses the division method for hashing.
// treats the key as a sequence of bytes, sums the ASCII
// values of the bytes, and mods the total by the number
// of buckets.
template <typename T>
size_t DefaultHash<T>::hash(const T& key) const {
	size_t res = 0;
	for (size_t i = 0; i < sizeof(key); ++i) {
		const unsigned char b = *(reinterpret_cast<const unsigned char *>(&key) + i);
		res += b;
	}
	return res % numBuckets_;
}


////////////////////////////////////////////////
// container class
////////////////////////////////////////////////

template < typename Key, typename Value, typename Compare = std::equal_to<Key>, typename Hash = DefaultHash<Key> >

class hashmap {
public:
	typedef pair<const Key, Value> Element;

	// constructor
	// invokes constructors for comparison and hash objects
	hashmap(const Compare& comp = Compare(),
		const Hash& hash = Hash());

	Element* find(const Key& x);      // returns pointer to element with key x,
									  // nullptr if not found
	pair<Value*, bool> insert(const Element& x);    // inserts the key/value pair 
	pair<Element*, bool> erase(const Key& x);         // erases element with key x, if exists
	Value& operator[] (const Key& x); // returns reference on value of
									  // element with key, inserts if does not exist
	void rehash(size_t);

private:
	// helper function for various searches
	typename list<Element>::iterator
		findElement(const Key& x,
			const size_t bucket);

	size_t size_;   // number of elements in the container
	Compare comp_;  // comparison functor, equal_to by default
	Hash hash_;     // hash functor 

					// hash contents: vector of buckets
					// each bucket is a list containing key->value pairs
	vector<list<Element>> elems_;
};


////////////////////////////////////////////////
// container member functions
////////////////////////////////////////////////

// Construct elems_ with the number of buckets.
template <typename Key, typename Value, typename Compare, typename Hash>
hashmap<Key, Value, Compare, Hash>::hashmap(
	const Compare& comp, const Hash& hash) :
	size_(0), comp_(comp), hash_(hash) {
	elems_ = vector<list<Element>>(hash_.numBuckets());
}


// helper function
template <typename Key, typename Value,
	typename Compare, typename Hash>
	typename list<pair<const Key, Value>>::iterator // return type
	hashmap<Key, Value, Compare, Hash>::findElement(const Key& x, size_t bucket) {

	// look for the key in the bucket
	for (auto it = elems_[bucket].begin();
		it != elems_[bucket].end(); ++it)
		if (comp_(it->first, x))
			return it;

	return elems_[bucket].end(); // element not found
}


// returns a pointer to the element with key x
// returns nullptr if no element with this key
template <typename Key, typename Value, typename Compare, typename Hash>
typename hashmap<Key, Value, Compare, Hash>::Element* hashmap<Key, Value, Compare, Hash>::find(const Key& x) {

	size_t bucket = hash_.hash(x);
	auto it = findElement(x, bucket);    // use the findElement() helper   

	if (it != elems_[bucket].end())
		// found the element. Return a pointer to it.
		return &(*it); // dereference the iterator to list 
					   // then take the address of list element

	else // didn't find the element -- return nullptr
		return nullptr;
}


// finds the element with key x, inserts an
// element with that key if none exists yet. Returns a reference to
// the value corresponding to that key.
template <typename Key, typename Value, typename Compare, typename Hash>
pair<Value*, bool> hashmap<Key, Value, Compare, Hash>::insert(const Element& x) {

	size_t bucket = hash_.hash(x.first);
	auto it = findElement(x.first, bucket);

	// Element doesn't exist
	if (it == elems_[bucket].end()) {

		++size_;
		elems_[bucket].push_back(x);

		auto new_location = findElement(x.first, bucket);
		return make_pair(&(new_location->second), true);

	}
	else {

		// It already exists, &() turns pointer into reference
		return make_pair(&(it->second), false);

	}

}


// removes the Element with key x, if it exists
template <typename Key, typename Value, typename Compare, typename Hash> // Template specifiers
pair<typename hashmap<Key, Value, Compare, Hash>::Element*, bool> // Return type
hashmap<Key, Value, Compare, Hash>::erase(const Key& x) {

	size_t elem_bucket = hash_.hash(x);
	auto elem_location = findElement(x, elem_bucket);

	// Element exists, we must now find the next one.
	if (elem_location != elems_[elem_bucket].end()) {

		///////////////////////////////////////////////
		// Start from our current position +1 in our
		// current bucket.
		auto current_bucket_it = elem_location;
		++current_bucket_it;  // ?can't increment elem_location?, so do it here.
		for (current_bucket_it; current_bucket_it != elems_[elem_bucket].end(); ++current_bucket_it) {

			/////////////////////////////////////////////
			// We've found the next element!
			// Delete the fuck out of x.
			//
			// Note that we must dereference the
			// iterator and return an address here: &(*)
			if (&(*current_bucket_it) != nullptr) {

				// Store key value before invalidating list
				// so that we can look it up afterwards.
				auto next_elem_key = current_bucket_it->first;

				// erase() invalidates all iterators.
				elems_[elem_bucket].erase(elem_location);

				// Re-validate next element iterator.
				auto next_elem_location = findElement(next_elem_key, elem_bucket);

				return make_pair(&(*next_elem_location), true);

			}

		}

		//////////////////////////////////////////////
		// If we've made it this far, then there
		// is no next element in our current bucket.
		//
		// We must now iterate through all buckets
		// that exist after ours to see if there is
		// a next element within those.
		for (size_t bucket_pos = elem_bucket + 1; bucket_pos < elems_.size(); ++bucket_pos) {

			// Iterate through each element inside of the current bucket.
			for (auto current_bucket_it = elems_[bucket_pos].begin(); current_bucket_it != elems_[bucket_pos].end(); ++current_bucket_it) {

				// We've found the next element!
				// Delete the fuck out of x.
				if (&(*current_bucket_it) != nullptr) {

					// Store key value before invalidating list
					// so that we can look it up afterwards.
					auto next_elem_key = current_bucket_it->first;

					// erase() invalidates all iterators.
					elems_[elem_bucket].erase(elem_location);

					// Re-validate next element iterator.
					auto next_elem_location = findElement(next_elem_key, bucket_pos);

					return make_pair(&(*next_elem_location), true);

				}

			}

		}

		//////////////////////////////////////////
		// At this point, the next element does
		// not exist at or after the bucket
		// we have to delete from.
		//
		// This means that we will have to
		// delete it and return a nullptr.
		elems_[elem_bucket].erase(elem_location);
		return make_pair(nullptr, true);

	}
	else {
		// Create unspecified element pointer
		Element* unspecified_element;
		return make_pair(unspecified_element, false);
	}

}


// returns reference to value of element with key x,
// inserts if does not exist
template <typename Key, typename Value, typename Compare, typename Hash>
Value& hashmap<Key, Value, Compare, Hash>::operator[] (const Key& x) {

	// I still know nothing about pointers vs references in C++ :|

	// insert() returns a pair of type <Value*, bool>,
	// we only care about the Value*.

	// Passing as a reference requires dereference
	// of the pointer stored in pair.first
	return *insert(make_pair(Key(x), Value())).first;

}

template <typename Key, typename Value, typename Compare, typename Hash>
void hashmap<Key, Value, Compare, Hash>::rehash(size_t n) {

	if (n <= size_) return;

	Hash new_hash(n);
	vector<list<Element>> new_elems(n);

	for (size_t current_bucket = 0; current_bucket < elems_.size(); ++current_bucket) {

		for (auto element_it = elems_[current_bucket].begin(); element_it != elems_[current_bucket].end(); std::advance(element_it, 1)) {
			if (&element_it != nullptr) {
				//std::cout << "Content of Element(" << &element << "): (Key: " << element.first << ", Value: " << element.second << ")\n";
				size_t element_new_bucket = new_hash.hash(element_it->first);
				new_elems[element_new_bucket].push_back(*element_it);
			}
		}

	}

	hash_ = new_hash;
	elems_ = new_elems; // causing a const error??
	size_ = n;

}
