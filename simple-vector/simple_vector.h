#pragma once

#include "array_ptr.h"

#include <utility>
#include <cassert>
#include <stdexcept>
#include <initializer_list>
#include <algorithm>
#include <iostream>

class ReserveProxyObj {
public:
    explicit ReserveProxyObj(size_t capacity)
        :  capacity_(capacity){}

    size_t GetCapacity() const noexcept {
        return capacity_;
    }
private:

    size_t capacity_;
};


ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}


template <typename Type>
class SimpleVector {

public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    explicit SimpleVector(size_t size)
        : items_(size)
        , size_(size)
        , capacity_(size){
        std::fill(begin(), end(), Type());
    }

    SimpleVector(ReserveProxyObj reserved)
        : items_(reserved.GetCapacity())
        , size_(0)
        , capacity_(reserved.GetCapacity()){
    }


    SimpleVector(size_t size, const Type& value)
        : items_(size)
        , size_(size)
        , capacity_(size){
        std::fill(begin(), end(), value);
    }

    SimpleVector(std::initializer_list<Type> init)
        : items_(init.size())
        , size_(init.size())
        , capacity_(init.size()){
        std::copy(init.begin(), init.end(), begin());
    }


    SimpleVector(const SimpleVector& other)
        : items_(other.capacity_)
        , size_(other.size_)
        , capacity_(other.capacity_){
        std::copy(other.begin(), other.end(), begin());
    }

    SimpleVector(SimpleVector&& other)
        : items_(std::move(other.items_))
        , size_(other.size_)
        , capacity_(other.capacity_) {
        other.size_ = 0;
        other.capacity_ = 0;
    }

    //Операторы
    Type& operator[](size_t index) noexcept {
        return items_.GetItems()[index];
    }

    const Type& operator[](size_t index) const noexcept {
        return items_.GetItems()[index];
    }

    SimpleVector& operator=(const SimpleVector& other) {
        if (this != &other) {
            if(other.IsEmpty()){
                Clear();
            } else{
                SimpleVector temp(other);
                swap(temp);
            }
        }
        return *this;
    }

    SimpleVector& operator=(SimpleVector&& rhs) {
        if (this != &rhs) {
            swap(rhs);
        }
        return *this;
    }


    //Методы
    void PushBack(const Type& value) {
        if (size_ == capacity_) {
            Reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        items_.GetItems()[size_++] = value;
    }

    void PushBack(Type&& value) {
        if (size_ == capacity_) {
            Reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        items_.GetItems()[size_++] = std::move(value);
    }


    Iterator Insert(ConstIterator pos, const Type& value) {
        assert(pos >= begin() && pos <= end());
        size_t index = pos - begin();

        if (size_ == capacity_) {
            Reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }

        // Сдвигаем элементы вправо
        std::copy_backward(begin() + index, end(), end() + 1);

        items_.GetItems()[index] = value;
        ++size_;
        return begin() + index;
    }


    Iterator Insert(ConstIterator pos, Type&& value) {
        assert(pos >= begin() && pos <= end());
        size_t index = pos - begin();

        if (size_ == capacity_) {
            Reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }

        // Сдвигаем элементы вправо
        std::move_backward(begin() + index, end(), end() + 1);

        items_.GetItems()[index] = std::move(value);
        ++size_;
        return begin() + index;
    }

    void PopBack() noexcept {
        if(!IsEmpty()){
            size_--;
        }
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
        size_t index = pos - begin();
        std::move(begin() + index + 1, end(), begin() + index);
        --size_;
        return begin() + index;
    }


    // Обменивает значение с другим вектором
    void swap(SimpleVector& other) noexcept {
        items_.swap(other.items_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    size_t GetSize() const noexcept {return size_;}
    size_t GetCapacity() const noexcept {return capacity_;}
    bool IsEmpty() const noexcept {return size_ == 0;}

    Type& At(size_t index) {
        if(index >= size_){
            throw std::out_of_range("Out of range");
        }
        return items_.GetItems()[index];
    }

    const Type& At(size_t index) const {
        if(index >= size_){
            throw std::out_of_range("Out of range");
        }
        return items_.GetItems()[index];
    }


    void Clear() noexcept {
        size_ = 0;
    }

    // Изменяет размер массива. Размер != вместимость.
    void Resize(size_t new_size) {
        if (new_size > capacity_) {
            Reserve(std::max(new_size, capacity_ == 0 ? 1 : capacity_ * 2));
        }

        if (new_size > size_) {
            for (size_t i = size_; i < new_size; ++i) {
                items_.GetItems()[i] = Type();
            }
        }

        size_ = new_size;
    }

    void Reserve(size_t new_capacity){
        if (new_capacity <= capacity_) {
            return;
        }

        ArrayPtr<Type> new_items(new Type[new_capacity]);
        std::copy(std::make_move_iterator(begin()), std::make_move_iterator(end()), new_items.GetItems());
        items_ = std::move(new_items);
        capacity_ = new_capacity;
    }

    //Итераторы
    Iterator begin() noexcept {
        return items_.GetItems();
    }

    Iterator end() noexcept {
        return items_.GetItems() + size_;
    }

    ConstIterator begin() const noexcept {
        return items_.GetItems();
    }

    ConstIterator end() const noexcept {
        return items_.GetItems() + size_;
    }

    ConstIterator cbegin() const noexcept {
        return items_.GetItems();
    }

    ConstIterator cend() const noexcept {
        return items_.GetItems() + size_;
    }

    void PrintInfo(){
        std::cout << "Elements: ";
        for(size_t i = 0; i < size_; ++i){
            std::cout << items_.GetItems()[i] << " ";
        }

        std::cout << "\nSize: " << size_;
        std::cout << "\nCapacity: " << capacity_;
    }
private:
    ArrayPtr<Type> items_;

    size_t size_ = 0;
    size_t capacity_ = 0;
};



template <typename Type>
bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return lhs.GetSize() == rhs.GetSize() &&
           std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
}
