// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/RasterSelectionTool/OdysseyPainterEditorRasterSelectionTool.h"
#include "GeomTools.h"
#include "OdysseyHUDPolygon.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyHUDSystem.h"
#include "OdysseyBrushShape.h"
#include "OdysseyMediaRaster.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorRasterSelectionTool::~UOdysseyPainterEditorRasterSelectionTool()
{
    if (mSelectionBlock)
    {
        mSelectionBlock.Reset();
        mSelectionBlock = nullptr;
    }
}

UOdysseyPainterEditorRasterSelectionTool::UOdysseyPainterEditorRasterSelectionTool():
    mIsSelectionAreaSet(false),
    mToolSelectionArea(nullptr),
    mPaintEngine(),
    mSelectionBlock(nullptr)
{
    Icon = *FOdysseyStyle::GetBrush("PainterEditor.ToolsTab.Lasso32");
}

bool UOdysseyPainterEditorRasterSelectionTool::IsActivable() const
{
    return true;
}

bool UOdysseyPainterEditorRasterSelectionTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    mToolSelectionArea = MakeShared<FOdysseyHUDPolygon>();
    mHUD->AddElement(mToolSelectionArea);

    switch (SelectionShape)
    {
        case EOdysseySelectionShape::Rectangle:
        {
            for ( int i = 0; i < 4; i++ )
            {
                mToolSelectionArea->GetPoints().Add(FVector2D(FMath::RoundToInt(iPointInTexture.x), FMath::RoundToInt(iPointInTexture.y)));
            }
        }
        break;
        
        case EOdysseySelectionShape::Freehand:
        {
            mToolSelectionArea->GetPoints().Add(FVector2D(FMath::RoundToInt(iPointInTexture.x), FMath::RoundToInt(iPointInTexture.y)));
        }
        break;

        case EOdysseySelectionShape::Ellipse:
        {
            mToolSelectionArea->GetPoints().Add(FVector2D(FMath::RoundToInt(iPointInTexture.x), FMath::RoundToInt(iPointInTexture.y)));
            mDownReference = FVector2D(FMath::RoundToInt(iPointInTexture.x), FMath::RoundToInt(iPointInTexture.y));
        }
        break;
        
        default:
            return false;
        break;
    }

    return true;
}

void UOdysseyPainterEditorRasterSelectionTool::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
}

void UOdysseyPainterEditorRasterSelectionTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    switch (SelectionShape)
    {
    case EOdysseySelectionShape::Rectangle:
        ConstrainSelectionToRectangle(FVector2D(FMath::RoundToInt(iPointInTexture.x), FMath::RoundToInt(iPointInTexture.y)));
        break;
    case EOdysseySelectionShape::Freehand:
        mToolSelectionArea->GetPoints().Add(FVector2D(FMath::RoundToInt(iPointInTexture.x), FMath::RoundToInt(iPointInTexture.y)));
        break;
    case EOdysseySelectionShape::Ellipse:
        ConstrainSelectionToEllipse( iPointInTexture );
        break;
    default:
        break;
    }
}

bool UOdysseyPainterEditorRasterSelectionTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    //Merge/replace/substract HUD of selection to already existing selection if it exists

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

    mHUD->RemoveElement(mToolSelectionArea);
   
    return true;
}

bool UOdysseyPainterEditorRasterSelectionTool::OnKeyDown(const FKey& iKey)
{
    if (iKey == EKeys::LeftShift || iKey == EKeys::RightShift)
    {
        Uniform = !Uniform;
        return true;
    }
    return false;
}

bool UOdysseyPainterEditorRasterSelectionTool::OnKeyUp(const FKey& iKey)
{
    if (iKey == EKeys::LeftShift || iKey == EKeys::RightShift)
    {
        Uniform = !Uniform;
        return true;
    }
    else if (iKey == EKeys::Enter || iKey == EKeys::SpaceBar || iKey == EKeys::Escape)
    {
        ClearSelection();
        return true;
    }

    return false;
}

void UOdysseyPainterEditorRasterSelectionTool::Load()
{
    UOdysseyPainterEditorTool::Load();
}

void UOdysseyPainterEditorRasterSelectionTool::Unload()
{
    ClearSelection();
}

bool UOdysseyPainterEditorRasterSelectionTool::IsSelectionAreaSet()
{
    return mIsSelectionAreaSet;
}

bool UOdysseyPainterEditorRasterSelectionTool::IsInSelectionArea(FVector2D iPoint)
{
    if( !mToolSelectionArea || mToolSelectionArea->GetPoints().Num() == 0 )
        return false;

    return FGeomTools2D::IsPointInPolygon(iPoint, mToolSelectionArea->GetPoints()); 
}

TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> UOdysseyPainterEditorRasterSelectionTool::GetSelectionBlock()
{
    return mSelectionBlock;
}

