
/*
 * SetActiveView.h
 *
 *  Created on: Jul 25, 2014
 *      Author: bastl
 */

#ifndef FUNCTIONVIEWEXTRA_H_
#define FUNCTIONVIEWEXTRA_H_

#include <IView.h>
#include <ILEDsAndButtonsHW.h>
#include <LEDRadioButtons.h>
#include <Switches.h>
//#include "InstrumentBar.h"
//#include "IButtonMap.h"

class FunctionViewExtra : public IView {
public:
	FunctionViewExtra();
	~FunctionViewExtra();
	void update();
private:
	LEDRadioButtons * triggerButtons_;
	LEDRadioButtons * swingButtons_;
};

#endif /* FUNCTIONVIEWEXTRA_H_ */
