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
							 panSelectRadioButtons_(0),
							 currentPattern_(0),
							 currentPan_(0) {
}

PatternView::~PatternView() {
	delete patternSelectRadioButtons_;
	delete panSelectRadioButtons_;
}

void PatternView::init(ILEDsAndButtonsHW * hw, PlayerSettings * settigns, IStepMemory * memory, InstrumentBar * instrumentBar, IButtonMap * buttonMap) {
	hw_ = hw;
	settings_ = settigns;
	memory_ = memory;
	instrumentBar_ = instrumentBar;
	buttonMap_ = buttonMap;
	currentPattern_ = settings_->getCurrentPattern();
	currentPan_ = currentPattern_ / 16;

	instrumentSwitches_.init(hw_, buttonMap_->getInstrumentButtonArray(), 6);
	patternSelectRadioButtons_ = new RadioButtons(hw_, buttonMap_->getStepButtonArray(), 16);
	patternSelectRadioButtons_->setSelectedButton(currentPattern_ % 16);
	panSelectRadioButtons_ = new RadioButtons(hw_, buttonMap_->getSubStepButtonArray(), 4);
	panSelectRadioButtons_->setSelectedButton(currentPan_);

	updateLEDsForPan();

	//Present instrument settings
	for (unsigned char i = 0; i < 6; i++) {
		bool instrumentStatus = settings_->isInstrumentOn(Step::DRUM, i);
		instrumentBar_->setInstrumentSelected(i, instrumentStatus);
		instrumentSwitches_.setStatus(i, instrumentStatus);
	}
}

void PatternView::updateLEDsForPan() {

	for (unsigned char panIndex = 0; panIndex < 4; panIndex++) {
		hw_->setLED(buttonMap_->getSubStepButtonIndex(panIndex), currentPan_ == panIndex ? ILEDHW::ON : ILEDHW::OFF);
	}

	unsigned char selectedButtonIndex;
	bool itemSelected = patternSelectRadioButtons_->getSelectedButton(selectedButtonIndex);
	for (unsigned char i = 0; i < 16; i++) {
		hw_->setLED(buttonMap_->getStepButtonIndex(i), (itemSelected && (selectedButtonIndex == i)) ? ILEDHW::ON : ILEDHW::OFF);
	}
}

void PatternView::updatePan() {
	panSelectRadioButtons_->update();
	unsigned char newPan;
	if (panSelectRadioButtons_->getSelectedButton(newPan) && newPan != currentPan_) {
		if (currentPattern_ / 16 == currentPan_) {
			patternSelectRadioButtons_->resetSelection();
		}
		currentPan_ = newPan;
		if (currentPattern_ / 16 == currentPan_) {
			patternSelectRadioButtons_->setSelectedButton(currentPattern_ % 16);
		}
		updateLEDsForPan();
	}
}

void PatternView::update() {

	updatePan();

	patternSelectRadioButtons_->update();
	instrumentSwitches_.update();

	unsigned char newPattern = 0;
	bool somethingSelected = patternSelectRadioButtons_->getSelectedButton(newPattern);
	newPattern += (currentPan_ * 16);
	if (somethingSelected && (newPattern != currentPattern_)) {
		hw_->setLED(buttonMap_->getStepButtonIndex(currentPattern_ % 16), ILEDHW::OFF);
		hw_->setLED(buttonMap_->getStepButtonIndex(newPattern % 16), ILEDHW::ON);
		settings_->setCurrentPattern(newPattern);
		currentPattern_ = newPattern;
		return;
	}
	for (unsigned char i = 0; i < 6; i++) {
		bool newStatus = instrumentSwitches_.getStatus(i);
		bool oldStatus = settings_->isInstrumentOn(Step::DRUM, i);
		if (newStatus != oldStatus) {
			settings_->setInstrumentOn(Step::DRUM, i, newStatus);
			instrumentBar_->setInstrumentSelected(i, newStatus);
		}
	}
}
