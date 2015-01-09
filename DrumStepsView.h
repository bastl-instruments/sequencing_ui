/*
 * drumStepsView.h
 *
 *  Created on: Jul 29, 2014
 *      Author: bastl
 */

#ifndef DRUMSTEPSVIEW_H_
#define DRUMSTEPSVIEW_H_

#include <IView.h>
#include <ILEDsAndButtonsHW.h>
#include <Switches.h>
#include "IButtonMap.h"

class DrumStepsView : public IView {
public:
	DrumStepsView();
	~DrumStepsView();
	void init(ILEDsAndButtonsHW * hw, IButtonMap * buttonMap);
	virtual void update();
	void setStatus(unsigned int status);
	bool getDownButton(unsigned char & button);
	unsigned int getNewOffs();
	unsigned int getNewOns();
	void setHighlightedButton(unsigned char index);
	void setIgnoreOffs(bool ignoreOffs);
	void setIgnoreAll(bool ignoreAll);
private:
	ILEDsAndButtonsHW * hw_;
	IButtonMap * buttonMap_;
	unsigned int currentStatus_;
	unsigned int lastStatus_;
	Switches stepSwitches_;
	char currentDownButton_;
	bool ignoreOffs_;
	bool ignoreAll_;
	char highlightedButton_;
};

inline bool DrumStepsView::getDownButton(unsigned char & button) {
	if (currentDownButton_ != -1) {
		button = currentDownButton_;
		return true;
	}
	return false;
}

inline void DrumStepsView::setIgnoreOffs(bool ignoreOffs) {
	ignoreOffs_ = ignoreOffs;
}

inline void DrumStepsView::setIgnoreAll(bool ignoreAll) {
	ignoreAll_ = ignoreAll;
}


#endif /* DRUMSTEPSVIEW_H_ */
