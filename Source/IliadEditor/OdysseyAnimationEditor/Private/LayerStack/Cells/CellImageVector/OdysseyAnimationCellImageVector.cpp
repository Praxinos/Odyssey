// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationCellImageVector.h"
#include "OdysseyRenderContext.h"

#include "OdysseyAnimation.h"
#include "ScreenPass.h"

#if WITH_EDITOR
#include "Editor.h"
#include "TextureCompiler.h"
#include "Engine/Texture2D.h"
#include "RenderGraphUtils.h"
#include "OdysseyTexture2DUtils.h"
#include "OdysseyBlendShader.h"
#include "TextureResource.h"
#include "OdysseyAnimationCellImageVectorExport.h"
#include "OdysseyAnimationCellImageVectorImport.h"
#include "OdysseyAnimationLayerImageVector.h"
#include "ULISLoaderModule.h"
#include "OdysseyVector.h"
#include "OdysseyMediaVector.h"
#include "OdysseyVectorBlock.h"
// from module OdysseyFile
#include "OdysseyFile.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyPixelFormat.h"
#include "ULISUtils.h"
#include "UObject/ObjectSaveContext.h"
#include "Undo/OdysseyVectorUndo.h"
#include "Undo/OdysseyVectorUndoSceneClear.h"
#include "ScopedTransaction.h"
#endif

#define LOCTEXT_NAMESPACE "Animation"

