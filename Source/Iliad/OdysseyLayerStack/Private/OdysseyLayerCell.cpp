// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyLayerCell.h"
#include "OdysseyLayerCellImport.h"
#include "OdysseyLayer.h"
#include "Misc/TransactionObjectEvent.h"
#include "ScreenPass.h"
#include "TextureCompiler.h"

UOdysseyLayer*
UOdysseyLayerCell::GetLayer() const
{
    return Cast<UOdysseyLayer>(GetOuter());
}

void
UOdysseyLayerCell::OldSerialize(FArchive& Ar)
{
    if( !Ar.IsLoading() )
        return;

    if (!FOdysseyLayerCellImport::Read( this, Ar ))
    {
        //Old Style No Chunk Loading
        Ar << Exposure;
    }
}

UOdysseyLayerStack*
UOdysseyLayerCell::GetLayerStack() const
{
    return GetLayer() ? GetLayer()->GetLayerStack() : nullptr;
}

#if WITH_EDITOR
FOdysseyMediaProvider
UOdysseyLayerCell::GetMediaProvider(uint32 iFrameIndex) const
{
    return FOdysseyMediaProvider();
}
#endif

FInt32Range
UOdysseyLayerCell::GetFrameRange() const
{
    if (!GetLayer())
        return FInt32Range::Empty();

    const TArray<FInt32Range>& frameRanges = GetLayer()->GetCellsFrameRanges();
    if (IndexInLayer < 0 || IndexInLayer >= frameRanges.Num())
        return FInt32Range::Empty();

    return GetLayer()->GetCellsFrameRanges()[IndexInLayer];
}

#if WITH_EDITOR
FSimpleMulticastDelegate&
UOdysseyLayerCell::OnThumbnailChanged()
{
    return mOnThumbnailChanged;
}

FSimpleMulticastDelegate&
UOdysseyLayerCell::OnThumbnailDirtied()
{
    return mOnThumbnailDirtied;
}
#endif

int
UOdysseyLayerCell::GetIndexInLayer() const
{
    return IndexInLayer;
}

int
UOdysseyLayerCell::GetExposure() const
{
    return Exposure;
}

#if WITH_EDITOR
int
UOdysseyLayerCell::GetMark() const
{
    return Mark;
}
#endif

void
UOdysseyLayerCell::SetExposure(int Value)
{
    Exposure = Value;

    if (GetLayer())
        GetLayer()->InvalidateCellsFrameRanges();

    RenderingCompositionChanged();
}

UTexture2D*
UOdysseyLayerCell::GetRenderTexture() const
{
    if (!Texture)
        const_cast<UOdysseyLayerCell*>(this)->InitTexture();

    return Texture;
}

void
UOdysseyLayerCell::InitTexture()
{
    if ( !Texture )
    {
        Texture = NewObject<UTexture2D>(this, TEXT("Texture"));
        Texture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
        Texture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
        Texture->Filter = TextureFilter::TF_Trilinear;
    }
}

#if WITH_EDITOR
void
UOdysseyLayerCell::SetExposureInteractive(int Value)
{
    Exposure = Value;

    if (GetLayer())
        GetLayer()->InvalidateCellsFrameRanges();

    RenderingCompositionChanged(true);
}

void
UOdysseyLayerCell::SetMark(int Value)
{
    Mark = Value;
}

void
UOdysseyLayerCell::PostTransacted(const FTransactionObjectEvent& iTransactionEvent)
{
    Super::PostTransacted(iTransactionEvent);

    if ( iTransactionEvent.GetEventType() != ETransactionObjectEventType::UndoRedo )
        return;

    const TArray<FName>& changedPropertyNames = iTransactionEvent.GetChangedProperties();
    if (changedPropertyNames.Contains(GET_MEMBER_NAME_CHECKED(UOdysseyLayerCell, Exposure)))
        RenderingCompositionChanged();

    if (changedPropertyNames.Contains(GET_MEMBER_NAME_CHECKED(UOdysseyLayerCell, OutOfPegs)))
    {
        mOnOutOfPegsChanged.Broadcast(false);
        RenderingChanged(false);
    }
}

