#pragma once

#include <cstdlib>
#include <cstring>
#include <gsl/gsl>
#include <stdexcept>
#include <string>

namespace ds {

template <class T>
class vec {
    size_t size_;
    size_t capacity_;
    gsl::owner<T*> data_;

    static constexpr size_t INITIAL_CAPACITY = 10;

    /**
     * @brief Get the next size of the vector from the current size.
     *
     * @param size The current size.
     * @return size_t The next size of the vector.
     */
    [[nodiscard]] inline auto
    next_size_(size_t size) const -> size_t
    {
        // 1.5x size
        // https://web.archive.org/web/20150806162750/http://www.gahcep.com/cpp-internals-stl-vector-part-1/
        return size + (size >> 2);
    }

    /**
     * @brief Allocate memory
     *
     * @param cap The amount of elements this should be able to hold.
     * @return A pointer to the data for this vector.
     */
    [[nodiscard]] inline auto
    alloc_(size_t cap) const -> gsl::owner<T*>
    {
        // NOLINTNEXTLINE(cppcoreguidelines-no-malloc,modernize-use-auto)
        gsl::owner<T*> ptr = static_cast<gsl::owner<T*>>(std::malloc(cap * sizeof(T)));
        if (ptr == nullptr)
            throw std::runtime_error("vec: could not allocate memory");

        return ptr;
    }

 public:
    /**
     * @brief Construct a new empty vec object, with a given capacity.
     *
     * @param capacity How many elements should this vector be able to hold initially.
     */
    explicit vec(size_t capacity = INITIAL_CAPACITY) :
        size_(0), capacity_(capacity), data_(alloc_(capacity_))
    {}

    /**
     * @brief Construct a new vec object with specified size, filled with elements.
     *
     * @param size The size of the vec.
     * @param elem The element to fill the vector with
     */
    explicit vec(size_t size, T elem) :
        size_(size), capacity_(size), data_(alloc_(capacity_))
    {
        for (size_t i = 0; i < size_; i++)
            data_[i] = elem;
    }

    /**
     * @brief Construct a new vec object from an initializer list
     *
     * @param init The initializer list with vector elements.
     */
    explicit vec(std::initializer_list<T> init) :
        size_(init.size()), capacity_(init.size()), data_(alloc_(capacity_))
    {
        auto init_data = std::data(init);
        for (size_t i = 0; i < init.size(); i++)
            data_[i] = init_data[i];
    }

    /**
     * @brief Destroy the vec object. Frees the internal array.
     */
    ~vec() { std::free(data_); } // NOLINT(cppcoreguidelines-no-malloc)

    /**
     * @brief Get a reference to the nth element.
     *
     * No bounds checking is performed.
     *
     * @param pos The position of the desired element
     * @return T& The element at position pos.
     */
    [[nodiscard]] inline auto
    operator[](size_t pos) -> T&
    {
        return data_[pos];
    }

    /**
     * @brief Get a const reference to the nth element.
     *
     * No bounds checking is performed.
     *
     * @param pos The position of the desired element
     * @return const T& The element at position pos.
     */
    [[nodiscard]] inline auto
    operator[](size_t pos) const -> const T&
    {
        return data_[pos];
    }

    /**
     * @brief Get a reference to the nth element.
     *
     * Bounds checking is performed.
     *
     * @param pos The position of the desired element
     * @exception std::out_of_range Index out of range of vector.
     * @return T& The element at position pos.
     */
    [[nodiscard]] inline auto
    at(size_t pos) -> T&
    {
        if (pos > size_)
            throw std::out_of_range("vec: index out of range!");
        return data_[pos];
    }

    /**
     * @brief Get a const reference to the nth element.
     *
     * Bounds checking is performed.
     *
     * @param pos The position of the desired element
     * @exception std::out_of_range Index out of range of vector.
     * @return  The element at position pos.
     */
    [[nodiscard]] inline auto
    at(size_t pos) const -> const T&
    {
        if (pos > size_)
            throw std::out_of_range("vec: index out of range!");
        return data_[pos];
    }

    /**
     * @brief Get the size of the vector.
     *
     * This is the number of used spaces within the vector.
     *
     * @return size_t The vector size.
     */
    [[nodiscard]] inline auto
    size() const -> size_t
    {
        return size_;
    }

    /**
     * @brief Get the capacity of the vector.
     *
     * This is the total size of the underlying array.
     *
     * @return size_t The vector capacity.
     */
    [[nodiscard]] inline auto
    capacity() const -> size_t
    {
        return capacity_;
    }
};

} // namespace ds
