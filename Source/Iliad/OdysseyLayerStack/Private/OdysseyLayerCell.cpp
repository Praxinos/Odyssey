// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyLayerCell.h"
#include "OdysseyLayerCellImport.h"
#include "OdysseyLayer.h"
#include "Misc/TransactionObjectEvent.h"
#include "ScreenPass.h"
#include "TextureCompiler.h"
#include "TextureResource.h"
#include "Engine/Texture2D.h"
#include "OdysseyBlendShader.h"

void
UOdysseyLayerCell::PostInitProperties() //override
{
    Super::PostInitProperties();
}

void
UOdysseyLayerCell::PostLoad() //override
{
    Super::PostLoad();

#if WITH_EDITOR
    if( Mark_DEPRECATED != -1 )
    {
        Marks.Add( 0, FCellMark{ Mark_DEPRECATED } );
        Mark_DEPRECATED = -1;
    }
#endif
}

void
UOdysseyLayerCell::PostDuplicate( EDuplicateMode::Type iDuplicateMode ) //override
{
    Super::PostDuplicate( iDuplicateMode );
}

//---

UOdysseyLayer*
UOdysseyLayerCell::GetLayer() const
{
    return Cast<UOdysseyLayer>(GetOuter());
}

#if WITH_EDITOR
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
#endif

int32
UOdysseyLayerCell::FrameInLayerToIndexInCell( FFrameNumber iFrame )
{
    FFrameNumber start_in_layer = GetFrameRange().GetLowerBoundValue();
    int32 index_in_cell = ( iFrame - start_in_layer ).Value;

    return index_in_cell;
}

FFrameNumber
UOdysseyLayerCell::IndexInCellToFrameInLayer( int32 iIndex )
{
    FFrameNumber start_in_layer = GetFrameRange().GetLowerBoundValue();
    int32 frame_in_layer = ( iIndex + start_in_layer ).Value;

    return frame_in_layer;
}

UOdysseyLayerStack*
UOdysseyLayerCell::GetLayerStack() const
{
    return GetLayer() ? GetLayer()->GetLayerStack() : nullptr;
}

#if WITH_EDITOR
FOdysseyMediaProvider
UOdysseyLayerCell::GetMediaProvider() const
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

FString
UOdysseyLayerCell::GetName( ECellNameIfEmpty iCellNameIfEmpty ) const
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
UOdysseyLayerCell::HasNoName() const
{
    return Name.IsEmpty();
}

int
UOdysseyLayerCell::GetMark() const
{
    return Marks.Find( 0 ) ? Marks.Find( 0 )->Index : -1;
}

TMap<int, FCellMark>
UOdysseyLayerCell::GetMarks() const
{
    return Marks;
}

#endif

void
UOdysseyLayerCell::SetExposure(int Value)
{
    Modify();
    Exposure = Value;
    ExposureChanged(false);
}

void
UOdysseyLayerCell::ExposureChanged(bool iIsInteractive)
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
UOdysseyLayerCell::GetRenderTexture() const
{
#if WITH_EDITOR
    if (!Texture)
        const_cast<UOdysseyLayerCell*>(this)->InitTexture();
#endif

    return Texture;
}

#if WITH_EDITOR
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

void
UOdysseyLayerCell::SetExposureInteractive(int Value)
{
    Modify();
    Exposure = Value;
    ExposureChanged(true);
}

void
UOdysseyLayerCell::SetName( FString iValue )
{
    Modify();

    Name = iValue;
}

void
UOdysseyLayerCell::SetMark(int iValue)
{
    Modify();

    if( iValue == -1 )
        Marks.Remove( 0 );
    else
        Marks.Add( 0, FCellMark{ iValue } );
}

void
UOdysseyLayerCell::SetMarks( const TMap<int, FCellMark>& iMarks )
{
    Modify();

    Marks = iMarks;
}

void
UOdysseyLayerCell::PostTransacted(const FTransactionObjectEvent& iTransactionEvent)
{
    Super::PostTransacted(iTransactionEvent);

    if ( iTransactionEvent.GetEventType() != ETransactionObjectEventType::UndoRedo )
        return;

    const TArray<FName>& changedPropertyNames = iTransactionEvent.GetChangedProperties();
    if (changedPropertyNames.Contains(GET_MEMBER_NAME_CHECKED(UOdysseyLayerCell, Exposure)))
    {
        ExposureChanged(false);
    }

    if (changedPropertyNames.Contains(GET_MEMBER_NAME_CHECKED(UOdysseyLayerCell, OutOfPegs)))
    {
        OutOfPegsChanged(false);
    }
}

UOdysseyLayerCell*
UOdysseyLayerCell::Break(int Frame, bool bClear)
{
    if (Frame <= 0 || Frame >= Exposure)
        return nullptr;

    Modify();

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
    OutOfPegsChanged(false);
}

void
UOdysseyLayerCell::SetOutOfPegsInteractive(FOdysseyLayerCellOutOfPegs Value)
{
    OutOfPegs = Value;
    OutOfPegsChanged(true);
}

void
UOdysseyLayerCell::OutOfPegsChanged(bool iIsInteractive)
{
    mOnOutOfPegsChanged.Broadcast(iIsInteractive);
    RenderingChanged(iIsInteractive);
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
    Texture->WaitForPendingInitOrStreaming();
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

        FMatrix transform = iSrcTransform;
#if WITH_EDITOR
        if ( iType & EOdysseyRenderingType::OutOfPegs )
            transform = OutOfPegsTransform() * iSrcTransform;
#endif

        FOdysseyBlendShader::BlendRect(
            iGraphBuilder,
            iFeatureLevel,
            iDestinationTexture,
            sourceTexture,
            iDestinationTexture,
            iSrcRect,
            iDstRect,
            transform,
            EOdysseyBlendingMode::kNormal,
            EOdysseyAlphaMode::kNormal,
            1.f,
            EOdysseyAntiAliasing::Anisotropic
        );
    };

    return true;
}

UTextureRenderTarget2D*
UOdysseyLayerCell::CreateRenderingRenderTarget() const
{
    return GetLayer()->CreateRenderingRenderTarget();
}

#if WITH_EDITOR

UTexture2D*
UOdysseyLayerCell::CreateExportTexture( const FString& iAssetName, const FString& iPackagePath, UClass* iAssetClass, UFactory* iFactory )
{
    return GetLayer()->CreateExportTexture(iAssetName, iPackagePath, iAssetClass, iFactory);
}

#endif
