//
// Created by mikhail on 4/30/20.
//

#ifndef FLOW_SRC_PROCESSING_CORRECTION_TASK_FW3S_H_
#define FLOW_SRC_PROCESSING_CORRECTION_TASK_FW3S_H_

#include "correction_task.h"

namespace Qn {

class CorrectionTaskFw3s : public CorrectionTask {
public:
  CorrectionTaskFw3s(const std::string &in_file, const std::string &incalib,
                     const std::string &qn_file, const std::string &efficiency,
                     const std::string &out_file);
private:
  virtual void Configure()
};

} // namespace Qn
#endif // FLOW_SRC_PROCESSING_CORRECTION_TASK_FW3S_H_
