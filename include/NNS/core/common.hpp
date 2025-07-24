#pragma once

#include <concepts>
#include <numeric>
#include <ranges>

namespace NNS
{

    // ──────────────────────────────────────────────────────────────
    //  Concepts
    // ──────────────────────────────────────────────────────────────

    template <typename T>
    concept Real = std::floating_point<T>;

    template <typename R>
    concept RealRange = std::ranges::sized_range<R> && Real<std::ranges::range_value_t<R>>;

    // ──────────────────────────────────────────────────────────────
    //  Algorithms
    // ──────────────────────────────────────────────────────────────

    template <RealRange R>
    double mean(const R &r)
    {
        if (std::ranges::empty(r))
            return 0.0;
        double sum = std::accumulate(std::ranges::begin(r),
                                     std::ranges::end(r),
                                     0.0);
        return sum / static_cast<double>(std::ranges::size(r));
    }

    struct binned_result
    {
        std::vector<int> counts;
        double origin;
        double width;
        std::size_t missing;
        bool last_bin_is_missing;
    };
    template <RealRange R>
    binned_result bin_data(const R &data, double width, double origin = 0.0, bool missing_last = false)
    {
        if (width <= 0.0)
        {
            throw std::invalid_argument("width must be positive");
        }

        std::vector<int> counts;
        std::size_t nmissing = 0;

        for (const auto &val : data)
        {
            if (std::isnan(val))
            {
                ++nmissing;
                continue;
            }

            if (val < origin)
            {
                continue;
            }

            auto bin = static_cast<std::size_t>((val - origin) / width);

            if (bin >= counts.size())
            {
                counts.resize(bin + 1);
            }

            ++counts[bin];
        }

        if (missing_last)
        {
            counts.push_back(static_cast<int>(nmissing));
        }

        return binned_result{
            .counts = std::move(counts),
            .origin = origin,
            .width = width,
            .missing = nmissing,
            .last_bin_is_missing = missing_last};
    }

} // namespace NNS
