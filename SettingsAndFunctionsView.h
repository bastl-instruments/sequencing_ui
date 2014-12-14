
/*
 * SetActiveView.h
 *
 *  Created on: Jul 25, 2014
 *      Author: bastl
 */

#ifndef SETTINGSANDFUNCTIONSVIEW_H_
#define SETTINGSANDFUNCTIONSVIEW_H_

#include <IView.h>
#include <ILEDsAndButtonsHW.h>
#include <IStepMemory.h>
#include <RadioButtons.h>
#include <Switches.h>
#include <Player.h>
#include "InstrumentBar.h"
#include "IButtonMap.h"

class SettingsAndFunctionsView : public IView {
public:
	SettingsAndFunctionsView();
	~SettingsAndFunctionsView();
	void init(ILEDsAndButtonsHW * hw, PlayerSettings * settings, InstrumentBar * instrumentBar,
			  IButtonMap * buttonMap, unsigned char  selectedPattern);
	void update();
	void updatePan(bool readHWStatus = true);
	unsigned char getSelectedPattern();
private:
	ILEDsAndButtonsHW * hw_;
	PlayerSettings * settings_;
	InstrumentBar * instrumentBar_;
	IButtonMap * buttonMap_;

	RadioButtons * panButtons_;
	Switches instrumentButtons_;

	unsigned char currentPattern_;
	unsigned char currentInstrumentEventTypes_;

};

inline unsigned char SettingsAndFunctionsView::getSelectedPattern() {
	return currentPattern_;
}

#endif /* SETTINGSANDFUNCTIONSVIEW_H_ */
