/*
 * PlayRecordView.h
 *
 *  Created on: Jul 31, 2014
 *      Author: bastl
 */

#ifndef PLAYRECORDVIEW_H_
#define PLAYRECORDVIEW_H_

#include <IView.h>
#include "IButtonMap.h"
#include <ILEDsAndButtonsHW.h>
#include <StepRecorder.h>
#include "DrumStepsView.h"
#include <Player.h>
#include <IStepMemory.h>

class PlayRecordView : public IView {
public:
	PlayRecordView();
	~PlayRecordView();
	void init(ILEDsAndButtonsHW * hw, StepRecorder * recorder, IButtonMap * buttonMap, StepSynchronizer * synchronizer);
	virtual void update();
private:
	ILEDsAndButtonsHW * hw_;
	StepRecorder * recorder_;
	IButtonMap * buttonMap_;
	unsigned int currentStatus_;
	StepSynchronizer * synchronizer_;
	unsigned char lastStepIndex_;
	bool turnOffStep_;

	unsigned int turnedSteps_;
	unsigned char turnedSubSteps_;
	bool stepReseted_;
	bool substepReseted_;

	void updateStatusView();
};

#endif /* PLAYRECORDVIEW_H_ */
