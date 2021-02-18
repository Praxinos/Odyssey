// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyTextureEditorToolsTab.h"

#include "OdysseyTextureEditor.h"

/////////////////////////////////////////////////////
// FOdysseyTextureEditorToolsTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyTextureEditorToolsTab::~FOdysseyTextureEditorToolsTab()
{
}

FOdysseyTextureEditorToolsTab::FOdysseyTextureEditorToolsTab(FOdysseyTextureEditor* iEditor)
	: FOdysseyPainterEditorToolsTab(iEditor)
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

FReply
FOdysseyTextureEditorToolsTab::OnClear()
{
    if(!mEditor->LayerStack())
        return FReply::Handled();

	if( mEditor->LayerStack()->GetCurrentLayer() == NULL )
        return FReply::Handled();

    //Record
    mEditor->LayerStack()->mDrawingUndo->StartRecord();
	mEditor->LayerStack()->mDrawingUndo->SaveData( 0, 0, mEditor->LayerStack()->Width(), mEditor->LayerStack()->Height() );
	mEditor->LayerStack()->mDrawingUndo->EndRecord();
    //EndRecord

    return FOdysseyPainterEditorToolsTab::OnClear();
}

FReply
FOdysseyTextureEditorToolsTab::OnFill()
{
    if(!mEditor->LayerStack())
        return FReply::Handled();

	if( mEditor->LayerStack()->GetCurrentLayer() == NULL )
        return FReply::Handled();

    //Record
    mEditor->LayerStack()->mDrawingUndo->StartRecord();
	mEditor->LayerStack()->mDrawingUndo->SaveData( 0, 0, mEditor->LayerStack()->Width(), mEditor->LayerStack()->Height() );
	mEditor->LayerStack()->mDrawingUndo->EndRecord();
    //EndRecord

    return FOdysseyPainterEditorToolsTab::OnFill();
}

FReply
FOdysseyTextureEditorToolsTab::OnUndo()
{
    FOdysseyPainterEditorToolsTab::OnUndo();

    if(!mEditor->LayerStack())
        return FReply::Handled();
    
	mEditor->LayerStack()->mDrawingUndo->LoadData();
    return FReply::Handled();
}

FReply
FOdysseyTextureEditorToolsTab::OnRedo()
{
    FOdysseyPainterEditorToolsTab::OnRedo();

    if(!mEditor->LayerStack())
        return FReply::Handled();
    
	mEditor->LayerStack()->mDrawingUndo->Redo();
    return FReply::Handled();
}

FReply
FOdysseyTextureEditorToolsTab::OnClearUndo()
{
    if(!mEditor->LayerStack())
        return FReply::Handled();
    
	mEditor->LayerStack()->mDrawingUndo->Clear();
    return FOdysseyPainterEditorToolsTab::OnClearUndo();
}
#undef LOCTEXT_NAMESPACE
