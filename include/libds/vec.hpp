/**
 * @file vec.hpp
 * @author Nino Maruszewski (nino.maruszewski@gmail.com)
 * @brief A dynamic array (vector) implementation.
 * @version 0.1
 * @date 2022-12-18
 *
 * MIT License
 *
 * Copyright (c) 2022 Nino Maruszewski
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */
#ifndef LIBDS_VEC_HPP
#define LIBDS_VEC_HPP

#include <cstdlib>
#include <cstring>

#include <stdexcept>
#include <string>
#include <utility>

/**
 * @brief Namespace for the data structures library.
 *
 * Based off of the STL implementation, but with some improvements.
 */
namespace ds {

/**
 * @brief An auto-resizing vector (i.e., a dynamic array).
 *
 * @tparam T The type of data this vector will hold.
 */
template <class T>
class vec {
 public:
    /**
     * @brief Unsigned integer type used for indicies.
     */
    using size_type = std::size_t;

    /**
     * @brief A random access iterator to @p T.
     */
    using iterator = T*;

    /**
     * @brief A constant random access iterator to @p T.
     */
    using const_iterator = const T*;

 private:
    size_type size_;
    size_type capacity_;
    T* data_;

    static constexpr size_type INITIAL_CAPACITY = 10;

#pragma region "Helpers"

    /**
     * @brief Get the next capacity of the vector from the current capacity.
     *
     * @param cap The current capacity.
     * @return size_type The next capacity of the vector.
     */
    [[nodiscard]] inline size_type
    next_capacity_(size_type cap) const noexcept
    {
        if (cap <= 1)
            return 2;

        // 1.5 * cap
        // https://web.archive.org/web/20150806162750/http://www.gahcep.com/cpp-internals-stl-vector-part-1/
        // NOLINTNEXTLINE(hicpp-signed-bitwise): size_type is guaranteed to be unsigned
        return cap + (cap >> 1);
    }

    /**
     * @brief Allocate memory
     *
     * @param cap The amount of elements this should be able to hold.
     * @return A pointer to the data for this vector.
     */
    [[nodiscard]] inline T*
    alloc_(size_type cap) const
    {
        if (cap == 0)
            return nullptr;

        // NOLINTNEXTLINE(cppcoreguidelines-no-malloc,modernize-use-auto)
        auto* ptr = static_cast<T*>(std::malloc(cap * sizeof(T)));
        if (ptr == nullptr)
            throw std::runtime_error("vec: could not allocate memory");

        return ptr;
    }

    /**
     * @brief Resize the internal data buffer.
     *
     * @param cap The amount of elements this should be able to hold.
     */
    inline void
    resize_(size_type new_cap)
    {
        if (new_cap == 0)
            free_();

        // NOLINTNEXTLINE(modernize-use-auto)
        auto* ptr = static_cast<T*>(std::realloc( // NOLINT(cppcoreguidelines-no-malloc)
            data_, new_cap * sizeof(T)
        ));

        if (ptr == nullptr)
            throw std::runtime_error("vec: could not allocate memory");

        data_ = ptr;
        capacity_ = new_cap;
    }

    /**
     * @brief Free the internal array.
     */
    inline void
    free_()
    {
        // Call destructors
        for (size_type i = 0; i < size_; i++)
            data_[i].~T();

        std::free(data_); // NOLINT(cppcoreguidelines-no-malloc)
        data_ = nullptr;
    }

    /**
     * @brief Copy two buffers, templated to this class.
     *
     * @param dest Where to copy to.
     * @param src Where to copy from.
     * @param size How many elements to copy.
     */
    inline void
    copy_(T* dest, T* src, size_type size) const noexcept
    {
        std::memmove(dest, src, size * sizeof(T));
    }

    /**
     * @brief Shift all elements from @p start to end() over @p places places.
     *
     * @param start Where to start shifting elements.
     * @param places How many places to shift the elements.
     */
    inline void
    shift_(size_type start, size_type places)
    {
        // Check if we have to resize
        size_type new_cap = capacity_;
        while (size_ + places > new_cap)
            new_cap = next_capacity_(new_cap);

        reserve(new_cap);

        // Shift elements down
        copy_(&data_[start + places], &data_[start], size_ - start);

        size_ += places;
    }

#pragma endregion

 public:
#pragma region "Constructors/Destructors and assignment operators"

    /**
     * @brief Construct a new empty vec object, with a given capacity.
     *
     * @param capacity How many elements should this vector be able to hold initially.
     */
    explicit vec(size_type capacity = INITIAL_CAPACITY) :
        size_(0), capacity_(capacity), data_(alloc_(capacity_))
    {}

    /**
     * @brief Construct a new vec object with specified size, filled with elements.
     *
     * @param size The size of the vec.
     * @param elem The element to fill the vector with
     */
    explicit vec(size_type size, T elem) :
        size_(size), capacity_(size), data_(alloc_(capacity_))
    {
        for (size_type i = 0; i < size_; i++)
            data_[i] = elem;
    }

