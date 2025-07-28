#pragma once

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <limits>
#include <numeric>
#include <ranges>
#include <stdexcept>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "../core/common.hpp"

namespace NNS {

template <Real T> static T median_sorted(const std::vector<T> &v) {
    const std::size_t n = v.size();
    if (n == 0)
        return std::numeric_limits<T>::quiet_NaN();

    if (n & 1)
        return v[n / 2];

    return (v[n / 2 - 1] + v[n / 2]) / static_cast<T>(2);
}

// =====================
// Nondiscrete Mode (v2)
// =====================

template <RealRange R, Real N = std::ranges::range_value_t<R>>
std::vector<double> mode_nondiscrete_multi_v2(const R &r) {
    using T = std::ranges::range_value_t<R>;

    // Filter out NaNs
    std::vector<T> data;
    data.reserve(std::ranges::size(r));
    for (const auto &v : r)
        if (!std::isnan(v))
            data.push_back(v);

    const std::size_t n = data.size();
    if (n == 0)
        return {};
    if (n <= 3) {
        std::sort(data.begin(), data.end());
        return {static_cast<double>(median_sorted(data))};
    }

    size_t num_bins = 128;
    BinsV2 bins = bin_data_v2(data, num_bins);
    int max_count = *std::max_element(bins.counts.begin(), bins.counts.end());

    std::vector<std::size_t> mode_bins;
    for (std::size_t i = 0; i < bins.counts.size(); ++i)
        if (bins.counts[i] == max_count)
            mode_bins.push_back(i);

    // Multiple mode bins: return their centers
    if (mode_bins.size() > 1) {
        std::vector<double> modes;
        modes.reserve(mode_bins.size());
        for (std::size_t i : mode_bins) {
            double center = bins.origin + (i + 0.5) * bins.bin_width;
            modes.push_back(center);
        }
        return modes;
    }

    // Refinement: weighted average of bin and neighbors
    const std::size_t z_c = mode_bins.front();
    const std::size_t left = (z_c == 0) ? 0 : z_c - 1;
    const std::size_t right =
        (z_c + 1 >= bins.counts.size()) ? bins.counts.size() - 1 : z_c + 1;

    double num = 0.0;
    double den = 0.0;
    for (std::size_t i = left; i <= right; ++i) {
        double center = bins.origin + (i + 0.5) * bins.bin_width;
        num += center * bins.counts[i];
        den += bins.counts[i];
    }

    return {num / den};
}

template <RealRange R, Real N = std::ranges::range_value_t<R>>
double mode_nondiscrete_v2(const R &r) {
    std::vector<double> modes = mode_nondiscrete_multi_v2(r);
    if (modes.empty())
        throw std::runtime_error("No mode found (unexpected)");
    return mean(modes);
}

// =====================
// Discrete Mode
// =====================

template <typename R, typename T = std::ranges::range_value_t<R>>
    requires std::integral<T>
std::vector<T> mode_discrete_multi(const R &r) {
    std::unordered_map<T, int> freq;
    int max_count = 0;

    for (const auto &v : r) {
        int count = ++freq[v];
        max_count = std::max(max_count, count);
    }

    if (max_count == 0)
        return {};

    std::vector<T> modes;
    for (const auto &[value, count] : freq) {
        if (count == max_count)
            modes.push_back(value);
    }

    std::sort(modes.begin(), modes.end());
    return modes;
}

template <typename R, typename T = std::ranges::range_value_t<R>>
    requires std::integral<T>
T mode_discrete(const R &r) {
    std::vector<T> modes = mode_discrete_multi(r);
    if (modes.empty())
        throw std::runtime_error("No mode found (unexpected)");
    return modes[modes.size() / 2]; // median of modes
}

enum class Method { MinMax, RiskNeutral };

enum class RiskNeutralType { Terminal, Discounted };

template <RealRange Range, Real T>
auto rescaleMinMax(const Range &x, T a, T b) -> std::vector<T> {
    auto [minIt, maxIt] = std::ranges::minmax_element(x);
    if (*minIt == *maxIt) {
        throw std::runtime_error("Cannot rescale: max(x) equals min(x)");
    }

    T minX = *minIt;
    T maxX = *maxIt;

    std::vector<T> output;
    output.reserve(std::ranges::size(x));

    for (const auto &val : x) {
        output.push_back(a + (b - a) * (val - minX) / (maxX - minX));
    }

    return output;
}

template <RealRange Range, Real T>
auto rescaleRiskNeutral(const Range &x, T S0, T r, T T_maturity,
                        RiskNeutralType type) -> std::vector<T> {
    if (S0 <= 0) {
        throw std::runtime_error("S0 must be positive for risk-neutral method");
    }

    T sum = std::accumulate(std::ranges::begin(x), std::ranges::end(x), T{0});
    T mean = sum / static_cast<T>(std::ranges::size(x));
    if (mean == T{0}) {
        throw std::runtime_error(
            "Mean of x is zero, cannot scale risk-neutrally");
    }

    T targetMean = (type == RiskNeutralType::Terminal)
                       ? S0 * std::exp(r * T_maturity)
                       : S0;

    T theta = std::log(targetMean / mean);

    std::vector<T> output;
    output.reserve(std::ranges::size(x));
    for (const auto &val : x) {
        output.push_back(val * std::exp(theta));
    }

    return output;
}

template <RealRange Range, Real T>
auto rescale(const Range &x, T a, T b, Method method = Method::MinMax,
                      std::optional<T> T_maturity = std::nullopt,
                      RiskNeutralType type = RiskNeutralType::Terminal)
    -> std::vector<T> {
    if (std::ranges::empty(x)) {
        throw std::runtime_error("Input range is empty");
    }

    switch (method) {
    case Method::MinMax:
        return rescaleMinMax(x, a, b);
    case Method::RiskNeutral:
        if (!T_maturity.has_value()) {
            throw std::runtime_error("T (time to maturity) must be provided "
                                     "for risk-neutral method");
        }
        return rescaleRiskNeutral(x, a, b, T_maturity.value(), type);
    default:
        throw std::runtime_error("Invalid method specified");
    }
}
} // namespace NNS
