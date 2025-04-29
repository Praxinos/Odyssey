// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationCellImageVector.h"

#include "OdysseyAnimationCellImageVectorImageRenderer.h"
#include "OdysseyAnimationCellImageVectorExport.h"
#include "OdysseyAnimationCellImageVectorImport.h"
#include "OdysseyAnimationLayerImageVector.h"
#include "ULISLoaderModule.h"
#include "OdysseyVector.h"
#include "OdysseyMediaVector.h"
#include "OdysseyVectorBlock.h"
#include "Misc/OdysseyDuplicate.h"
#include "OdysseyAnimation.h"
// from module OdysseyFile
#include "OdysseyFile.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorTagInbetweener.h"

UOdysseyAnimationCellImageVector::~UOdysseyAnimationCellImageVector()
{
    if (mVectorCell)
    {
        if( mVectorCell->GetParent() )
        {
            mVectorCell->GetParent()->RemoveChild( mVectorCell.Get() );
        }
    }
}

void
UOdysseyAnimationCellImageVector::PostInitProperties()
{
    UOdysseyAnimationLayerImageVector* layerImageVector = Cast<UOdysseyAnimationLayerImageVector>(GetLayer());

    Super::PostInitProperties();

    if (GetFlags() & RF_ClassDefaultObject)
        return;

    // bind refresh function to delegates on existing vector scenes at load. Needed to refresh necessary widgets.
    UOdysseyAnimationLayerImageVector::OnIsColoredChanged().AddUObject( this, &UOdysseyAnimationCellImageVector::OnIsColoredChanged );
    UOdysseyAnimationLayerImageVector::OnIsWireframeChanged().AddUObject( this, &UOdysseyAnimationCellImageVector::OnIsWireframeChanged );

    mVectorBlockId = FGuid::NewGuid();
    mVectorBlock = MakeShared<FOdysseyVectorBlock>();
    mVectorBlock->OnInvalidated().AddUObject(this, &UOdysseyAnimationCellImageVector::OnVectorBlockInvalidated);

    mVectorBlock->GetEngine().OnNotifyDelegate().AddUObject(this, &UOdysseyAnimationCellImageVector::OnVectorEngineNotify);

    UOdysseyAnimation* animation = GetAnimation();
    if (animation->GetWidth() < 0 || animation->GetHeight() < 0)
        return;

    mVectorCell = MakeShared<FOdysseyVectorCell>( this
                                                , new FOdysseyVectorGroupPaint( "Scene" ) );
    // The reading process needs a valid sharedenv as the top object.
    layerImageVector->GetVectorLayer()->AppendChild( mVectorCell.Get() );

    ::ULIS::eFormat format = ::ULIS::Format_BGRA8;
    switch(animation->GetFormat())
    {
        case EOdysseyAnimationFormat::BGRA8: format = ::ULIS::Format_BGRA8;
        case EOdysseyAnimationFormat::RGBAF: format = ::ULIS::Format_RGBAF;
    }

    mVectorBlock->Init(mVectorBlockId, mVectorCell, animation->GetWidth(), animation->GetHeight(), format);
}

void
UOdysseyAnimationCellImageVector::PostDuplicate(EDuplicateMode::Type iDuplicateMode)
{
    Super::PostDuplicate(iDuplicateMode);
    UOdysseyAnimation* animation = GetAnimation();
    mVectorBlockId = FGuid::NewGuid();

    ::ULIS::eFormat format = ::ULIS::Format_BGRA8;
    switch(animation->GetFormat())
    {
        case EOdysseyAnimationFormat::BGRA8: format = ::ULIS::Format_BGRA8;
        case EOdysseyAnimationFormat::RGBAF: format = ::ULIS::Format_RGBAF;
    }

    mVectorBlock->Init(mVectorBlockId, mVectorCell, animation->GetWidth(), animation->GetHeight(), format);
}

FOdysseyVectorImportV2*
UOdysseyAnimationCellImageVector::GetImporterV2()
{
    return &mImporterV2;
}

