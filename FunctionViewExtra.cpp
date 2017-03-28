/*
 * FunctionsViewExtra.cpp
 *
 *  Created on: Jul 25, 2014
 *      Author: bastl
 */

#include "FunctionViewExtra.h"
#include <BitArrayOperations.h>
#include "SekvojModulePool.h"

void FunctionViewExtra::init() {
	triggerButtons_.init(SekvojModulePool::hw_, SekvojModulePool::buttonMap_->getSubStepButtonArray() + 1, 3);
	//swingButtons_ = new LEDRadioButtons(SekvojModulePool::hw_, SekvojModulePool::buttonMap_->getStepButtonArray(), 4);
	triggerButtons_.setSelectedButton((char)(SekvojModulePool::settings_->getTriggerLength()));
	//swingButtons_->setSelectedButton((char)(SekvojModulePool::settings_->getMultiplication()));
}

void FunctionViewExtra::update() {

	triggerButtons_.update();
	//swingButtons_->update();

	//Quantization settings
	unsigned char triggerLengthIndex = 0;
	if (triggerButtons_.getSelectedButton(triggerLengthIndex)) {
		SekvojModulePool::settings_->setTriggerLength(triggerLengthIndex);
	} else {
		triggerButtons_.setSelectedButton(SekvojModulePool::settings_->getTriggerLength());
	}

	//Quantization settings
	//unsigned char swingIndex = 0;
	//if (triggerButtons_->getSelectedButton(swingIndex)) {
	//	SekvojModulePool::settings_->setSwing(swingIndex);
	//} else {
	//	swingButtons_->setSelectedButton(SekvojModulePool::settings_->getSwing());
	//}
}