UOdysseyAnimationCellImageVector::~UOdysseyAnimationCellImageVector()
{
#if WITH_EDITOR
    if (mVectorCell)
    {
        if( mVectorCell->GetParent() )
        {
            mVectorCell->GetParent()->RemoveChild( mVectorCell.Get() );
        }
    }
#endif
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

UTexture2D*
UOdysseyAnimationCellImageVector::GetRenderTexture() const
{
#if WITH_EDITOR
    if (!Texture)
        const_cast<UOdysseyAnimationCellImageVector*>(this)->InitTexture();
#endif

    return Texture;
}

void
UOdysseyAnimationCellImageVector::UpdateTextureSize()
{
    if (!Texture)
        return;

    FIntRect rect = GetDefaultRenderRect();

    uint32 layerWidth = rect.Width();
    uint32 layerHeight = rect.Height();
#if WITH_EDITORONLY_DATA
    uint32 textureWidth = Texture->Source.GetSizeX();
    uint32 textureHeight = Texture->Source.GetSizeY();
    if ( textureWidth != layerWidth || textureHeight != layerHeight)
    {
        Odyssey::ResizeTexture2D(Texture, layerWidth, layerHeight);
    }
#endif
}

bool
UOdysseyAnimationCellImageVector::BuildRenderPipelineInternal(
    IOdysseyTextureRenderingAbility::FRenderFunction& oRenderFunction,
    const TArray<const IOdysseyTextureRenderingAbility*>& iParents, const FOdysseyRenderContext* RenderContext
) const
{
#if WITH_EDITOR
    bool drawingFlagsChanged = UpdateDrawingFlags();
    if ( drawingFlagsChanged )
    {
        const_cast<UOdysseyAnimationCellImageVector*>(this)->InitTexture();
    }
    if ( mVectorBlock->NeedsRender() )
    {
        ::ULIS::FRectI rect = mVectorBlock->GetSanitizedRect();
        mVectorBlock->Render(mDrawingFlags);
        FOdysseySurfaceTexture2DEditable surface(GetRenderTexture(), GetVectorBlock()->GetBlock(mDrawingFlags));
        surface.Invalidate({ rect });
    }
#endif

#if WITH_EDITOR
    if ( !Texture )
        const_cast<UOdysseyAnimationCellImageVector*>(this)->InitTexture();

    Texture->BlockOnAnyAsyncBuild();
    Texture->WaitForPendingInitOrStreaming();
#endif

    const FMatrix CellTransform = GetContentTransform(RenderContext);
    const FTextureRHIRef SourceRHI = Texture->GetResource()->TextureRHI;
    oRenderFunction = [SourceRHI, CellTransform](
        FRDGBuilder& iGraphBuilder,
        ERHIFeatureLevel::Type iFeatureLevel,
        FRDGTextureRef iDestinationTexture,
        const FIntRect& iSrcRect,
        const FIntRect& iDstRect,
        const FMatrix& iSrcTransform
        )
    {
        AddClearRenderTargetPass(iGraphBuilder, iDestinationTexture, FLinearColor::Transparent, iDstRect);

        FRDGTextureRef sourceTexture = iGraphBuilder.RegisterExternalTexture(CreateRenderTarget(SourceRHI, TEXT("UOdysseyAnimationCellImageVector::sourceTexture")));

        const FMatrix transform = CellTransform * iSrcTransform;

        FOdysseyBlendShader::BlendRect(
            iGraphBuilder,
            iFeatureLevel,
            iDestinationTexture,
            sourceTexture,
            iDestinationTexture,
            iSrcRect,
            iDstRect,
            transform,
            EOdysseyColorBlendMode::Normal,
            EOdysseyAlphaBlendMode::Normal,
            1.f,
            EOdysseyAntiAliasing::Anisotropic
        );
    };

    return true;
}

#if WITH_EDITOR
bool
UOdysseyAnimationCellImageVector::UpdateDrawingFlags() const
{
    UOdysseyAnimationLayerImageVector* layer = Cast<UOdysseyAnimationLayerImageVector>(GetLayer());

    uint64 drawingFlags = !layer->IsColored() ? FOdysseyVectorEngine::DRAWING_IGNORECOLOR : 0;
    drawingFlags |= layer->IsWireframe() ? FOdysseyVectorEngine::DRAWING_WIREFRAME : 0;
    bool changed = drawingFlags != mDrawingFlags;
    mDrawingFlags = drawingFlags;
    return changed;
}

void
UOdysseyAnimationCellImageVector::InitTexture()
{
    if ( !Texture )
    {
        Texture = NewObject<UTexture2D>(this, TEXT("Texture"));
        Texture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
        Texture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
        Texture->Filter = TextureFilter::TF_Trilinear;
    }

    UTexture2D* texture = GetRenderTexture();
    InitTextureWithBlockData(mVectorBlock->GetBlock(mDrawingFlags).Get(), texture, TextureSourceFormatForULISFormat(mVectorBlock->GetFormat()));
    texture->UpdateResource();
    FTextureCompilingManager::Get().FinishCompilation({ texture });
}

TSharedPtr<::ULIS::FBlock>
UOdysseyAnimationCellImageVector::GetBlock() const
{
    return mVectorBlock->GetBlock(mDrawingFlags);
}

void
UOdysseyAnimationCellImageVector::PostInitProperties()
{
    UOdysseyAnimationLayerImageVector* layerImageVector = Cast<UOdysseyAnimationLayerImageVector>(GetLayer());

    Super::PostInitProperties();

    if (GetFlags() & RF_ClassDefaultObject)
        return;

    mVectorBlockId = FGuid::NewGuid();
    mVectorBlock = MakeShared<FOdysseyVectorBlock>();
    mVectorBlock->OnInvalidated().AddUObject(this, &UOdysseyAnimationCellImageVector::OnVectorBlockInvalidated);

    UOdysseyAnimation* animation = GetAnimation();
    if (animation->GetWidth() < 0 || animation->GetHeight() < 0)
        return;

    mVectorCell = MakeShared<FOdysseyVectorCell>( this, new FOdysseyVectorGroupPaint( "Scene" ) );
    // The reading process needs a valid sharedenv as the top object.
    layerImageVector->GetVectorLayer()->AppendChild( mVectorCell.Get() );

    ::ULIS::eFormat format = ::ULIS::Format_BGRA8;
    switch(animation->GetFormat())
    {
        case EOdysseyAnimationFormat::BGRA8: format = ::ULIS::Format_BGRA8; break;
        case EOdysseyAnimationFormat::RGBAF: format = ::ULIS::Format_RGBAF; break;
    }

    mVectorBlock->Init(mVectorBlockId, mVectorCell, animation->GetWidth(), animation->GetHeight(), format);
}

void
UOdysseyAnimationCellImageVector::PostDuplicate(EDuplicateMode::Type iDuplicateMode)
{
    Super::PostDuplicate(iDuplicateMode);
    UpdateTextureSize();
    UOdysseyAnimation* animation = GetAnimation();
    mVectorBlockId = FGuid::NewGuid();

    ::ULIS::eFormat format = ::ULIS::Format_BGRA8;
    switch(animation->GetFormat())
    {
        case EOdysseyAnimationFormat::BGRA8: format = ::ULIS::Format_BGRA8; break;
        case EOdysseyAnimationFormat::RGBAF: format = ::ULIS::Format_RGBAF; break;
    }

    mVectorBlock->Init(mVectorBlockId, mVectorCell, animation->GetWidth(), animation->GetHeight(), format);

    InitTexture();
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
    UpdateTextureSize();

    if (GetFlags() & RF_ClassDefaultObject)
        return;

    UOdysseyAnimation* animation = GetAnimation();

    ::ULIS::eFormat format = ::ULIS::Format_BGRA8;
    switch(animation->GetFormat())
    {
        case EOdysseyAnimationFormat::BGRA8: format = ::ULIS::Format_BGRA8; break;
        case EOdysseyAnimationFormat::RGBAF: format = ::ULIS::Format_RGBAF; break;
    }

    mVectorBlock->Init(mVectorBlockId, mVectorCell, animation->GetWidth(), animation->GetHeight(), format);
    InitTexture();

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

void
UOdysseyAnimationCellImageVector::OnVectorBlockInvalidated( const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive)
{
    RenderingChanged( ::ULISUtils::ToIntRects(iRects), iIsInteractive);
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
UOdysseyAnimationCellImageVector::PreSave(FObjectPreSaveContext SaveContext)
{
    Super::PreSave(SaveContext);

    //Setting mDrawingFlags here ensures the Texture will update correctly on the next call to BuildTextureRenderer()
    //If some drawing flags are needed
    //As UpdateDrawingFlags() will return true and enforce Texture redraw.
    mDrawingFlags = 0;
    InitTexture();
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

#endif

#undef LOCTEXT_NAMESPACE
