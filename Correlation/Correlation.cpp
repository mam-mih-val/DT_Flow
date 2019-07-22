// Flow Vector Correction Framework
//
// Copyright (C) 2018  Lukas Kreis, Ilya Selyuzhenkov
// Contact: l.kreis@gsi.de; ilya.selyuzhenkov@gmail.com
// For a full list of contributors please see docs/Credits
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "Correlation.h"

namespace Qn {

void Correlation::FillCorrelation(const std::vector<unsigned long> &eventindex,
                                  int iterationoffset,
                                  u_int iteration,
                                  const DataContainers &input) {
  const auto &datacontainer = input[iteration];
  iterationoffset += iteration;
  if (iteration + 1==input.size()) {
    int ibin = 0;
    for (auto &bin : datacontainer) {
      if (!datacontainer.IsIntegrated()) {
        int i_index = 0;
        for (auto index : index_[iteration][ibin]) {
          int pos = i_index + iterationoffset;
          c_index_[pos] = index;
          ++i_index;
        }
      }
      contents_[iteration] = bin;
      data_correlation_.At(c_index_).validity =
          std::all_of(contents_.begin(), contents_.end(), [](const QVector &q) { return q.n() > 0; });
      data_correlation_.At(c_index_).result = function_(contents_);
      int iq = 0;
      int iw = 0;
      for (const auto &q : contents_) {
        if (use_weights_[iq]) {
          data_correlation_.At(c_index_).w_vect[iw] = q.sum_weights_;
          ++iw;
        }
        ++iq;
      }
      ++ibin;
    }
    return;
  }
  int ibin = 0;
  for (const auto &bin : datacontainer) {
    int offset = iterationoffset;
    if (!datacontainer.IsIntegrated()) {
      int i_index = 0;
      for (auto index : index_[iteration][ibin]) {
        offset += i_index;
        c_index_[offset] = index;
        ++i_index;
      }
    }
    contents_[iteration] = bin;
    FillCorrelation(eventindex, offset, iteration + 1, input);
    ++ibin;
  }
}

void Correlation::Fill(const Correlation::DataContainers &input, const std::vector<unsigned long> &eventindex) {
  for (auto &data : data_correlation_) {
    data.validity = false;
  }
  for (auto &q : contents_) {
    q.n_ = 0;
  }
  uint iteration = 0;
  int ii = 0;
  for (auto eventind : eventindex) {
    c_index_[ii] = eventind;
    ++ii;
  }
  FillCorrelation(eventindex, ii, iteration, input);
}

void Correlation::CreateCorrelationContainer(const Correlation::DataContainers &inputs) {
  int i = 0;
  contents_.resize(inputs.size());
  data_correlation_.AddAxes(axes_event_);
  auto size = axes_event_.size();
  for (auto &input : inputs) {
    if (!input.IsIntegrated()) size += input.GetAxes().size();
    std::vector<std::vector<unsigned long>> indexmap;
    for (std::size_t j = 0; j < input.size(); ++j) {
      std::vector<unsigned long> indices;
      input.GetIndex(indices, j);
      indexmap.push_back(indices);
    }
    index_.push_back(indexmap);
    if (!input.IsIntegrated()) {
      auto axes = input.GetAxes();
      for (auto &axis : axes) {
        auto original_name = axis.Name();
//        if (!names_.empty()) { axis.SetName(std::to_string(i) + "_" + names_[i] + "_" + original_name); }
//        else { axis.SetName(std::to_string(i) + "_" + original_name); }
        axis.SetName(std::to_string(i) + "_" + original_name);
        data_correlation_.AddAxis(axis);
      }
      ++i;
    }
  }
  c_index_.resize(size);
  auto size_weights = 0;
  for (auto w : use_weights_) {
    if (w) ++size_weights;
  }
  for (auto &data : data_correlation_) {
    data.w_vect.resize(size_weights);
  }
}

}