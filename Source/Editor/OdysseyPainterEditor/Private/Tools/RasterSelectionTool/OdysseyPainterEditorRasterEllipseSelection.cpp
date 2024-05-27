// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/RasterSelectionTool/OdysseyPainterEditorRasterEllipseSelection.h"
#include "OdysseyBrushShape.h"
#include "OdysseyHUDPolygon.h"
#include "OdysseyMediaRaster.h"
#include "PainterEditor/OdysseyPainterEditor.h"

FOdysseyPainterEditorRasterEllipseSelection::FOdysseyPainterEditorRasterEllipseSelection(TArray<FVector2D>& iSelectionArea) :
    FOdysseyPainterEditorRasterSelection(iSelectionArea)
{

}


FOdysseyPainterEditorRasterEllipseSelection::~FOdysseyPainterEditorRasterEllipseSelection()
{

}


bool FOdysseyPainterEditorRasterEllipseSelection::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    mSelectionArea.Add(FVector2D(FMath::RoundToInt(iPointInTexture.x), FMath::RoundToInt(iPointInTexture.y)));
    mDownReference = FVector2D(FMath::RoundToInt(iPointInTexture.x), FMath::RoundToInt(iPointInTexture.y));
    return true;
}

EMouseCursor::Type
FOdysseyPainterEditorRasterEllipseSelection::GetMouseCursor() const
{
    return EMouseCursor::Default;
}

void FOdysseyPainterEditorRasterEllipseSelection::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    mSelectionArea.Empty();
    FVector2D referencePoint = FVector2D(FMath::RoundToInt(iPointInTexture.x), FMath::RoundToInt(iPointInTexture.y));
    FVector2D center = (mDownReference + referencePoint) / 2;

    FOdysseyPoint point = iPointInTexture;
    if (false/*Uniform*/)
    {
        int shiftX = referencePoint.X - mDownReference.X;
        int shiftY = referencePoint.Y - mDownReference.Y;

        int signX = shiftX < 0 ? -1 : 1;
        int signY = shiftY < 0 ? -1 : 1;

        int mult = signX == signY ? 1 : -1;

        if (FMath::Abs(shiftX) > FMath::Abs(shiftY))
        {
            referencePoint.X = mDownReference.X + shiftX;
            referencePoint.Y = mDownReference.Y + shiftX * mult;
        }
        else
        {
            referencePoint.X = mDownReference.X + shiftY * mult;
            referencePoint.Y = mDownReference.Y + shiftY;
        }
    }

    int a = FMath::Abs( mDownReference.X - referencePoint.X ) / 2;
    int b = FMath::Abs( mDownReference.Y - referencePoint.Y ) / 2;
    UOdysseyBrushShape::GenerateEllipsePoints( center, a, b, 0, mSelectionArea );
}

bool FOdysseyPainterEditorRasterEllipseSelection::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
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
    */
    return true;
}

bool FOdysseyPainterEditorRasterEllipseSelection::OnKeyUp(const FKey& iKey)
{
    return FOdysseyPainterEditorRasterSelection::OnKeyUp(iKey);
}

TArray<::ULIS::FRectI> FOdysseyPainterEditorRasterEllipseSelection::GetSelectionAreaAsScanlines()
{
    TArray<::ULIS::FRectI> rectangles;
    /*
    ::ULIS::FRectI boundingBox = GetSelectionAreaBoundingRect();
    int maxX = boundingBox.x + boundingBox.w;
    int maxY = boundingBox.y + boundingBox.h;
    int minX = boundingBox.x;
    int minY = boundingBox.y;

    for (int y = minY; y <= maxY; y++)
    {
        std::vector< int > nodesX;
        int j = int(mSelectionArea.Num() - 1);

        for (int i = 0; i < mSelectionArea.Num(); i++)
        {
            if ((mSelectionArea[i].Y < y && mSelectionArea[j].Y >= y) || (mSelectionArea[j].Y < y && mSelectionArea[i].Y >= y))
            {
                nodesX.push_back(int(mSelectionArea[i].X + double(y - mSelectionArea[i].Y) / double(mSelectionArea[j].Y - mSelectionArea[i].Y) * (mSelectionArea[j].X - mSelectionArea[i].X)));
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
    */
    return rectangles;
}
