#pragma once

#include "Memory.hpp"

namespace sdk {
template <typename T> struct NativeArray {
    ~NativeArray() {
        if (elements != nullptr) {
            sdk::memory::deallocate(elements);
        }

        elements = nullptr;
        num = 0;
    }

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

    uint32_t size() const {
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

    T& emplace() {
        if (elements == nullptr) {
            elements = (T*)sdk::memory::allocate(sizeof(T) * 1);

            num_allocated = 1;
            num = 1;

            elements[0] = T{};
        } else {
            if (num >= num_allocated) {
                T* new_elements = (T*)sdk::memory::allocate(sizeof(T) * (num_allocated + 1));

                for (uint32_t i = 0; i < num; i++) {
                    new_elements[i] = elements[i];
                }

                sdk::memory::deallocate(elements);
                elements = new_elements;
                ++num_allocated;
            }

            elements[num++] = T{};
        }

        return elements[num - 1];
    }

    void push_back(T& value) {
        emplace() = value;
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
    T* elements{nullptr};
    uint32_t num{0};
    uint32_t num_allocated;
};

template <typename T>
struct NativeArrayNoCapacity {
    ~NativeArrayNoCapacity() {
        if (elements != nullptr) {
            sdk::memory::deallocate(elements);
        }

        elements = nullptr;
        num = 0;
    }

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

    void relocate_pointers(T* new_location, uint32_t location_count) {
        if constexpr (sizeof(T) < sizeof(uintptr_t)) {
            return;
        }

        if (elements == nullptr) {
            return;
        }

        for (uint32_t i = 0; i < num * sizeof(T); i++) {
            const auto ptr = *(uintptr_t*)((uintptr_t)elements + i);

            if (ptr >= (uintptr_t)elements && ptr < (uintptr_t)elements + (num * sizeof(T))) {
                *(uintptr_t*)((uintptr_t)new_location + i) = (uintptr_t)new_location + (ptr - (uintptr_t)elements);
            }
        }
    }

    bool resize(uint32_t new_size, bool fix_pointers = false) {
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

                if (fix_pointers) {
                    relocate_pointers(new_elements, num);
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

    T& emplace(bool fix_pointers = false) {
        if (elements == nullptr) {
            elements = (T*)sdk::memory::allocate(sizeof(T) * 1);

            num = 1;
            elements[0] = T{};
        } else {
            T* new_elements = (T*)sdk::memory::allocate(sizeof(T) * (num_allocated + 1));

            for (uint32_t i = 0; i < num; i++) {
                new_elements[i] = elements[i];
            }

            if (fix_pointers) {
                relocate_pointers(new_elements, num);
            }

            sdk::memory::deallocate(elements);

            elements = new_elements;
            elements[num++] = T{};
        }

        return elements[num - 1];
    }

    void push_back(T& value, bool fix_pointers = false) {
        emplace(fix_pointers) = value;
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
    T* elements{nullptr};

    union {
        uint64_t num{0};
        uint64_t num_allocated;
    };
};
} // namespace sdk