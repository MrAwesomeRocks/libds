#include "libds/libds.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Name is libds", "[library]") {
    REQUIRE(name() == "libds");
}
