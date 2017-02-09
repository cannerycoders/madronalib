
// MadronaLib: a C++ framework for DSP applications.
// Copyright (c) 2013 Madrona Labs LLC. http://www.madronalabs.com
// Distributed under the MIT license: http://madrona-labs.mit-license.org/

#ifndef __ML_SIGNAL_REPORTER_H
#define __ML_SIGNAL_REPORTER_H

// MLSignalReporter is a mixin class used by objects like MLPluginController
// that display one or more published signals from the DSP engine. 

#include "MLPluginProcessor.h"
#include "MLSignalView.h"

#pragma mark MLSignalReporter 

class MLSignalReporter
{
public:
	MLSignalReporter(MLPluginProcessor* p);
    ~MLSignalReporter();
    
 	// add a signal view entry to the map and connect it to a new signal viewer.
	MLSignalView* addSignalViewToMap(ml::Symbol p, MLWidget* w, ml::Symbol attr, int size, int priority = 0, int frameSize = 1);
	
	// view all of the signals in the map.
	void viewSignals();

protected:
	void viewChangedSignals();	
	void viewAllSignals();
	int viewOneSignal(ml::Symbol signalName, bool forceView, int priority = 0);
	void redrawSignals();
	
	typedef std::shared_ptr<MLSignalView> MLSignalViewPtr;
	typedef std::list<MLSignalViewPtr> MLSignalViewList;
	typedef std::map<ml::Symbol, MLSignalViewList> MLSignalViewListMap;
	typedef std::map<ml::Symbol, MLSignalPtr> SymbolToSignalMap;
	typedef std::map<ml::Symbol, int> ViewPriorityMap;

	MLPluginProcessor* mpProcessor;
	SymbolToSignalMap mSignalBuffers;
	SymbolToSignalMap mSignalBuffers2;
    ViewPriorityMap mViewPriorityMap;
    
    // map of view lists
	MLSignalViewListMap mSignalViewsMap;

    int mViewIndex;
    std::vector<ml::Symbol> mSignalNames;
	
	bool mNeedsRedraw;
	int mVoices;
};

#endif // __ML_SIGNAL_REPORTER_H
