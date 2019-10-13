//
// Created by Lukas Kreis on 29.06.17.
//

#ifndef FLOW_QNTestTask_H
#define FLOW_QNTestTask_H

#include <vector>
#include <array>
#include <random>

#include "CorrectionManager.h"
#include "Selector.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TTreeReader.h"
#include "DataTreeVarManager.h"

#include "HADES_constants.h"

namespace Qn {
/**
 * Qn vector analysis TestTask. It is to be configured by the user.
 * @brief TestTask for analysing qn vectors
 */
class CorrectionTask {
	public:
	CorrectionTask(std::string filelist, std::string incalib);
	~CorrectionTask() = default;
	void SetSelectorConfiguration(bool perChannel=false, std::string signal="adc", float min=80.0, float max=999.0, int=14);
	void Run();

	private:
	/**
	 * Initializing function of Correction Task class. Makes configuration of Correction Manager.
	 */
	void Initialize();
	/**
	 * Processes one event;
	 */
	void Process();
	/**
	 * Finalizes TestTask. Called after processing is done.
	 */
	void Finalize();
	void ProgressBar(float progress);

	protected:

	std::shared_ptr<TFile> out_file_;
	std::shared_ptr<TFile> in_calibration_file_;
	std::shared_ptr<TFile> out_calibration_file_;
	std::shared_ptr<DataTreeVarManager> fVarManager;
	TTree *out_tree_;
	Qn::CorrectionManager fManager;
	bool write_tree_;
};
}
#endif //FLOW_QNTASK_H
