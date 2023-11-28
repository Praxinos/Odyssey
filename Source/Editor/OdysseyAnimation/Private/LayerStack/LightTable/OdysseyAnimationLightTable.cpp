// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"

FOdysseyAnimationLightTable::~FOdysseyAnimationLightTable()
{
}

FOdysseyAnimationLightTable::FOdysseyAnimationLightTable(UOdysseyAnimationLayer* iLayer)
    : mOwnerLayer(iLayer)
    , mSourceLayer(iLayer)
    , mDisplayPosition(EOdysseyLightTableDisplayPosition::UnderLayer)
    , mDisplayMode(EOdysseyLightTableDisplayMode::Default)
{
    //assume 10 frames on the left + 10 frames on the left + 1 current frame
    for (int i = -1; i >= -GetRange(); i--)
    {
        FKeyData keyData;
        keyData.mIsActivated = false;
        keyData.mOpacity = 0.5f;
        mKeysData.Add(i, keyData);
    }

    for (int i = 1; i <= GetRange(); i++)
    {
        FKeyData keyData;
        keyData.mIsActivated = false;
        keyData.mOpacity = 0.5f;
        mKeysData.Add(i, keyData);
    }
    
    mKeysData[1].mIsActivated = true;
    mKeysData[-1].mIsActivated = true;
}

UOdysseyAnimationLayer*
FOdysseyAnimationLightTable::GetOwnerLayer() const
{
    return mOwnerLayer;
}

UOdysseyAnimationLayer*
FOdysseyAnimationLightTable::GetSourceLayer() const
{
    return mSourceLayer;
}

EOdysseyLightTableDisplayPosition
FOdysseyAnimationLightTable::GetDisplayPosition() const
{
    return mDisplayPosition;
}

EOdysseyLightTableDisplayMode
FOdysseyAnimationLightTable::GetDisplayMode() const
{
    return mDisplayMode;
}

bool
FOdysseyAnimationLightTable::GetKeyIsActivated(int iIndex) const
{
    if (iIndex == 0 || iIndex > GetRange() || iIndex < -GetRange() )
        return false;

    return mKeysData[iIndex].mIsActivated;
}

int
FOdysseyAnimationLightTable::GetKeyOffset(int iIndex) const
{
    if (iIndex == 0 || iIndex > GetRange() || iIndex < -GetRange() )
        return 0;

    return mKeysData[iIndex].mOffset;
}

float
FOdysseyAnimationLightTable::GetKeyOpacity(int iIndex) const
{
    if (iIndex == 0 || iIndex > GetRange() || iIndex < -GetRange() )
        return 0;

    return mKeysData[iIndex].mOpacity;
}

FOdysseyAnimationLightTable::eFrameDisplayMode
FOdysseyAnimationLightTable::GetKeyDisplayMode() const
{
    switch ( mDisplayMode )
    {
        case EOdysseyLightTableDisplayMode::Default: return eFrameDisplayMode::kDefault;
        case EOdysseyLightTableDisplayMode::Color: return eFrameDisplayMode::kColored;
        case EOdysseyLightTableDisplayMode::Tint: return eFrameDisplayMode::kTint;
        case EOdysseyLightTableDisplayMode::ColorGradient: return eFrameDisplayMode::kColored;
        case EOdysseyLightTableDisplayMode::TintGradient: return eFrameDisplayMode::kTint;
    }

    return eFrameDisplayMode::kDefault;
}

::ULIS::FColor
FOdysseyAnimationLightTable::GetKeyColor(int iIndex) const
{
    //TODO:
    return ::ULIS::FColor();
}

const TMap<int, FOdysseyAnimationLightTable::FKeyData>&
FOdysseyAnimationLightTable::GetKeysData() const
{
    return mKeysData;
}

int
FOdysseyAnimationLightTable::GetRange() const
{
    return 10;
}

TSharedPtr<IOdysseyImageRenderer>
FOdysseyAnimationLightTable::BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame, FImageRendererFilter iFilter) const
{
    if (iFilter.IsBound() && !iFilter.Execute(this))
        return nullptr;
    
    return MakeShared<FOdysseyAnimationLightTableImageRenderer>(SharedThis(this), iFrame, iRenderType, GetImageRenderingRects(), iFilter);
}

TArray<FGuid>
FOdysseyAnimationLightTable::GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrameIndex) const
{
    TArray<FGuid> idComposition = { GetImageRenderingId() };

    if(!mSourceLayer)
        return idComposition;

    for (int i = -1; i >= -GetRange(); i--)
    {
        if (!mKeysData[i].mIsActivated)
            continue;

        //Find the cell or frame 
        int offset = mKeysData[i].mOffset;
        int celFrameIndex = INDEX_NONE;
        idComposition.Append(mSourceLayer->GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType::Render, iFrameIndex + offset));
    }

    for (int i = 1; i <= GetRange(); i++)
    {
        if (!mKeysData[i].mIsActivated)
            continue;

        //Find the cell or frame 
        int offset = mKeysData[i].mOffset;
        int celFrameIndex = INDEX_NONE;
        idComposition.Append(mSourceLayer->GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType::Render, iFrameIndex + offset));
    }

    return idComposition;
}

TArray<::ULIS::FRectI>
FOdysseyAnimationLightTable::GetImageRenderingRects() const
{
    if (!mSourceLayer)
        return {};

    return mSourceLayer->GetImageRenderingRects();
}
