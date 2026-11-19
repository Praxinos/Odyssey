// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationCell.h"

#include "Engine/Texture2D.h"
#include "Misc/TransactionObjectEvent.h"
#include "ScreenPass.h"
#include "TextureCompiler.h"
#include "TextureResource.h"

#include "OdysseyAnimation.h"
#include "OdysseyBlendShader.h"
#include "OdysseyAnimationLayer.h"
#include "OdysseyAnimationCellImport.h"
#include "OdysseyRenderContext.h"
#include "OdysseyTexture2DUtils.h"

void
UOdysseyAnimationCell::PostInitProperties() //override
{
    Super::PostInitProperties();
}

void
UOdysseyAnimationCell::PostLoad() //override
{
    Super::PostLoad();

#if WITH_EDITOR
    if( Mark_DEPRECATED != -1 )
    {
        Marks.Add( 0, FCellMark{ Mark_DEPRECATED } );
        Mark_DEPRECATED = -1;
    }
#endif
    UpdateTextureSize();
}

void
UOdysseyAnimationCell::UpdateTextureSize()
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

void
UOdysseyAnimationCell::PostDuplicate( EDuplicateMode::Type iDuplicateMode ) //override
{
    Super::PostDuplicate( iDuplicateMode );

    UpdateTextureSize();
}

//---

UOdysseyAnimationLayer*
UOdysseyAnimationCell::GetLayer() const
{
    return Cast<UOdysseyAnimationLayer>(GetOuter());
}

#if WITH_EDITOR
void
UOdysseyAnimationCell::OldSerialize(FArchive& Ar)
{
    if( !Ar.IsLoading() )
        return;

    if (!FOdysseyAnimationCellImport::Read( this, Ar ))
    {
        //Old Style No Chunk Loading
        Ar << Exposure;
    }
}
#endif

int32
UOdysseyAnimationCell::FrameInLayerToIndexInCell( FFrameNumber iFrame )
{
    FFrameNumber start_in_layer = GetFrameRange().GetLowerBoundValue();
    int32 index_in_cell = ( iFrame - start_in_layer ).Value;

    return index_in_cell;
}

FFrameNumber
UOdysseyAnimationCell::IndexInCellToFrameInLayer( int32 iIndex )
{
    FFrameNumber start_in_layer = GetFrameRange().GetLowerBoundValue();
    int32 frame_in_layer = ( iIndex + start_in_layer ).Value;

    return frame_in_layer;
}

UOdysseyLayerStack*
UOdysseyAnimationCell::GetLayerStack() const
{
    return GetLayer() ? GetLayer()->GetLayerStack() : nullptr;
}

#if WITH_EDITOR
FOdysseyMediaProvider
UOdysseyAnimationCell::GetMediaProvider() const
{
    return FOdysseyMediaProvider();
}
#endif

FInt32Range
UOdysseyAnimationCell::GetFrameRange() const
{
    if (!GetLayer())
        return FInt32Range::Empty();

    const TArray<FInt32Range>& frameRanges = GetLayer()->GetCellsFrameRanges();
    if (IndexInLayer < 0 || IndexInLayer >= frameRanges.Num())
        return FInt32Range::Empty();

    return GetLayer()->GetCellsFrameRanges()[IndexInLayer];
}

int
UOdysseyAnimationCell::GetIndexInLayer() const
{
    return IndexInLayer;
}

int
UOdysseyAnimationCell::GetExposure() const
{
    return Exposure;
}

#if WITH_EDITOR

FString
UOdysseyAnimationCell::GetName( ECellNameIfEmpty iCellNameIfEmpty ) const
{
    if( Name.IsEmpty() )
    {
        if( iCellNameIfEmpty == ECellNameIfEmpty::IndexInLayer )
        {
            return FString::FromInt( GetIndexInLayer() );
        }
    }

    return Name;
}

bool
UOdysseyAnimationCell::HasNoName() const
{
    return Name.IsEmpty();
}

int
UOdysseyAnimationCell::GetMark() const
{
    return Marks.Find( 0 ) ? Marks.Find( 0 )->Index : -1;
}

