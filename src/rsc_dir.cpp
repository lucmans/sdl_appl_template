#include "rsc_dir.hpp"

#include "exception.hpp"

#include <string>
#include <filesystem>
#include <fstream>


namespace fs = std::filesystem;


bool RscDir::valid = false;
fs::path RscDir::rsc_path = fs::path();


/*static*/ const fs::path& RscDir::get() {
    if (!RscDir::valid)
        throw Exception("Resource directory path is not yet set");

    return RscDir::rsc_path;
}


/*static*/ void RscDir::set(fs::path new_path) {
    if (!fs::exists(new_path))
        throw Exception("Resource path '" + new_path.string() + "' does not exist");
    if (!fs::is_directory(new_path))
        throw Exception("Resource path '" + new_path.string() + "' is not a directory");

    // clean the path string (for printing it to cli)
    new_path = new_path.lexically_normal();

    if constexpr (RscDir::make_path_absolute) {
        try {
            new_path = std::filesystem::canonical(new_path);
        }
        catch (const std::exception& e) {
            throw Exception("Failed to make canonical path from resource path\nStdlib error: " + std::string(e.what()));
        }
    }

    // verify we have the right rsc directory
    if (!std::filesystem::exists(new_path / "verify"))
        throw Exception("Resource directory verification file does not exist");

    std::ifstream verification_file(new_path / "verify");
    if (!verification_file.is_open())
        throw Exception("Failed to open resource directory verification file");

    std::string line;
    verification_file >> line;
    if (line != hash_in_file)
        throw Exception("Resource directory verification file has wrong content");

    RscDir::rsc_path = new_path;
    RscDir::valid = true;
}


/*static*/ bool RscDir::is_valid() noexcept {
    return RscDir::valid;
}
