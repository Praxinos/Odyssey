// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditorColorWheelTab.h"

#include "Color/SOdysseyColorSelector.h"
#include "OdysseyPainterEditor.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorColorWheelTab"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorColorWheelTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorColorWheelTab::~FOdysseyPainterEditorColorWheelTab()
{
}

FOdysseyPainterEditorColorWheelTab::FOdysseyPainterEditorColorWheelTab(FOdysseyPainterEditor* iEditor)
	: FOdysseyEditorTab(TEXT("OdysseyPainterEditor_ColorSelector")//Keep ColorSelector instead of ColorWheel because changing that ID would show an empty panel to users who already opened the previous ColorSelector Panel
    , LOCTEXT( "OdysseyPainterEditorColorWheelTab", "Color Wheel" )
    , FSlateIcon( "OdysseyStyle", "PainterEditor.ColorWheel16" ))
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyEditorTab interface

TSharedPtr<SWidget>
FOdysseyPainterEditorColorWheelTab::CreateWidget()
{
	return SNew( SOdysseyColorSelector )
		.Color_Raw(this, &FOdysseyPainterEditorColorWheelTab::Color)
        .OnColorChange_Raw(this, &FOdysseyPainterEditorColorWheelTab::OnColorChange);
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

::ULIS::FColor
FOdysseyPainterEditorColorWheelTab::Color() const
{
    return mEditor->PaintColor();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

void
FOdysseyPainterEditorColorWheelTab::OnColorChange( eOdysseyEventState::Type iEventState, const ::ULIS::FColor& iColor )
{
    mEditor->PaintColor(iColor);

    if (iEventState == eOdysseyEventState::kSet)
    {
	    mEditor->PaintEngine()->SetColor( iColor );
    }
}

#undef LOCTEXT_NAMESPACE
