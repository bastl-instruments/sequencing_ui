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
								 quantizationButtons_(0) {
}

SettingsAndFunctionsView::~SettingsAndFunctionsView() {
	delete quantizationButtons_;
	instrumentBar_->setActive(true);
}


void SettingsAndFunctionsView::init(ILEDsAndButtonsHW * hw, PlayerSettings * settings,
						 	 	 	 	InstrumentBar * instrumentBar, IButtonMap * buttonMap) {
	hw_ = hw;
	settings_ = settings;
	instrumentBar_ = instrumentBar;
	instrumentBar_->setActive(false);
	buttonMap_ = buttonMap;
	quantizationButtons_ = new RadioButtons(hw_, buttonMap_->getSubStepButtonArray(), 3);
	instrumentButtons_.init(hw_, buttonMap_->getInstrumentButtonArray(), 6);
	char buttonIndex = ((char)(settings_->getRecordQuantizationType())) - 1;
	if (buttonIndex != -1) {
		quantizationButtons_->setSelectedButton(buttonIndex);
	}

	reflectQuantizationSettings();

	for (unsigned char i = 0; i < 6; i++) {
		bool isOn = settings_->getDrumInstrumentEventType(i) == PlayerSettings::GATE;
		hw_->setLED(buttonMap_->getInstrumentButtonIndex(i), isOn ? ILEDHW::ON : ILEDHW::OFF);
		instrumentButtons_.setStatus(i, isOn);
	}

}

void SettingsAndFunctionsView::reflectQuantizationSettings() {
	unsigned char index = (char)(settings_->getRecordQuantizationType());
	for (unsigned char buttonIndex = 0; buttonIndex < 3; buttonIndex++) {
		hw_->setLED(buttonMap_->getSubStepButtonIndex(buttonIndex),
				buttonIndex < index ? ILEDHW::ON : ILEDHW::OFF);
	}
}


void SettingsAndFunctionsView::update() {

	instrumentButtons_.update();
	quantizationButtons_->update();

	unsigned char quantizationIndex = 0;
	PlayerSettings::QuantizationType newType = quantizationButtons_->getSelectedButton(quantizationIndex) ?
			(PlayerSettings::QuantizationType)(quantizationIndex + 1) : PlayerSettings::_1_64;

	if (newType != settings_->getRecordQuantizationType()) {
		settings_->setRecordQuantizationType(newType);
		reflectQuantizationSettings();
	}

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
}



