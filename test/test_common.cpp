#include <vector>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "rinside.h"

#include <NNS/core/common.hpp>

using Catch::Approx;

TEST_CASE("test mean - basic")
{
    std::vector<double> v{4, 4, 4, 4};
    double m = NNS::mean(v);
    REQUIRE(m == Approx(4.0));
}

TEST_CASE("bin_data: basic binning with no NaNs", "[binning]")
{
    std::vector<double> data = {1.1, 2.3, 2.7, 3.8, 0.5}; // width = 1.0
    auto result = NNS::bin_data(data, 1.0, 0.0);

    REQUIRE(result.origin == Approx(0.0));
    REQUIRE(result.width == Approx(1.0));
    REQUIRE(result.missing == 0);
    REQUIRE_FALSE(result.last_bin_is_missing);

    REQUIRE(result.counts.size() == 4);
    REQUIRE(result.counts[0] == 1);
    REQUIRE(result.counts[1] == 1);
    REQUIRE(result.counts[2] == 2);
    REQUIRE(result.counts[3] == 1);
}

TEST_CASE("bin_data: handles NaNs properly", "[binning][NaN]")
{
    std::vector<double> data = {1.0, std::numeric_limits<double>::quiet_NaN(), 2.5};
    auto result = NNS::bin_data(data, 1.0, 0.0);

    REQUIRE(result.missing == 1);
    REQUIRE_FALSE(result.last_bin_is_missing);

    REQUIRE(result.counts.size() == 3);
    REQUIRE(result.counts[0] == 0);
    REQUIRE(result.counts[1] == 1);
    REQUIRE(result.counts[2] == 1);
}

TEST_CASE("bin_data: missing_last appends count", "[binning][missing_last]")
{
    std::vector<double> data = {
        0.5,
        std::numeric_limits<double>::quiet_NaN(),
        1.5,
        std::numeric_limits<double>::quiet_NaN()};

    auto result = NNS::bin_data(data, 1.0, 0.0, true);

    REQUIRE(result.missing == 2);
    REQUIRE(result.last_bin_is_missing);

    // bin 0: 0.5, bin 1: 1.5
    REQUIRE(result.counts.size() == 3);
    REQUIRE(result.counts[0] == 1);
    REQUIRE(result.counts[1] == 1);
    REQUIRE(result.counts[2] == 2);
}