// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyPainterEditorColorWheelTab.h"

#include "Widgets/Color/SOdysseyAdvancedColorWheel.h"
#include "OdysseyPainterEditor.h"
#include "UObject/OdysseyObjectEditorUtils.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

const FName&
FOdysseyPainterEditorColorWheelTab::StaticId()
{
    static FName Id = TEXT("OdysseyPainterEditor_ColorWheel"); //Keep ColorSelector instead of ColorWheel because changing that ID would show an empty panel to users who already opened the previous ColorSelector Panel
    return Id;
}

/////////////////////////////////////////////////////
// FOdysseyPainterEditorColorWheelTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorColorWheelTab::~FOdysseyPainterEditorColorWheelTab()
{
}

FOdysseyPainterEditorColorWheelTab::FOdysseyPainterEditorColorWheelTab(FOdysseyPainterEditor* iEditor)
    : FOdysseyEditorTab(LOCTEXT( "color-wheel-tab.name", "Color Wheel" ), FSlateIcon( "OdysseyStyle", "PainterEditor.ColorWheel16" ))
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyEditorTab interface

TSharedPtr<SWidget>
FOdysseyPainterEditorColorWheelTab::CreateWidget()
{
    return SNew( SOdysseyAdvancedColorWheel )
        .MinDesiredWidth(   150 )
        .MinDesiredHeight(  150 )
        .MaxDesiredWidth(   300 )
        .MaxDesiredHeight(  300 )
        .DesiredWidth(   150 )
        .DesiredHeight(  150 )
        .Color_Raw(this, &FOdysseyPainterEditorColorWheelTab::Color)
        .OnColorChanged_Raw(this, &FOdysseyPainterEditorColorWheelTab::OnColorChanged);
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

const FName&
FOdysseyPainterEditorColorWheelTab::GetId() const
{
    return StaticId();
}

::ULIS::FColor
FOdysseyPainterEditorColorWheelTab::Color() const
{
    return mEditor->PaintColor().GetValue();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

void
FOdysseyPainterEditorColorWheelTab::OnColorChanged( eOdysseyEventState::Type iEventState, const ::ULIS::FColor& iColor )
{
    mEditor->PaintColor( iColor, iEventState == eOdysseyEventState::kSet );
}

#undef LOCTEXT_NAMESPACE
