// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyTextureEditorData.h"

#include "OdysseyTextureAssetUserData.h"
#include "ULISLoaderModule.h"


/////////////////////////////////////////////////////
// FOdysseyTextureEditorData
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyTextureEditorData::~FOdysseyTextureEditorData()
{
}

FOdysseyTextureEditorData::FOdysseyTextureEditorData(UTexture2D* iTexture)
    : mTextureWrapper( iTexture )
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization
void
FOdysseyTextureEditorData::Init()
{   
    FOdysseyPainterEditorData::Init();
    mTextureWrapper.OnPreSaveDelegate().AddRaw(this, &FOdysseyTextureEditorData::OnTexturePreSave);
    // Support undo/redo
    //mDisplaySurface->Texture()->SetFlags( RF_Transactional );
    //GEditor->RegisterForUndo( this );
}


//--------------------------------------------------------------------------------------
//--------------------------------------------------------- FAssetEditorToolkit override

FOdysseyLayerStack*
FOdysseyTextureEditorData::LayerStack() const
{
    return mTextureWrapper.LayerStack();
}

FOdysseyTextureWrapper&
FOdysseyTextureEditorData::TextureWrapper()
{
	return mTextureWrapper;
}
UTexture2D*
FOdysseyTextureEditorData::Texture()
{
	return mTextureWrapper.Texture();
}

FOdysseySurfaceEditable*
FOdysseyTextureEditorData::DisplaySurface()
{
	return mTextureWrapper.Surface();
}

void
FOdysseyTextureEditorData::OnTexturePreSave()
{
    PaintEngine()->Flush();
}

void
FOdysseyTextureEditorData::OnCloseRequested()
{
    PaintEngine()->Flush();

    //TODO: Move in the right place
    if (LayerStack())
    {
        LayerStack()->mDrawingUndo->Clear();
    }

    mTextureWrapper.Texture(nullptr);
}