#ifndef RCPP_SAFE_INCLUDE_H
#define RCPP_SAFE_INCLUDE_H

// ── Silence unused-parameter warnings for third-party headers ────────────────
#if defined(__clang__)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wunused-parameter"
#elif defined(__GNUC__) || defined(__GNUG__)
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include <RInside.h>
#include <sstream>

#if defined(__clang__)
  #pragma clang diagnostic pop
#elif defined(__GNUC__) || defined(__GNUG__)
  #pragma GCC diagnostic pop
#endif


namespace NNS_test::rinside {

    /// RInside singleton
    inline RInside& r() {
        static RInside instance{}; 
        return instance;
    }

    inline void require_pkg(const std::string& pkg) {
        
        std::ostringstream cmd;
        cmd << "if (!requireNamespace('" << pkg
            << "', quietly = TRUE)) install.packages('"
            << pkg << "', repos = 'https://cloud.r-project.org'); "
               "library(" << pkg << ')';
        r().parseEvalQ(cmd.str());
    }

    #define RCPP_SAFE_AUTOLOAD(pkg)                                     \
        namespace {                                                     \
            const int _rcpp_safe_autoload_##pkg = ([] {                 \
                ::NNS_test::rinside::require_pkg(#pkg);                         \
                return 0;                                               \
            })();                                                       \
        }
} // namespace r_inside_global

RCPP_SAFE_AUTOLOAD(LegacyNNS)

#endif // RCPP_SAFE_INCLUDE_H
