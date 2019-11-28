/*******************************************************************************
 * src/benchmark.cpp
 *
 * Copyright (C) 2019 Florian Kurpicz <florian.kurpicz@tu-dortmund.de>
 *
 * All rights reserved. Published under the BSD-2 license in the LICENSE file.
 ******************************************************************************/

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <tlx/cmdline_parser.hpp>
#include <sdsl/construct_lcp.hpp>


#include "sdsl_helper.hpp"
#include "timer.hpp"

#include "divsufsort.h"
#include "divsufsort64.h"

#include "lcp_naive.hpp"
#include "lcp_phi.hpp"

class lcp_benchmark {

public:
  void run() {
    for (std::string const& file_path : file_paths_) {
      auto const text = load_text(file_path);

      timer t;

      std::cout << "RESULT algo=";
      if (algorithm_ == "d") {
        std::cout << "divsufsort-lcp ";
        std::cout << "text.size() " << text.size() << std::endl;
        std::vector<int32_t> sa(text.size(), 0);
        std::vector<int32_t> lcp(text.size(), 0);
        divsufsort(text.data(), sa.data(), text.size());
      } else if (algorithm_ == "n") {
        std::cout << "naive ";
        std::vector<int32_t> sa(text.size(), 0);
        t.reset();
        divsufsort(text.data(), sa.data(), text.size());
        size_t const sa_time = t.get_and_reset();
        auto lcp = lcp_naive(text, sa);
        size_t const lcp_time = t.get_and_reset();
        std::cout << "sa_time=" << sa_time << " "
                  << "lcp_time=" << lcp_time << " ";
        
      } else if (algorithm_ == "phi") {
        std::cout << "phi ";
        std::vector<int32_t> sa(text.size(), 0);
        t.reset();
        divsufsort(text.data(), sa.data(), text.size());
        size_t const sa_time = t.get_and_reset();
        auto lcp = lcp_phi(text, sa);
        size_t const lcp_time = t.get_and_reset();
        std::cout << "sa_time=" << sa_time << " "
                  << "lcp_time=" << lcp_time << " ";
      } else if (algorithm_ == "k") {
        std::cout << "sdsl-kasai ";
        sdsl_helper helper(file_path, prefix_size_);
        t.reset();
        helper.construct_sa();
        size_t const sa_time = t.get_and_reset();
        auto config = helper.get_config();
        sdsl::construct_lcp_kasai<8>(config);
        size_t const lcp_time = t.get_and_reset();
        std::cout << "sa_time=" << sa_time << " "
                  << "lcp_time=" << lcp_time << " ";
      } else if (algorithm_ == "phi2") {
        std::cout << "sdsl-phi ";
        sdsl_helper helper(file_path, prefix_size_);
        t.reset();
        helper.construct_sa();
        size_t const sa_time = t.get_and_reset();
        auto config = helper.get_config();
        sdsl::construct_lcp_PHI<8>(config);
        size_t const lcp_time = t.get_and_reset();
        std::cout << "sa_time=" << sa_time << " "
                  << "lcp_time=" << lcp_time << " ";
      } else if (algorithm_ == "sephi") {
        std::cout << "sdsl-se-phi ";
        sdsl_helper helper(file_path, prefix_size_);
        t.reset();
        helper.construct_sa();
        size_t const sa_time = t.get_and_reset();
        auto config = helper.get_config();
        sdsl::construct_lcp_semi_extern_PHI(config);
        size_t const lcp_time = t.get_and_reset();
        std::cout << "sa_time=" << sa_time << " "
                  << "lcp_time=" << lcp_time << " ";
      } else if (algorithm_ == "go") {
        std::cout << "sdsl-go ";
        sdsl_helper helper(file_path, prefix_size_);
        t.reset();
        helper.construct_sa();
        size_t const sa_time = t.get_and_reset();
        helper.construct_bwt();
        size_t const bwt_time = t.get_and_reset();
        auto config = helper.get_config();
        sdsl::construct_lcp_go(config);
        size_t const lcp_time = t.get_and_reset();
        std::cout << "sa_time=" << sa_time << " "
                  << "bwt_time=" << bwt_time << " "
                  << "lcp_time=" << lcp_time << " ";
      } else if (algorithm_ == "gophi") {
        std::cout << "sdsl-go-phi ";
        sdsl_helper helper(file_path, prefix_size_);
        t.reset();
        helper.construct_sa();
        size_t const sa_time = t.get_and_reset();
        helper.construct_bwt();
        size_t const bwt_time = t.get_and_reset();
        auto config = helper.get_config();
        sdsl::construct_lcp_goPHI(config);
        size_t const lcp_time = t.get_and_reset();
        std::cout << "sa_time=" << sa_time << " "
                  << "bwt_time=" << bwt_time << " "
                  << "lcp_time=" << lcp_time << " ";
      } else if (algorithm_ == "bwt") {
        std::cout << "sdsl-bwt ";
        sdsl_helper helper(file_path, prefix_size_);
        t.reset();
        helper.construct_sa();
        size_t const sa_time = t.get_and_reset();
        helper.construct_bwt();
        size_t const bwt_time = t.get_and_reset();
        auto config = helper.get_config();
        sdsl::construct_lcp_bwt_based2(config);
        size_t const lcp_time = t.get_and_reset();
        std::cout << "sa_time=" << sa_time << " "
                  << "bwt_time=" << bwt_time << " "
                  << "lcp_time=" << lcp_time << " ";
      } else if (algorithm_ == "bwt2") {
        std::cout << "sdsl-bwt2 ";
        sdsl_helper helper(file_path, prefix_size_);
        t.reset();
        helper.construct_sa();
        size_t const sa_time = t.get_and_reset();
        helper.construct_bwt();
        size_t const bwt_time = t.get_and_reset();
        auto config = helper.get_config();
        sdsl::construct_lcp_bwt_based2(config);
        size_t const lcp_time = t.get_and_reset();
        std::cout << "sa_time=" << sa_time << " "
                  << "bwt_time=" << bwt_time << " "
                  << "lcp_time=" << lcp_time << " ";
      }
    }
  }

private:
  std::vector<uint8_t> load_text(std::string const& file_path) {
    std::ifstream stream(file_path.c_str(), std::ios::in | std::ios::binary);
    if (!stream) {
      std::cerr << "File " << file_path << " not found" << std::endl;
      std::exit(-1);
    }

    std::filesystem::path p(file_path);

    uint64_t file_size = std::filesystem::file_size(p);
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
  std::string algorithm_;
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
  cp.add_string('a', "algorithm", benchmark.algorithm_,
                "The algorithm that is used to construct the LCP array. "
                "Available are: "
                "[n]aive, [p]hi, and [d]ivsufsort (default).");

  if(!cp.process(argc, argv)) {
    std::exit(-1);
  }

  benchmark.run();
  
  return 0;
}

/******************************************************************************/
