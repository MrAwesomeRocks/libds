#include "libds/vec.hpp"

#include <catch2/catch_test_macros.hpp>

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

        // These are UB but shouldn't fail.
        CHECK_NOTHROW(arr[5] = 5);
        CHECK_NOTHROW(arr[6] = 6);

        for (size_t i = 0; i < 6; i++)
            CHECK(arr[i] == i);
    }

    SECTION("function-style")
    {
        for (unsigned i = 0; i < arr.size(); i++)
            arr.at(i) = 0;

        // These are UB but shouldn't fail.
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

        for (size_t i = 0; i < 6; i++)
            CHECK(data[i] == i);
    }
}
