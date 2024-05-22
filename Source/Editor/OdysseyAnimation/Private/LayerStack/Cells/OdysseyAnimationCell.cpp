// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "LayerStack/Cells/OdysseyAnimationCellExport.h"
#include "LayerStack/Cells/OdysseyAnimationCellImport.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "OdysseyAnimation.h"

/* FOdysseyAnimationCell::FOnLengthChanged&
FOdysseyAnimationCell::OnLengthChanged()
{
    static FOnLengthChanged onLengthChanged;
    return onLengthChanged;
} */

FOdysseyAnimationCell::~FOdysseyAnimationCell()
{
}

FOdysseyAnimationCell::FOdysseyAnimationCell(int iLength, UOdysseyAnimationLayer* iLayer)
    : mLayer(iLayer)
    , mLength(iLength)
    , mMarkId(INDEX_NONE)
{
}

UOdysseyAnimationLayer*
FOdysseyAnimationCell::GetLayer() const
{
    return mLayer;
}

int
FOdysseyAnimationCell::GetLength() const
{
    return mLength;
}

int
FOdysseyAnimationCell::GetMarkId() const
{
    return mMarkId;
}

void
FOdysseyAnimationCell::PostDuplicate()
{

}

void
FOdysseyAnimationCell::Serialize(FArchive& Ar)
{
    if( Ar.IsSaving() )
    {
        FOdysseyAnimationCellExport::Write( this, Ar );
    }

    if( Ar.IsLoading() )
    {
        if (!FOdysseyAnimationCellImport::Read( this, Ar ))
        {
            //Old Style No Chunk Loading
            Ar << mLength;
        }
    }
}

void
FOdysseyAnimationCell::PostLoad()
{

}

FOdysseyMediaProvider
FOdysseyAnimationCell::GetMediaProvider(uint32 iFrameIndex) const
{
    return FOdysseyMediaProvider();
}

TSharedPtr<FOdysseyAnimationCell>
FOdysseyAnimationCell::CreateCellFromFrame(uint32 iFrameIndex) const
{
    return nullptr;
}

bool
FOdysseyAnimationCell::IsOutOfPegs() const
{
    return mOutOfPegsPan != FVector2D(0, 0) || mOutOfPegsRotation != 0.f || mOutOfPegsZoom != 1.f;
}

FVector2D
FOdysseyAnimationCell::OutOfPegsPan() const
{
    return mOutOfPegsPan;
}

float
FOdysseyAnimationCell::OutOfPegsRotation() const
{
    return mOutOfPegsRotation;
}

float
FOdysseyAnimationCell::OutOfPegsZoom() const
{
    return mOutOfPegsZoom;
}

::ULIS::FMat3F
FOdysseyAnimationCell::OutOfPegsTransform() const
{
    UOdysseyAnimationLayer* layer = GetLayer();
    if (!layer)
        return ::ULIS::FMat3F();

    UOdysseyAnimation* animation = layer->GetAnimation();
    if (!animation)
        return ::ULIS::FMat3F();

    return ::ULIS::FMat3F::MakeTranslationMatrix(animation->Width() / 2.f, animation->Height() / 2.f)
            * ::ULIS::FMat3F::MakeTranslationMatrix(mOutOfPegsPan.X, mOutOfPegsPan.Y)
            * ::ULIS::FMat3F::MakeRotationMatrix(FMath::DegreesToRadians(mOutOfPegsRotation))
            * ::ULIS::FMat3F::MakeScaleMatrix(mOutOfPegsZoom, mOutOfPegsZoom)
            * ::ULIS::FMat3F::MakeTranslationMatrix(animation->Width() / -2.f, animation->Height() / -2.f);
}

void
FOdysseyAnimationCell::SetOutOfPegsPan(const FVector2D& iPan, bool iIsInteractive)
{
    mOutOfPegsPan = iPan;
    mOnOutOfPegsChanged.Broadcast(iIsInteractive);
    ImageRenderingChanged(iIsInteractive);
}

void
FOdysseyAnimationCell::SetOutOfPegsRotation(float iRotation, bool iIsInteractive)
{
    mOutOfPegsRotation = iRotation;
    mOnOutOfPegsChanged.Broadcast(iIsInteractive);
    ImageRenderingChanged(iIsInteractive);
}

void
FOdysseyAnimationCell::SetOutOfPegsZoom(float iZoom, bool iIsInteractive)
{
    mOutOfPegsZoom = iZoom;
    mOnOutOfPegsChanged.Broadcast(iIsInteractive);
    ImageRenderingChanged(iIsInteractive);
}

FOdysseyAnimationCell::FOnOutOfPegsChanged&
FOdysseyAnimationCell::OnOutOfPegsChanged()
{
    return mOnOutOfPegsChanged;
}