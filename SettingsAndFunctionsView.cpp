/*
 * SettingsAndFunctionsView.cpp
 *
 *  Created on: Jul 25, 2014
 *      Author: bastl
 */

#include "SettingsAndFunctionsView.h"
#include <BitArrayOperations.h>

SettingsAndFunctionsView::SettingsAndFunctionsView() : hw_(0),
								 settings_(0),
								 instrumentBar_(0),
								 buttonMap_(0),
								 panButtons_(0) {
}

SettingsAndFunctionsView::~SettingsAndFunctionsView() {
	//delete panButtons_;
	instrumentBar_->setActive(true);
}

void SettingsAndFunctionsView::init(ILEDsAndButtonsHW * hw, PlayerSettings * settings,
						 	 	 	 	InstrumentBar * instrumentBar, IButtonMap * buttonMap) {
	hw_ = hw;
	settings_ = settings;
	instrumentBar_ = instrumentBar;
	instrumentBar_->setActive(false);
	buttonMap_ = buttonMap;
	//panButtons_ = new RadioButtons(hw_, buttonMap_->getSubStepButtonArray(), 4);
	instrumentButtons_.init(hw_, buttonMap_->getInstrumentButtonArray(), 6);

	//Update pan buttons so it selects pan where the selected pattern is
	/*for (unsigned char i = 0; i < 4; i++) {
		bool inCurrentPan = (i == (currentPattern_ / 16));
		hw_->setLED(buttonMap_->getSubStepButtonIndex(i), inCurrentPan ? ILEDHW::ON : ILEDHW::OFF);
		if (inCurrentPan) {
			panButtons_->setSelectedButton(i);
		}
	}

	updatePan(false);

	*/

	for (unsigned char i = 0; i < 6; i++) {
		bool isOn = settings_->getDrumInstrumentEventType(i) == PlayerSettings::GATE;
		hw_->setLED(buttonMap_->getInstrumentButtonIndex(i), isOn ? ILEDHW::ON : ILEDHW::OFF);
		instrumentButtons_.setStatus(i, isOn);
	}

}

void SettingsAndFunctionsView::updatePan(bool readHWStatus) {
	/*if (readHWStatus) {
		panButtons_->update();
	}
	unsigned char selectedPan;
	panButtons_->getSelectedButton(selectedPan);
	bool inCurrentPan = (selectedPan == (currentPattern_ / 16));
	for (unsigned char i = 0; i < 16; i++) {
		bool currentButton = (i == (currentPattern_ % 16));
		hw_->setLED(buttonMap_->getStepButtonIndex(i), inCurrentPan && currentButton ? ILEDHW::ON : ILEDHW::OFF);
	}*/
}

void SettingsAndFunctionsView::update() {

	updatePan();
	instrumentButtons_.update();

	// Update playing instruments settings
	for (unsigned char i = 0; i < 6; i++) {
		PlayerSettings::DrumInstrumentEventType currentType = settings_->getDrumInstrumentEventType(i);
		PlayerSettings::DrumInstrumentEventType newType = instrumentButtons_.getStatus(i) ?
				PlayerSettings::GATE : PlayerSettings::TRIGGER;
		if (currentType != newType) {
			hw_->setLED(buttonMap_->getInstrumentButtonIndex(i), newType == PlayerSettings::GATE ? ILEDHW::ON : ILEDHW::OFF);
			settings_->setDrumInstrumentEventType(i, newType);
		}
	}

	/*for (unsigned char i = 0; i < 16; i++) {
		if (hw_->getButtonState(i) == IButtonHW::DOWN) {
			unsigned char currentPan;
			panButtons_->getSelectedButton(currentPan);
			unsigned char newPattern = currentPan * 16 + i;
			if (newPattern != currentPattern_) {
				hw_->setLED(currentPattern_ % 16, ILEDHW::OFF);
				currentPattern_ = newPattern;
				hw_->setLED(currentPattern_ % 16, ILEDHW::ON);
			}
		}
	}*/
}



