// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyTextureEditorBrushSelectorTab.h"
#include "OdysseyTextureEditor.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorBrushSelectorTab"

/////////////////////////////////////////////////////
// FOdysseyTextureEditorBrushSelectorTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyTextureEditorBrushSelectorTab::~FOdysseyTextureEditorBrushSelectorTab()
{
}

FOdysseyTextureEditorBrushSelectorTab::FOdysseyTextureEditorBrushSelectorTab(FOdysseyTextureEditor* iEditor)
	: FOdysseyPainterEditorBrushSelectorTab(iEditor)
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

void
FOdysseyTextureEditorBrushSelectorTab::OnBrushSelected( UOdysseyBrush* iBrush )
{
    mEditor->PaintEngine()->Brush(iBrush);
    if( mEditor->GetGUI()->GetTopTab()->IsEraserButtonActive() ) // this is clearly a patch, tabs shouldn't be aware of each other
    {
        mEditor->GetGUI()->GetTopTab()->SetToolDefaultAlphaMode( mEditor->PaintEngine()->GetAlphaModeModifier() );
        mEditor->GetGUI()->GetTopTab()->SetToolDefaultBlendingMode( mEditor->PaintEngine()->GetBlendingModeModifier() );
        mEditor->PaintEngine()->SetAlphaModeModifier( ::ULIS::eAlphaMode::Alpha_Erase );
        mEditor->PaintEngine()->SetBlendingModeModifier( ::ULIS::eBlendMode::Blend_Back );
        mEditor->SelectedAlphaMode( mEditor->PaintEngine()->GetAlphaModeModifier() );
    }
    else
    {
        mEditor->SelectedAlphaMode( mEditor->PaintEngine()->GetAlphaModeModifier() );
    }
    return;
}

#undef LOCTEXT_NAMESPACE

