//
// Created by Lukas Kreis on 20.11.17.
//

#ifndef FLOW_DATACONTAINERHELPER_H
#define FLOW_DATACONTAINERHELPER_H

#include <TGraphAsymmErrors.h>
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TBrowser.h"
#include "TEnv.h"

#include "Profile.h"
#include "Sample.h"
#include "EventShape.h"

namespace Qn {
//Forward declaration of DataContainer
template<typename T>
class DataContainer;
namespace Internal {

/**
 * @brief       Template class to facilitate the drawing of Projections of DataContainer.
 * @attention   Takes ownership of graph and will delete it at the end of the lifetime.
 * @tparam T    Type of Graph to draw.
 */
template<typename T>
struct ProjectionDrawable : public TNamed {

  ProjectionDrawable() = default;
  ~ProjectionDrawable() override {
    if (graph) delete graph;
  }
  explicit ProjectionDrawable(T projection) :
      graph(std::move(projection)) {
    SetName(projection->GetName());
    SetTitle(projection->GetTitle());
  }

  void Browse(TBrowser *b) override {
    TString opt = gEnv->GetValue("TGraph.BrowseOption", "");
    if (opt.IsNull()) {
      opt = b ? b->GetDrawOption() : "AlP PLC PMC Z";
      opt = (opt=="") ? "ALP PMC PLC Z" : opt.Data();
    }
    graph->Draw(opt);
    gPad->Update();
  }
  T graph = nullptr;
};
}
/**
 * Helper class for DataContainer used for visualization.
 */
class DataContainerHelper {
 public:
  enum class Errors { Yonly, XandY };

  static TGraphAsymmErrors *DataToProfileGraph(const Qn::DataContainer<Qn::Sample> &data, Errors x);
  static TGraphAsymmErrors *DataToProfileGraphShifted(const Qn::DataContainer<Qn::Sample> &data,
                                                      int i,
                                                      int max,
                                                      Errors x);
  static TMultiGraph *DataToMultiGraph(const Qn::DataContainer<Qn::Sample> &data,
                                       const std::string &axisname,
                                       Errors x);

 private:
  friend DataContainer<Qn::Sample>;
  friend DataContainer<Qn::EventShape>;
  static void SampleBrowse(Qn::DataContainer<Qn::Sample> *data, TBrowser *b);
  static void EventShapeBrowse(Qn::DataContainer<Qn::EventShape> *data, TBrowser *b);
  static void NDraw(Qn::DataContainer<Qn::Sample> &data, std::string option, const std::string &axis_name);

};
}

#endif //FLOW_DATACONTAINERHELPER_H
