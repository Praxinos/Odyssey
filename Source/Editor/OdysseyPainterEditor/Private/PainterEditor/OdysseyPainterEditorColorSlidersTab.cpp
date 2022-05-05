// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPainterEditorColorSlidersTab.h"

#include "Color/SOdysseyColorSliders.h"
#include "OdysseyPainterEditor.h"
#include "ObjectEditorUtils.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorColorSlidersTab"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorColorSlidersTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorColorSlidersTab::~FOdysseyPainterEditorColorSlidersTab()
{
}

FOdysseyPainterEditorColorSlidersTab::FOdysseyPainterEditorColorSlidersTab(FOdysseyPainterEditor* iEditor)
	: FOdysseyEditorTab(TEXT("OdysseyPainterEditor_ColorSliders"),
                            LOCTEXT( "OdysseyPainterEditorColorSlidersTab", "Color Sliders" ),
                            FSlateIcon( "OdysseyStyle", "PainterEditor.ColorSliders_2_16" ))
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyEditorTab interface

TSharedPtr<SWidget>
FOdysseyPainterEditorColorSlidersTab::CreateWidget()
{
	return SNew( SOdysseyColorSliders )
		.Color_Raw(this, &FOdysseyPainterEditorColorSlidersTab::Color)
        .OnColorChange_Raw(this, &FOdysseyPainterEditorColorSlidersTab::OnColorChange);
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
FOdysseyPainterEditorColorSlidersTab::OnColorChange( eOdysseyEventState::Type iEventState, const ::ULIS::FColor& iColor )
{
    mEditor->PaintColor().SetValue( iColor );

    if (iEventState == eOdysseyEventState::kSet)
    {
	    //TriggerStateChanged
        //PATCH: should be automatic in the new drawing Tool, fix it asap

        UOdysseyDrawingTool* drawingTool = Cast<UOdysseyDrawingTool>(mEditor->GetSelectedTool());
        if (!drawingTool)
            return;
        
        FObjectEditorUtils::SetPropertyValue(drawingTool->GetBrushOptions(), "Color", FOdysseyBrushColor(mEditor->PaintColor()));
    }
}

#undef LOCTEXT_NAMESPACE
