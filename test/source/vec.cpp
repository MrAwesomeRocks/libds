#include "libds/vec.hpp"

#include <catch2/catch_test_macros.hpp>

// NOLINTBEGIN(modernize-loop-convert)

TEST_CASE("Accessors", "[vec]")
{
    ds::vec<unsigned> arr{1, 2, 3, 4, 5};

    REQUIRE(arr.size() == 5);
    REQUIRE(arr.capacity() == 5);

    SECTION("Array-style")
    {
        for (size_t i = 0; i < arr.size(); i++)
            CHECK(arr[i] == i + 1);

        // These are UB but shouldn't fail.
        CHECK_NOTHROW(arr[5]);
        CHECK_NOTHROW(arr[6]);
    }

    SECTION("function-style")
    {
        for (size_t i = 0; i < arr.size(); i++)
            CHECK(arr.at(i) == i + 1);

        CHECK_THROWS_AS(arr.at(5), std::out_of_range);
        CHECK_THROWS_AS(arr.at(6), std::out_of_range);
    }

    SECTION("data accessor")
    {
        unsigned* data = arr.data();
        for (size_t i = 0; i < arr.size(); i++)
            CHECK(data[i] == i + 1);
    }

    SECTION("front/back")
    {
        CHECK(arr.front() == 1);
        CHECK(arr.back() == 5);
    }
}

TEST_CASE("Mutators", "[vec]")
{
    ds::vec<unsigned> arr(5, 0);

    REQUIRE(arr.size() == 5);
    REQUIRE(arr.capacity() == 5);
    for (size_t i = 0; i < arr.size(); i++)
        REQUIRE(arr[i] == 0);

    SECTION("Array-style")
    {
        for (unsigned i = 0; i < arr.size(); i++)
            arr[i] = i;

        for (size_t i = 0; i < arr.size(); i++)
            CHECK(arr[i] == i);
    }

    SECTION("function-style")
    {
        for (unsigned i = 0; i < arr.size(); i++)
            arr.at(i) = 0;

        // These are UB and should fail.
        CHECK_THROWS_AS(arr.at(5) = 0, std::out_of_range);
        CHECK_THROWS_AS(arr.at(6) = 0, std::out_of_range);

        for (size_t i = 0; i < arr.size(); i++)
            CHECK(arr[i] == 0);
    }

    SECTION("data accessor")
    {
        unsigned* data = arr.data();
        for (unsigned i = 0; i < arr.size(); i++)
            data[i] = i;

        for (size_t i = 0; i < arr.size(); i++)
            CHECK(data[i] == i);
    }

    SECTION("front/back")
    {
        arr.front() = 256;
        CHECK(arr.front() == 256);
        CHECK(arr[0] == 256);

        arr.back() = 512;
        CHECK(arr.back() == 512);
        CHECK(arr[arr.size() - 1] == 512);
    }
}

// NOLINTEND(modernize-loop-convert)

TEST_CASE("Iterators", "[vec]")
{
    ds::vec<unsigned> arr(5, 0);

    REQUIRE(arr.size() == 5);
    REQUIRE(arr.capacity() == 5);
    for (size_t i = 0; i < arr.size(); i++) // NOLINT(modernize-loop-convert)
        REQUIRE(arr[i] == 0);

    SECTION("Explicit iterator")
    {
        // NOLINTNEXTLINE(modernize-loop-convert)
        for (auto* it = arr.begin(); it != arr.end(); it++)
            *it = 1;

        // NOLINTNEXTLINE(modernize-loop-convert)
        for (auto* it = arr.begin(); it != arr.end(); it++)
            CHECK(*it == 1);
    }

    SECTION("For-each loop")
    {
        for (auto& val : arr)
            val = 2;

        for (const auto& val : arr)
            CHECK(val == 2);
    }
}

