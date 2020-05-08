//
// Created by mikhail on 5/8/20.
//

#ifndef FLOW_SRC_PROCESSING_TASK_FACTORY_TRACK_DENSITY_H_
#define FLOW_SRC_PROCESSING_TASK_FACTORY_TRACK_DENSITY_H_
#include "task_factory.h"
#include "track_density.h"

class TaskFactoryTrackDensity : public TaskFactory {
public:
  static TaskFactoryTrackDensity* GetInstance(){
    if(!instance_)
      instance_ = new TaskFactoryTrackDensity;
    return instance_;
  }
  void RunJob() override{
    DataTreeVarManager::GetInstance()->OpenFiles(config_.input_data);
    DataTreeVarManager::GetInstance()->SwitchOnCentrality();
    DataTreeVarManager::GetInstance()->SwitchOnSelector();
    Selector::GetInstance()->SetTrigger(config_.trigger);
    RunCorrectionTask("nothing");
    RunCorrectionTask("qn.root");
    RunTrackDensity();
  }

protected:
  void RunCorrectionTask(const std::string& calib_file) override {
    Qn::CorrectionTaskFull task;
    task.SetInCalibrationFile(calib_file);
    task.SetOutCalibrationFile("qn.root");
    task.Run();
  }
  void RunTrackDensity(){
    TrackDensity task;
    task.AddDataFiles(config_.input_data);
    task.AddQnFile("output.root");
    task.Run();
  }
  void RunCorrelationTask() override {};
  static TaskFactoryTrackDensity* instance_;
  TaskFactoryTrackDensity() : TaskFactory() {}
  ~TaskFactoryTrackDensity() =  default;

};

#endif // FLOW_SRC_PROCESSING_TASK_FACTORY_TRACK_DENSITY_H_
