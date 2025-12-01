#include <array>
#include <cstddef>

template <typename Type>
class ArrayPtr {
public:
    ArrayPtr() = default;

    ArrayPtr(Type* p)
        : raw_ptr_(p) {}
    
    ArrayPtr(const size_t& size)
        : raw_ptr_(size ? new Type[size] : nullptr) {}

    ~ArrayPtr() {
        delete[] raw_ptr_;
        raw_ptr_ = nullptr;
    }

    ArrayPtr(ArrayPtr&& other){
        std::swap(raw_ptr_, other.raw_ptr_);
    }

    ArrayPtr& operator=(ArrayPtr&& other) {
        std::swap(raw_ptr_, other.raw_ptr_);
        return *this;
    }

    void swap(ArrayPtr& rhs) noexcept{
        std::swap(raw_ptr_, rhs.raw_ptr_);
    }

    // Возвращает ссылку на массив указателей
    Type* GetItems() noexcept {
        return raw_ptr_;
    }

    // Возвращает константную ссылку на массив указателей
    const Type* GetItems() const noexcept {
        return raw_ptr_;
    }

    void Reset(Type* ptr = nullptr) {
        delete[] raw_ptr_;
        raw_ptr_ = ptr;
    }

    Type* Release() noexcept {
        Type* ptr = raw_ptr_;
        raw_ptr_ = nullptr;
        return ptr;
    }

private:
    Type* raw_ptr_ = nullptr;
};
