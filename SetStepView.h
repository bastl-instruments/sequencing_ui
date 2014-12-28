/*
 * SetStepView.h
 *
 *  Created on: Jul 27, 2014
 *      Author: Martin Baar
 */

#ifndef SETSTEPVIEW_H_
#define SETSTEPVIEW_H_

#include <IView.h>
#include <ILEDsAndButtonsHW.h>
#include <IStepMemory.h>
#include <RadioButtons.h>
#include <Switches.h>
#include <Player.h>
#include "DrumStepsView.h"
#include "InstrumentBar.h"
#include "IButtonMap.h"

class SetStepView : public IView {
public:
	SetStepView();
	~SetStepView();
	void init(ILEDsAndButtonsHW * hw, IStepMemory * memory, Player * player, InstrumentBar * instrumentBar,
			IButtonMap * buttonMap, unsigned char pattern, unsigned char instrumentCount, unsigned char initialInstrument/*, bool useVelocities*/);
	void update();
	void updateMutes();
	void updateConfiguration();
	//void updateVelocity();
	unsigned char getSelectedIndstrumentIndex();
	void setPlaying(bool isPlaying);
private:
	ILEDsAndButtonsHW * hw_;
	IStepMemory * memory_;
	Player * player_;
	InstrumentBar * instrumentBar_;
	IButtonMap * buttonMap_;

	unsigned char currentPattern_;
	unsigned char currentPanIndex_;
	unsigned char currentInstrumentIndex_;
	DrumStep::DrumVelocityType currentVelocity_;

	unsigned int currentStatuses_;

	RadioButtons * panButtons_;
	RadioButtons * instrumentButtons_;
	//RadioButtons * velocityRadio_;
	Switches subStepSwitches_;
	DrumStepsView * drumStepView_;

	bool inSubStepMode_;

	unsigned char instrumentCount_;
	//bool useVelocities_;
	bool isPlaying_;

};

inline void SetStepView::setPlaying(bool isPlaying) {
	isPlaying_ = isPlaying;
}

inline unsigned char SetStepView::getSelectedIndstrumentIndex() {
	return currentInstrumentIndex_;
}

#endif /* SETSTEPVIEW_H_ */
