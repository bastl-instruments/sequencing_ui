/*
 * PatternView.cpp
 *
 *  Created on: Jul 25, 2014
 *      Author: bastl
 */

#include "PatternView.h"
#include <BitArrayOperations.h>

PatternView::PatternView() : hw_(0), settings_(0), memory_(0), instrumentBar_(0),
							 patternSelectRadioButtons_(0),
							 currentPattern_(0) {
  currentInstrumentStatuses_ = 0;
}

PatternView::~PatternView() {
	delete patternSelectRadioButtons_;
}

void PatternView::init(ILEDsAndButtonsHW * hw, PlayerSettings * settigns, IStepMemory * memory, InstrumentBar * instrumentBar, IButtonMap * buttonMap) {
	hw_ = hw;
	settings_ = settigns;
	memory_ = memory;
	instrumentBar_ = instrumentBar;
	buttonMap_ = buttonMap;
	currentPattern_ = settings_->getCurrentPattern();

	instrumentSwitches_.init(hw_, buttonMap_->getInstrumentButtonArray(), 6);
	patternSelectRadioButtons_ = new RadioButtons(hw_, buttonMap_->getStepButtonArray(), 16);
	patternSelectRadioButtons_->setSelectedButton(currentPattern_);

	for (unsigned char i = 0; i < 16; i++) {
		hw_->setLED(buttonMap_->getStepButtonIndex(i), (i == currentPattern_)  ? ILEDHW::ON : ILEDHW::OFF);
	}
	reflectPatternChange();
}

void PatternView::reflectPatternChange() {
	settings_->setCurrentPattern(currentPattern_);
	memory_->getPatternSettings(/*currentPattern_*/0, &currentInstrumentStatuses_);
	for (unsigned char i = 0; i < 6; i++) {
		bool instrumentStatus = GETBIT(currentInstrumentStatuses_, i);
		instrumentBar_->setInstrumentSelected(i, instrumentStatus);
		settings_->setInstrumentOn(Step::DRUM, i, instrumentStatus);
		instrumentSwitches_.setStatus(i, instrumentStatus);
	}
}

void PatternView::update() {
	patternSelectRadioButtons_->update();
	instrumentSwitches_.update();

	unsigned char newPattern = 0;
	if (patternSelectRadioButtons_->getSelectedButton(newPattern) && newPattern != currentPattern_) {
		hw_->setLED(buttonMap_->getStepButtonIndex(currentPattern_), ILEDHW::OFF);
		hw_->setLED(buttonMap_->getStepButtonIndex(newPattern), ILEDHW::ON);
		currentPattern_ = newPattern;
		reflectPatternChange();
		return;
	}
	for (unsigned char i = 0; i < 6; i++) {
		bool newStatus = instrumentSwitches_.getStatus(i);
		bool oldStatus = GETBIT(currentInstrumentStatuses_, i);
		if (newStatus != oldStatus) {
			settings_->setInstrumentOn(Step::DRUM, i, newStatus);
			SETBIT(currentInstrumentStatuses_, i, newStatus);
			memory_->setPatternSettings(/*currentPattern_*/ 0, &currentInstrumentStatuses_);
			instrumentBar_->setInstrumentSelected(i, newStatus);
		}
	}
}
