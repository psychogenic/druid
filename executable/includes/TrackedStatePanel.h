/*
 * TrackedStatePanel.h
 *
 *  Created on: Aug 10, 2014
 *      Author: malcalypse
 */

#ifndef TRACKEDSTATEPANEL_H_
#define TRACKEDSTATEPANEL_H_

#include <libDruid/SerialDruid.h>
#include "Widgets.h"
#include <map>
#include <vector>

namespace DRUID {


typedef struct TrackedStatePanelEntriesStruct {

	wxStaticText * label;
	wxStaticText * value;
	SUIUserTrackedState * tracked_state;

} TrackedStatePanelEntries;

typedef std::map<uint8_t, TrackedStatePanelEntries> TrackedStatePanelEntriesMap;



class TrackedStatePanel : public wxPanel {
public:
	TrackedStatePanel(wxWindow *parent);

	void updateDisplay(SUIUserIdxToTrackedStateVariablePtr & trackedVarData);

	void clearDisplay();


private:
	wxBoxSizer *topsizer;
	wxBoxSizer *vsizer;
	TrackedStatePanelEntriesMap tracked_entries;


};

} /* namespace DRUID */

#endif /* TRACKEDSTATEPANEL_H_ */
