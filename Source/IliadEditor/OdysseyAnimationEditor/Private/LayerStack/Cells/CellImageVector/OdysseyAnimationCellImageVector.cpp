// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationCellImageVector.h"
#include "OdysseyRenderContext.h"

#include "OdysseyAnimation.h"

#include "Editor.h"
#include "OdysseyAnimationCellImageVectorExport.h"
#include "OdysseyAnimationCellImageVectorImport.h"
#include "OdysseyAnimationLayerImageVector.h"
#include "OdysseyVector.h"
#include "OdysseyMediaVector.h"
// from module OdysseyFile
#include "OdysseyFile.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorTagInbetweener.h"
#include "Undo/OdysseyVectorUndo.h"
#include "Undo/OdysseyVectorUndoSceneClear.h"
#include "ScopedTransaction.h"

#define LOCTEXT_NAMESPACE "Animation"

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

UOdysseyAnimationCellImageVector::UOdysseyAnimationCellImageVector()
{
}

TArray<FGuid>
UOdysseyAnimationCellImageVector::GetRenderingComposition(uint64 iRenderType, int iFrameIndex) const
{
    return { GetRenderingId() };
}

FIntRect
UOdysseyAnimationCellImageVector::GetDefaultRenderRect() const
{
    UOdysseyAnimation* animation = GetAnimation();
    if ( !animation )
        return FIntRect(0, 0, 0, 0);

    return FIntRect(0, 0, animation->GetWidth(), animation->GetHeight());
}

bool
UOdysseyAnimationCellImageVector::BuildRenderPipelineInternal(
    IOdysseyTextureRenderingAbility::FRenderFunction& oRenderFunction,
    const TArray<const IOdysseyTextureRenderingAbility*>& iParents, const FOdysseyRenderContext* RenderContext
) const
{
    return GetVectorContent().BuildRenderFunction(oRenderFunction, GetContentTransform(RenderContext));
}

uint64
UOdysseyAnimationCellImageVector::GetDrawingFlags() const
{
    const UOdysseyAnimationLayerImageVector* Layer = Cast<UOdysseyAnimationLayerImageVector>(GetLayer());
    if (!Layer)
    {
        return 0;
    }
    uint64 DrawingFlags = !Layer->IsColored() ? FOdysseyVectorEngine::DRAWING_IGNORECOLOR : 0;
    DrawingFlags |= Layer->IsWireframe() ? FOdysseyVectorEngine::DRAWING_WIREFRAME : 0;
    return DrawingFlags;
}

FOdysseyVectorContent&
UOdysseyAnimationCellImageVector::GetVectorContent() const
{
    // A new vector cell, e.g. created when loading or duplicating, gets a new content.
    if (!mVectorContent || mVectorContent->GetCell() != mVectorCell)
    {
        UOdysseyAnimationCellImageVector* MutableThis = const_cast<UOdysseyAnimationCellImageVector*>(this);
        mVectorContent = MakeShared<FOdysseyVectorContent>(MutableThis->GetRenderingId(), mVectorCell,
            [MutableThis]()
            {
                FOdysseyVectorContent::FDescription Description;
                if (const UOdysseyAnimation* Animation = MutableThis->GetAnimation())
                {
                    Description.Size = MutableThis->GetDefaultRenderRect().Size();
                    // As the former render texture: decoded in 8-bit animations, sampled as it is in float ones.
                    Description.bSRGB = Animation->GetFormat() == EOdysseyAnimationFormat::BGRA8;
                }
                return Description;
            },
            [MutableThis]()
            {
                return MutableThis->GetDrawingFlags();
            },
            [MutableThis](const TArray<FIntRect>& Rects, bool bInteractive)
            {
                MutableThis->RenderingChanged(Rects, bInteractive);
            });
    }
    return *mVectorContent;
}

void
UOdysseyAnimationCellImageVector::BeginDestroy()
{
    mVectorContent.Reset();
    Super::BeginDestroy();
}

void
UOdysseyAnimationCellImageVector::PostInitProperties()
{
    UOdysseyAnimationLayerImageVector* layerImageVector = Cast<UOdysseyAnimationLayerImageVector>(GetLayer());

    Super::PostInitProperties();

    if (GetFlags() & RF_ClassDefaultObject)
        return;

    UOdysseyAnimation* animation = GetAnimation();
    if (animation->GetWidth() < 0 || animation->GetHeight() < 0)
        return;

    mVectorCell = MakeShared<FOdysseyVectorCell>( this, new FOdysseyVectorGroupPaint( "Scene" ) );
    // The reading process needs a valid sharedenv as the top object.
    layerImageVector->GetVectorLayer()->AppendChild( mVectorCell.Get() );

    // Follows the redraws of the scene from now on; renders it when displayed.
    GetVectorContent();
}

void
UOdysseyAnimationCellImageVector::PostDuplicate(EDuplicateMode::Type iDuplicateMode)
{
    Super::PostDuplicate(iDuplicateMode);
    // The render of the original is not duplicated: the content follows the scene of the duplicate.
    GetVectorContent();
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

void
UOdysseyAnimationCellImageVector::Serialize(FArchive& Ar)
{
    UOdysseyAnimationLayerImageVector* layerImageVector = Cast<UOdysseyAnimationLayerImageVector>(GetLayer());

    Super::Serialize(Ar);

    if (GetFlags() & RF_ClassDefaultObject)
        return;

    if( Ar.IsSaving() && !Ar.IsTransacting() && !Ar.IsCooking() )
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

    // Follows the redraws of the loaded scene; renders it when displayed.
    GetVectorContent();

    // textures must be assigned to brushes in PostLoad and not in Serialize(), because the UAsset won't be fully loaded
    // and there dimensions would be 0 at that point.
    mImporterV2.PostLoadTextures();

    mVectorCell->Invalidate( FOdysseyVectorObjectInvalidationFlags() );
    mVectorCell->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    mVectorCell->GetLayer()->RequestRedraw( mVectorCell.Get(), 0 );
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

FOdysseyMediaProvider
UOdysseyAnimationCellImageVector::GetMediaProvider() const
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
UOdysseyAnimationCellImageVector::OnRefreshReferencedPalette(UOdysseyPalette* iPalette)
{
    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetMediaProvider().GetMedias<FOdysseyMediaVector>();

    if (mediaVectors.IsEmpty())
        return;

    for (int i = 0; i < mediaVectors.Num(); i++)
    {
        FOdysseyVectorCell* vectorCell = mediaVectors[i]->GetScene()->GetCell();
        vectorCell->GetLayer()->RequestRedraw(vectorCell, 0);
    }
}

void
UOdysseyAnimationCellImageVector::Clear()
{
    // needed for undos
    if (GUndo)
    {
        FScopedTransaction ScopedTransaction(LOCTEXT("animation-cell-vector.clear", "Clear"));
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoSceneClear( mVectorCell->GetScene() );
        GUndo->StoreUndo(GEditor, TUniquePtr<FOdysseyVectorUndo>(undo));
    }

    mVectorCell->SetScene( new FOdysseyVectorGroupPaint("Scene") );
    mVectorCell->GetLayer()->RequestRedraw( mVectorCell.Get(), 0 );
    mVectorCell->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
}

#undef LOCTEXT_NAMESPACE
