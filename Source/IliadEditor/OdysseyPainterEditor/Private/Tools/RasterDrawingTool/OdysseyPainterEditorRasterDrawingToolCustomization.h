// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "IDetailCustomization.h"

class UOdysseyPainterEditorRasterDrawingTool;

class FOdysseyPainterEditorRasterDrawingToolCustomization : public IDetailCustomization
{
public:
    /** IDetailCustomization interface */
    virtual void CustomizeDetails( IDetailLayoutBuilder& DetailBuilder ) override;

private:
    UOdysseyPainterEditorRasterDrawingTool* GetTool( const IDetailLayoutBuilder& iDetailBuilder );

private:
    UOdysseyPainterEditorRasterDrawingTool* mTool;
};
