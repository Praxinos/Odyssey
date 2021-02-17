// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditorBrushExposedParametersTab.h"

#include "Brush/SOdysseyBrushExposedParameters.h"
#include "OdysseyPainterEditor.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorBrushExposedParametersTab"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorBrushExposedParametersTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorBrushExposedParametersTab::~FOdysseyPainterEditorBrushExposedParametersTab()
{
}

FOdysseyPainterEditorBrushExposedParametersTab::FOdysseyPainterEditorBrushExposedParametersTab(FOdysseyPainterEditor* iEditor)
	: FOdysseyPainterEditorTab(TEXT("OdysseyPainterEditor_BrushExposedParameters"),
                            LOCTEXT( "OdysseyPainterEditorBrushExposedParametersTab", "Brush Parameters" ),
                            FSlateIcon( "OdysseyStyle", "PainterEditor.BrushExposedParameters16" ))
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyPainterEditorTab interface

TSharedPtr<SWidget>
FOdysseyPainterEditorBrushExposedParametersTab::CreateWidget()
{
    //TODO: Should have a Brush or BrushInstance Getter
	return SNew( SOdysseyBrushExposedParameters )
        .BrushInstance(this, &FOdysseyPainterEditorBrushExposedParametersTab::BrushInstance )
        .OnParameterChanged_Raw(this, &FOdysseyPainterEditorBrushExposedParametersTab::OnParameterChanged );
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

UOdysseyBrushAssetBase*
FOdysseyPainterEditorBrushExposedParametersTab::BrushInstance() const
{
    return mEditor->PaintEngine()->BrushInstance();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

void
FOdysseyPainterEditorBrushExposedParametersTab::OnParameterChanged()
{   
    mEditor->PaintEngine()->TriggerStateChanged();
}

#undef LOCTEXT_NAMESPACE
