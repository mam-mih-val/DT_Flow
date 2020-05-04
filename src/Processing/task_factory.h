//
// Created by mikhail on 4/30/20.
//

#ifndef FLOW_SRC_PROCESSING_TASK_FACTORY_H_
#define FLOW_SRC_PROCESSING_TASK_FACTORY_H_
#include "correction_task.h"
#include "correlation_task.h"
#include "correction_task_fw3s.h"
#include "correction_task_rnd.h"

struct Configuration{
  enum methods{
    FW3S=0,
    FWRS
  };
  std::string input_data;
  std::string input_calib;
  std::string efficiency_file;
  std::string output_file;
  std::string trigger;
  int method;
  int pid_code;
};

class TaskFactory {
public:
  static TaskFactory* GetInstance(){
    if(!instance_)
      instance_ = new TaskFactory();
    return instance_;
  }
  void SetConfig(const Configuration &config) { config_ = config; }

  Qn::CorrectionTask* CreateCorrectionTask(){
    Qn::CorrectionTask* task;
    switch (config_.method) {
    case Configuration::FW3S :
      task = new Qn::CorrectionTaskFw3s();
      break;
    case Configuration::FWRS :
      task = new Qn::CorrectionTaskRnd();
      break;
    }
    task->SetInCalibrationFile(config_.input_calib);
    task->SetOutFile(config_.output_file);
    DataTreeVarManager::GetInstance()->OpenFiles(config_.input_data);
    Selector::GetInstance()->SetPid(config_.pid_code);
    Selector::GetInstance()->SetTrigger(config_.trigger);
    OccupancyCorrections::GetInstance()->SetEfficiencyFile(config_.efficiency_file);
    return task;
  }

  protected:
  static TaskFactory *instance_;
  TaskFactory() = default;
  ~TaskFactory() = default;
  Configuration config_;
};

#endif // FLOW_SRC_PROCESSING_TASK_FACTORY_H_
