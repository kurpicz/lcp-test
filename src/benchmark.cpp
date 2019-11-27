/*******************************************************************************
 * src/benchmark.cpp
 *
 * Copyright (C) 2019 Florian Kurpicz <florian.kurpicz@tu-dortmund.de>
 *
 * All rights reserved. Published under the BSD-2 license in the LICENSE file.
 ******************************************************************************/

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <tlx/cmdline_parser.hpp>

#include "divsufsort.h"
#include "divsufsort64.h"

class lcp_benchmark {

public:
  void run() {
    for (std::string const& file_path : file_paths_) {
      auto text = load_text(file_path);
      std::cout << "text.size() " << text.size() << std::endl;
      std::vector<int32_t> sa(text.size(), 0);
      divsufsort(text.data(), sa.data(), text.size());
    }
  }

private:
  std::vector<uint8_t> load_text(std::string const& file_path) {
    std::ifstream stream(file_path.c_str(), std::ios::in | std::ios::binary);
    if (!stream) {
      std::cerr << "File " << file_path << " not found" << std::endl;
      std::exit(-1);
    }

    stream.seekg(0, std::ios::end);
    uint64_t file_size = stream.tellg();
    if (prefix_size_ > 0) {
      file_size = std::min(prefix_size_, file_size);
    }
    stream.seekg(0);
    std::vector<uint8_t> result(file_size);
    stream.read(reinterpret_cast<char*>(result.data()), file_size);
    stream.close();
    result.back() = uint8_t(0);
    return result;
  }

public:
  std::vector<std::string> file_paths_;
  uint64_t prefix_size_ = 0;
}; // class lcp_benchmark

int32_t main(int32_t argc, char* const argv[]) {
  tlx::CmdlineParser cp;
  lcp_benchmark benchmark;

  cp.set_description("Benchmarks for main memory LCP construction algorithms.");
  cp.set_author("Florian Kurpicz <florian.kurpicz@tu-dortmund.de");

  cp.add_stringlist('f', "file", benchmark.file_paths_,
                    "Path(s) to the input files.");
  cp.add_bytes('s', "size", benchmark.prefix_size_,
               "Optional size of prefix of inputs that should be considered");
  

  if(!cp.process(argc, argv)) {
    std::exit(-1);
  }

  benchmark.run();
  
  return 0;
}

/******************************************************************************/