TEST_CASE("Copy things", "[vec]")
{
    ds::vec<unsigned> vec1{1, 2, 3, 4, 5};
    unsigned* data1 = vec1.data();

    REQUIRE(vec1.size() == 5);
    REQUIRE(vec1.capacity() == 5);
    REQUIRE(data1 != nullptr);

    SECTION("Copy constructor")
    {
        ds::vec<unsigned> vec2(vec1);

        CHECK(vec2.size() == 5);
        CHECK(vec2.size() == 5);
        CHECK(vec2.data() != nullptr);
        CHECK(vec2.data() != data1);

        for (size_t i = 0; i < vec2.size(); i++)
            CHECK(vec2[i] == i + 1);
    }

    SECTION("Copy assignment operator")
    {
        ds::vec<unsigned> vec2;
        vec2 = vec1;

        CHECK(vec2.size() == 5);
        CHECK(vec2.size() == 5);
        CHECK(vec2.data() != nullptr);
        CHECK(vec2.data() != data1);

        for (size_t i = 0; i < vec2.size(); i++)
            CHECK(vec2[i] == i + 1);
    }

    SECTION("Copy assignment operator - self assignment")
    {
        vec1 = vec1; // NOLINT(clang-diagnostic-self-assign-overloaded)

        CHECK(vec1.size() == 5);
        CHECK(vec1.size() == 5);
        CHECK(vec1.data() != nullptr);
        CHECK(vec1.data() == data1);

        for (size_t i = 0; i < vec1.size(); i++)
            CHECK(vec1[i] == i + 1);
    }
}

TEST_CASE("Move things", "[vec]")
{
    SECTION("Move constructor")
    {
        ds::vec<unsigned> orig{1, 2, 3, 4, 5};
        auto* data_ptr = orig.data();

        REQUIRE(orig.size() == 5);
        REQUIRE(orig.capacity() == 5);
        REQUIRE(data_ptr != nullptr);

        ds::vec<unsigned> cur(std::move(orig));

        CHECK(cur.size() == 5);
        CHECK(cur.capacity() == 5);
        CHECK(cur.data() == data_ptr);

        // NOLINTBEGIN(clang-analyzer-cplusplus.Move,bugprone-use-after-move)
        CHECK(orig.empty());
        CHECK(orig.capacity() == 0);
        CHECK(orig.data() == nullptr);
        // NOLINTEND(clang-analyzer-cplusplus.Move,bugprone-use-after-move)
    }

    SECTION("Move assignment operator")
    {
        ds::vec<unsigned> orig{1, 2, 3, 4, 5};
        auto* data_ptr = orig.data();

        REQUIRE(orig.size() == 5);
        REQUIRE(orig.capacity() == 5);
        REQUIRE(data_ptr != nullptr);

        ds::vec<unsigned> cur;
        cur = std::move(orig);

        CHECK(cur.size() == 5);
        CHECK(cur.capacity() == 5);
        CHECK(cur.data() == data_ptr);

        // NOLINTBEGIN(clang-analyzer-cplusplus.Move,bugprone-use-after-move)
        CHECK(orig.empty());
        CHECK(orig.capacity() == 0);
        CHECK(orig.data() == nullptr);
        // NOLINTEND(clang-analyzer-cplusplus.Move,bugprone-use-after-move)
    }

    SECTION("Move assignment operator - self assignment")
    {
        ds::vec<unsigned> orig{1, 2, 3, 4, 5};
        auto* data_ptr = orig.data();

        REQUIRE(orig.size() == 5);
        REQUIRE(orig.capacity() == 5);
        REQUIRE(data_ptr != nullptr);

        // NOLINTNEXTLINE(clang-diagnostic-self-move)
        orig = std::move(orig);

        CHECK(orig.size() == 5);
        CHECK(orig.capacity() == 5);
        CHECK(orig.data() == data_ptr);
    }
}

TEST_CASE("Deconstructor", "[vec]")
{
    class dec_test {
        bool* flag_;

     public:
        explicit dec_test(bool* flag) : flag_(flag) { *flag = false; }

        dec_test(const dec_test& other) = default;
        dec_test(dec_test&& other) = delete;

        dec_test& operator=(const dec_test& other) = default;
        dec_test& operator=(dec_test&& other) = delete;

        ~dec_test() noexcept { *flag_ = true; }
    };

    ds::vec deconstructed(3, /*elem=*/false);
    auto* arr = new ds::vec{
        dec_test(deconstructed.data()),
        dec_test(deconstructed.data() + 1),
        dec_test(deconstructed.data() + 2),
    };

    REQUIRE(arr->size() == 3);
    REQUIRE(arr->capacity() == 3);
    for (bool& flag : deconstructed)
        flag = false;

    delete arr;

    for (const bool& flag : deconstructed)
        CHECK(flag);
}

