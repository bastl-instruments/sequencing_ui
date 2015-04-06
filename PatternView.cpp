/*
 * PatternView.cpp
 *
 *  Created on: Jul 25, 2014
 *      Author: bastl
 */

#include "PatternView.h"
#include <BitArrayOperations.h>
#include "SekvojModulePool.h"

PatternView::PatternView() : patternSelectRadioButtons_(0),
							 panSelectRadioButtons_(0),
							 currentPattern_(0),
							 currentPan_(0) {
}


PatternView::~PatternView() {
	delete patternSelectRadioButtons_;
	delete panSelectRadioButtons_;
}

void PatternView::init() {
	currentPattern_ = SekvojModulePool::settings_->getCurrentPattern();
	currentPan_ = currentPattern_ / 16;

	instrumentSwitches_.init(SekvojModulePool::hw_, SekvojModulePool::buttonMap_->getInstrumentButtonArray(), 6);
	patternSelectRadioButtons_ = new LEDRadioButtons(SekvojModulePool::hw_, SekvojModulePool::buttonMap_->getStepButtonArray(), 16);
	patternSelectRadioButtons_->setSelectedButton(currentPattern_ % 16);
	panSelectRadioButtons_ = new LEDRadioButtons(SekvojModulePool::hw_, SekvojModulePool::buttonMap_->getSubStepButtonArray(), 4);
	panSelectRadioButtons_->setSelectedButton(currentPan_);

	//Present instrument settings
	for (unsigned char i = 0; i < 6; i++) {
		bool instrumentStatus = SekvojModulePool::settings_->isInstrumentOn(i);
		SekvojModulePool::instrumentBar_->setInstrumentSelected(i, instrumentStatus);
		instrumentSwitches_.setStatus(i, instrumentStatus);
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
	} else {
		panSelectRadioButtons_->setSelectedButton(currentPan_);
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
		SekvojModulePool::settings_->setCurrentPattern(newPattern);
		currentPattern_ = newPattern;
		return;
	}
	for (unsigned char i = 0; i < 6; i++) {
		bool newStatus = instrumentSwitches_.getStatus(i);
		bool oldStatus = SekvojModulePool::settings_->isInstrumentOn(i);
		if (newStatus != oldStatus) {
			SekvojModulePool::settings_->setInstrumentOn(i, newStatus);
			SekvojModulePool::instrumentBar_->setInstrumentSelected(i, newStatus);
		}
	}
}
