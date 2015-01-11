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
	virtual void update();
private:
	unsigned int currentStatus_;
	unsigned char lastStepIndex_;
	bool turnOffStep_;

	unsigned int turnedSteps_;
	unsigned int turnedSubSteps_;
	bool stepReseted_;
	bool substepReseted_;

	void updateStatusView();
	void updateStatusItem(unsigned char step,
						  unsigned char stepLength,
						  unsigned int & statuses,
	  	  	  	  	  	  bool & resetStatus,
						  unsigned char * buttonsReference,
						  bool markCurrentStep = false);
};

#endif /* PLAYRECORDVIEW_H_ */
