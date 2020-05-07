//
// Created by mikhail on 5/7/20.
//

#ifndef FLOW_SRC_PROCESSING_TASK_FACTORY_FW3S_H_
#define FLOW_SRC_PROCESSING_TASK_FACTORY_FW3S_H_
#include "task_factory.h"

class TaskFactoryFw3s : public TaskFactory {
public:
  static TaskFactoryFw3s* GetInstance(){
    if( !instance_ )
      instance_ = new TaskFactoryFw3s();
    return instance_;
  }
  void RunJob() override {
    DataTreeVarManager::GetInstance()->OpenFiles(config_.input_data);
    DataTreeVarManager::GetInstance()->SwitchOnCentrality();
    DataTreeVarManager::GetInstance()->SwitchOnSelector();
    Selector::GetInstance()->SetTrigger(config_.trigger);
    RunCorrectionTask("nothing");
    RunCorrectionTask("qn.root");
    RunCorrectionTask("qn.root");
    RunCorrelationTask();
  }
protected:
  static TaskFactoryFw3s* instance_;
  TaskFactoryFw3s() : TaskFactory() {};
  ~TaskFactoryFw3s() = default;
  void RunCorrectionTask(const std::string& calib_file) override {
    Qn::CorrectionTaskFw3s task;
    task.SetInCalibrationFile(calib_file);
    task.SetOutCalibrationFile("qn.root");
    task.Run();
  }
  void RunCorrelationTask() override{
    CorrelationTaskFw3s task;
    task.AddFiles("output.root");
    task.Run();
  }
};

#endif // FLOW_SRC_PROCESSING_TASK_FACTORY_FW3S_H_
