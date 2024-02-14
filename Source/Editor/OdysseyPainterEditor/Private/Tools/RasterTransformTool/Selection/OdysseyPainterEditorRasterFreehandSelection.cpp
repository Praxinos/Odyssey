// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/RasterTransformTool/Selection/OdysseyPainterEditorRasterFreehandSelection.h"
#include "OdysseyHUDPolygon.h"
#include "OdysseyHUDElement.h"
#include "OdysseyMediaRaster.h"
#include "OdysseyPainterEditor.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorRasterFreehandSelection::~UOdysseyPainterEditorRasterFreehandSelection()
{
}

UOdysseyPainterEditorRasterFreehandSelection::UOdysseyPainterEditorRasterFreehandSelection()
{
}

bool UOdysseyPainterEditorRasterFreehandSelection::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if (!mIsSelectionAreaSet) //Creating a zone for the selection
    {
        TArray<FVector2D> areaPoints;
        areaPoints.Add(FVector2D(FMath::RoundToInt(iPointInTexture.x), FMath::RoundToInt(iPointInTexture.y)));
        mSelectionArea = new FOdysseyHUDPolygon(FName("SelectionArea"), areaPoints);
        mHUD->AddElement(mSelectionArea);

        return true;
    }

    return false;
}

void UOdysseyPainterEditorRasterFreehandSelection::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    mMouseCursor = EMouseCursor::Default;
}

void UOdysseyPainterEditorRasterFreehandSelection::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    if (!mIsSelectionAreaSet)
    {
        mSelectionArea->GetPoints().Add(FVector2D(FMath::RoundToInt(iPointInTexture.x), FMath::RoundToInt(iPointInTexture.y)));
    }
}

bool UOdysseyPainterEditorRasterFreehandSelection::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
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

        TArray<::ULIS::FRectI> rectangles = GetSelectionAreaAsScanlines();

        mSelectionBlock = MakeShareable(new ::ULIS::FBlock(boundingBox.w, boundingBox.h, format));
        ClearBlock(mSelectionBlock);

        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);

        for (int i = 0; i < rectangles.Num(); i++)
        {
            int decalX = FMath::Min(rectangles[i].x, 0);
            int decalY = FMath::Min(rectangles[i].y, 0);

            ctx.Copy(
                *rasterBlock->GetBlock(),
                *mSelectionBlock,
                rectangles[i],
                ::ULIS::FVec2I(-decalX - boundingBox.x + rectangles[i].x, -decalY - boundingBox.y + rectangles[i].y),
                ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
                0,
                nullptr,
                nullptr
            );
        }

        ctx.Finish();

        mIsSelectionAreaSet = true;
        return true;
    }
    return false;
}

bool UOdysseyPainterEditorRasterFreehandSelection::OnKeyUp(const FKey& iKey)
{
    return UOdysseyPainterEditorRasterSelection::OnKeyUp(iKey);
}

TArray<::ULIS::FRectI> UOdysseyPainterEditorRasterFreehandSelection::GetSelectionAreaAsScanlines()
{
    TArray<::ULIS::FRectI> rectangles;
    ::ULIS::FRectI boundingBox = GetSelectionAreaBoundingRect();
    int maxX = boundingBox.x + boundingBox.w;
    int maxY = boundingBox.y + boundingBox.h;
    int minX = boundingBox.x;
    int minY = boundingBox.y;

    TArray<FVector2D>& points = mSelectionArea->GetPoints();

    for (int y = minY; y <= maxY; y++)
    {
        std::vector< int > nodesX;
        int j = int(points.Num() - 1);

        for (int i = 0; i < points.Num(); i++)
        {
            if ((points[i].Y < y && points[j].Y >= y) || (points[j].Y < y && points[i].Y >= y))
            {
                nodesX.push_back(int(points[i].X + double(y - points[i].Y) / double(points[j].Y - points[i].Y) * (points[j].X - points[i].X)));
            }
            j = i;
        }

        int i = 0;
        int size = int(nodesX.size() - 1);
        while (i < size)
        {
            if (nodesX[i] > nodesX[i + 1])
            {
                int temp = nodesX[i];
                nodesX[i] = nodesX[i + 1];
                nodesX[i + 1] = temp;
                if (i > 0)
                    i--;
            }
            else
            {
                i++;
            }
        }

        for (i = 0; i < nodesX.size(); i += 2)
        {
            if (nodesX[i] > maxX) break;
            if (nodesX[i + 1] > minX)
            {
                if (nodesX[i] < minX)
                    nodesX[i] = minX;
                if (nodesX[i + 1] > maxX)
                    nodesX[i + 1] = maxX;

                rectangles.Add(::ULIS::FRectI::FromXYWH(nodesX[i], y, nodesX[i + 1] - nodesX[i], 1));
            }
        }
    }

    return rectangles;
}