::ULIS::FRectI UOdysseyPainterEditorRasterSelectionTool::GetSelectionAreaBoundingRect()
{
    if (mToolSelectionArea && mToolSelectionArea->GetPoints().Num() != 0)
    {
        int minX = mToolSelectionArea->GetPoints()[0].X;
        int maxX = mToolSelectionArea->GetPoints()[0].X;
        int minY = mToolSelectionArea->GetPoints()[0].Y;
        int maxY = mToolSelectionArea->GetPoints()[0].Y;
        for (int i = 1; i < mToolSelectionArea->GetPoints().Num(); i++)
        {
            minX = FMath::Min(minX, mToolSelectionArea->GetPoints()[i].X);
            maxX = FMath::Max(maxX, mToolSelectionArea->GetPoints()[i].X);
            minY = FMath::Min(minY, mToolSelectionArea->GetPoints()[i].Y);
            maxY = FMath::Max(maxY, mToolSelectionArea->GetPoints()[i].Y);
        }
        return ::ULIS::FRectI::FromMinMax(minX, minY, maxX, maxY);
    }
    return ::ULIS::FRectI::FromXYWH(0, 0, 0, 0);
}

bool UOdysseyPainterEditorRasterSelectionTool::IsSelectionValid(::ULIS::FRectI iSelectionArea)
{
    if (iSelectionArea.w > 8192 || iSelectionArea.h > 8192) //Unreal limitations + very slow in ULIS at these sizes
        return false;

    return true;
}

void UOdysseyPainterEditorRasterSelectionTool::ClearSelection()
{
    mHUD->EmptyElements();
    mIsSelectionAreaSet = false;
    if( mSelectionBlock )
    {
        mSelectionBlock.Reset();
        mSelectionBlock = nullptr;
    }
    mToolSelectionArea = nullptr;
}

TArray<::ULIS::FRectI> UOdysseyPainterEditorRasterSelectionTool::GetSelectionAreaAsScanlines()
{
    TArray<::ULIS::FRectI> rectangles;
    ::ULIS::FRectI boundingBox = GetSelectionAreaBoundingRect();
    int maxX = boundingBox.x + boundingBox.w;
    int maxY = boundingBox.y + boundingBox.h;
    int minX = boundingBox.x;
    int minY = boundingBox.y;

    TArray<FVector2D>& points = mToolSelectionArea->GetPoints();

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

void UOdysseyPainterEditorRasterSelectionTool::ClearBlock(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock)
{
    if (!iBlock)
        return;

    ::ULIS::eFormat format = iBlock->Format();
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
    ::ULIS::FEvent clearEvent;

    ctx.Clear(
        *iBlock,
        iBlock->Rect(),
        ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
        0,
        nullptr,
        &clearEvent);

    ctx.Finish();
}

void UOdysseyPainterEditorRasterSelectionTool::ConstrainSelectionToEllipse(const FOdysseyPoint& iPointInTexture)
{
    mToolSelectionArea->GetPoints().Empty();
    FVector2D referencePoint = FVector2D(FMath::RoundToInt(iPointInTexture.x), FMath::RoundToInt(iPointInTexture.y));
    FVector2D center = (mDownReference + referencePoint) / 2;

    FOdysseyPoint point = iPointInTexture;
    if (Uniform)
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

    int a = FMath::Abs(mDownReference.X - referencePoint.X) / 2;
    int b = FMath::Abs(mDownReference.Y - referencePoint.Y) / 2;
    UOdysseyBrushShape::GenerateEllipsePoints(center, a, b, 0, mToolSelectionArea->GetPoints());
}

void UOdysseyPainterEditorRasterSelectionTool::ConstrainSelectionToRectangle(FVector2D iPosition)
{
    if (mToolSelectionArea->GetPoints().Num() == 0)
        return;

    if (Uniform)
    {
        int shiftX = iPosition.X - mToolSelectionArea->GetPoints()[0].X;
        int shiftY = iPosition.Y - mToolSelectionArea->GetPoints()[0].Y;

        int signX = shiftX < 0 ? -1 : 1;
        int signY = shiftY < 0 ? -1 : 1;

        int mult = signX == signY ? 1 : -1;

        if (FMath::Abs(shiftX) > FMath::Abs(shiftY))
        {
            iPosition.X = mToolSelectionArea->GetPoints()[0].X + shiftX;
            iPosition.Y = mToolSelectionArea->GetPoints()[0].Y + shiftX * mult;
        }
        else
        {
            iPosition.X = mToolSelectionArea->GetPoints()[0].X + shiftY * mult;
            iPosition.Y = mToolSelectionArea->GetPoints()[0].Y + shiftY;
        }
    }

    mToolSelectionArea->GetPoints()[2] = iPosition;
    mToolSelectionArea->GetPoints()[1] = FVector2D(iPosition.X, mToolSelectionArea->GetPoints()[0].Y);
    mToolSelectionArea->GetPoints()[3] = FVector2D(mToolSelectionArea->GetPoints()[0].X, iPosition.Y);
}