TMap<int, FCellMark>
UOdysseyAnimationCell::GetMarks() const
{
    return Marks;
}

#endif

void
UOdysseyAnimationCell::SetExposure(int Value)
{
    Modify();
    Exposure = Value;
    ExposureChanged(false);
}

void
UOdysseyAnimationCell::ExposureChanged(bool iIsInteractive)
{
    if( !iIsInteractive )
    {
#if WITH_EDITOR
        TArray<int> indexes;
        Marks.GetKeys( indexes );
        for( int index : indexes )
        {
            if( index >= Exposure )
                Marks.Remove( index );
        }
#endif
    }

    if (GetLayer())
        GetLayer()->InvalidateCellsFrameRanges();

    RenderingCompositionChanged(iIsInteractive);
}

UTexture2D*
UOdysseyAnimationCell::GetRenderTexture() const
{
#if WITH_EDITOR
    if (!Texture)
        const_cast<UOdysseyAnimationCell*>(this)->InitTexture();
#endif

    return Texture;
}

#if WITH_EDITOR
UTexture2D*
UOdysseyAnimationCell::GetExistingRenderTexture() const
{
    return Texture;
}

void
UOdysseyAnimationCell::DiscardRenderTexture()
{
    Texture = nullptr;
}

void
UOdysseyAnimationCell::InitTexture()
{
    if ( !Texture )
    {
        Texture = NewObject<UTexture2D>(this, TEXT("Texture"));
        Texture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
        Texture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
        Texture->Filter = TextureFilter::TF_Trilinear;
    }
}

void
UOdysseyAnimationCell::SetExposureInteractive(int Value)
{
    Modify();
    Exposure = Value;
    ExposureChanged(true);
}

void
UOdysseyAnimationCell::SetName( FString iValue )
{
    Modify();

    Name = iValue;
}

void
UOdysseyAnimationCell::SetMark(int iValue)
{
    Modify();

    if( iValue == -1 )
        Marks.Remove( 0 );
    else
        Marks.Add( 0, FCellMark{ iValue } );
}

void
UOdysseyAnimationCell::SetMarks( const TMap<int, FCellMark>& iMarks )
{
    Modify();

    Marks = iMarks;
}

void
UOdysseyAnimationCell::PostTransacted(const FTransactionObjectEvent& iTransactionEvent)
{
    Super::PostTransacted(iTransactionEvent);

    if ( iTransactionEvent.GetEventType() != ETransactionObjectEventType::UndoRedo )
        return;

    const TArray<FName>& changedPropertyNames = iTransactionEvent.GetChangedProperties();
    if (changedPropertyNames.Contains(GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Exposure)))
    {
        ExposureChanged(false);
    }

    if (changedPropertyNames.Contains(GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, OutOfPegs)))
    {
        OutOfPegsChanged(false);
    }
}

bool
UOdysseyAnimationCell::IsSelectedInEditor() const
{
    return IsValidChecked(this) && OdysseyAnimationCellsSelection::GIsCellSelectedInEditor && OdysseyAnimationCellsSelection::GIsCellSelectedInEditor(this);
}

UOdysseyAnimationCell*
UOdysseyAnimationCell::Break(int Frame, bool bClear)
{
    if (Frame <= 0 || Frame >= Exposure)
        return nullptr;

    Modify();

    UOdysseyAnimationCell* newCell = nullptr;
    if (bClear)
    {
        newCell = GetLayer()->AddCell(GetClass(), IndexInLayer + 1);
    }
    else
    {
        newCell = GetLayer()->CopyCell(this, IndexInLayer + 1);
    }

    newCell->SetExposure(Exposure - Frame);
    SetExposure(Frame);

    return newCell;
}

bool
UOdysseyAnimationCell::IsOutOfPegs() const
{
    return OutOfPegs.Pan != FVector2D(0, 0) || OutOfPegs.Rotation != 0.f || OutOfPegs.Zoom != 100.f;
}

FOdysseyAnimationCellOutOfPegs
UOdysseyAnimationCell::GetOutOfPegs() const
{
    return OutOfPegs;
}

