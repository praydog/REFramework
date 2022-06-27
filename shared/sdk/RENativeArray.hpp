#pragma once

#include "Memory.hpp"

namespace sdk {
template <typename T> struct NativeArray {
    T* begin() const {
        if (elements == nullptr || num == 0 || num_allocated == 0) {
            return nullptr;
        }

        return elements;
    }

    T* end() const {
        if (elements == nullptr || num == 0 || num_allocated == 0) {
            return nullptr;
        }

        return elements + num;
    }

    uint64_t size() const {
        return num;
    }

    bool empty() const {
        if (elements == nullptr || num == 0 || num_allocated == 0) {
            return true;
        }

        return false;
    }

    void clear() {
        if (elements == nullptr) {
            return;
        }

        num = 0;
        
        return;
    }

    bool resize(uint32_t new_size) {
        if (new_size == num) {
            return true;
        }

        if (new_size == 0) {
            clear();
            return;
        }

        if (new_size > num_allocated) {
            T* new_elements = (T*)sdk::memory::allocate(sizeof(T) * new_size);
            if (new_elements == nullptr) {
                return false;
            }

            if (elements != nullptr) {
                for (uint32_t i = 0; i < num; i++) {
                    if (i >= new_size) {
                        break;
                    }

                    new_elements[i] = elements[i];
                }

                sdk::memory::deallocate(elements);
            }

            elements = new_elements;
            num_allocated = new_size;
        } else if (new_size > num) {
            for (uint32_t i = num; i < new_size; i++) {
                elements[i] = T{};
            }
        } else if (new_size < num) {
            for (uint32_t i = new_size - 1; i < num; i++) {
                elements[i] = T{};
            }
        }

        num = new_size;
    }

    void push_back(const T& value) {
        if (elements == nullptr) {
            elements = (T*)sdk::memory::allocate(sizeof(T) * 1);
            if (elements == nullptr) {
                return;
            }

            elements[0] = value;
            num_allocated = 1;
            num = 1;
        } else {
            if (num <= num_allocated) {
                T* new_elements = (T*)sdk::memory::allocate(sizeof(T) * (num_allocated + 1));
                if (new_elements == nullptr) {
                    return;
                }

                for (uint32_t i = 0; i < num; i++) {
                    new_elements[i] = elements[i];
                }

                sdk::memory::deallocate(elements);
                elements = new_elements;
            }

            elements[num++] = value;
        }
    }

    const T& operator[] (uint32_t index) const {
        if (index >= num || elements == nullptr) {
            return T{};
        }

        return elements[index];
    }

    T& operator[] (uint32_t index) {
        if (index >= num || elements == nullptr) {
            return T{};
        }

        return elements[index];
    }
    
public:
    T* elements;
    uint32_t num;
    uint32_t num_allocated;
};

template <typename T>
struct NativeArrayNoCapacity {
        T* begin() const {
        if (elements == nullptr || num == 0 || num_allocated == 0) {
            return nullptr;
        }

        return elements;
    }

    T* end() const {
        if (elements == nullptr || num == 0 || num_allocated == 0) {
            return nullptr;
        }

        return elements + num;
    }

    uint64_t size() const {
        return num;
    }

    bool empty() const {
        if (elements == nullptr || num == 0 || num_allocated == 0) {
            return true;
        }

        return false;
    }

    void clear(bool force = false) {
        if (force) {
            if (elements != nullptr) {
                sdk::memory::deallocate(elements);
                elements = nullptr;
            }
        }

        num = 0;
        
        return;
    }

    bool resize(uint32_t new_size) {
        if (new_size == num) {
            return true;
        }

        if (new_size == 0) {
            clear();
            return;
        }

        if (new_size > num_allocated || elements == nullptr) {
            T* new_elements = (T*)sdk::memory::allocate(sizeof(T) * new_size);
            if (new_elements == nullptr) {
                return false;
            }

            if (elements != nullptr) {
                for (uint32_t i = 0; i < num; i++) {
                    if (i >= new_size) {
                        break;
                    }

                    new_elements[i] = elements[i];
                }

                sdk::memory::deallocate(elements);
            }

            elements = new_elements;
            num_allocated = new_size;
        } else if (new_size > num) {
            for (uint32_t i = num; i < new_size; i++) {
                elements[i] = T{};
            }
        } else if (new_size < num) {
            for (uint32_t i = new_size - 1; i < num; i++) {
                elements[i] = T{};
            }
        }

        num = new_size;
    }

    void push_back(const T& value) {
        if (elements == nullptr) {
            elements = (T*)sdk::memory::allocate(sizeof(T) * 1);
            if (elements == nullptr) {
                return;
            }

            elements[0] = value;
            num_allocated = 1;
        } else {
            T* new_elements = (T*)sdk::memory::allocate(sizeof(T) * (num_allocated + 1));
            if (new_elements == nullptr) {
                return;
            }

            for (uint32_t i = 0; i < num; i++) {
                new_elements[i] = elements[i];
            }

            sdk::memory::deallocate(elements);

            elements = new_elements;
            elements[num++] = value;
        }
    }

    T& emplace() {
        push_back(T{});
        return elements[num - 1];
    }

    void pop_back() {
        if (elements == nullptr || num == 0) {
            return;
        }

        num--;
    }

    void erase(uint32_t index) {
        if (index >= num || elements == nullptr) {
            return;
        }

        for (uint32_t i = index; i < num - 1; ++i) {
            elements[i] = elements[i + 1];
        }

        num--;
    }

    const T& operator[] (uint32_t index) const {
        return elements[index];
    }

    T& operator[] (uint32_t index) {
        return elements[index];
    }

public:
    T* elements;

    union {
        uint64_t num;
        uint64_t num_allocated;
    };
};
} // namespace sdk