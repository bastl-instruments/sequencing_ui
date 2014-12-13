/*
 * SetStepView.cpp
 *
 *  Created on: Jul 25, 2014
 *      Author: bastl
 */

#include "SetStepView.h"
#include <BitArrayOperations.h>


SetStepView::SetStepView() : hw_(0),
								 memory_(0),
								 player_(0),
								 instrumentBar_(0),
								 buttonMap_(0),
								 currentPattern_(0),
								 currentPanIndex_(0),
								 currentInstrumentIndex_(0),
								 currentVelocity_(DrumStep::NORMAL),
								 currentStatuses_(0),
								 panButtons_(0),
								 instrumentButtons_(0),
								 velocityRadio_(0),
								 drumStepView_(0),
								 inSubStepMode_(false),
								 instrumentCount_(10),
								 useVelocities_(false){
}

SetStepView::~SetStepView() {
	delete panButtons_;
	delete instrumentButtons_;
	if (useVelocities_) {
		delete velocityRadio_;
	}
	delete drumStepView_;
}

void SetStepView::init(ILEDsAndButtonsHW * hw, IStepMemory * memory, Player * player, InstrumentBar * instrumentBar,
		IButtonMap * buttonMap, unsigned char pattern, unsigned char instrumentCount, unsigned char initialInstrument, bool useVelocities) {
	hw_ = hw;
	memory_ = memory;
	player_ = player;
	instrumentBar_ = instrumentBar;
	buttonMap_ = buttonMap;
	currentPattern_ = pattern;
	instrumentCount_ = instrumentCount;
	useVelocities_ = useVelocities;
	currentInstrumentIndex_ = initialInstrument;
	panButtons_ = new RadioButtons(hw, buttonMap_->getSubStepButtonArray(), 4);
	subStepSwitches_.init(hw, buttonMap_->getSubStepButtonArray(), 4);
	instrumentButtons_ = new RadioButtons(hw, buttonMap_->getInstrumentButtonArray(), instrumentCount_);
	if (useVelocities_) {
		velocityRadio_ = new RadioButtons(hw_, buttonMap_->getVelocityButtonArray(), 2);
	}
	drumStepView_ = new DrumStepsView();
	drumStepView_->init(hw_, buttonMap_);
	updateConfiguration();
}

void SetStepView::updateConfiguration() {
	for (unsigned char i = 0; i < 4; i++) {
		hw_->setLED(buttonMap_->getSubStepButtonIndex(i), i == currentPanIndex_ ? ILEDHW::ON : ILEDHW::OFF);
	}
	for (unsigned char i = 0; i < instrumentCount_; i++) {
		instrumentBar_->setInstrumentSelected(i, i == currentInstrumentIndex_);
	}
	updateMutes();
}

void SetStepView::updateMutes() {
	unsigned char data[4] = {0, 0, 0, 0};
	memory_->getActivesAndMutesForNote(currentInstrumentIndex_, currentPattern_, currentPanIndex_ * 2, data);
	currentStatuses_ = ~((((unsigned int)data[3]) << 8) | data[2]);
	drumStepView_->setStatus(currentStatuses_);
}

void SetStepView::updateVelocity() {
	if (useVelocities_) {
		velocityRadio_->update();
		unsigned char newVelocity = 0;
		if (currentVelocity_ != DrumStep::DOWN) hw_->setLED(buttonMap_->getVelocityButtonIndex(0), ILEDHW::OFF);
		if (currentVelocity_ != DrumStep::UP) hw_->setLED(buttonMap_->getVelocityButtonIndex(1), ILEDHW::OFF);
		hw_->setLED(buttonMap_->getVelocityButtonIndex(1), ILEDHW::OFF);

		if (velocityRadio_->getSelectedButton(newVelocity)) {
			if (newVelocity == 0) {
				currentVelocity_ = DrumStep::DOWN;
				hw_->setLED(buttonMap_->getVelocityButtonIndex(0), ILEDHW::ON);
			} else {
				currentVelocity_ = DrumStep::UP;
				hw_->setLED(buttonMap_->getVelocityButtonIndex(1), ILEDHW::ON);
			}
		} else {
			currentVelocity_ = DrumStep::NORMAL;
		}
	}
}