void
UOdysseyAnimationCell::SetOutOfPegs(FOdysseyAnimationCellOutOfPegs Value)
{
    OutOfPegs = Value;
    OutOfPegsChanged(false);
}

void
UOdysseyAnimationCell::SetOutOfPegsInteractive(FOdysseyAnimationCellOutOfPegs Value)
{
    OutOfPegs = Value;
    OutOfPegsChanged(true);
}

void
UOdysseyAnimationCell::OutOfPegsChanged(bool iIsInteractive)
{
    mOnOutOfPegsChanged.Broadcast(iIsInteractive);
    RenderingChanged(iIsInteractive);
}

FMatrix
UOdysseyAnimationCell::OutOfPegsTransform() const
{
    FIntRect rect = GetDefaultRenderRect();

    FMatrix matrix = FMatrix::Identity;

    matrix *= FTranslationMatrix::Make(FVector(rect.Width() / -2.f, rect.Height() / -2.f, 0));
    matrix *= FRotationMatrix::Make(FRotator(0, OutOfPegs.Rotation, 0));
    matrix *= FScaleMatrix::Make(FVector(OutOfPegs.Zoom / 100.f, OutOfPegs.Zoom / 100.f, 1.f));
    matrix *= FTranslationMatrix::Make(FVector(OutOfPegs.Pan.X, OutOfPegs.Pan.Y, 0));
    matrix *= FTranslationMatrix::Make(FVector(rect.Width() / 2.f, rect.Height() / 2.f, 0));

    return matrix;
}

UOdysseyAnimationCell::FOnOutOfPegsChanged&
UOdysseyAnimationCell::OnOutOfPegsChanged()
{
    return mOnOutOfPegsChanged;
}

#endif

FMatrix
UOdysseyAnimationCell::GetContentTransform(const FOdysseyRenderContext* RenderContext) const
{
#if WITH_EDITOR
    if (RenderContext && (RenderContext->Type & EOdysseyRenderingType::OutOfPegs))
    {
        return OutOfPegsTransform();
    }
#endif
    return FMatrix::Identity;
}

EOdysseyColorConversion
UOdysseyAnimationCell::GetContentColorConversion(const FOdysseyRenderContext* RenderContext) const
{
#if WITH_EDITOR
    const UOdysseyAnimation* Animation = GetAnimation();
    if (Animation && !Animation->UsesLinearCompositing())
    {
        return EOdysseyColorConversion::Encode;
    }
#endif
    return EOdysseyColorConversion::None;
}

bool
UOdysseyAnimationCell::BuildRenderPipelineInternal(
    IOdysseyTextureRenderingAbility::FRenderFunction& oRenderFunction,
    const TArray<const IOdysseyTextureRenderingAbility*>& iParents, const FOdysseyRenderContext* RenderContext
) const
{
#if WITH_EDITOR
    if ( !Texture )
        const_cast<UOdysseyAnimationCell*>(this)->InitTexture();

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

        FRDGTextureRef sourceTexture = iGraphBuilder.RegisterExternalTexture(CreateRenderTarget(SourceRHI, TEXT("UOdysseyAnimationCell::sourceTexture")));

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

UTextureRenderTarget2D*
UOdysseyAnimationCell::CreateRenderingRenderTarget() const
{
    return GetLayer()->CreateRenderingRenderTarget();
}

#if WITH_EDITOR

UTexture2D*
UOdysseyAnimationCell::CreateExportTexture( const FString& iAssetName, const FString& iPackagePath, UClass* iAssetClass, UFactory* iFactory )
{
    return GetLayer()->CreateExportTexture(iAssetName, iPackagePath, iAssetClass, iFactory);
}

#endif

#if WITH_EDITOR
namespace OdysseyAnimationCellsSelection {
    TFunction<bool(const UObject*)> GIsCellSelectedInEditor;
}
#endif

UOdysseyAnimation*
UOdysseyAnimationCell::GetAnimation() const
{
    UOdysseyAnimationLayer* Layer = GetLayer();
    return Layer ? Layer->GetAnimation() : nullptr;
}