UOdysseyLayerCell*
UOdysseyLayerCell::Break(int Frame, bool bClear)
{
    if (Frame <= 0 || Frame >= Exposure)
        return nullptr;

    UOdysseyLayerCell* newCell = nullptr;
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

void
UOdysseyLayerCell::DirtyThumbnail()
{
    if (ThumbnailIsDirty)
        return;

    ThumbnailIsDirty = true;
    mOnThumbnailDirtied.Broadcast();
}

void
UOdysseyLayerCell::UndirtyThumbnail()
{
    ThumbnailIsDirty = false;
}

bool
UOdysseyLayerCell::IsThumbnailDirty() const
{
    return ThumbnailIsDirty;
}

bool
UOdysseyLayerCell::IsOutOfPegs() const
{
    return OutOfPegs.Pan != FVector2D(0, 0) || OutOfPegs.Rotation != 0.f || OutOfPegs.Zoom != 100.f;
}

FOdysseyLayerCellOutOfPegs
UOdysseyLayerCell::GetOutOfPegs() const
{
    return OutOfPegs;
}

void
UOdysseyLayerCell::SetOutOfPegs(FOdysseyLayerCellOutOfPegs Value)
{
    OutOfPegs = Value;
    mOnOutOfPegsChanged.Broadcast(false);
    RenderingChanged(false);
}

void
UOdysseyLayerCell::SetOutOfPegsInteractive(FOdysseyLayerCellOutOfPegs Value)
{
    OutOfPegs = Value;
    mOnOutOfPegsChanged.Broadcast(true);
    RenderingChanged(true);
}

FMatrix
UOdysseyLayerCell::OutOfPegsTransform() const
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

UOdysseyLayerCell::FOnOutOfPegsChanged&
UOdysseyLayerCell::OnOutOfPegsChanged()
{
    return mOnOutOfPegsChanged;
}

#endif

bool
UOdysseyLayerCell::BuildRenderPipelineInternal(
    FFrameNumber iFrame,
    uint64 iType,
    IOdysseyTextureRenderingAbility::FRenderFunction& oRenderFunction,
    const IOdysseyTextureRenderingAbility::FCanRenderFunction& iCanRenderFunction,
    const TArray<const IOdysseyTextureRenderingAbility*>& iParents
) const
{
#if WITH_EDITOR
    if ( !Texture )
        const_cast<UOdysseyLayerCell*>(this)->InitTexture();

    Texture->BlockOnAnyAsyncBuild();
#endif

    oRenderFunction = [this, iType](
        FRDGBuilder& iGraphBuilder,
        ERHIFeatureLevel::Type iFeatureLevel,
        FRDGTextureRef iDestinationTexture,
        const FIntRect& iSrcRect,
        const FIntRect& iDstRect,
        const FMatrix& iSrcTransform
        )
    {
        AddClearRenderTargetPass(iGraphBuilder, iDestinationTexture, FLinearColor::Transparent, iDstRect);

        FRDGTextureRef sourceTexture = iGraphBuilder.RegisterExternalTexture(CreateRenderTarget(Texture->GetResource()->TextureRHI, TEXT("UOdysseyLayerCell::sourceTexture")));

        FOdysseyBlendShader::BlendRect(
            iGraphBuilder,
            iFeatureLevel,
            iDestinationTexture,
            sourceTexture,
            iDestinationTexture,
            iSrcRect,
            iDstRect,
            iType & EOdysseyRenderingType::OutOfPegs ? OutOfPegsTransform() * iSrcTransform : iSrcTransform,
            EOdysseyBlendingMode::kNormal,
            EOdysseyAlphaMode::kNormal,
            1.f,
            EOdysseyAntiAliasing::Anisotropic
        );
    };

    return true;
}
