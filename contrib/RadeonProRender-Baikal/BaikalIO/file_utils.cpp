#include <experimental/filesystem>
#include <deque>
#include <algorithm>

namespace Baikal
{
    inline bool is_dot(std::experimental::filesystem::path const &path)
    {
        std::string const &filename = path.string();
        return filename.size() == 1 && filename[0] == '.';
    }

    inline bool is_dotdot(std::experimental::filesystem::path const &path)
    {
        std::string const &filename = path.string();
        return filename.size() == 2 && filename[0] == '.' && filename[1] == '.';
    }

    bool FindFilenameFromCaseInsensitive(std::string const &req_filename, std::string &actual_filename, bool resolve_symlinks = true)
    {
        const std::experimental::filesystem::path file_path(req_filename);

        // First, check if file with required filename exists
        if (std::experimental::filesystem::exists(file_path))
        {
            actual_filename = req_filename;
            return true;
        }

        // Split path to components
        std::deque<std::experimental::filesystem::path> components;
        for (auto &f : file_path.relative_path())
        {
            components.push_back(f);
        }

        // Start from the current directory
        std::experimental::filesystem::path result = std::experimental::filesystem::current_path();

        while (!components.empty())
        {
            // Get a path component
            std::experimental::filesystem::path c = std::move(components.front());
            components.pop_front();

            if (is_dot(c)) // <path>/.
            {
                // Ensure that this is a directory and skip the dot
                if (!std::experimental::filesystem::is_directory(result))
                {
                    // Is not a directory
                    return false;
                }
            }
            else if (is_dotdot(c)) // <path>/..
            {
                result /= c;
                if (!std::experimental::filesystem::exists(result))
                {
                    // Upper directory is not exist
                    return false;
                }
            }
            else // <path>/<other symbols>
            {
                // The path doesn't exist
                if (!std::experimental::filesystem::exists(result / c))
                {
                    // Path in this letter case doesn't exist...
                    // But we'll try to find it in another case
                    bool component_found = false;

                    if (!std::experimental::filesystem::is_directory(result))
                    {
                        // Something went wrong, we're not in a directory
                        return false;
                    }

                    for (auto const &entry : std::experimental::filesystem::directory_iterator(result))
                    {
                        // Get only name of the folder/file
                        std::experimental::filesystem::path found_name = entry.path().filename();

                        // Move to lower case and compare path strings
                        std::string path_str = found_name.string();
                        std::transform(path_str.begin(), path_str.end(), path_str.begin(), ::tolower);

                        std::string req_str = c.string();
                        std::transform(req_str.begin(), req_str.end(), req_str.begin(), ::tolower);

                        if (path_str == req_str)
                        {
                            // Found!
                            component_found = true;
                            result /= found_name;
                            break;
                        }
                    }

                    // Give up, the path really doesn't exist
                    if (!component_found)
                    {
                        return false;
                    }
                }
                else
                {
                    // If the path exists, simply concatenate the result with the component
                    result /= c;
                }

                // If the result is a symbolic link, resolve it
                if (std::experimental::filesystem::is_symlink(result) && resolve_symlinks)
                {
                    // Get a target of the link
                    std::experimental::filesystem::path target = std::experimental::filesystem::read_symlink(result);

                    if (target.is_absolute())
                    {
                        // The target is an absolute path
                        // Start from the root folder on the next iteration
                        result = target.root_path();
                        // And remove the root folder
                        target = target.relative_path();
                    }
                    else
                    {
                        // If the target is a relative path
                        // Start from the base folder of the link on the next iteration
                        result.remove_filename();
                    }

                    // Push resolved components of the link to the container
                    components.insert(components.begin(), target.begin(), target.end());
                }
            }
        }

        // Final check
        if (!std::experimental::filesystem::exists(result))
        {
            return false;
        }

        actual_filename = result.string();

        return true;
    }

} // namespace Baikal
