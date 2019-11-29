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
#include <tlx/math/aggregate.hpp>
#include <sdsl/construct_lcp.hpp>


#include "sdsl_helper.hpp"
#include "timer.hpp"

#include "divsufsort.h"
#include "divsufsort64.h"
#include "sais.h"

#include "lcp_naive.hpp"
#include "lcp_phi.hpp"


#include <unistd.h>
#include <sys/resource.h>
#include <stdio.h>

class lcp_benchmark {

public:
  void run() {
    for (std::string const& file_path : file_paths_) {
      auto const text = load_text(file_path);

      timer t;
      tlx::Aggregate<size_t> sa_times;
      tlx::Aggregate<size_t> lcp_times;
      tlx::Aggregate<size_t> bwt_times;

      auto run_divsufsort_lcp = [&](){
        std::vector<int32_t> sa(text.size(), 0);
        std::vector<int32_t> lcp(text.size(), 0);
        t.reset();
        divsuflcpsort(text.data(), sa.data(), lcp.data(), text.size());
        size_t const total_time = t.get_and_reset();
        divsufsort(text.data(), sa.data(), text.size());
        size_t const sa_time = t.get_and_reset();
        sa_times.add(sa_time);
        lcp_times.add(total_time - sa_time);
      };

      auto run_sais_lite_lcp = [&](){
        std::vector<int32_t> sa(text.size(), 0);
        std::vector<int32_t> lcp(text.size(), 0);
        t.reset();
        saislcp(const_cast<unsigned char*>(text.data()), sa.data(), lcp.data(),
                text.size());
        size_t const total_time = t.get_and_reset();
        sais(const_cast<unsigned char*>(text.data()), sa.data(), text.size());
        size_t const sa_time = t.get_and_reset();
        sa_times.add(sa_time);
        lcp_times.add(total_time - sa_time);
      };

      auto run_kasai = [&]() {
        std::vector<int32_t> sa(text.size(), 0);
        t.reset();
        divsufsort(text.data(), sa.data(), text.size());
        size_t const sa_time = t.get_and_reset();
        auto lcp = lcp_naive(text, sa);
        size_t const lcp_time = t.get_and_reset();
        sa_times.add(sa_time);
        lcp_times.add(lcp_time);
      };

      auto run_phi = [&](){
        std::vector<int32_t> sa(text.size(), 0);
        t.reset();
        divsufsort(text.data(), sa.data(), text.size());
        size_t const sa_time = t.get_and_reset();
        auto lcp = lcp_phi(text, sa);
        size_t const lcp_time = t.get_and_reset();
        sa_times.add(sa_time);
        lcp_times.add(lcp_time);
      };

      auto run_sdsl_kasai = [&](){
        sdsl_helper helper(file_path, prefix_size_);
        t.reset();
        helper.construct_sa();
        size_t const sa_time = t.get_and_reset();
        auto config = helper.get_config();
        sdsl::construct_lcp_kasai<8>(config);
        size_t const lcp_time = t.get_and_reset();
        sa_times.add(sa_time);
        lcp_times.add(lcp_time);
      };

      auto run_sdsl_phi = [&](){
        sdsl_helper helper(file_path, prefix_size_);
        t.reset();
        helper.construct_sa();
        size_t const sa_time = t.get_and_reset();
        auto config = helper.get_config();
        sdsl::construct_lcp_PHI<8>(config);
        size_t const lcp_time = t.get_and_reset();
        sa_times.add(sa_time);
        lcp_times.add(lcp_time);
      };

      auto run_sdsl_se_phi = [&](){
        sdsl_helper helper(file_path, prefix_size_);
        t.reset();
        helper.construct_sa();
        size_t const sa_time = t.get_and_reset();
        auto config = helper.get_config();
        sdsl::construct_lcp_semi_extern_PHI(config);
        size_t const lcp_time = t.get_and_reset();
        sa_times.add(sa_time);
        lcp_times.add(lcp_time);
      };

      auto run_sdsl_go = [&](){
        sdsl_helper helper(file_path, prefix_size_);
        t.reset();
        helper.construct_sa();
        size_t const sa_time = t.get_and_reset();
        helper.construct_bwt();
        size_t const bwt_time = t.get_and_reset();
        auto config = helper.get_config();
        sdsl::construct_lcp_go(config);
        size_t const lcp_time = t.get_and_reset();
        sa_times.add(sa_time);
        bwt_times.add(bwt_time);
        lcp_times.add(lcp_time);
      };

       auto run_sdsl_go_phi = [&](){
        sdsl_helper helper(file_path, prefix_size_);
        t.reset();
        helper.construct_sa();
        size_t const sa_time = t.get_and_reset();
        helper.construct_bwt();
        size_t const bwt_time = t.get_and_reset();
        auto config = helper.get_config();
        sdsl::construct_lcp_goPHI(config);
        size_t const lcp_time = t.get_and_reset();
        sa_times.add(sa_time);
        bwt_times.add(bwt_time);
        lcp_times.add(lcp_time);
      };

      auto run_sdsl_bwt_based = [&](){
        sdsl_helper helper(file_path, prefix_size_);
        t.reset();
        helper.construct_sa();
        size_t const sa_time = t.get_and_reset();
        helper.construct_bwt();
        size_t const bwt_time = t.get_and_reset();
        auto config = helper.get_config();
        sdsl::construct_lcp_bwt_based(config);
        size_t const lcp_time = t.get_and_reset();
        sa_times.add(sa_time);
        bwt_times.add(bwt_time);
        lcp_times.add(lcp_time);
      };

      auto run_sdsl_bwt_based2 = [&](){
        sdsl_helper helper(file_path, prefix_size_);
        t.reset();
        helper.construct_sa();
        size_t const sa_time = t.get_and_reset();
        helper.construct_bwt();
        size_t const bwt_time = t.get_and_reset();
        auto config = helper.get_config();
        sdsl::construct_lcp_bwt_based2(config);
        size_t const lcp_time = t.get_and_reset();
        sa_times.add(sa_time);
        bwt_times.add(bwt_time);
        lcp_times.add(lcp_time);
      };

      std::cout << "RESULT algo=" << algorithm_ << " ";
      for (size_t i = 0; i < runs_; ++i) {
        if (algorithm_ == "divsufsort_lcp") { run_divsufsort_lcp(); }
        else if (algorithm_ == "sais_lite_lcp") { run_sais_lite_lcp(); }
        else if (algorithm_ == "kasai") { run_kasai(); }
        else if (algorithm_ == "phi") { run_phi(); }
        else if (algorithm_ == "kasai_sdsl") { run_sdsl_kasai(); }
        else if (algorithm_ == "phi_sdsl") { run_sdsl_phi(); }
        else if (algorithm_ == "sephi") { run_sdsl_se_phi(); }
        else if (algorithm_ == "go") { run_sdsl_go(); }
        else if (algorithm_ == "gophi") { run_sdsl_go_phi(); }
        else if (algorithm_ == "bwt") { run_sdsl_bwt_based(); }
        else if (algorithm_ == "bwt2") { run_sdsl_bwt_based2(); }
      }

      std::cout << "sa_time=" << sa_times.average() << " "
                << "lcp_time=" << lcp_times.average() << " "
                << "bwt_time=" << bwt_times.average() << " "
                << "memory=" << getPeakRSS() << " "
                << "text_size=" << text.size() << " "
                << "file_path=" << file_path <<  " "
	        << "runs=" << runs_ << std::endl;
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

  // The SDSL overwrites malloc. Therefore, we cannot use malloc_count
  // for most of these algorithms and we use rusage for all
  size_t getPeakRSS() {
    struct rusage rusage;
    getrusage( RUSAGE_SELF, &rusage);
    return (size_t)(rusage.ru_maxrss * 1024L);
  }

public:
  std::vector<std::string> file_paths_;
  uint64_t prefix_size_ = 0;
  size_t runs_ = 5;
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
  cp.add_size_t('r', "runs", benchmark.runs_,
                "Number of runs that average running time is measured for.");
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