TEST_CASE("Manually changing capacity", "[vec]")
{
    ds::vec<unsigned> vec1(5, 1);

    REQUIRE(vec1.capacity() == 5);
    REQUIRE(vec1.size() == 5);

    SECTION("Reserve")
    {
        vec1.reserve(1);

        CHECK(vec1.capacity() == 5);
        CHECK(vec1.size() == 5);
        for (const auto& val : vec1)
            CHECK(val == 1);

        vec1.reserve(10);

        CHECK(vec1.capacity() == 10);
        CHECK(vec1.size() == 5);
        for (const auto& val : vec1)
            CHECK(val == 1);
    }

    SECTION("Shrink")
    {
        vec1.shrink_to_fit();

        CHECK(vec1.capacity() == 5);
        CHECK(vec1.size() == 5);
        for (const auto& val : vec1)
            CHECK(val == 1);

        vec1.shrink_to_fit();

        CHECK(vec1.capacity() == 5);
        CHECK(vec1.size() == 5);
        for (const auto& val : vec1)
            CHECK(val == 1);
    }
}

TEST_CASE("Clearing", "[vec]")
{
    ds::vec arr{1, 2, 3};

    REQUIRE(arr.size() == 3);
    REQUIRE(arr.capacity() == 3);

    arr.clear();

    CHECK(arr.empty());
    CHECK(arr.capacity() == 3);
    CHECK(arr.begin() == arr.end());
}

TEST_CASE("Insertion", "[vec]")
{
    ds::vec<unsigned> arr{1, 2, 3};

    REQUIRE(arr.size() == 3);
    REQUIRE(arr.capacity() == 3);

    SECTION("Simple")
    {
        arr.insert(0, 0);

        CHECK(arr == ds::vec<unsigned>{0, 1, 2, 3});
        CHECK(arr.capacity() == 4);

        arr.insert(arr.size(), 4);

        CHECK(arr == ds::vec<unsigned>{0, 1, 2, 3, 4});
        CHECK(arr.capacity() == 6);

        arr.insert(2, 555);

        CHECK(arr == ds::vec<unsigned>{0, 1, 555, 2, 3, 4});
        CHECK(arr.capacity() == 6);
    }

    SECTION("Copied")
    {
        arr = {1, 2, 3};

        REQUIRE(arr.size() == 3);

        // Insert 4 fives at position 2
        arr.insert(2, 4, 5);

        CHECK(arr == ds::vec<unsigned>{1, 2, 5, 5, 5, 5, 3});
        CHECK(arr.capacity() == 9);
    }

    SECTION("Init list")
    {
        arr = {1, 2, 3};

        REQUIRE(arr.size() == 3);

        arr.insert(2, {5, 6, 7, 0});

        CHECK(arr == ds::vec<unsigned>{1, 2, 5, 6, 7, 0, 3});
        CHECK(arr.capacity() == 9);
    }
}

TEST_CASE("Equality operators", "[vec]")
{
    // NOLINTBEGIN(readability-container-size-empty)
    SECTION("Empty vector")
    {
        CHECK_FALSE(ds::vec{1, 2, 3} == ds::vec<int>(0));
        CHECK(ds::vec{1, 2, 3} != ds::vec<int>(0));

        CHECK(ds::vec<int>(0) == ds::vec<int>(0));
        CHECK_FALSE(ds::vec<int>(0) != ds::vec<int>(0));
    }
    // NOLINTEND(readability-container-size-empty)

    SECTION("Different sizes")
    {
        CHECK_FALSE(ds::vec{1, 2, 3} == ds::vec{1});
        CHECK(ds::vec{1, 2, 3} != ds::vec{1});
    }

    SECTION("Same size different elements")
    {
        CHECK_FALSE(ds::vec{1, 2, 3} == ds::vec{1, 2, 4});
        CHECK(ds::vec{1, 2, 3} != ds::vec{1, 2, 4});

        CHECK_FALSE(ds::vec{1, 2, 3} == ds::vec{2, 2, 3});
        CHECK(ds::vec{1, 2, 3} != ds::vec{2, 2, 3});
    }

    SECTION("Identical vectors")
    {
        ds::vec arr{1, 2, 3};

        CHECK(arr == ds::vec{1, 2, 3});
        CHECK_FALSE(arr != ds::vec{1, 2, 3});

        CHECK(arr == arr);
        CHECK_FALSE(arr != arr);
    }
}
