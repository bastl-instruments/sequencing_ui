
/*
 * SetActiveView.h
 *
 *  Created on: Jul 25, 2014
 *      Author: bastl
 */

#ifndef SETACTIVEVIEW_H_
#define SETACTIVEVIEW_H_

#include <IView.h>
#include <ILEDsAndButtonsHW.h>
#include <IStepMemory.h>
#include <RadioButtons.h>
#include <Switches.h>
#include <Player.h>
#include "InstrumentBar.h"
#include "IButtonMap.h"

class SetActiveView : public IView {
public:
	SetActiveView();
	~SetActiveView();
	void init(	ILEDsAndButtonsHW * hw, IStepMemory * memory, Player * player, InstrumentBar * instrumentBar,
			IButtonMap * buttonMap, unsigned char currentInstrumentIndex = 0);
	void update();
	unsigned char getSelectedInstrumentIndex();
private:
	ILEDsAndButtonsHW * hw_;
	IStepMemory * memory_;
	Player * player_;
	InstrumentBar * instrumentBar_;
	IButtonMap * buttonMap_;

	unsigned char currentPanIndex_;
	unsigned char currentInstrumentIndex_;

	unsigned int currentStatuses_;

	RadioButtons * panButtons_;
	RadioButtons * instrumentButtons_;
	Switches stepButtons_;

	void updateActives();
	void updateConfiguration();
	ILEDHW::LedState getLEDStateFromActiveMultiStatus(IStepMemory::ActiveMultiStatus status);
	void setActiveUpTo(unsigned char stepUpTo, bool instrumentSelected);
};

inline unsigned char SetActiveView::getSelectedInstrumentIndex() {
	return currentInstrumentIndex_;
}


#endif /* SETACTIVEVIEW_H_ */
