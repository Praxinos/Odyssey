// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditorBrushSelectorTab.h"
#include "Brush/SOdysseyBrushSelector.h"
#include "Brush/SOdysseyBrushExposedParameters.h"
#include "OdysseyBrushPreferencesOverrides.h"
#include "OdysseyBrushAssetBase.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorStrokeOptionsTab.h"
#include "OdysseyPainterEditorTopTab.h"
#include "OdysseyPainterEditorViewportTab.h"
#include "SOdysseyPaintModifiers.h"
#include "Models/OdysseyPainterEditorCommands.h"
#include "StrokeEngine/OdysseyStrokeEngine.h"
#include "ObjectEditorUtils.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorBrushSelectorTab"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorBrushSelectorTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorBrushSelectorTab::~FOdysseyPainterEditorBrushSelectorTab()
{
}

FOdysseyPainterEditorBrushSelectorTab::FOdysseyPainterEditorBrushSelectorTab(FOdysseyPainterEditor* iEditor)
	: FOdysseyEditorTab(TEXT("OdysseyPainterEditor_BrushSelector"),
                            LOCTEXT( "OdysseyPainterEditorBrushSelectorTab", "Brush Selector" ),
                            FSlateIcon( "OdysseyStyle", "PainterEditor.BrushSelector16" ))
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyEditorTab interface

TSharedPtr<SWidget>
FOdysseyPainterEditorBrushSelectorTab::CreateWidget()
{
    return SNew( SVerticalBox )
        + SVerticalBox::Slot()
            .AutoHeight()
            [
                            SNew( SOdysseyBrushSelector )
                            .Brush( this, &FOdysseyPainterEditorBrushSelectorTab::Brush )
                            .OnBrushChanged_Raw( this, &FOdysseyPainterEditorBrushSelectorTab::OnBrushSelected )
            ]
        + SVerticalBox::Slot()
            .FillHeight(1.0f)
            [
                            SNew( SOdysseyBrushExposedParameters )
                            .BrushInstance( this, &FOdysseyPainterEditorBrushSelectorTab::BrushInstance )
                            .OnParameterChanged_Raw( this, &FOdysseyPainterEditorBrushSelectorTab::OnParameterChanged )
            ];
}

void
FOdysseyPainterEditorBrushSelectorTab::BindShortcuts(FBaseToolkit* iToolkit)
{
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyPainterEditorCommands& painterEditorCommands = FOdysseyPainterEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyPainterEditorBrushSelectorTab::__VA_ARGS__ ), FCanExecuteAction() );

	MAP_ACTION(painterEditorCommands.RefreshBrush, RefreshBrush )

    #undef MAP_ACTION
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

UOdysseyBrush*
FOdysseyPainterEditorBrushSelectorTab::Brush() const
{
    return mEditor->StrokeEngine()->GetBrush();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

void
FOdysseyPainterEditorBrushSelectorTab::OnBrushSelected( UOdysseyBrush* iBrush )
{
	//mEditor->StrokeEngine()->SetBrush(iBrush);
    FObjectEditorUtils::SetPropertyValue(mEditor->StrokeEngine(), "Brush", iBrush);
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Methods

void
FOdysseyPainterEditorBrushSelectorTab::RefreshBrush()
{
    mEditor->StrokeEngine()->RefreshBrushInstance();
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

UOdysseyBrushAssetBase*
FOdysseyPainterEditorBrushSelectorTab::BrushInstance() const
{
    return mEditor->StrokeEngine()->GetBrushInstance();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

void
FOdysseyPainterEditorBrushSelectorTab::OnParameterChanged()
{   
    //TODO: Find a way to do this automatically ?
    // mEditor->PaintEngine()->TriggerStateChanged();
}


#undef LOCTEXT_NAMESPACE