void SetStepView::update() {

	instrumentButtons_->update();
	drumStepView_->update();

	updateVelocity();

	unsigned char newInstrument = 0;
	if (instrumentButtons_->getSelectedButton(newInstrument) && currentInstrumentIndex_ != newInstrument) {
		currentInstrumentIndex_ = newInstrument;
		currentPanIndex_ = 0;
		panButtons_->setSelectedButton(0);
		updateConfiguration();
		return;
	}

	unsigned char currentButtonDown = 0;
	if (drumStepView_->getDownButton(currentButtonDown) != inSubStepMode_) {
		if (!inSubStepMode_) {
			inSubStepMode_ = true;
			drumStepView_->setIgnoreOffs(false);
			DrumStep step = memory_->getDrumStep(currentInstrumentIndex_, currentPattern_, (currentPanIndex_ * 16) + currentButtonDown);
			bool anyOn = false;
			for (unsigned char i = 0; i < 4; i++) {
				bool substepHasNote = step.getSubStep(i) != DrumStep::OFF;
				anyOn = anyOn || substepHasNote;
				hw_->setLED(buttonMap_->getSubStepButtonIndex(i), substepHasNote ? ILEDHW::ON : ILEDHW::OFF);
				subStepSwitches_.setStatus(i, substepHasNote);
			}
			if (!anyOn) {
				step.setSubStep(0, currentVelocity_);
				memory_->setDrumStep(currentInstrumentIndex_, currentPattern_, (currentPanIndex_ * 16) + currentButtonDown, step);
				hw_->setLED(buttonMap_->getSubStepButtonIndex(0), ILEDHW::ON);
				subStepSwitches_.setStatus(0, true);
			}
		} else {
			inSubStepMode_ = false;
			for (unsigned char i = 0; i < 4; i++) {
				hw_->setLED(buttonMap_->getSubStepButtonIndex(i), i == currentPanIndex_ ? ILEDHW::ON : ILEDHW::OFF);
			}
		}
	}
	if (inSubStepMode_) {
		subStepSwitches_.update();
		DrumStep step = memory_->getDrumStep(currentInstrumentIndex_, currentPattern_, (currentPanIndex_ * 16) + currentButtonDown);
		for (unsigned char i = 0; i < 4; i++) {
			bool substepHasNote = step.getSubStep(i) != DrumStep::OFF;
			if (substepHasNote != subStepSwitches_.getStatus(i)) {
				drumStepView_->setIgnoreOffs(true);
				step.setSubStep(i, substepHasNote ? DrumStep::OFF : currentVelocity_);
				hw_->setLED(buttonMap_->getSubStepButtonIndex(i), !substepHasNote ? ILEDHW::ON : ILEDHW::OFF);
				memory_->setDrumStep(currentInstrumentIndex_, currentPattern_, (currentPanIndex_ * 16) + currentButtonDown, step);
			}
		}
	} else {
		panButtons_->update();
	}


	unsigned char newPan = 0;
	if (panButtons_->getSelectedButton(newPan) && currentPanIndex_ != newPan) {
		currentPanIndex_ = newPan;
		updateConfiguration();
		return;
	}

	unsigned int newOffs = drumStepView_->getNewOffs();
	unsigned int newOns = drumStepView_->getNewOns();
	if (newOffs != 0 || newOns != 0) {
		for (unsigned char i = 0; i < 16; i++) {
			bool changeToOn = GETBIT(newOns, i);
			bool changeToOff = GETBIT(newOffs, i);
			if (changeToOn || changeToOff) {
				DrumStep step = memory_->getDrumStep(currentInstrumentIndex_, currentPattern_, (currentPanIndex_ * 16) + i);
				step.setMuted(changeToOff);
				memory_->setDrumStep(currentInstrumentIndex_, currentPattern_, (currentPanIndex_ * 16) + i, step);
			}

		}
	}

	unsigned char nextStep = player_->getCurrentInstrumentStep(currentInstrumentIndex_);
	if (nextStep / 16 == currentPanIndex_) {
		drumStepView_->setHighlightedButton(nextStep % 16);
	} else {
		drumStepView_->setHighlightedButton(-1);
	}
}

