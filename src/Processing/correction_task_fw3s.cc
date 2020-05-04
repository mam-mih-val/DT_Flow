//
// Created by mikhail on 4/30/20.
//

#include "correction_task_fw3s.h"
Qn::CorrectionTaskFw3s::CorrectionTaskFw3s(const std::string &in_file,
                                           const std::string &incalib,
                                           const std::string &qn_file,
                                           const std::string &efficiency,
                                           const std::string &out_file)
    : CorrectionTask(in_file, incalib, qn_file, efficiency, out_file) {}
