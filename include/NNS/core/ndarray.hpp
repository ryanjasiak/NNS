
#pragma once

#include <mdspan>
#include <array>
#include <concepts>
#include <type_traits>
#include <cstddef>

namespace NNS
{

    // -----------------------------------------------------------------------------
    //  Basic memory‑location tag
    // -----------------------------------------------------------------------------
    enum class memory_location
    {
        host,
        device // extend (managed, pinned, etc.) as your back‑ends grow
    };

    // -----------------------------------------------------------------------------
    //  Pointer‑location traits (specialise for your GPU pointer wrappers)
    // -----------------------------------------------------------------------------
    template <class Ptr>
    struct pointer_location
    {
        static constexpr memory_location value = memory_location::host;
    };
    template <
        class T,
        std::size_t Rank,
        class Layout = std::layout_right,
        class Accessor = std::default_accessor<T>>
    class ndarray_view
    {
    public:
        using mdspan_t = std::mdspan<T,
                                     std::dextents<std::size_t, Rank>,
                                     Layout,
                                     Accessor>;

        // ---------------------------------------------------------------------
        //  Constructors
        // ---------------------------------------------------------------------
        constexpr ndarray_view() = default;

        // 1. Explicit location provided (matches original API)
        constexpr ndarray_view(T *ptr,
                               const std::array<std::size_t, Rank> &shape,
                               memory_location loc = memory_location::host)
            : span_(ptr, shape), loc_(loc) {}

        // 2. Location deduced from pointer‑traits (works with fancy ptrs too)
        template <class Ptr>
            requires std::is_convertible_v<Ptr, T *>
        constexpr ndarray_view(Ptr ptr,
                               const std::array<std::size_t, Rank> &shape)
            : span_(ptr, shape),
              loc_(pointer_location<std::remove_cvref_t<Ptr>>::value)
        {
        }

        // ---------------------------------------------------------------------
        //  Forwarded mdspan interface (subset)
        // ---------------------------------------------------------------------
        constexpr auto extent(std::size_t d) const noexcept { return span_.extent(d); }

        constexpr T *data() noexcept { return span_.data_handle(); }
        constexpr const T *data() const noexcept { return span_.data_handle(); }

        constexpr memory_location location() const noexcept { return loc_; }

        template <typename... Idx>
        constexpr T &operator[](Idx... i) noexcept { return span_[i...]; }
        template <class... Idx>
        constexpr const T &operator()(Idx... i) const noexcept { return span_[i...]; }

    private:
        mdspan_t span_;
        memory_location loc_{memory_location::host};
    };

    template <
        class T,
        std::size_t Rank,
        class Layout = std::layout_right,
        class Allocator = std::allocator<T>>
    class ndarray
    {
        using extents_t = std::dextents<std::size_t, Rank>;
        using mdspan_t = std::mdspan<T, extents_t, Layout>;

    public:
        using value_type = T;

        // ---------- construction -------------------------------------------------
        explicit ndarray(const std::array<std::size_t, Rank> &shape,
                         memory_location loc = memory_location::host,
                         const Allocator &alloc = Allocator{})
            : shape_(shape),
              size_(std::accumulate(shape.begin(), shape.end(),
                                    std::size_t{1}, std::multiplies<>())),
              storage_(detail::allocate<T>(size_, loc, alloc), deleter{loc, alloc}),
              span_(storage_.get(), shape_),
              loc_(loc) {}

        // move‑only
        ndarray(ndarray &&) noexcept = default;
        ndarray &operator=(ndarray &&) noexcept = default;
        ndarray(const ndarray &) = delete;
        ndarray &operator=(const ndarray &) = delete;

        // ---------- mdspan‑compatible API ----------------------------------------
        constexpr auto extent(std::size_t d) const noexcept { return span_.extent(d); }
        constexpr T *data() noexcept { return span_.data_handle(); }
        constexpr const T *data() const noexcept { return span_.data_handle(); }
        constexpr std::size_t size() const noexcept { return size_; }

        template <class... Idx>
        T &operator[](Idx... i) noexcept { return span_[i...]; }
        template <class... Idx>
        const T &operator()(Idx... i) const noexcept { return span_[i...]; }

        constexpr memory_location location() const noexcept { return loc_; }

        // ---------- implicit view conversion ------------------------------------
        operator ndarray_view<T, Rank, Layout>() const noexcept
        {
            return ndarray_view<T, Rank, Layout>(data(), shape_, loc_);
        }

    private:
        struct deleter
        {
            memory_location loc;
            Allocator alloc;
            void operator()(T *p) const noexcept { detail::deallocate(p, loc, alloc); }
        };

        std::array<std::size_t, Rank> shape_;
        std::size_t size_{0};
        std::unique_ptr<T[], deleter> storage_;
        mdspan_t span_;
        memory_location loc_;
    };

} // namespace NNS