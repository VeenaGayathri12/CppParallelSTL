// based on C++17 Complete by Nicolai Josuttis, https://leanpub.com/cpp17

#include <chrono>
#include <execution>   // for the execution policy
#include <filesystem>  // filesystem library
#include <iostream>
#include <numeric>  // for transform_reduce()
#include <vector>

void PrintTiming(
    const char* title,
    const std::chrono::time_point<std::chrono::steady_clock>& start) {
  const auto end = std::chrono::steady_clock::now();
  std::cout << title << ": "
            << std::chrono::duration<double, std::milli>(end - start).count()
            << " ms\n";
}

template <typename Policy>
uintmax_t ComputeTotalFileSize(const std::vector<std::filesystem::path>& paths,
                               Policy policy) {
  return std::transform_reduce(
      policy, paths.cbegin(), paths.cend(),  // range
      std::uintmax_t{0},                     // initial value
      std::plus<>(),                         // accumulate ...
      [](const std::filesystem::path& p) {   //  file size if regular file
        return is_regular_file(p) ? file_size(p) : std::uintmax_t{0};
      });
}

int main(int argc, char* argv[]) {
  // root directory is passed as command line argument:
  if (argc < 3) {
    std::cout << "Usage: " << argv[0] << " <path> <parallel:1|0>\n";
    return EXIT_FAILURE;
  }
  std::filesystem::path root{argv[1]};

  int executionPolicyMode = atoi(argv[2]);
  std::cout << "Using " << (executionPolicyMode ? "PAR" : "SEQ") << " Policy\n";

  // init list of all file paths in passed file tree:
  auto start = std::chrono::steady_clock::now();
  std::vector<std::filesystem::path> paths;
  try {
    std::filesystem::recursive_directory_iterator dirpos{root};
    std::copy(begin(dirpos), end(dirpos), std::back_inserter(paths));
  } catch (const std::exception& e) {
    std::cerr << "EXCEPTION: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  PrintTiming("gathering all the paths", start);

  std::cout << "number of files: " << std::size(paths) << "\n";

  // for (const auto& p : paths)
  //	std::cout << p.filename() << "\n";

  // accumulate size of all regular files:
  start = std::chrono::steady_clock::now();
  uintmax_t FinalSize = 0;
  if (executionPolicyMode)
    FinalSize = ComputeTotalFileSize(paths, std::execution::par);
  else
    FinalSize = ComputeTotalFileSize(paths, std::execution::seq);

  PrintTiming("computing the sizes", start);

  std::cout << "size of all " << paths.size()
            << " regular files: " << FinalSize / 1024 << " kbytes\n";
}
