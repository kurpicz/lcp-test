/*******************************************************************************
 * include/lcp_phi.hpp
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
std::vector<IndexType> lcp_phi(std::vector<uint8_t> const& text,
                               std::vector<IndexType> const& sa) {
  std::vector<IndexType> phi(sa.size(), 0);
  phi.back() = sa[sa.size() - 1];
  for (size_t i = 1; i < sa.size(); ++i) {
    phi[sa[i]] = sa[i - 1];
  }
  IndexType cur_lcp = 0;
  for (size_t i = 0; i < sa.size(); ++i) {
    IndexType pre = phi[i];
    IndexType max_lcp = text.size() - std::max<IndexType>(i, pre);
    while (cur_lcp < max_lcp && text[i + cur_lcp] == text[pre + cur_lcp]) {
      ++cur_lcp;
    }
    phi[i] = cur_lcp;
    cur_lcp -= (cur_lcp > 0) ? 1 : 0;
  }
  std::vector<IndexType> lcp(sa.size());
  for (size_t i = 0; i < phi.size(); ++i) {
    lcp[i] = phi[sa[i]];
  }
  return lcp;
}

/******************************************************************************/
