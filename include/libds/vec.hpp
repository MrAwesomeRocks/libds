#ifndef LIBDS_VEC_HPP
#define LIBDS_VEC_HPP

#include <gsl/gsl>

#include <cstdlib>
#include <cstring>

#include <stdexcept>
#include <string>

namespace ds {

template <class T>
class vec {
 public:
    using size_type = std::size_t;

 private:
    size_type size_;
    size_type capacity_;
    gsl::owner<T*> data_;

    static constexpr size_type INITIAL_CAPACITY = 10;

    /**
     * @brief Get the next size of the vector from the current size.
     *
     * @param size The current size.
     * @return size_type The next size of the vector.
     */
    [[nodiscard]] inline size_type
    get_next_size_(size_type size) const
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
    [[nodiscard]] inline gsl::owner<T*>
    alloc_(size_type cap) const
    {
        if (cap == 0)
            return static_cast<gsl::owner<T*>>(nullptr);

        // NOLINTNEXTLINE(cppcoreguidelines-no-malloc,modernize-use-auto)
        gsl::owner<T*> ptr = static_cast<gsl::owner<T*>>(std::malloc(cap * sizeof(T)));
        if (ptr == nullptr)
            throw std::runtime_error("vec: could not allocate memory");

        return ptr;
    }

    /**
     * @brief Copy two buffers, templated to this class.
     *
     * @param dest Where to copy to.
     * @param src Where to copy from.
     * @param size How many elements to copy.
     */
    inline void
    copy_(T* dest, T* src, size_type size) const
    {
        std::memcpy(dest, src, size * sizeof(T));
    }

 public:
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
        for (size_type i = 0; i < init.size(); i++)
            data_[i] = init_data[i];
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
    vec(const vec&& other) noexcept :
        size_(std::exchange(other.size_, 0)),
        capacity_(std::exchange(other.capacity_, 0)),
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

        // we manage a heap-buffer resource
        if (capacity_ < other.size_) {
            // resource in *this cannot be reused
            std::free(data_); // NOLINT(cppcoreguidelines-no-malloc)

            // Preserve invariants
            data_ = static_cast<gsl::owner<T*>>(nullptr);
            size_ = 0;
            capacity_ = 0;

            // Allocate new data
            data_ = alloc_(other.size_);
            size_ = other.size_;
            capacity_ = size_;
        }

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
        std::free(data_); // NOLINT(cppcoreguidelines-no-malloc)

        // Leave the other in a valid state
        size_ = std::exchange(other.size_, 0);
        capacity_ = std::exchange(other.capacity_, 0);
        data_ = std::exchange( // NOLINT(cppcoreguidelines-owning-memory)
            other.data_, nullptr
        );

        return *this;
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
    [[nodiscard]] inline T&
    operator[](size_type pos)
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
    operator[](size_type pos) const
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
    [[nodiscard]] inline const T&
    at(size_type pos) const
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
     * @return size_type The vector size.
     */
    [[nodiscard]] inline size_type
    size() const
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
    capacity() const
    {
        return capacity_;
    }
};

} // namespace ds

#endif // LIBDS_VEC_HPP
