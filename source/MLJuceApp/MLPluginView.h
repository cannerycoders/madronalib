
// MadronaLib: a C++ framework for DSP applications.
// Copyright (c) 2013 Madrona Labs LLC. http://www.madronalabs.com
// Distributed under the MIT license: http://madrona-labs.mit-license.org/

#ifndef __ML_PLUGINVIEW_H__
#define __ML_PLUGINVIEW_H__

#include "MLPluginController.h"
#include "MLAppView.h"
#include "MLPluginProcessor.h"
#include "MLAppView.h"
#include "MLPageView.h"
#include "MLDrawing.h"

// view for looking at changes of plugins. 
//
class MLPluginView : 
	public MLAppView
{
public:
	MLPluginView(MLPluginProcessor* const pProcessor, MLPluginController* pR);
    ~MLPluginView();
	
	void setGridUnits(double gx, double gy);
	void setContent(MLAppView* contentView);

	MLPluginProcessor* getProcessor() const { return mpProcessor; } // TO GO

	void addSignalView(ml::Symbol p, MLWidget* w, ml::Symbol attr, int size, int priority = 0, int frameSize = 1);

	// component setup 
	//
	MLPluginView* addSubView(const MLRect & r, const ml::Symbol name);

	// TODO all these methods for adding different kinds of Widgets have a lot of duplication.
	// instead there should be a Widget factory that can make them by name. 
	// any extra property connections / other setup can be done in a new virtual initWidget() or something.
	
	MLDial* addDial(const char * displayName, const MLRect & r, const ml::Symbol paramName, 
		const Colour& color = defaultColor);	
	MLMultiSlider* addMultiSlider(const char * displayName, const MLRect & r, const ml::Symbol paramName, int n, 
		const Colour& color = defaultColor);
	MLMultiButton* addMultiButton(const char * displayName, const MLRect & r, const ml::Symbol paramName, int n, 
		const Colour& color = defaultColor);
	MLButton* addToggleButton(const char * displayName, const MLRect & r, const char * name,
                              const Colour& color = defaultColor, const float sizeMultiplier = 1.0f);
	MLButton* addTriToggleButton(const char * displayName, const MLRect & r, const char * name,
                              const Colour& color = defaultColor, const float sizeMultiplier = 1.0f);

	MLDial* addMultDial(const MLRect & r, const ml::Symbol paramName, const Colour& color);

	MLEnvelope* addEnvelope(const MLRect & r, const ml::Symbol paramName);

protected:		
	MLPluginProcessor* mpProcessor;
	MLPluginController* mpController;

private:
    // (prevent copy constructor and operator= being generated..)
    MLPluginView (const MLPluginView&);
    const MLPluginView& operator= (const MLPluginView&);
};



#endif // __ML_PLUGINVIEW_H__