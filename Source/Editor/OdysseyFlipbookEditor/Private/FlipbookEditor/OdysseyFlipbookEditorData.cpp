// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyFlipbookEditorData.h"

#include "ULISLoaderModule.h"
#include "PaperFlipbook.h"
#include "PaperSprite.h"
#include "OdysseyTextureAssetUserData.h"
#include "OdysseyFlipbookWrapper.h"



/////////////////////////////////////////////////////
// FOdysseyFlipbookEditorData
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyFlipbookEditorData::~FOdysseyFlipbookEditorData()
{
	if (mPreviewSurface) {
		delete mPreviewSurface;
		mPreviewSurface = nullptr;
	}
}

FOdysseyFlipbookEditorData::FOdysseyFlipbookEditorData(TSharedPtr<FOdysseyFlipbookWrapper>& iFlipbookWrapper)
    : mFlipbookWrapper( iFlipbookWrapper )
    , mTextureWrapper( nullptr )
	, mPreviewSurface(new FOdysseySurfaceReadOnly(nullptr))
{
    mTextureWrapper.OnPreSaveDelegate().AddRaw(this, &FOdysseyFlipbookEditorData::OnTexturePreSave);
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization
void
FOdysseyFlipbookEditorData::Init()
{
    FOdysseyPainterEditorData::Init();
    // Get Flipbook keyFrames
    if (mFlipbookWrapper->Flipbook()->GetNumKeyFrames() <= 0)
    {
        //We don't need to initialize anything if there is no keyFrames
        return;
    }
    Texture(mFlipbookWrapper->GetKeyframeTexture(0));
}

void
FOdysseyFlipbookEditorData::Texture(UTexture2D* iTexture)
{
    mTextureWrapper.Texture(iTexture);
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------- FAssetEditorToolkit override

FOdysseyLayerStack*
FOdysseyFlipbookEditorData::LayerStack() const
{
    return mTextureWrapper.LayerStack();
}


TSharedPtr<FOdysseyFlipbookWrapper>&
FOdysseyFlipbookEditorData::FlipbookWrapper()
{
	return mFlipbookWrapper;
}

FOdysseyTextureWrapper&
FOdysseyFlipbookEditorData::TextureWrapper()
{
	return mTextureWrapper;
}

UTexture2D*
FOdysseyFlipbookEditorData::Texture()
{
	return mTextureWrapper.Texture();
}

FOdysseySurfaceEditable*
FOdysseyFlipbookEditorData::DisplaySurface()
{
	return mTextureWrapper.Surface();
}

FOdysseySurfaceReadOnly*
FOdysseyFlipbookEditorData::PreviewSurface()
{
	return mPreviewSurface;
}

void
FOdysseyFlipbookEditorData::OnTexturePreSave()
{
    PaintEngine()->Flush();
}

void
FOdysseyFlipbookEditorData::OnCloseRequested()
{
    PaintEngine()->Flush();

    //TODO: Move in the right place
    if (LayerStack())
    {
        LayerStack()->mDrawingUndo->Clear();
    }

    mTextureWrapper.Texture(nullptr);
}
