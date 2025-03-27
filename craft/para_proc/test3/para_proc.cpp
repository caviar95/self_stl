#include <iostream>
#include <vector>

#include <unistd.h>

int test_1(int argc, char** argv) {
    std::string input_file, output_file;
    bool verbose = false;
    std::vector<std::string> positional_args;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            std::cout << "Usage: " << argv << " [-h] [-i INPUT] [-o OUTPUT] [--verbose] [files...]\n";
            return 0;
        } else if (arg == "-i" || arg == "--input") {
            if (i + 1 < argc) {
                input_file = argv[++i];
            } else {
                std::cerr << "Error: Missing argument for " << arg << "\n";
                return 1;
            }
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                output_file = argv[++i];
            } else {
                std::cerr << "Error: Missing argument for " << arg << "\n";
                return 1;
            }
        } else if (arg == "--verbose") {
            verbose = true;
        } else if (arg == '-') {
            std::cerr << "Error: Unknown option " << arg << "\n";
            return 1;
        } else {
            positional_args.push_back(arg);
        }
    }

    // 处理位置参数和逻辑
    std::cout << "Input: " << input_file << "\nOutput: " << output_file << "\nVerbose: " << verbose << "\n";
    for (const auto& file : positional_args) {
        std::cout << "Positional: " << file << "\n";
    }
    return 0;
}

int test2(int argc, char** argv) {
    int opt;
    const char* input_file = nullptr;
    const char* output_file = nullptr;
    bool verbose = false;

    while ((opt = getopt(argc, argv, "hi:o:v")) != -1) {
        switch (opt) {
            case 'h':
                std::cout << "Usage: " << argv << " [-h] [-i INPUT] [-o OUTPUT] [-v]\n";
                return 0;
            case 'i':
                input_file = optarg;
                break;
            case 'o':
                output_file = optarg;
                break;
            case 'v':
                verbose = true;
                break;
            case '?':
                std::cerr << "Error: Unknown option or missing argument.\n";
                return 1;
        }
    }

    // 处理位置参数（非选项参数）
    std::vector<std::string> positional_args;
    for (int i = optind; i < argc; ++i) {
        positional_args.push_back(argv[i]);
    }

    std::cout << "Input: " << (input_file ? input_file : "") << "\nOutput: " << (output_file ? output_file : "") << "\nVerbose: " << verbose << "\n";
    return 0;
}

#include <getopt.h>

int test_3(int argc, char** argv) {
    const char* input_file = nullptr;
    const char* output_file = nullptr;
    bool verbose = false;

    struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"input", required_argument, 0, 'i'},
        {"output", required_argument, 0, 'o'},
        {"verbose", no_argument, 0, 'v'},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "hi:o:v", long_options, nullptr)) != -1) {
        switch (opt) {
            case 'h':
                std::cout << "Usage: " << argv << " [--help] [--input INPUT] [--output OUTPUT] [--verbose]\n";
                return 0;
            case 'i':
                input_file = optarg;
                break;
            case 'o':
                output_file = optarg;
                break;
            case 'v':
                verbose = true;
                break;
            case '?':
                std::cerr << "Error: Invalid option or missing argument.\n";
                return 1;
        }
    }

    // 处理位置参数
    std::vector<std::string> positional_args;
    for (int i = optind; i < argc; ++i) {
        positional_args.push_back(argv[i]);
    }

    std::cout << "Input: " << (input_file ? input_file : "") << "\nOutput: " << (output_file ? output_file : "") << "\nVerbose: " << verbose << "\n";
    return 0;
}

int main(int argc, char** argv) {
    
}
