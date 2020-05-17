#ifndef VECTOR_H
#define VECTOR_H

#include <cstdio>
#include <algorithm>
#include <assert.h>
#include <iostream>

template <typename T>
struct vector
{
    typedef T* iterator;
    typedef T const* const_iterator;

    vector() {                              // O(1) nothrow
        data_ = nullptr;
        size_ = 0;
        capacity_ = 0;
    };


    vector(vector const& other) : vector() {                 // O(N) strong
        if (other.capacity_ == 0) {
            size_ = 0;
            capacity_ = 0;
            data_ = nullptr;
            return;
        }
        size_t new_size = other.size_;
        size_t new_capacity = other.size_;
        size_t i = 0;
        T* new_data;
        try {
            new_data = static_cast<T*>(operator new(new_size * sizeof(T)));

            for (; i < new_size; i++) {
                new(new_data + i) T(other[i]);
            }
        } catch (std::exception const& exc) {
            for (size_t j = 0; j < i; j++) {
                new_data[j].~T();
            }
            throw;
        }
        for (size_t i = 0; i < size_; i++) {
            data_[i].~T();
        }
        operator delete(data_);
        data_ = new_data;
        size_ = new_size;
        capacity_ = new_capacity;
    }

    vector& operator=(vector const& other) {
        vector<T> temp(other);
        swap(temp);
        return *this;
    } // O(N) strong

    ~vector() {
        for (size_t i = 0; i < size_; i++) {
            data_[i].~T();
        }
        operator delete(data_);
    }                              // O(N) nothrow

    T& operator[](size_t i) {
        return data_[i];
    }                // O(1) nothrow
    T const& operator[](size_t i) const {
        return data_[i];
    }    // O(1) nothrow

    T* data() {
        return data_;
    };                              // O(1) nothrow
    T const* data() const {
        return data_;
    }                  // O(1) nothrow
    size_t size() const {
        assert (size_ >= 0);
        return size_;
    }                    // O(1) nothrow

    T& front() {
        assert (size_ >= 0);
        return data_[0];
    }                             // O(1) nothrow
    T const& front() const {
        assert (size_ > 0);
        return data_[0];
    };                 // O(1) nothrow

    T& back() {
        assert (size_ > 0);
        return data_[size_ - 1];
    }                              // O(1) nothrow
    T const& back() const {
        assert (size_ > 0);
        return data_[size_ - 1];
    }                  // O(1) nothrow
    void push_back(T const& elem) {
        T temp(elem);
        if (size_ == capacity_) {
            reserve(capacity_ * 2 + 1);
        }
        new(data_ + size_++) T(temp);
    }               // O(1)* strong
    void pop_back() {
        assert(size_ != 0);
        data_[--size_].~T();
    }                        // O(1) nothrow

    bool empty() const {
        return size_ == 0;
    }                     // O(1) nothrow

    size_t capacity() const {
        return capacity_;
    };                // O(1) nothrow
    void reserve(size_t new_capacity) {
        if (new_capacity <= capacity_) {
            return;
        }
        new_buffer(new_capacity);
    }                   // O(N) strong
    void shrink_to_fit() {
        if (size_ == 0) {
            new_buffer(0);
        } else if (capacity_ != size_)
            new_buffer(size_);
    }                   // O(N) strong

    void clear() {
        for (size_t i = 0; i < size_; i++) {
            pop_back();
        }
    };                           // O(N) nothrow

    void swap(vector& other) {
        std::swap(capacity_, other.capacity_);
        std::swap(size_, other.size_);
        std::swap(data_, other.data_);
    }                     // O(1) nothrow

    iterator begin() {
        return data_;
    }                       // O(1) nothrow
    iterator end() {
        return data_ + size_;
    }                         // O(1) nothrow

    const_iterator begin() const {
        return data_;
    };           // O(1) nothrow
    const_iterator end() const {
        return data_ + size_;
    };             // O(1) nothrow

    iterator insert(iterator pos_it, T const& elem) {
        size_t pos = pos_it - begin();
        push_back(elem);
        size_t i = size_ - 1;
        for (; i > pos; i--) {
            std::swap(data_[i], data_[i - 1]);
        }
        return data_ + i;
    } // O(N) weak
    iterator insert(const_iterator pos, T const& elem) {
        return insert(const_cast<iterator>(pos), elem);
    }; // O(N) weak

    iterator erase(iterator pos_it) {
        size_t pos = pos_it - begin();
        size_t i = pos;
        for (; i + 1 < size_; i++) {
            std::swap(data_[i], data_[i + 1]);
        }
        pop_back();
        return begin() + pos;
    }           // O(N) weak
    iterator erase(const_iterator pos) {
        return erase(const_cast<iterator >(pos));
    }     // O(N) weak

    iterator erase(iterator first, iterator last) {
        for (iterator l = first, r = last; r != end(); l++, r++) {
            std::swap(*l, *r);
        }
        for (size_t i = 0; i < last - first; i++) {
            pop_back();
        }
        return data_ + (last - first);
    } // O(N) weak
    iterator erase(const_iterator first, const_iterator last) {
        for (iterator l = const_cast<iterator >(first), r = const_cast<iterator >(last); r != end(); l++, r++) {
            std::swap(*l, *r);
        }
        for (size_t i = 0; i < last - first; i++) {
            pop_back();
        }
        return data_ + (last - first);
    }; // O(N) weak

private:
    size_t increase_capacity() {
        reserve(2 * capacity_ + 1);
        return 2 * capacity_ + 1;
    };
    void push_back_realloc(T const& elem) {
        new(data_ + size_++) T(elem);
    }
    void new_buffer(size_t new_capacity) {
        T* new_data;
        size_t i = 0;
        try {
            new_data = static_cast<T*>(operator new(new_capacity * sizeof(T)));
            for (; i < size_; i++) {
                new(new_data + i) T(data_[i]);
            }
        } catch (std::exception const& exc) {
            for (size_t j = 0; j < i; j++) {
                new_data[j].~T();
            }
            throw;
        }
        for (size_t i = 0; i < size_; i++) {
            data_[i].~T();
        }
        if (capacity_ != 0) {
            operator delete(data_);
        }
        if (new_capacity == 0) {
            operator delete(new_data);
            data_ = nullptr;
            capacity_ = 0;
        } else {
            data_ = new_data;
            capacity_ = new_capacity;
        }
    }

private:
    T* data_;
    size_t size_;
    size_t capacity_;
};

#endif // VECTOR_H
