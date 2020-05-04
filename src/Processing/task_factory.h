//
// Created by mikhail on 4/30/20.
//

#ifndef FLOW_SRC_PROCESSING_TASK_FACTORY_H_
#define FLOW_SRC_PROCESSING_TASK_FACTORY_H_
#include "correction_task.h"
#include "correlation_task.h"

class TaskFactory {
public:
  static TaskFactory* GetInstance(){
    if(!instance_)
      instance_ = new TaskFactory();
    return instance_;
  }
  Qn::CorrectionTask* CreateCorrectionTask(){

  }
  CorrelationTask* CreateCorrelationTask(){

  }
protected:
  static TaskFactory *instance_;
  TaskFactory() = default;
};

#endif // FLOW_SRC_PROCESSING_TASK_FACTORY_H_
