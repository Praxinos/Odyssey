// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPainterEditorColorWheelTab.h"

#include "Color/SOdysseyColorSelector.h"
#include "OdysseyPainterEditor.h"
#include "UObject/OdysseyObjectEditorUtils.h"

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
    return mEditor->PaintColor().GetValue();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

void
FOdysseyPainterEditorColorWheelTab::OnColorChange( eOdysseyEventState::Type iEventState, const ::ULIS::FColor& iColor )
{
    mEditor->PaintColor( iColor, iEventState == eOdysseyEventState::kSet );
}

#undef LOCTEXT_NAMESPACE
