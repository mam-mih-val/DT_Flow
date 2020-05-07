//
// Created by mikhail on 4/30/20.
//

#ifndef FLOW_SRC_PROCESSING_TASK_FACTORY_H_
#define FLOW_SRC_PROCESSING_TASK_FACTORY_H_

#include "correction_task.h"
#include "correction_task_full.h"
#include "correction_task_fw3s.h"
#include "correction_task_fw3x.h"
#include "correction_task_rnd.h"
#include "correlation_task.h"
#include "correlation_task_fw3s.h"
#include "correlation_task_fw3x.h"
#include "correlation_task_rnd.h"

struct Configuration{
  std::string input_data;
  std::string efficiency_file;
  std::string trigger="PT2";
  int pid_code=14;
};

class TaskFactory {
public:
  TaskFactory( const TaskFactory& ) = delete;
  void operator=(const TaskFactory&) = delete;

  static TaskFactory* GetInstance(){
    if(!instance_)
      instance_ = new TaskFactory();
    return instance_;
  }
  void SetConfig(const Configuration &config) { config_ = config; }
  virtual void RunJob(){  }

protected:
  virtual void RunCorrectionTask(const std::string& calib_file){ }
  virtual void RunCorrelationTask(){}

  static TaskFactory *instance_;
  TaskFactory() = default;
  ~TaskFactory() = default;
  Configuration config_;
};

#endif // FLOW_SRC_PROCESSING_TASK_FACTORY_H_
