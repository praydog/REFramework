namespace sdk {
struct SystemArray;
}

#pragma once

#include <vector>

#include "REManagedObject.hpp"

namespace sdk {
struct SystemArray;

struct SystemArray : public ::REManagedObject {
    size_t get_size();
    ::REManagedObject* get_element(int32_t index);
    void set_element(int32_t index, ::REManagedObject* value);
    std::vector<::REManagedObject*> get_elements();

    using size_type = size_t;
    using value_type = ::REManagedObject*;

    class iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using size_type = size_t;
        using value_type = ::REManagedObject*;
        using difference_type = std::ptrdiff_t;
        using reference = ::REManagedObject*;
        using pointer = ::REManagedObject*;

        iterator(::sdk::SystemArray* array, size_t start = 0)
            : m_parent{array}, 
            m_index{start} 
        {

        }

        value_type operator*() const {
            return m_parent->get_element(m_index);
        }

        operator value_type() const {
            return m_parent->get_element(m_index);
        }

        // setter
        iterator& operator=(value_type value) {
            m_parent->set_element(m_index, value);
            return *this;
        }

        iterator& operator++() {
            m_index++;
            return *this;
        }

        iterator& operator--() {
            m_index--;
            return *this;
        }

        iterator& operator+=(size_t n) {
            m_index += n;
            return *this;
        }

        iterator& operator-=(size_t n) {
            m_index -= n;
            return *this;
        }

        iterator operator+(size_t n) const {
            return iterator{m_parent, m_index + n};
        }

        iterator operator-(size_t n) const {
            return iterator{m_parent, m_index - n};
        }

        bool operator!=(const iterator& other) const {
            return m_index != other.m_index || m_parent != other.m_parent;
        }

        bool operator==(const iterator& other) const {
            return m_index == other.m_index && m_parent == other.m_parent;
        }

    private:
        ::sdk::SystemArray* m_parent{};
        size_t m_index{0};
    };

    iterator begin() {
        return iterator{this};
    }

    iterator end() {
        return iterator{this, get_size()};
    }

    size_type size() {
        return get_size();
    }

    value_type operator[](size_t index) {
        return get_element(index);
    }

    value_type at(size_t index) {
        return get_element(index);
    }

    bool empty() {
        return get_size() == 0;
    }
};
}