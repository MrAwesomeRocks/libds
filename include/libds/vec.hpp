#ifndef LIBDS_VEC_HPP
#define LIBDS_VEC_HPP

#include <cstdlib>
#include <cstring>

#include <stdexcept>
#include <string>
#include <utility>

namespace ds {

template <class T>
class vec {
 public:
    using size_type = std::size_t;
    using iterator = T*;
    using const_iterator = const T*;

 private:
    size_type size_;
    size_type capacity_;
    T* data_;

    static constexpr size_type INITIAL_CAPACITY = 10;

#pragma region "Helpers"

    /**
     * @brief Get the next size of the vector from the current size.
     *
     * @param size The current size.
     * @return size_type The next size of the vector.
     */
    [[nodiscard]] inline size_type
    get_next_size_(size_type size) const noexcept
    {
        // 1.5x size
        // https://web.archive.org/web/20150806162750/http://www.gahcep.com/cpp-internals-stl-vector-part-1/
        // NOLINTNEXTLINE(hicpp-signed-bitwise): size_t is guaranteed to be unsigned
        return size + (size >> 2);
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
        for (size_t i = 0; i < size_; i++)
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
        std::memcpy(dest, src, size * sizeof(T));
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

#pragma endregion
};

} // namespace ds

#endif // LIBDS_VEC_HPP
