/*
 * SetActiveView.cpp
 *
 *  Created on: Jul 25, 2014
 *      Author: bastl
 */

#include "SetActiveView.h"
#include <BitArrayOperations.h>

SetActiveView::SetActiveView() : hw_(0),
								 memory_(0),
								 player_(0),
								 instrumentBar_(0),
								 buttonMap_(0),
								 currentPanIndex_(0),
								 currentInstrumentIndex_(0),
								 currentStatuses_(0),
								 panButtons_(0),
								 instrumentButtons_(0){
}

SetActiveView::~SetActiveView() {
	delete panButtons_;
	delete instrumentButtons_;
}

void SetActiveView::init(ILEDsAndButtonsHW * hw, IStepMemory * memory, Player * player,
						 InstrumentBar * instrumentBar, IButtonMap * buttonMap,
						 unsigned char currentInstrumentIndex) {
	hw_ = hw;
	memory_ = memory;
	player_ = player;
	instrumentBar_ = instrumentBar;
	buttonMap_ = buttonMap;
	currentInstrumentIndex_ = currentInstrumentIndex;
	panButtons_ = new RadioButtons(hw_, buttonMap_->getSubStepButtonArray(), 4);
	instrumentButtons_ = new RadioButtons(hw_, buttonMap_->getInstrumentButtonArray(), 6);
	updateConfiguration();
}

void SetActiveView::updateConfiguration() {
	for (unsigned char i = 0; i < 4; i++) {
		hw_->setLED(buttonMap_->getSubStepButtonIndex(i), i == currentPanIndex_ ? ILEDHW::ON : ILEDHW::OFF);
	}
	for (unsigned char i = 0; i < 6; i++) {
		instrumentBar_->setInstrumentSelected(i, i == currentInstrumentIndex_);
	}
	updateActives();
}

void SetActiveView::updateActives() {
	unsigned char data[4] = {0, 0, 0, 0};
	memory_->getActivesAndMutesForNote(currentInstrumentIndex_, currentPanIndex_ * 2,  data);
	for (unsigned char i = 0; i < 16; i++) {
		bool stepActive = GETBIT(data[i / 8], i % 8);
		hw_->setLED(buttonMap_->getStepButtonIndex(i), stepActive ? ILEDHW::ON : ILEDHW::OFF);
	}
}

void SetActiveView::update() {

	panButtons_->update();
	instrumentButtons_->update();

	unsigned char newInstrument = 0;
	if (instrumentButtons_->getSelectedButton(newInstrument) && currentInstrumentIndex_ != newInstrument) {
		currentInstrumentIndex_ = newInstrument;
		currentPanIndex_ = 0;
		panButtons_->setSelectedButton(0);
		updateConfiguration();
		return;
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

