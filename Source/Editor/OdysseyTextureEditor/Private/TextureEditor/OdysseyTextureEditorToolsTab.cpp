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

void
FOdysseyTextureEditorToolsTab::Clear()
{
    if(!mEditor->LayerStack())
        return;

	if( mEditor->LayerStack()->GetCurrentLayer() == NULL )
        return;

    //Record
    mEditor->LayerStack()->mDrawingUndo->StartRecord();
	mEditor->LayerStack()->mDrawingUndo->SaveData( 0, 0, mEditor->LayerStack()->Width(), mEditor->LayerStack()->Height() );
	mEditor->LayerStack()->mDrawingUndo->EndRecord();
    //EndRecord

    FOdysseyPainterEditorToolsTab::Clear();
}

void
FOdysseyTextureEditorToolsTab::Fill()
{
    if(!mEditor->LayerStack())
        return;

	if( mEditor->LayerStack()->GetCurrentLayer() == NULL )
        return;

    //Record
    mEditor->LayerStack()->mDrawingUndo->StartRecord();
	mEditor->LayerStack()->mDrawingUndo->SaveData( 0, 0, mEditor->LayerStack()->Width(), mEditor->LayerStack()->Height() );
	mEditor->LayerStack()->mDrawingUndo->EndRecord();
    //EndRecord

    FOdysseyPainterEditorToolsTab::Fill();
}

void
FOdysseyTextureEditorToolsTab::Undo()
{
    FOdysseyPainterEditorToolsTab::Undo();

    if(!mEditor->LayerStack())
        return;
    
	mEditor->LayerStack()->mDrawingUndo->LoadData();
}

void
FOdysseyTextureEditorToolsTab::Redo()
{
    FOdysseyPainterEditorToolsTab::Redo();

    if(!mEditor->LayerStack())
        return;
    
	mEditor->LayerStack()->mDrawingUndo->Redo();
}

void
FOdysseyTextureEditorToolsTab::ClearUndo()
{
    if(!mEditor->LayerStack())
        return;
    
	mEditor->LayerStack()->mDrawingUndo->Clear();
    FOdysseyPainterEditorToolsTab::ClearUndo();
}

#undef LOCTEXT_NAMESPACE
