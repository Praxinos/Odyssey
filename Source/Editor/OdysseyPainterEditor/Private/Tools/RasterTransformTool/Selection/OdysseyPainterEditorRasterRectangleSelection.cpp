// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/RasterTransformTool/Selection/OdysseyPainterEditorRasterRectangleSelection.h"
#include "OdysseyHUDPolygon.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorRasterRectangleSelection"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorRasterRectangleSelection::~UOdysseyPainterEditorRasterRectangleSelection()
{
}

UOdysseyPainterEditorRasterRectangleSelection::UOdysseyPainterEditorRasterRectangleSelection()
{
}

bool UOdysseyPainterEditorRasterRectangleSelection::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if (!mIsSelectionAreaSet) //Creating a zone for the selection
    {
        TArray<FVector2D> areaPoints;
        for (int i = 0; i < 4; i++)
        {
            areaPoints.Add(FVector2D(iPointInTexture.x, iPointInTexture.y));
        }

        mSelectionArea = new FOdysseyHUDPolygon(FName("SelectionArea"), areaPoints);
        mHUD->AddElement(mSelectionArea);
        return true;
    }

    return false;
}

void UOdysseyPainterEditorRasterRectangleSelection::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{

}

void UOdysseyPainterEditorRasterRectangleSelection::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    if (!mIsSelectionAreaSet)
        ConstrainSelectionToRectangle(FVector2D(iPointInTexture.x, iPointInTexture.y));
}

bool UOdysseyPainterEditorRasterRectangleSelection::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
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

    return false;
}

bool UOdysseyPainterEditorRasterRectangleSelection::OnKeyUp(const FKey& iKey)
{
    return UOdysseyPainterEditorRasterSelection::OnKeyUp( iKey );
}

void UOdysseyPainterEditorRasterRectangleSelection::ConstrainSelectionToRectangle(FVector2D iPosition)
{
    if( mSelectionArea->GetPoints().Num() != 0)
    {
        TArray<FVector2D>& points = mSelectionArea->GetPoints();
        points[2] = iPosition;
        points[1] = FVector2D(iPosition.X, points[0].Y);
        points[3] = FVector2D(points[0].X, iPosition.Y);
    }
}

#undef LOCTEXT_NAMESPACE
