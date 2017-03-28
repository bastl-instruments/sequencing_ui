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
#include <PlayerSettings.h>
#include "DrumStepsView.h"
#include "InstrumentBar.h"
#include "IButtonMap.h"

class SetStepView : public IView {
public:
	void init(unsigned char pattern, unsigned char instrumentCount,
			  unsigned char initialInstrument /*, bool useVelocities*/, unsigned char selectedBar);
	void update();
	void updateMutes();
	void updateConfiguration();
	//void updateVelocity();
	unsigned char getSelectedIndstrumentIndex();
	unsigned char getSelectedBarIndex();
	void setPlaying(bool isPlaying);
private:

	unsigned char currentPattern_;
	unsigned char currentPanIndex_;
	unsigned char currentInstrumentIndex_;

	unsigned int currentStatuses_;

	RadioButtons panButtons_;
	RadioButtons instrumentButtons_;
	//RadioButtons * velocityRadio_;
	Switches subStepSwitches_;
	DrumStepsView drumStepView_;

	bool inSubStepMode_;

	unsigned char instrumentCount_;
	//bool useVelocities_;
	bool isPlaying_;
	void updateCursor();
};

inline void SetStepView::setPlaying(bool isPlaying) {
	isPlaying_ = isPlaying;
}

inline unsigned char SetStepView::getSelectedIndstrumentIndex() {
	return currentInstrumentIndex_;
}

inline unsigned char SetStepView::getSelectedBarIndex() {
	return currentPanIndex_;
}

#endif /* SETSTEPVIEW_H_ */
