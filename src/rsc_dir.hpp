#pragma once

#include <filesystem>
#include <string>


class RscDir {
    // static only class, so disallow construction
    private:
        RscDir();

    public:
        // throws exception if not yet set to a valid path
        static const std::filesystem::path& get();

        // throws an exception if new path is not valid
        // strong exception guarantee
        static void set(std::filesystem::path new_path);

        static bool is_valid() noexcept;


    private:
        /* config */
        static const bool make_path_absolute = false;


        /* member variables */
        static std::filesystem::path rsc_path;
        static bool valid;
};