    /**
     * @brief Construct a new vec object from an initializer list
     *
     * @param init The initializer list with vector elements.
     */
    vec(std::initializer_list<T> init) :
        size_(init.size()), capacity_(init.size()), data_(alloc_(capacity_))
    {
        auto init_data = std::data(init);
        for (size_type i = 0; i < init.size(); i++) {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            data_[i] = init_data[i];
        }
    }

    /**
     * @brief Copy constructor.
     *
     * @param other The vector to copy to this one.
     */
    vec(const vec& other) :
        size_(other.size_), capacity_(other.capacity_), data_(alloc_(capacity_))
    {
        copy_(data_, other.data_, capacity_);
    }

    /**
     * @brief Move constructor.
     *
     * @param other The vector to move to this one.
     */
    vec(vec&& other) noexcept :
        size_(std::exchange(other.size_, 0U)),
        capacity_(std::exchange(other.capacity_, 0U)),
        data_(std::exchange(other.data_, nullptr))
    {}

    /**
     * @brief Copy assignment operator.
     *
     * @param other The assigned object.
     * @return The new object.
     */
    vec&
    operator=(const vec& other)
    {
        // Guard self assignment
        if (this == &other)
            return *this;

        // Check if we can reuse our array
        if (capacity_ < other.size_)
            resize_(other.size_);

        size_ = other.size_;

        copy_(data_, other.data_, capacity_);
        return *this;
    }

    /**
     * @brief Move assignment operator.
     *
     * @param other The assigned object.
     * @return The new object.
     */
    vec&
    operator=(vec&& other) noexcept
    {
        // Guard self-assignment
        if (this == &other)
            return *this;

        // Free our resources
        free_();

        // Leave the other in a valid state
        size_ = std::exchange(other.size_, 0);
        capacity_ = std::exchange(other.capacity_, 0);
        data_ = std::exchange(other.data_, nullptr);

        return *this;
    }

    /**
     * @brief Destroy the vec object. Frees the internal array.
     */
    ~vec() noexcept { free_(); }

#pragma endregion

#pragma region "Accessors"

