// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVector.h"

#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVectorImageRenderer.h"
#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVectorExport.h"
#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVectorImport.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"
#include "ULISLoaderModule.h"
#include "OdysseyVector.h"
#include "OdysseyMediaVector.h"
#include "OdysseyVectorBlock.h"
#include "Misc/OdysseyDuplicate.h"
#include "OdysseyAnimation.h"
// from module OdysseyFile
#include "OdysseyFile.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorRoot.h"
#include "OdysseyVectorGroupPaint.h"

UOdysseyAnimationCellImageVector::~UOdysseyAnimationCellImageVector()
{
    delete mRoot;
    mRoot = nullptr;
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
    if (animation->GetWidth() < 0 || animation->GetHeight() < 0)
        return;

    mRoot = new FOdysseyVectorRoot( Cast<UOdysseyAnimationLayerImageVector>(GetLayer())
                                  , this
                                  , new FOdysseyVectorGroupPaint( "Scene" )
                                  , (double)animation->GetWidth()
                                  , (double)animation->GetHeight() );

    mVectorBlock->Init(mVectorBlockId, mRoot->GetEngine(), animation->GetWidth(), animation->GetHeight(), animation->GetFormat());
}

void
UOdysseyAnimationCellImageVector::PostDuplicate(EDuplicateMode::Type iDuplicateMode)
{
    Super::PostDuplicate(iDuplicateMode);
    UOdysseyAnimation* animation = GetAnimation();
    mVectorBlockId = FGuid::NewGuid();
    mVectorBlock->Init(mVectorBlockId, mRoot->GetEngine(), animation->GetWidth(), animation->GetHeight(), animation->GetFormat());
}

FOdysseyVectorEngine*
UOdysseyAnimationCellImageVector::GetEngine() const
{
    return mRoot->GetEngine();
}

FOdysseyVectorRoot*
UOdysseyAnimationCellImageVector::GetRoot() const
{
    return mRoot;
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
    UOdysseyAnimationLayerImageVector* layerImageVector = Cast<UOdysseyAnimationLayerImageVector>(GetLayer());

    Super::Serialize(Ar);

    if (GetFlags() & RF_ClassDefaultObject)
        return;

    if( Ar.IsSaving() )
    {
        FOdysseyAnimationCellImageVectorExport::Write( this, Ar );
    }

    if( Ar.IsLoading() )
    {
        if ( !mRoot )
        {
            UOdysseyAnimation* animation = GetAnimation();
            mRoot = new FOdysseyVectorRoot( layerImageVector
                                          , this
                                          , new FOdysseyVectorGroupPaint( "Scene" )
                                          , (double)animation->GetWidth()
                                          , (double)animation->GetHeight() );
            // The reading process needs a valid sharedenv as the top object.
            layerImageVector->GetSharedEnv()->AppendChild( mRoot );
        }

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
    UOdysseyAnimationLayerImageVector* layerImageVector = Cast<UOdysseyAnimationLayerImageVector>(GetLayer());

    Super::OldSerialize(Ar);

    if( Ar.IsSaving() )
    {
        FOdysseyAnimationCellImageVectorExport::Write( this, Ar );
    }

    if( Ar.IsLoading() )
    {
        if ( !mRoot )
        {
            UOdysseyAnimation* animation = GetAnimation();
            mRoot = new FOdysseyVectorRoot( layerImageVector
                                          , this
                                          , new FOdysseyVectorGroupPaint( "Scene" )
                                          , (double)animation->GetWidth()
                                          , (double)animation->GetHeight() );
            // The reading process needs a valid sharedenv as the top object.
            layerImageVector->GetSharedEnv()->AppendChild( mRoot );
        }

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
    mVectorBlock->Init(mVectorBlockId, mRoot->GetEngine(), animation->GetWidth(), animation->GetHeight(), animation->GetFormat());
    mRoot->GetEngine()->Invalidate( 0 );
    FOdysseyVectorEngine::Notify( mRoot->GetScene(), FOdysseyVectorEngine::NOTIFY_ALL );
}

void
UOdysseyAnimationCellImageVector::OnIsColoredChanged(UOdysseyAnimationLayerImageVector* iLayer)
{
    if (iLayer != GetLayer())
        return;

    mRoot->GetEngine()->Invalidate( 0 );
}

void
UOdysseyAnimationCellImageVector::OnIsWireframeChanged(UOdysseyAnimationLayerImageVector* iLayer)
{
    if (iLayer != GetLayer())
        return;

    mRoot->GetEngine()->Invalidate( 0 );
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

    return { ::ULIS::FRectI::FromXYWH(0, 0, animation->GetWidth(), animation->GetHeight()) };
}

FOdysseyMediaProvider
UOdysseyAnimationCellImageVector::GetMediaProvider(uint32 iFrameIndex) const
{
    //Don't create a mediaRaster if there is an image render in use
    FScopeLock lock(&mImageRenderingMutex);

    TSharedPtr<FOdysseyMediaVector> mediaVector = MakeShared<FOdysseyMediaVector>(mRoot->GetScene());
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

    if (!iIsInteractive)
        DirtyThumbnail();
}

// Implements Interface IOdysseyVectorAnimationCell::GetEngine
FOdysseyVectorEngine*
UOdysseyAnimationCellImageVector::GetEngine()
{
    return mRoot->GetEngine();
}

// Implements Interface IOdysseyVectorAnimationCell::GetIndex
int32
UOdysseyAnimationCellImageVector::GetIndex()
{
    return IndexInLayer;
}

// Implements Interface IOdysseyVectorAnimationCell::GetLength
uint32
UOdysseyAnimationCellImageVector::GetLength()
{
    return Exposure;
}

// Implements Interface IOdysseyVectorAnimationCell::GetFrame
uint32
UOdysseyAnimationCellImageVector::GetFrame()
{
    return GetFrameRange().GetLowerBoundValue();
}
