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


class PlayRecordView : public IView {
public:
	PlayRecordView();
	~PlayRecordView();
	void init(IButtonHW * hw, StepRecorder * recorder, IButtonMap * buttonMap);
	virtual void update();
private:
	IButtonHW * hw_;
	StepRecorder * recorder_;
	IButtonMap * buttonMap_;
	unsigned int currentStatus_;
};

#endif /* PLAYRECORDVIEW_H_ */
