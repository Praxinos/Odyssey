// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTableExport.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTableImport.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTableImageRenderer.h"
#include "LayerStack/Cells/OdysseyAnimationCellsContainer.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"

FOdysseyAnimationLightTable::~FOdysseyAnimationLightTable()
{
}

FOdysseyAnimationLightTable::FOdysseyAnimationLightTable(UOdysseyAnimationLayer* iLayer)
    : mLayer(iLayer)
    , mDisplayPosition(EOdysseyLightTableDisplayPosition::UnderLayer)
    , mPreviousKeysColor(FColor::Orange)
    , mNextKeysColor(FColor(0, 128, 255)) 
    , mPreviousKeysContrast(0.f)
    , mNextKeysContrast(0.f)
{
    //assume 10 frames on the left + 10 frames on the left + 1 current frame
    for (int i = -1; i >= -GetRange(); i--)
    {
        FOdysseyAnimationLightTableKey key;
        key.mIsActivated = (i == -1);
        key.mOpacity = 0.5f;
        mKeys.Add(key);
    }

    for (int i = 1; i <= GetRange(); i++)
    {
        FOdysseyAnimationLightTableKey key;
        key.mIsActivated = (i == 1);
        key.mOpacity = 0.5f;
        mKeys.Add(key);
    }
}

UOdysseyAnimationLayer*
FOdysseyAnimationLightTable::GetLayer() const
{
    return mLayer;
}

EOdysseyLightTableDisplayPosition
FOdysseyAnimationLightTable::GetDisplayPosition() const
{
    return mDisplayPosition;
}

const FOdysseyAnimationLightTableKey*
FOdysseyAnimationLightTable::GetKey(int iIndex) const
{
    if (iIndex == 0 || iIndex > GetRange() || iIndex < -GetRange() )
        return nullptr;

    int index = iIndex > 0 ? iIndex + GetRange() - 1 :  -iIndex - 1;

    return &mKeys[index];
}

FOdysseyAnimationLightTableKey*
FOdysseyAnimationLightTable::GetKey(int iIndex)
{
    if (iIndex == 0 || iIndex > GetRange() || iIndex < -GetRange() )
        return nullptr;

    int index = iIndex > 0 ? iIndex + GetRange() - 1 :  -iIndex - 1;

    return &mKeys[index];
}

bool
FOdysseyAnimationLightTable::GetKeyIsActivated(int iIndex) const
{
    const FOdysseyAnimationLightTableKey* key = GetKey(iIndex);
    if (!key)
        return false;

    return key->mIsActivated;
}

float
FOdysseyAnimationLightTable::GetKeyOpacity(int iIndex) const
{
    const FOdysseyAnimationLightTableKey* key = GetKey(iIndex);
    if (!key)
        return 0.f;

    return key->mOpacity;
}

::ULIS::FColor
FOdysseyAnimationLightTable::GetKeyColor(int iIndex) const
{
    return iIndex > 0 ? ::ULIS::FColor::FromRGBAF(mNextKeysColor.R, mNextKeysColor.G, mNextKeysColor.B, mNextKeysColor.A) : ::ULIS::FColor::FromRGBAF(mPreviousKeysColor.R, mPreviousKeysColor.G, mPreviousKeysColor.B, mPreviousKeysColor.A);
}

const FLinearColor&
FOdysseyAnimationLightTable::GetNextKeysColor() const
{
    return mNextKeysColor;
}

const FLinearColor&
FOdysseyAnimationLightTable::GetPreviousKeysColor() const
{
    return mPreviousKeysColor;
}

float
FOdysseyAnimationLightTable::GetNextKeysContrast() const
{
    return mNextKeysContrast;
}

float
FOdysseyAnimationLightTable::GetPreviousKeysContrast() const
{
    return mPreviousKeysContrast;
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

    if(!mLayer)
        return idComposition;

    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = mLayer->GetCellsContainer();
    if (!cellsContainer)
        return idComposition;

    for (int i = -1; i >= -GetRange(); i--)
    {
        if (!GetKeyIsActivated(i))
            continue;

        int cellIndex = cellsContainer->GetCellIndexAtFrame(iFrameIndex);
        if (cellIndex == INDEX_NONE)
            continue;

        cellIndex += i;
        if (cellIndex < 0)
            continue;

        int cellStartFrame = cellsContainer->GetCellFrame(cellsContainer->GetCells()[cellIndex]);

        //Find the cell or frame 
        idComposition.Append(mLayer->GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType::Render, cellStartFrame));
    }

    for (int i = 1; i <= GetRange(); i++)
    {
        if (!GetKeyIsActivated(i))
            continue;

        //Find the cell or frame 
        int cellIndex = cellsContainer->GetCellIndexAtFrame(iFrameIndex);
        if (cellIndex == INDEX_NONE)
            continue;

        cellIndex += i;
        if (cellIndex >= cellsContainer->GetCells().Num())
            continue;

        int cellStartFrame = cellsContainer->GetCellFrame(cellsContainer->GetCells()[cellIndex]);
        idComposition.Append(mLayer->GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType::Render, cellStartFrame));
    }

    return idComposition;
}

TArray<::ULIS::FRectI>
FOdysseyAnimationLightTable::GetImageRenderingRects() const
{
    if (!mLayer)
        return {};

    return mLayer->GetImageRenderingRects();
}

void
FOdysseyAnimationLightTable::Serialize(FArchive& Ar)
{
    if ( Ar.IsTransacting() || !Ar.IsPersistent() )
        return;

    if( Ar.IsSaving() )
    {
        FOdysseyAnimationLightTableExport::Write( this, Ar );
    }

    if( Ar.IsLoading() )
    {
        FOdysseyAnimationLightTableImport::Read( this, Ar );
    }
}