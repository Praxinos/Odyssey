// IDDN FR.001.250001.004.S.X.2019.000.00000
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
	: FOdysseyPainterEditorTab(TEXT("OdysseyPainterEditor_ColorSelector"), //Keep ColorSelector instead of ColorWheel because changing that ID would show an empty panel to users who already opened the previous ColorSelector Panel
                            LOCTEXT( "OdysseyPainterEditorColorWheelTab", "Color Wheel" ),
                            FSlateIcon( "OdysseyStyle", "PainterEditor.ColorWheel_16" ))
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyPainterEditorTab interface

TSharedPtr<SWidget>
FOdysseyPainterEditorColorWheelTab::CreateWidget()
{
	return SNew( SOdysseyColorSelector )
		.Color_Raw(this, &FOdysseyPainterEditorColorWheelTab::Color)
        .OnColorChange_Raw(this, &FOdysseyPainterEditorColorWheelTab::OnColorChange);
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

::ul3::FPixelValue
FOdysseyPainterEditorColorWheelTab::Color() const
{
    return mEditor->PaintColor();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

void
FOdysseyPainterEditorColorWheelTab::OnColorChange( eOdysseyEventState::Type iEventState, const ::ul3::FPixelValue& iColor )
{
    mEditor->PaintColor(iColor);

    if (iEventState == eOdysseyEventState::kSet)
    {
	    mEditor->PaintEngine()->SetColor( iColor );
    }
}

#undef LOCTEXT_NAMESPACE
