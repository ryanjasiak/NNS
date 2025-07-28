#pragma once

#include <concepts>
#include <numeric>
#include <ranges>

namespace NNS {

// ──────────────────────────────────────────────────────────────
//  Concepts
// ──────────────────────────────────────────────────────────────

template <typename T>
concept Real = std::floating_point<T>;

template <typename R>
concept RealRange = std::ranges::sized_range<R> &&
                    requires(R r, std::ranges::range_size_t<R> i) {
                        {
                            r[i]
                        } -> std::same_as<std::ranges::range_reference_t<R>>;
                    } && Real<std::ranges::range_value_t<R>>;

// ──────────────────────────────────────────────────────────────
//  Algorithms
// ──────────────────────────────────────────────────────────────

template <RealRange R> inline double mean(const R &r) {
    if (std::ranges::empty(r))
        return 0.0;
    double sum =
        std::accumulate(std::ranges::begin(r), std::ranges::end(r), 0.0);
    return sum / static_cast<double>(std::ranges::size(r));
}

struct binned_result {
    std::vector<int> counts;
    double origin;
    double width;
    std::size_t missing;
    bool last_bin_is_missing;
};
template <RealRange R>
binned_result bin_data(const R &data, double width, double origin = 0.0,
                       bool missing_last = false) {
    if (width <= 0.0) {
        throw std::invalid_argument("width must be positive");
    }

    std::vector<int> counts;
    std::size_t nmissing = 0;

    for (const auto &val : data) {
        if (std::isnan(val)) {
            ++nmissing;
            continue;
        }

        if (val < origin) {
            continue;
        }

        auto bin = static_cast<std::size_t>((val - origin) / width);

        if (bin >= counts.size()) {
            counts.resize(bin + 1);
        }

        ++counts[bin];
    }

    if (missing_last) {
        counts.push_back(static_cast<int>(nmissing));
    }

    return binned_result{.counts = std::move(counts),
                         .origin = origin,
                         .width = width,
                         .missing = nmissing,
                         .last_bin_is_missing = missing_last};
}

struct BinsV2 {
    std::vector<int> counts;
    double bin_width;
    double origin;
};
template <RealRange R, Real N = std::ranges::range_value_t<R>>
BinsV2 bin_data_v2(const R &r, int num_bins = 128) {
    if (std::ranges::empty(r) || num_bins <= 0) {
        throw std::invalid_argument(
            "Data must not be empty and num_bins must be positive.");
    }

    auto [_min, _max] =
        std::minmax_element(std::ranges::begin(r), std::ranges::end(r));
    double min = static_cast<double>(*_min);
    double max = static_cast<double>(*_max);

    double bin_width = (max - min) / static_cast<double>(num_bins);

    if (min == max) {
        std::vector<int> bins(num_bins, 0);
        bins[0] = static_cast<int>(std::ranges::size(r));
        return BinsV2{
            .counts = std::move(bins), .bin_width = bin_width, .origin = min};
    }

    std::vector<int> bins(num_bins, 0);
    for (const auto &elem : r) {
        double value = static_cast<double>(elem);
        int bin_index = static_cast<int>((value - min) / bin_width);
        if (bin_index == num_bins)
            bin_index--; // Include max in last bin
        bins[bin_index]++;
    }

    return BinsV2{
        .counts = std::move(bins), .bin_width = bin_width, .origin = min};
}
template <RealRange R> double gravity_v1(const R &x, bool discrete = false) {
    using T = std::ranges::range_value_t<R>;
    const std::size_t l = std::ranges::size(x);

    if (l == 0)
        throw std::invalid_argument("Input range must be non-empty");

    std::vector<T> x_s(x.begin(), x.end());
    std::ranges::sort(x_s);

    if (l <= 3) {
        if (l == 1)
            return x_s[0];
        if (l == 2)
            return (x_s[0] + x_s[1]) / 2.0;
        return x_s[1]; // Already sorted; median of 3 elements
    }

    if (std::ranges::all_of(x_s, [&](T v) { return v == x_s[0]; }))
        return static_cast<double>(x_s[0]);

    const double range = static_cast<double>(x_s.back() - x_s.front());
    if (range == 0.0)
        return static_cast<double>(x_s[0]);

    // Approximate quantiles
    auto q_interp = [&](double frac) -> double {
        double idx = frac * static_cast<double>(l);
        auto i = static_cast<std::size_t>(idx);
        double f = idx - i;

        if (i == 0)
            return x_s[0];
        if (i >= l - 1)
            return x_s[l - 1];
        return (1 - f) * x_s[i - 1] + f * x_s[i];
    };

    const double q1 = q_interp(0.25);
    const double q2 = q_interp(0.50); // median
    const double q3 = q_interp(0.75);

    // Histogram-based estimate
    BinsV2 bins = bin_data_v2(x_s, 128);
    const auto &counts = bins.counts;

    auto max_count = *std::max_element(counts.begin(), counts.end());
    std::vector<std::size_t> mode_bins;
    for (std::size_t i = 0; i < counts.size(); ++i) {
        if (counts[i] == max_count)
            mode_bins.push_back(i);
    }

    std::vector<std::size_t> z_ind;
    if (mode_bins.size() > 1) {
        z_ind.resize(counts.size());
        std::iota(z_ind.begin(), z_ind.end(), 0);
    } else {
        std::size_t center = mode_bins[0];
        std::size_t left = (center == 0) ? 0 : center - 1;
        std::size_t right = std::min(counts.size() - 1, center + 1);
        for (std::size_t i = left; i <= right; ++i)
            z_ind.push_back(i);
    }

    // Bin centers
    std::vector<double> z_names;
    z_names.reserve(counts.size());
    for (std::size_t i = 0; i < counts.size(); ++i) {
        z_names.push_back(bins.origin + i * bins.bin_width);
    }

    double weighted_sum = 0.0;
    double weight_total = 0.0;
    for (std::size_t i : z_ind) {
        weighted_sum += z_names[i] * counts[i];
        weight_total += counts[i];
    }

    double m = weighted_sum / weight_total;
    double mu = mean(x_s);
    double quartile_avg = (q1 + q2 + q3) / 3.0;

    double result = (q2 + m + mu + quartile_avg) / 4.0;
    if (std::isnan(result))
        result = q2;

    if (discrete)
        return (result - std::floor(result) < 0.5) ? std::floor(result)
                                                   : std::ceil(result);
    else
        return result;
}

} // namespace NNS
