// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditorBrushSelectorTab.h"

#include "Brush/SOdysseyBrushSelector.h"
#include "OdysseyBrushPreferencesOverrides.h"
#include "OdysseyBrushAssetBase.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorStrokeOptionsTab.h"
#include "OdysseyPainterEditorTopTab.h"
#include "OdysseyPainterEditorViewportTab.h"
#include "SOdysseyPaintModifiers.h"
#include "SOdysseyStrokeOptions.h"
#include "PainterEditor/OdysseyPainterEditorState.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorBrushSelectorTab"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorBrushSelectorTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorBrushSelectorTab::~FOdysseyPainterEditorBrushSelectorTab()
{
}

FOdysseyPainterEditorBrushSelectorTab::FOdysseyPainterEditorBrushSelectorTab(FOdysseyPainterEditor* iEditor)
	: FOdysseyPainterEditorTab(TEXT("OdysseyPainterEditor_BrushSelector"),
                            LOCTEXT( "OdysseyPainterEditorBrushSelectorTab", "Brush Selector" ),
                            FSlateIcon( "OdysseyStyle", "PainterEditor.BrushSelector16" ))
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyPainterEditorTab interface

TSharedPtr<SWidget>
FOdysseyPainterEditorBrushSelectorTab::CreateWidget()
{
	return SNew( SOdysseyBrushSelector )
        .Brush(this, &FOdysseyPainterEditorBrushSelectorTab::Brush)
        .OnBrushChanged_Raw( this, &FOdysseyPainterEditorBrushSelectorTab::OnBrushSelected );
}

TSharedRef< SDockTab >
FOdysseyPainterEditorBrushSelectorTab::SpawnTab( const FSpawnTabArgs& iArgs )
{
    check( iArgs.GetTabId() == ID() );

    return SNew( SDockTab )
        .Label( DisplayName() )
        .ShouldAutosize( true )
        [
            Widget().ToSharedRef()
        ];
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

UOdysseyBrush*
FOdysseyPainterEditorBrushSelectorTab::Brush() const
{
    return mEditor->PaintEngine()->Brush();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

void
FOdysseyPainterEditorBrushSelectorTab::OnBrushSelected( UOdysseyBrush* iBrush )
{
	mEditor->PaintEngine()->Brush(iBrush);
}

#undef LOCTEXT_NAMESPACE
