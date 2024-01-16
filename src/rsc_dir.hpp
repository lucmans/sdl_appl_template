#pragma once

#include <filesystem>
#include <string_view>


class RscDir {
    public:
        // static only class, so disallow construction
        RscDir() = delete;

        // throws exception if not yet set to a valid path
        // don't forget to call `.string()` on the final path for Windows compatibility
        static const std::filesystem::path& get();

        // throws an exception if new path is not valid
        // strong exception guarantee
        static void set(std::filesystem::path new_path);

        static bool is_valid() noexcept;


        /* config */
        static constexpr bool make_path_absolute = false;
        static constexpr std::string_view hash_in_file = "8ee8a8613fadc96701484b23c1d3764e";


    private:
        static std::filesystem::path rsc_path;
        static bool valid;
};
