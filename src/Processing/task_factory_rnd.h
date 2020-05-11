//
// Created by mikhail on 5/7/20.
//

#ifndef FLOW_SRC_PROCESSING_TASK_FACTORY_RND_H_
#define FLOW_SRC_PROCESSING_TASK_FACTORY_RND_H_
#include "task_factory.h"

class TaskFactoryRnd : public TaskFactory {
public:
  static TaskFactoryRnd* GetInstance(){
    if( !instance_ ){
      instance_ = new TaskFactoryRnd();
    }
    return instance_;
  }
  void RunJob() override {
    DataTreeVarManager::GetInstance()->OpenFiles(config_.input_data);
    DataTreeVarManager::GetInstance()->SwitchOnCentrality();
    DataTreeVarManager::GetInstance()->SwitchOnSelector();
    Selector::GetInstance()->SetTrigger(config_.trigger);
    Selector::GetInstance()->SetPid(config_.pid_code);
    RunFullCorrectionTask("nothing");
    RunFullCorrectionTask("qn.root");
    OccupancyCorrections::GetInstance()->SetQnFile("pavel.root");
    OccupancyCorrections::GetInstance()->SetEfficiencyFile(config_.efficiency_file);
    RunCorrectionTask("nothing");
    RunCorrectionTask("qn.root");
    RunCorrectionTask("qn.root");
    RunCorrelationTask();
  }
protected:
  static TaskFactoryRnd* instance_;
  TaskFactoryRnd() : TaskFactory(){}
  ~TaskFactoryRnd() = default;
  void RunCorrectionTask(const std::string& calib_file) override {
    Qn::CorrectionTaskRnd task;
    task.SetInCalibrationFile(calib_file);
    task.SetOutCalibrationFile("qn.root");
    task.Run();
  }
  void RunFullCorrectionTask(const std::string& calib_file){
    Qn::CorrectionTaskFull task;
    task.SetInCalibrationFile(calib_file);
    task.SetOutCalibrationFile("qn.root");
    task.SetOutFile("pavel.root");
    task.Run();
  }
  void RunCorrelationTask() override{
    CorrelationTaskRnd task;
    task.AddFiles("output.root");
    task.Run();
  }
};

#endif // FLOW_SRC_PROCESSING_TASK_FACTORY_RND_H_
