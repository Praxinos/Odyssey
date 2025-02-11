// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyPainterEditorColorSlidersTab.h"

#include "Widgets/Color/SOdysseyColorSliders.h"
#include "OdysseyPainterEditor.h"
#include "ObjectEditorUtils.h"
#include "UObject/OdysseyObjectEditorUtils.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

const FName&
FOdysseyPainterEditorColorSlidersTab::StaticId()
{
    static FName Id = TEXT("OdysseyPainterEditor_ColorSliders");
    return Id;
}

/////////////////////////////////////////////////////
// FOdysseyPainterEditorColorSlidersTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorColorSlidersTab::~FOdysseyPainterEditorColorSlidersTab()
{
}

FOdysseyPainterEditorColorSlidersTab::FOdysseyPainterEditorColorSlidersTab(FOdysseyPainterEditor* iEditor)
    : FOdysseyEditorTab(LOCTEXT( "color-sliders-tab.name", "Color Sliders" ), FSlateIcon( "OdysseyStyle", "PainterEditor.ColorSliders_2_16" ))
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyEditorTab interface

const FName&
FOdysseyPainterEditorColorSlidersTab::GetId() const
{
    return StaticId();
}

TSharedPtr<SWidget>
FOdysseyPainterEditorColorSlidersTab::CreateWidget()
{
    return SNew( SOdysseyColorSliders )
        .Color_Raw(this, &FOdysseyPainterEditorColorSlidersTab::Color)
        .OnColorChanged_Raw(this, &FOdysseyPainterEditorColorSlidersTab::OnColorChanged);
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

::ULIS::FColor
FOdysseyPainterEditorColorSlidersTab::Color() const
{
    return mEditor->PaintColor().GetValue();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

void
FOdysseyPainterEditorColorSlidersTab::OnColorChanged( eOdysseyEventState::Type iEventState, const ::ULIS::FColor& iColor )
{
    mEditor->PaintColor( iColor, iEventState == eOdysseyEventState::kSet );
}

#undef LOCTEXT_NAMESPACE
