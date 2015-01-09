/*
 * SetActiveView.cpp
 *
 *  Created on: Jul 25, 2014
 *      Author: bastl
 */

#include "SetActiveAndPlayingView.h"
#include <BitArrayOperations.h>

SetActiveAndPlayingView::SetActiveAndPlayingView() : hw_(0),
								 memory_(0),
								 player_(0),
								 settings_(0),
								 instrumentBar_(0),
								 buttonMap_(0),
								 currentPattern_(0),
								 currentPanIndex_(0),
								 currentInstrumentIndex_(0),
								 currentStatuses_(0),
								 panButtons_(0){
}

SetActiveAndPlayingView::~SetActiveAndPlayingView() {
	delete panButtons_;
}

void SetActiveAndPlayingView::init(ILEDsAndButtonsHW * hw, IStepMemory * memory, Player * player, PlayerSettings * settings,
						 InstrumentBar * instrumentBar, IButtonMap * buttonMap, unsigned char selectedInstrument) {
	hw_ = hw;
	memory_ = memory;
	player_ = player;
	settings_ = settings;
	currentInstrumentIndex_ = selectedInstrument;
	instrumentBar_ = instrumentBar;
	buttonMap_ = buttonMap;
	panButtons_ = new RadioButtons(hw_, buttonMap_->getSubStepButtonArray(), 4);
	instrumentButtons_.init(hw_, buttonMap_->getInstrumentButtonArray(), 6);
	updateConfiguration();
}

void SetActiveAndPlayingView::updateConfiguration() {
	for (unsigned char i = 0; i < 4; i++) {
		hw_->setLED(buttonMap_->getSubStepButtonIndex(i), i == currentPanIndex_ ? ILEDHW::ON : ILEDHW::OFF);
	}
	for (unsigned char i = 0; i < 6; i++) {
		bool instrumentStatus = settings_->isInstrumentOn(i);
		instrumentBar_->setInstrumentSelected(i, instrumentStatus);
		instrumentButtons_.setStatus(i, instrumentStatus);
	}

	updateActives();
}

void SetActiveAndPlayingView::updateActives() {
	unsigned char * data;
	memory_->getActivesAndMutesForNote(currentInstrumentIndex_, currentPanIndex_,  data);
	for (unsigned char i = 0; i < 16; i++) {
		bool stepActive = GETBIT(data[i / 8], i % 8);
		hw_->setLED(buttonMap_->getStepButtonIndex(i), stepActive ? ILEDHW::ON : ILEDHW::OFF);
	}
}

void SetActiveAndPlayingView::update() {

	panButtons_->update();
	instrumentButtons_.update();

	// Update playing instruments settings
	for (unsigned char i = 0; i < 6; i++) {
		bool newStatus = instrumentButtons_.getStatus(i);
		bool oldStatus = settings_->isInstrumentOn(i);
		if (newStatus != oldStatus) {
			settings_->setInstrumentOn(i, newStatus);
			instrumentBar_->setInstrumentSelected(i, newStatus);
		}
	}

	unsigned char newPan = 0;
	if (panButtons_->getSelectedButton(newPan) && currentPanIndex_ != newPan) {
		currentPanIndex_ = newPan;
		updateConfiguration();
		return;
	}
	for (unsigned char i = 0; i < 16; i++) {
		bool lastState = GETBIT(currentStatuses_, i);
		bool currentState = hw_->getButtonState(buttonMap_->getStepButtonIndex(i));
		bool shift = false;
		unsigned char pressedStep = (currentPanIndex_ * 16) + i;
		if (lastState != currentState) {
			//Update
			currentState ? SETBITTRUE(currentStatuses_, i) : SETBITFALSE(currentStatuses_, i);
			if (shift) {
				DrumStep step = memory_->getDrumStep(currentInstrumentIndex_, pressedStep);
				step.setActive(!step.isActive());
				memory_->setDrumStep(currentInstrumentIndex_, pressedStep, step);
				hw_->setLED(buttonMap_->getStepButtonIndex(i), step.isActive() ? ILEDHW::ON : ILEDHW::OFF);
			} else if (currentState) {
				for (int stepIndex = 0; stepIndex < 64; stepIndex++) {
					bool newState = stepIndex <= pressedStep;
					DrumStep step = memory_->getDrumStep(currentInstrumentIndex_, stepIndex);
					step.setActive(newState);
					memory_->setDrumStep(currentInstrumentIndex_, stepIndex, step);
					if (stepIndex / 16 == currentPanIndex_) {
						hw_->setLED(buttonMap_->getStepButtonIndex(stepIndex % 16), newState ? ILEDHW::ON : ILEDHW::OFF);
					}
				}
				player_->changeActivesForCurrentStep(currentInstrumentIndex_, pressedStep + 1);
			}

		}
	}
}

