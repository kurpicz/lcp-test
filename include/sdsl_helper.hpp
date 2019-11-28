/*******************************************************************************
 * include/sdsl_prepare.hpp
 *
 * Copyright (C) 2019 Florian Kurpicz <florian.kurpicz@tu-dortmund.de>
 *
 * All rights reserved. Published under the BSD-2 license in the LICENSE file.
 ******************************************************************************/

#pragma once

#include <filesystem>

#include <sdsl/config.hpp>
#include <sdsl/construct.hpp>
#include <sdsl/construct_sa.hpp>

class sdsl_helper {

public:

  static constexpr auto UNIQUE_FOLDER_NAME = "sdsl_helper_tmp";
  static constexpr auto UNIQUE_FILE_NAME = "sdsl_helper_file";

  sdsl_helper(std::string const& file_path, uint64_t const prefix_size = 0) {
    namespace fs = std::filesystem;
    fs::create_directories(UNIQUE_FOLDER_NAME);
    fs::path src_path(file_path);
    std::string const dst_file_path = std::string(UNIQUE_FOLDER_NAME) + "/" +
      std::string(UNIQUE_FILE_NAME);
    fs::path dst_path(dst_file_path);
    fs::copy(src_path, dst_path);

    uint64_t new_file_size = (prefix_size > 0) ?
      std::min(prefix_size, fs::file_size(dst_path)) : fs::file_size(dst_path);
    fs::resize_file(dst_path, new_file_size - 1);

    sdsl::load_vector_from_file(text_, dst_path, 1);
    sdsl::append_zero_symbol(text_);
    sdsl::store_to_cache(text_, sdsl::conf::KEY_TEXT, config_);
  }

  ~sdsl_helper() {
    std::filesystem::remove_all(UNIQUE_FOLDER_NAME);
  }

  void construct_sa() {
    sdsl::construct_sa<8>(config_);
  }

  void construct_bwt() {
    sdsl::construct_bwt<8>(config_);
  }

  sdsl::cache_config get_config() const {
    return config_;
  }

private:
  sdsl::cache_config config_;
  sdsl::int_vector<8> text_;
  sdsl::int_vector<> sa_;
}; // class sdsl_helper

/******************************************************************************/
