// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVector.h"

#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVectorImageRenderer.h"
#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVectorExport.h"
#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVectorImport.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"
#include "ULISLoaderModule.h"
#include "OdysseyMediaVector.h"
#include "OdysseyVectorBlock.h"
#include "Misc/OdysseyDuplicate.h"
#include "OdysseyAnimation.h"
// from module OdysseyFile
#include "OdysseyFile.h"

UOdysseyAnimationCellImageVector::~UOdysseyAnimationCellImageVector()
{
    delete mEngine;
    mEngine = nullptr;
}

void
UOdysseyAnimationCellImageVector::PostInitProperties()
{
	Super::PostInitProperties();
	
	if (GetFlags() & RF_ClassDefaultObject)
		return;

    // bind refresh function to delegates on existing vector scenes at load. Needed to refresh necessary widgets.
    UOdysseyAnimationLayerImageVector::OnIsColoredChanged().AddUObject( this, &UOdysseyAnimationCellImageVector::OnIsColoredChanged );
    UOdysseyAnimationLayerImageVector::OnIsWireframeChanged().AddUObject( this, &UOdysseyAnimationCellImageVector::OnIsWireframeChanged );

	mVectorBlockId = FGuid::NewGuid();
    mVectorBlock = MakeShared<FOdysseyVectorBlock>();
    mVectorBlock->OnInvalidated().AddUObject(this, &UOdysseyAnimationCellImageVector::OnVectorBlockInvalidated);	

	UOdysseyAnimation* animation = GetAnimation();
	if (animation->Width() < 0 || animation->Height() < 0)
		return;

	mEngine = new FOdysseyVectorEngine( new FOdysseyVectorGroupPaint( "Scene" )
									, (double)animation->Width()
									, (double)animation->Height() );
    
    mVectorBlock->Init(mVectorBlockId, mEngine, animation->Width(), animation->Height(), animation->Format());
}

FOdysseyVectorEngine*
UOdysseyAnimationCellImageVector::GetEngine() const
{
    return mEngine;
}

TSharedPtr<FOdysseyVectorBlock>
UOdysseyAnimationCellImageVector::GetVectorBlock() const
{
    return mVectorBlock;
}

FGuid
UOdysseyAnimationCellImageVector::GetVectorBlockId()
{
    return mVectorBlockId;
}

void
UOdysseyAnimationCellImageVector::SetVectorBlockId( FGuid iVectorBlockId )
{
    mVectorBlockId = iVectorBlockId;
}

void
UOdysseyAnimationCellImageVector::Serialize(FArchive& Ar)
{
    Super::Serialize(Ar);
	
	if (GetFlags() & RF_ClassDefaultObject)
		return;

    if( Ar.IsSaving() )
    {
        FOdysseyAnimationCellImageVectorExport::Write( this, Ar );
    }

    if( Ar.IsLoading() )
    {
        if (!FOdysseyAnimationCellImageVectorImport::Read( this, Ar ))
        {
            //Old Style No Chunk Loading
            checkf(false, TEXT("Failed to read chunks"));
        }
    }
}

void
UOdysseyAnimationCellImageVector::OldSerialize(FArchive& Ar)
{
    Super::OldSerialize(Ar);

    if( Ar.IsSaving() )
    {
        FOdysseyAnimationCellImageVectorExport::Write( this, Ar );
    }

    if( Ar.IsLoading() )
    {
        if (!FOdysseyAnimationCellImageVectorImport::Read( this, Ar ))
        {
            //Old Style No Chunk Loading
            checkf(false, TEXT("Failed to read chunks"));
        }
    }
}

void
UOdysseyAnimationCellImageVector::PostLoad()
{
	Super::PostLoad();
	
	if (GetFlags() & RF_ClassDefaultObject)
		return;

	UOdysseyAnimation* animation = GetAnimation();
    mVectorBlock->Init(mVectorBlockId, mEngine, animation->Width(), animation->Height(), animation->Format());
    mEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY );
}

void
UOdysseyAnimationCellImageVector::OnIsColoredChanged(UOdysseyAnimationLayerImageVector* iLayer)
{
    if (iLayer != GetLayer())
        return;

    mEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyAnimationCellImageVector::OnIsWireframeChanged(UOdysseyAnimationLayerImageVector* iLayer)
{
    if (iLayer != GetLayer())
        return;

    mEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyAnimationCellImageVector::IsImageRenderingGameThreadOnly() const
{
    TSharedPtr<FOdysseyMediaVector> mediaVector = mMediaVector.Pin();
    return !!mediaVector;
}

TSharedPtr<IOdysseyImageRenderer>
UOdysseyAnimationCellImageVector::BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame, FImageRendererFilter iFilter) const
{
    if (iFilter.IsBound() && !iFilter.Execute(this))
        return nullptr;
    
    return MakeShared<FOdysseyAnimationCellImageVectorImageRenderer>(this, iFrame, iRenderType, GetImageRenderingRects(), iFilter);
}

TArray<FGuid>
UOdysseyAnimationCellImageVector::GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrameIndex) const
{
    return { GetImageRenderingId() };
}

TArray<::ULIS::FRectI>
UOdysseyAnimationCellImageVector::GetImageRenderingRects() const
{
	UOdysseyAnimation* animation = GetAnimation();
	if (!animation)
		return {};

    return { ::ULIS::FRectI::FromXYWH(0, 0, animation->Width(), animation->Height()) };
}

FOdysseyMediaProvider
UOdysseyAnimationCellImageVector::GetMediaProvider(uint32 iFrameIndex) const
{
    //Don't create a mediaRaster if there is an image render in use
    FScopeLock lock(&mImageRenderingMutex);

    TSharedPtr<FOdysseyMediaVector> mediaVector = MakeShared<FOdysseyMediaVector>(mEngine->GetScene());
    mMediaVector = mediaVector;
    FOdysseyMediaProvider mediaProvider;
    mediaProvider.Add(mediaVector);
    return mediaProvider;
}

FCriticalSection*
UOdysseyAnimationCellImageVector::GetImageRenderingMutex() const
{
    return &mImageRenderingMutex;
}

void
UOdysseyAnimationCellImageVector::OnVectorBlockInvalidated( const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive)
{
    ImageRenderingChanged( iRects, iIsInteractive);
}
