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
  enum methods{
    FW3S=0,
    FWRS,
    FW3X,
    FULL
  };
  std::string input_data;
  std::string input_calib;
  std::string efficiency_file;
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
  void RunJob(){
    ConfigureJob();
    for( const auto& task : correction_tasks_ )
      task->Run();
    for( const auto& task : correlation_tasks_ )
      task->Run();
  }

protected:
  void ConfigureJob(){
    DataTreeVarManager::GetInstance()->OpenFiles(config_.input_data);
    Selector::GetInstance()->SetTrigger(config_.trigger);
    Selector::GetInstance()->SetPid(config_.pid_code);
    switch( config_.method ){
      case Configuration::FW3S:
        ConfigureFw3s();
        break;
      case Configuration::FW3X:
        ConfigureFw3x();
        break;
      case Configuration::FWRS:
        ConfigureRnd();
        break;
      default:
        std::cout << "Error: unknown method" << std::endl;
        std::abort();
    }
  }
  void ConfigureFw3s(){
    for( int i=0; i<3; ++i ){
      correction_tasks_.push_back( new Qn::CorrectionTaskFw3s );
      correction_tasks_.back()->SetInCalibrationFile(config_.input_calib);
      std::string out_file_name = "output_" + std::to_string(i);
    }
    correlation_tasks_.push_back( new CorrelationTaskFw3s );
    correlation_tasks_.back()->AddFiles("output_2");
  }
  void ConfigureFw3x(){
    for( int i=0; i<3; ++i ){
      correction_tasks_.push_back( new Qn::CorrectionTaskFw3x );
      correction_tasks_.back()->SetInCalibrationFile(config_.input_calib);
      std::string out_file_name = "output_" + std::to_string(i);
    }
    correlation_tasks_.push_back( new CorrelationTaskFw3x );
    correlation_tasks_.back()->AddFiles("output_2");
  }
  void ConfigureRnd(){
    OccupancyCorrections::GetInstance()->SetEfficiencyFile(config_.efficiency_file);
    for( int i=0; i<3; ++i ){
      correction_tasks_.push_back( new Qn::CorrectionTaskRnd );
      correction_tasks_.back()->SetInCalibrationFile(config_.input_calib);
      std::string out_file_name = "output_" + std::to_string(i);
    }
    correlation_tasks_.push_back( new CorrelationTaskRnd );
    correlation_tasks_.back()->AddFiles("output_2");
  }
  static TaskFactory *instance_;
  std::vector<Qn::CorrectionTask*> correction_tasks_;
  std::vector<CorrelationTask*> correlation_tasks_;
  TaskFactory() = default;
  ~TaskFactory() = default;
  Configuration config_;
};

#endif // FLOW_SRC_PROCESSING_TASK_FACTORY_H_
