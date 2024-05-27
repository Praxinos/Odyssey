// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/RasterSelectionTool/OdysseyPainterEditorRasterRectangleSelection.h"
#include "OdysseyHUDPolygon.h"
#include "OdysseyMediaRaster.h"
#include "OdysseyPainterEditor.h"

FOdysseyPainterEditorRasterRectangleSelection::FOdysseyPainterEditorRasterRectangleSelection(TArray<FVector2D>& iSelectionArea):
    FOdysseyPainterEditorRasterSelection( iSelectionArea )
{

}


FOdysseyPainterEditorRasterRectangleSelection::~FOdysseyPainterEditorRasterRectangleSelection()
{

}


bool FOdysseyPainterEditorRasterRectangleSelection::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    mSelectionArea.Empty();
    for (int i = 0; i < 4; i++)
    {
        mSelectionArea.Add(FVector2D(FMath::RoundToInt(iPointInTexture.x), FMath::RoundToInt(iPointInTexture.y)));
    }

    return true;
}

EMouseCursor::Type
FOdysseyPainterEditorRasterRectangleSelection::GetMouseCursor() const
{
    return EMouseCursor::Default;
}

void FOdysseyPainterEditorRasterRectangleSelection::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    ConstrainSelectionToRectangle(FVector2D(FMath::RoundToInt(iPointInTexture.x), FMath::RoundToInt(iPointInTexture.y)));
}

bool FOdysseyPainterEditorRasterRectangleSelection::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
/*
    if (!mIsSelectionAreaSet)
    {
        TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaRaster>();
        if (mediaRasters.Num() <= 0)
            return false;

        TSharedPtr<FOdysseyRasterBlock> rasterBlock = mediaRasters[0]->GetRasterBlock();
        mPaintEngine.RasterBlock(rasterBlock);

        ::ULIS::eFormat format = rasterBlock->GetFormat();
        ::ULIS::FRectI boundingBox = GetSelectionAreaBoundingRect();

        if (!IsSelectionValid(boundingBox))
        {
            ClearSelection();
            return true;
        }

        int decalX = FMath::Min(boundingBox.x, 0);
        int decalY = FMath::Min(boundingBox.y, 0);

        mSelectionBlock = MakeShareable(new ::ULIS::FBlock(boundingBox.w, boundingBox.h, format));
        ClearBlock(mSelectionBlock);

        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);

        ctx.Copy(
            *rasterBlock->GetBlock(),
            *mSelectionBlock,
            boundingBox,
            ::ULIS::FVec2I(-decalX, -decalY),
            ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
            0,
            nullptr,
            nullptr
        );

        ctx.Finish();

        mIsSelectionAreaSet = true;
        return true;
    }

    return false;*/
    return true;
}

bool FOdysseyPainterEditorRasterRectangleSelection::OnKeyUp(const FKey& iKey)
{
    return FOdysseyPainterEditorRasterSelection::OnKeyUp( iKey );
}

void FOdysseyPainterEditorRasterRectangleSelection::ConstrainSelectionToRectangle(FVector2D iPosition)
{
    if (mSelectionArea.Num() == 0)
        return;

    if (false/*Uniform*/)
    {
        int shiftX = iPosition.X - mSelectionArea[0].X;
        int shiftY = iPosition.Y - mSelectionArea[0].Y;

        int signX = shiftX < 0 ? -1 : 1;
        int signY = shiftY < 0 ? -1 : 1;

        int mult = signX == signY ? 1 : -1;

        if (FMath::Abs(shiftX) > FMath::Abs(shiftY))
        {
            iPosition.X = mSelectionArea[0].X + shiftX;
            iPosition.Y = mSelectionArea[0].Y + shiftX * mult;
        }
        else
        {
            iPosition.X = mSelectionArea[0].X + shiftY * mult;
            iPosition.Y = mSelectionArea[0].Y + shiftY;
        }
    }

    mSelectionArea[2] = iPosition;
    mSelectionArea[1] = FVector2D(iPosition.X, mSelectionArea[0].Y);
    mSelectionArea[3] = FVector2D(mSelectionArea[0].X, iPosition.Y);
}