FOdysseyVectorCell*
UOdysseyAnimationCellImageVector::GetVectorCell() const
{
    return mVectorCell.Get();
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

    if( Ar.IsSaving() && !Ar.IsTransacting() )
    {
        FOdysseyAnimationCellImageVectorExport::Write( this, Ar );
    }

    if( Ar.IsLoading() && !Ar.IsTransacting() )
    {
        if ( !mVectorCell )
        {
            UOdysseyAnimation* animation = GetAnimation();
            mVectorCell = MakeShared<FOdysseyVectorCell>( this
                                                        , new FOdysseyVectorGroupPaint( "Scene" ) );
            // The reading process needs a valid sharedenv as the top object.
            layerImageVector->GetVectorLayer()->AppendChild( mVectorCell.Get() );
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
        if ( !mVectorCell )
        {
            UOdysseyAnimation* animation = GetAnimation();
            mVectorCell = MakeShared<FOdysseyVectorCell>( this
                                                        , new FOdysseyVectorGroupPaint( "Scene" ) );
            // The reading process needs a valid sharedenv as the top object.
            layerImageVector->GetVectorLayer()->AppendChild( mVectorCell.Get() );
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

    ::ULIS::eFormat format = ::ULIS::Format_BGRA8;
    switch(animation->GetFormat())
    {
        case EOdysseyAnimationFormat::BGRA8: format = ::ULIS::Format_BGRA8;
        case EOdysseyAnimationFormat::RGBAF: format = ::ULIS::Format_RGBAF;
    }

    mVectorBlock->Init(mVectorBlockId, mVectorCell, animation->GetWidth(), animation->GetHeight(), format);
    //mVectorCell->GetLayer()->InvalidateCell( mVectorCell.Get() );
    FOdysseyVectorEngine::Notify( mVectorCell->GetScene(), FOdysseyVectorEngine::NOTIFY_ALL );

    // textures must be assigned to brushes in PostLoad and not in Serialize(), because the UAsset won't be fully loaded
    // and there dimensions would be 0 at that point.
    mImporterV2.PostLoadTextures();
}

void
UOdysseyAnimationCellImageVector::OnIsColoredChanged(UOdysseyAnimationLayerImageVector* iLayer)
{
    if (iLayer != GetLayer())
        return;

    mVectorCell->GetLayer()->RequestRedraw( mVectorCell.Get(), 0 );
}

void
UOdysseyAnimationCellImageVector::OnIsWireframeChanged(UOdysseyAnimationLayerImageVector* iLayer)
{
    if (iLayer != GetLayer())
        return;

    mVectorCell->GetLayer()->RequestRedraw( mVectorCell.Get(), 0 );
}

bool
UOdysseyAnimationCellImageVector::IsImageRenderingGameThreadOnly() const
{
    TSharedPtr<FOdysseyMediaVector> mediaVector = mMediaVector.Pin();
    return !!mediaVector;
}

TSharedPtr<IOdysseyImageRenderer>
UOdysseyAnimationCellImageVector::BuildImageRenderer(EOdysseyRenderingType iRenderType, int iFrame, FImageRendererFilter iFilter) const
{
    if (iFilter.IsBound() && !iFilter.Execute(this))
        return nullptr;

    return MakeShared<FOdysseyAnimationCellImageVectorImageRenderer>(this, iFrame, iRenderType, GetRenderingRects(), iFilter);
}

TArray<FGuid>
UOdysseyAnimationCellImageVector::GetRenderingComposition(EOdysseyRenderingType iRenderType, int iFrameIndex) const
{
    return { GetRenderingId() };
}

TArray<FIntRect>
UOdysseyAnimationCellImageVector::GetRenderingRects() const
{
    UOdysseyAnimation* animation = GetAnimation();
    if (!animation)
        return {};

    return { FIntRect(0, 0, animation->GetWidth(), animation->GetHeight()) };
}

FOdysseyMediaProvider
UOdysseyAnimationCellImageVector::GetMediaProvider(uint32 iFrameIndex) const
{
    //Don't create a mediaRaster if there is an image render in use
    FScopeLock lock(&mImageRenderingMutex);

    TSharedPtr<FOdysseyMediaVector> mediaVector = MakeShared<FOdysseyMediaVector>(mVectorCell->GetScene());
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
    TArray<FIntRect> intRects;
    intRects.Reserve(iRects.Num());
    for (const ::ULIS::FRectI& rect : iRects )
    {
        FIntRect intRect(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h);
        intRects.Add(intRect);
    }

    RenderingChanged( intRects, iIsInteractive);

    if (!iIsInteractive)
        DirtyThumbnail();
}

// Implements Interface IOdysseyVectorCell::GetIndex
int32
UOdysseyAnimationCellImageVector::GetIndex()
{
    return IndexInLayer;
}

// Implements Interface IOdysseyVectorCell::GetLength
uint32
UOdysseyAnimationCellImageVector::GetLength()
{
    return Exposure;
}

// Implements Interface IOdysseyVectorCell::GetFrame
uint32
UOdysseyAnimationCellImageVector::GetFrame()
{
    return GetFrameRange().GetLowerBoundValue();
}

void
UOdysseyAnimationCellImageVector::OnVectorEngineNotify(FOdysseyVectorGroupPaint* iScene, uint64 iSignalFlags)
{
    if (iScene->GetCell()->GetCellInterface() != this)
        return;

    if (iSignalFlags & FOdysseyVectorEngine::NOTIFY_UPDATE_HUD)
    {
        iScene->GetCell()->ResetHUD();
    }
}
