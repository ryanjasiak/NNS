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

} // namespace NNS