    /**
     * @brief Get a reference to the nth element.
     *
     * No bounds checking is performed.
     *
     * @param pos The position of the desired element
     * @return T& The element at position pos.
     */
    [[nodiscard]] inline T&
    operator[](size_type pos) noexcept
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
    [[nodiscard]] inline const T&
    operator[](size_type pos) const noexcept
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
    [[nodiscard]] inline T&
    at(size_type pos)
    {
        if (pos >= size_)
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
     * @return T& The element at position pos.
     */
    [[nodiscard]] inline const T&
    at(size_type pos) const
    {
        if (pos >= size_)
            throw std::out_of_range("vec: index out of range!");
        return data_[pos];
    }

    /**
     * @brief Get the first element.
     *
     * @return T& A reference to the first element.
     */
    [[nodiscard]] inline T&
    front() noexcept
    {
        return data_[0];
    }

    /**
     * @brief Get the first element.
     *
     * @return T& A reference to the first element.
     */
    [[nodiscard]] inline const T&
    front() const noexcept
    {
        return data_[0];
    }

    /**
     * @brief Get the last element.
     *
     * @return T& A reference to the last element.
     */
    [[nodiscard]] inline T&
    back() noexcept
    {
        return data_[size_ - 1];
    }

    /**
     * @brief Get the last element.
     *
     * @return T& A reference to the last element.
     */
    [[nodiscard]] inline const T&
    back() const noexcept
    {
        return data_[size_ - 1];
    }

    /**
     * @brief Get access to the underlying data vector.
     *
     * Guaranteed to be valid up to size() elements.
     *
     * @return T* The underlying vector of data.
     */
    [[nodiscard]] inline T*
    data() noexcept
    {
        // We still own the data, by the definition of this fn
        return static_cast<T*>(data_);
    }

    /**
     * @brief Get access to the underlying data vector.
     *
     * Guaranteed to be valid up to size() elements.
     *
     * @return const T* The underlying vector of data.
     */
    [[nodiscard]] inline const T*
    data() const noexcept
    {
        // We still own the data, by the definition of this fn
        return static_cast<T*>(data_);
    }

#pragma endregion

#pragma region "Capacity"

    /**
     * @brief Check if the vector is empty.
     *
     * @return bool Whether or not the vector is empty.
     */
    [[nodiscard]] inline bool
    empty() const noexcept
    {
        return size_ == 0;
    }

    /**
     * @brief Get the size of the vector.
     *
     * This is the number of used spaces within the vector.
     *
     * @return size_type The vector size.
     */
    [[nodiscard]] inline size_type
    size() const noexcept
    {
        return size_;
    }

    /**
     * @brief Get the capacity of the vector.
     *
     * This is the total size of the underlying array.
     *
     * @return size_type The vector capacity.
     */
    [[nodiscard]] inline size_type
    capacity() const noexcept
    {
        return capacity_;
    }

    /**
     * @brief Resize the vector to be able to hold at least @p new_cap elements.
     *
     * Does nothing if the desired capacity is less than the current capacity.
     *
     * @param new_cap The new desired capacity of the vector.
     */
    inline void
    reserve(size_type new_cap)
    {
        if (new_cap > capacity_)
            resize_(new_cap);
    }

    /**
     * @brief Remove any unused space from this vector.
     *
     * This sets capacity() to size();
     */
    inline void
    shrink_to_fit()
    {
        resize_(size_);
    }

#pragma endregion

#pragma region "Iterators"

    /**
     * @brief Get an iterator pointing to the beginning of this vector.
     *
     * @return iterator The iterator.
     */
    [[nodiscard]] inline iterator
    begin() noexcept
    {
        // We still own the data, by the definition of this fn
        return static_cast<T*>(data_);
    }

    /**
     * @brief Get an iterator pointing to the beginning of this vector.
     *
     * @return iterator The iterator.
     */
    [[nodiscard]] inline const_iterator
    begin() const noexcept
    {
        // We still own the data, by the definition of this fn
        return static_cast<T*>(data_);
    }

    /**
     * @brief Get an iterator pointing to the end of this vector.
     *
     * @return iterator The iterator.
     */
    [[nodiscard]] inline iterator
    end() noexcept
    {
        return data_ + size_;
    }

    /**
     * @brief Get an iterator pointing to the end of this vector.
     *
     * @return iterator The iterator.
     */
    [[nodiscard]] inline const_iterator
    end() const noexcept
    {
        return data_ + size_;
    }

#pragma endregion

#pragma region "Modifiers"

    /* Everthing here changes vector size */

    /**
     * @brief Clear the contents of the vector.
     *
     * Does not change the capacity.
     */
    inline void
    clear() noexcept
    {
        size_ = 0;
    }

    /**
     * @brief Insert an element @p elem at position @p pos.
     *
     * Copies the element into the vector.
     * Can insert one past the end of the vector (at size());
     *
     * @param pos The position to insert the element in (zero indexed).
     * @param elem The element to insert.
     * @return An iterator pointing to the new element.
     */
    inline iterator
    insert(size_type pos, const T& elem)
    {
        shift_(pos, 1);
        data_[pos] = elem;

        return data_ + pos;
    }

    /**
     * @brief Insert an element @p elem at position @p pos.
     *
     * Moves the element into the vector.
     * Can insert one past the end of the vector (at size());
     *
     * @param pos The position to insert the element in (zero indexed).
     * @param elem The element to insert.
     * @return An iterator pointing to the new element.
     */
    inline iterator
    insert(size_type pos, T&& elem)
    {
        shift_(pos, 1);
        data_[pos] = elem;

        return data_ + pos;
    }

    /**
     * @brief Insert @p count copies of @p elem at position @p pos.
     *
     * Can insert one past the end of the vector (at size());
     *
     * @param pos The position to insert the element in (zero indexed).
     * @param elem The element to insert.
     * @return An iterator pointing to the first element inserted.
     */
    inline iterator
    insert(size_type pos, size_type count, const T& elem)
    {
        shift_(pos, count);

        for (size_type i = pos; i < pos + count; i++)
            data_[i] = elem;

        return data_ + pos;
    }

    /**
     * @brief Insert @p elems at position @p pos.
     *
     * Can insert one past the end of the vector (at size());
     *
     * @param pos The position to insert the element in (zero indexed).
     * @param elems The elements to insert.
     * @return An iterator pointing to the first element inserted.
     */
    inline iterator
    insert(size_type pos, std::initializer_list<T> elems)
    {
        shift_(pos, elems.size());

        auto* elem_data = std::data(elems);
        for (size_type i = 0; i < elems.size(); i++)
            data_[i + pos] = elem_data[i];

        return data_ + pos;
    }

#pragma endregion

#pragma region "Equality operators"

    inline friend bool
    operator==(const ds::vec<T>& lhs, const ds::vec<T>& rhs)
    {
        // Check if they are the same object
        if (&lhs == &rhs)
            return true;

        // Check if the vectors have different sizes
        if (lhs.size_ != rhs.size_)
            return false;

        // Finally, check each element
        for (size_type i = 0; i < lhs.size_; i++) {
            if (lhs[i] != rhs[i])
                return false;
        }
        return true;
    }

    inline friend bool
    operator!=(const ds::vec<T>& lhs, const ds::vec<T>& rhs)
    {
        return !(lhs == rhs);
    }

#pragma endregion
};

} // namespace ds

#endif // LIBDS_VEC_HPP
