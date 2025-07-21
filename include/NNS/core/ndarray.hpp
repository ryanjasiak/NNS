#include <mdspan>

namespace NNS
{

    enum class memory_location
    {
        host,
        device
    };

    template <class T, std::size_t Rank>
    class ndarray
    {
    public:
        using mdspan_t = std::mdspan<T, std::dextents<std::size_t, Rank>>;

        constexpr ndarray() = default;
        constexpr ndarray(T *ptr,
                          std::array<std::size_t, Rank> shape,
                          memory_location loc = memory_location::host)
            : span_(ptr, shape), loc_(loc) {}

        // Forward accessors
        constexpr auto extent(std::size_t d) const noexcept { return span_.extent(d); }
        constexpr T *data() noexcept { return span_.data_handle(); }
        constexpr const T *data() const noexcept { return span_.data_handle(); }
        constexpr memory_location location() const noexcept { return loc_; }

        template <class... Idx>
        constexpr T &operator()(Idx... i) noexcept { return span_(i...); }
        template <class... Idx>
        constexpr const T &operator()(Idx... i) const noexcept { return span_(i...); }

// mdspan supports slicing via std::submdspan (P0009r16); you can alias it:
#ifdef __cpp_lib_submdspan
        template <class... SliceSpec>
        constexpr auto subview(SliceSpec... spec) const
        {
            return std::submdspan(span_, spec...);
        }
#else
// fallback implementation or static_assert
#endif

    private:
        mdspan_t span_;
        memory_location loc_{memory_location::host};
    };

} // namespace NNS
