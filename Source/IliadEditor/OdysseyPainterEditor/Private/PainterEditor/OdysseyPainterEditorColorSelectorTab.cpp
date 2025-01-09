// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyPainterEditorColorSelectorTab.h"

#include "Color/SOdysseyColorSelector.h"
#include "OdysseyPainterEditor.h"
#include "UObject/OdysseyObjectEditorUtils.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

const FName&
FOdysseyPainterEditorColorSelectorTab::StaticId()
{
    static FName Id = TEXT("OdysseyPainterEditor_ColorSelector"); //Keep ColorSelector instead of ColorWheel because changing that ID would show an empty panel to users who already opened the previous ColorSelector Panel
    return Id;
}

/////////////////////////////////////////////////////
// FOdysseyPainterEditorColorSelectorTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorColorSelectorTab::~FOdysseyPainterEditorColorSelectorTab()
{
}

FOdysseyPainterEditorColorSelectorTab::FOdysseyPainterEditorColorSelectorTab(FOdysseyPainterEditor* iEditor)
    : FOdysseyEditorTab(LOCTEXT( "color-selector-tab.name", "Color Selector" ), FSlateIcon( "OdysseyStyle", "PainterEditor.ColorWheel16" ))
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyEditorTab interface

TSharedPtr<SWidget>
FOdysseyPainterEditorColorSelectorTab::CreateWidget()
{
    return SNew( SOdysseyColorSelector )
        .Color_Raw(this, &FOdysseyPainterEditorColorSelectorTab::Color)
        .OnColorChange_Raw(this, &FOdysseyPainterEditorColorSelectorTab::OnColorChange);
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

const FName&
FOdysseyPainterEditorColorSelectorTab::GetId() const
{
    return StaticId();
}

::ULIS::FColor
FOdysseyPainterEditorColorSelectorTab::Color() const
{
    return mEditor->PaintColor().GetValue();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

void
FOdysseyPainterEditorColorSelectorTab::OnColorChange( eOdysseyEventState::Type iEventState, const ::ULIS::FColor& iColor )
{
    mEditor->PaintColor( iColor, iEventState == eOdysseyEventState::kSet );
}

#undef LOCTEXT_NAMESPACE
