#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <iostream>
#include <stdexcept>
#include <vector>

#include <NNS/algorithms/central_tendencies.hpp>

using Catch::Approx;
using namespace NNS;

TEST_CASE("NNS::mode_nondiscrete_multi_v2 - basic case") {
    std::vector<double> v{1.0, 2.0, 3.0, 3.0};
    auto modes = NNS::mode_nondiscrete_multi_v2(v);
    REQUIRE_FALSE(modes.empty());
    for (auto m : modes) {
        std::cout << "m: " << m << std::endl;
    }
    REQUIRE(modes[0] == Approx(3.0).margin(0.01));
}

TEST_CASE("NNS::mode_nondiscrete_v2 - mean of modes") {
    std::vector<double> v{1.0, 2.0, 2.0, 3.0, 3.0};
    auto m = NNS::mode_nondiscrete_v2(v);
    std::cout << "m: " << m << std::endl;
    REQUIRE(m == Approx(2.5));
}

TEST_CASE("NNS::mode_nondiscrete_v2 - small set uses median fallback") {
    std::vector<double> v{1.0, 3.0};
    auto m = NNS::mode_nondiscrete_v2(v);
    REQUIRE(m == Approx(2.0));
}

TEST_CASE("NNS::mode_discrete_multi - unique mode") {
    std::vector<int> v{1, 2, 2, 3, 3, 3, 4};
    auto modes = NNS::mode_discrete_multi(v);
    REQUIRE(modes.size() == 1);
    REQUIRE(modes[0] == 3);
}

TEST_CASE("NNS::mode_discrete_multi - multiple modes") {
    std::vector<int> v{1, 1, 2, 2, 3, 3};
    auto modes = NNS::mode_discrete_multi(v);
    REQUIRE(modes.size() == 3);
    REQUIRE((modes == std::vector<int>{1, 2, 3}));
}

TEST_CASE("NNS::mode_discrete - median of multiple modes") {
    std::vector<int> v{5, 5, 3, 3, 4, 4};
    auto m = NNS::mode_discrete(v);
    REQUIRE(m == 4);
}

TEST_CASE("NNS::mode_discrete - throws on empty") {
    std::vector<int> v{};
    REQUIRE_THROWS_AS(NNS::mode_discrete(v), std::runtime_error);
}

TEST_CASE("NNS::mode_nondiscrete_v2 - throws on empty") {
    std::vector<double> v{};
    REQUIRE_THROWS_AS(NNS::mode_nondiscrete_v2(v), std::runtime_error);
}

TEST_CASE("gravity_v1 - small input returns median") {
    std::vector<double> x1{10.0};
    std::vector<double> x2{10.0, 20.0};
    std::vector<double> x3{5.0, 10.0, 15.0};

    REQUIRE(NNS::gravity_v1(x1) == Approx(10.0));
    REQUIRE(NNS::gravity_v1(x2) == Approx(15.0));
    REQUIRE(NNS::gravity_v1(x3) == Approx(10.0));
}

TEST_CASE("gravity_v1 - all elements identical") {
    std::vector<double> x(100, 42.0);
    REQUIRE(NNS::gravity_v1(x) == Approx(42.0));
}

TEST_CASE("gravity_v1 - simple normal data") {
    std::vector<double> x{1.0, 2.0, 2.0, 3.0, 4.0};
    double g = NNS::gravity_v1(x);
    REQUIRE(g == Approx(2.1).margin(0.01));
}

TEST_CASE("gravity_v1 - discrete output") {
    std::vector<double> x{1.0, 2.0, 2.0, 3.0, 4.0};
    double g = NNS::gravity_v1(x, true);
    REQUIRE((g == 2.0 || g == 3.0));
}

TEST_CASE("gravity_v1 - skewed distribution") {
    std::vector<double> x{1.0, 2.0, 3.0, 100.0};
    double g = NNS::gravity_v1(x);
    REQUIRE(g < 30.0);
}

TEST_CASE("gravity_v1 - known value stability") {
    std::vector<double> x;
    for (int i = 0; i < 100; ++i)
        x.push_back(static_cast<double>(i));

    double g = NNS::gravity_v1(x);
    REQUIRE(g == Approx(49.5).margin(0.5));
}

TEST_CASE("gravity_v1 - throws on empty input") {
    std::vector<double> empty;
    REQUIRE_THROWS_AS(NNS::gravity_v1(empty), std::invalid_argument);
}

TEST_CASE("LegacyNNSRescale - MinMax scaling") {
    std::vector<double> x = {1.0, 2.0, 3.0};
    auto rescaled = rescale(x, 5.0, 10.0, Method::MinMax);

    REQUIRE(rescaled.size() == x.size());
    REQUIRE(rescaled[0] == Approx(5.0));
    REQUIRE(rescaled[1] == Approx(7.5));
    REQUIRE(rescaled[2] == Approx(10.0));
}

TEST_CASE("LegacyNNSRescale - RiskNeutral Terminal") {
    std::vector<double> x = {98.0, 100.0, 102.0};
    double S0 = 100.0;
    double r = 0.05;
    double T = 1.0;
    auto rescaled =
        rescale(x, S0, r, Method::RiskNeutral, std::optional<double>{T},
                RiskNeutralType::Terminal);

    double expected_mean = S0 * std::exp(r * T);

    double actual_mean =
        std::accumulate(rescaled.begin(), rescaled.end(), 0.0) /
        rescaled.size();
    REQUIRE(actual_mean == Approx(expected_mean).margin(0.01));
}

TEST_CASE("LegacyNNSRescale - RiskNeutral Discounted") {
    std::vector<double> x = {98.0, 100.0, 102.0};
    double S0 = 100.0;
    double r = 0.05;
    double T = 1.0;

    auto rescaled =
        rescale(x, S0, r, Method::RiskNeutral, std::optional<double>{T},
                RiskNeutralType::Discounted);

    double expected_mean = S0;

    double actual_mean =
        std::accumulate(rescaled.begin(), rescaled.end(), 0.0) /
        rescaled.size();
    REQUIRE(actual_mean == Approx(expected_mean).margin(0.01));
}

TEST_CASE("LegacyNNSRescale - Exception on empty input") {
    std::vector<double> x;
    REQUIRE_THROWS_AS(rescale(x, 5.0, 10.0), std::runtime_error);
}

TEST_CASE("LegacyNNSRescale - Exception on riskneutral without T") {
    std::vector<double> x = {1.0, 2.0, 3.0};
    REQUIRE_THROWS_AS(rescale(x, 100.0, 0.05, Method::RiskNeutral),
                      std::runtime_error);
}

TEST_CASE("LegacyNNSRescale - Exception on constant input for MinMax") {
    std::vector<double> x = {42.0, 42.0, 42.0};
    REQUIRE_THROWS_AS(rescale(x, 5.0, 10.0), std::runtime_error);
}