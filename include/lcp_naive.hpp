/*******************************************************************************
 * include/lcp_naive.hpp
 *
 * Copyright (C) 2019 Florian Kurpicz <florian.kurpicz@tu-dortmund.de>
 *
 * All rights reserved. Published under the BSD-2 license in the LICENSE file.
 ******************************************************************************/

#pragma once

#include <algorithm>
#include <cstdint>
#include <vector>

template <typename IndexType>
std::vector<IndexType> lcp_naive(std::vector<uint8_t> const& text,
                                 std::vector<IndexType> const& sa) {
  std::vector<IndexType> inverse_sa(sa.size(), 0);
  std::vector<IndexType> lcp(sa.size(), 0);
  for (size_t i = 0; i < sa.size(); ++i) {
    inverse_sa[sa[i]] = i;
  }
  IndexType cur_lcp = 0;
  for (size_t i = 0; i < sa.size(); ++i) {
    if (inverse_sa[i] != 0) {
      IndexType pre = sa[inverse_sa[i] - 1];
      IndexType max_lcp = text.size() - std::max<IndexType>(i, pre);
      while (cur_lcp < max_lcp && text[i + cur_lcp] == text[pre + cur_lcp]) {
        ++cur_lcp;
      }
      lcp[inverse_sa[i]] = cur_lcp;
      cur_lcp -= (cur_lcp > 0) ? 1 : 0;
    }
  }
  return lcp;
}

/******************************************************************************/
