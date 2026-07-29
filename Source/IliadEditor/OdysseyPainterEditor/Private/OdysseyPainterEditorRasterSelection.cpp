// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorRasterSelection.h"

#include "ULISLoaderModule.h"
#include "OdysseyHUDLine.h"
#include "OdysseyHUDPolygon.h"

UOdysseyPainterEditorRasterSelection::~UOdysseyPainterEditorRasterSelection()
{
    if (mRasterBlock)
        mRasterBlock->OnBlockCommited().RemoveAll(this);
}

UOdysseyPainterEditorRasterSelection::UOdysseyPainterEditorRasterSelection()
    : mHUD(MakeShared<FOdysseyHUDElement>())
{
    mDottedSelectionCustomization.mColors.Add(FLinearColor::Black);
    mDottedSelectionCustomization.mColors.Add(FLinearColor::White);
    mDottedSelectionCustomization.mGapLength = 5.f;
    mDottedSelectionCustomization.mSegmentLength = 10.f;
    mDottedSelectionCustomization.mSpeed = 10.f;
    mDottedSelectionCustomization.mIsActive = false;
    mHUD->SetCustomization( mDottedSelectionCustomization );
    mShowHUD = true;
}

FSimpleMulticastDelegate&
UOdysseyPainterEditorRasterSelection::OnChanged()
{
    return mOnChanged;
}

void
UOdysseyPainterEditorRasterSelection::Init(int iWidth, int iHeight)
{
    if( mRasterBlock )
        mRasterBlock->OnBlockCommited().RemoveAll(this);

    mRasterBlock = MakeShared<FOdysseyRasterBlock>(this, iWidth, iHeight, ::ULIS::Format_GF);
    mBlock = mRasterBlock->GetBlock(); //This initialize the block inside of mRasterBlock
    mRasterMutator.SetRasterBlock( mRasterBlock );

    Clear();
    RefreshHUD();

    mRasterBlock->OnBlockCommited().AddUObject(this, &UOdysseyPainterEditorRasterSelection::OnBlockCommited);

    mOnChanged.Broadcast();
}

void
UOdysseyPainterEditorRasterSelection::Reset()
{
    if( mRasterBlock )
        mRasterBlock->OnBlockCommited().RemoveAll(this);

    mBlock = nullptr;
    mRasterBlock = nullptr;
    RefreshHUD();

    mOnChanged.Broadcast();
}

void
UOdysseyPainterEditorRasterSelection::Clear()
{
    ::ULIS::FRectI boundingRect = ::ULIS::FRectI::FromXYWH(0, 0, mBlock->Width(), mBlock->Height());

    mRasterMutator.EditTilesFromRects(
        { boundingRect },
        [&](TSharedPtr<::ULIS::FBlock> iBlock, const FOdysseyInvalidTileMap& iTileMap) -> TArray<::ULIS::FEvent>
        {
            ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_GF);

            ctx.Clear(*mBlock);

            ctx.Finish();

            return { };
        }
    );

    mRasterMutator.Commit();

    RefreshHUD();

    mOnChanged.Broadcast();
}

bool
UOdysseyPainterEditorRasterSelection::IsEmpty() const
{
    return mBoundingRect.Area() <= 0;
}

void
UOdysseyPainterEditorRasterSelection::Add(const TArray<FVector2D>& iPolygon)
{
    ::ULIS::FRectI boundingRect = ComputeBoundingRect(iPolygon);

    std::vector<::ULIS::FVec2I> points;
    for (const FVector2D& point : iPolygon)
    {
        points.push_back(::ULIS::FVec2I(point.X, point.Y));
    }

    mRasterMutator.EditTilesFromRects(
        { boundingRect },
        [&](TSharedPtr<::ULIS::FBlock> iBlock, const FOdysseyInvalidTileMap& iTileMap) -> TArray<::ULIS::FEvent>
        {
            ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_GF);

            ctx.DrawPolygon(*mBlock, points, ::ULIS::FColor::FromGrey8(255), true);

            ctx.Finish();

            return { };
        }
    );

    mRasterMutator.Commit();

    RefreshHUD();

    mOnChanged.Broadcast();
}

void
UOdysseyPainterEditorRasterSelection::Substract(const TArray<FVector2D>& iPolygon)
{
    ::ULIS::FRectI boundingRect = ComputeBoundingRect(iPolygon);

    std::vector<::ULIS::FVec2I> points;
    for ( const FVector2D& point : iPolygon )
    {
        points.push_back(::ULIS::FVec2I(point.X, point.Y));
    }

    mRasterMutator.EditTilesFromRects(
        { boundingRect },
        [&](TSharedPtr<::ULIS::FBlock> iBlock, const FOdysseyInvalidTileMap& iTileMap) -> TArray<::ULIS::FEvent>
        {
            ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_GF);

            ctx.DrawPolygon(*mBlock, points, ::ULIS::FColor::FromGrey8(0), true);
            ctx.Finish();

            return { };
        }
    );

    mRasterMutator.Commit();

    RefreshHUD();

    mOnChanged.Broadcast();
}

void UOdysseyPainterEditorRasterSelection::Invert()
{
    if( !mBlock )
        return;

    ::ULIS::FRectI boundingRect = ::ULIS::FRectI::FromXYWH(0, 0, mBlock->Width(), mBlock->Height());

    mRasterMutator.EditTilesFromRects(
        { boundingRect },
        [&](TSharedPtr<::ULIS::FBlock> iBlock, const FOdysseyInvalidTileMap& iTileMap) -> TArray<::ULIS::FEvent>
        {
            ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_GF);
            ctx.FilterInto(
                [this](const ::ULIS::FPixel& iSrcPixel, ::ULIS::FPixel& iDstPixel, uint64 iNumPixels)
                {
                    for (int i = 0; i < iNumPixels; i++, iSrcPixel.Next(), iDstPixel.Next())
                    {
                        if (iSrcPixel.GreyF() == 0.f)
                            iDstPixel.SetGreyF(1.f);
                        else
                            iDstPixel.SetGreyF(0.f);
                    }
                }
                , *mBlock
                , *mBlock
            );

            ctx.Finish();

            return { };
        }
    );

    mRasterMutator.Commit();

    RefreshHUD();
    mOnChanged.Broadcast();
}

::ULIS::FRectI
UOdysseyPainterEditorRasterSelection::GetMaskBoundingRect() const
{
    return mBoundingRect;
}

TSharedPtr<FOdysseyRasterBlock>
UOdysseyPainterEditorRasterSelection::GetRasterBlock()
{
    return mRasterBlock;
}

TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>
UOdysseyPainterEditorRasterSelection::GetBlock()
{
    return mBlock;
}

TSharedPtr<FOdysseyHUDElement>
UOdysseyPainterEditorRasterSelection::GetHUD()
{
    return mHUD;
}

void
UOdysseyPainterEditorRasterSelection::RefreshHUD()
{
    mHUD->EmptyElements();
    mHUD->InactivateCustomization();

    mBoundingRect = ::ULIS::FRectI::FromXYWH(0, 0, 0, 0);

    if (!mBlock)
        return;

    TArray<FVector2D> points;
    TArray<FIntEdge> edges;

    for (int y = 0; y <= mBlock->Height(); y++)
    {
        float* topLine = y > 0 ? (float*)mBlock->ScanlineBits(y - 1) : nullptr;
        float* line = y < mBlock->Height() ? (float*)mBlock->ScanlineBits(y) : nullptr;
        for (int x = 0; x <= mBlock->Width(); x++)
        {
            float leftValue = line && x > 0 ? line[x - 1] : 0.f;
            float topValue = topLine && x < mBlock->Width() ? topLine[x] : 0.f;
            float value = line && x < mBlock->Width() ? line[x] : 0.f;

            if (leftValue == 0.f && value != 0.f || leftValue != 0.f && value == 0.f)
            {
                //left vertical Line
                points.Add(FVector2D(x, y));
                points.Add(FVector2D(x, y+1));
                edges.Add({ FIntPoint(x, y), FIntPoint(x, y + 1) });
            }

            if (topValue == 0.f && value != 0.f || topValue != 0.f && value == 0.f)
            {
                //top horizontal Line
                points.Add(FVector2D(x, y));
                points.Add(FVector2D(x+1, y));
                edges.Add({ FIntPoint(x, y), FIntPoint(x + 1, y) });
            }
        }
    }

    mBoundingRect = ComputeBoundingRect(points);

    if (!mShowHUD)
        return;

    TArray<TArray<FVector2D>> contours = BuildContours(edges);

    for (const TArray<FVector2D>& contour : contours)
    {
        TSharedPtr<FOdysseyHUDPolygon> polygonHUD = MakeShared<FOdysseyHUDPolygon>();
        TArray<FVector2D>& polygonPoints = polygonHUD->GetPoints();
        for( FVector2D point : contour )
        {
            polygonPoints.Add(point);
        }

        mHUD->AddElement(polygonHUD);
    }

    mHUD->ActivateCustomization();

}

void UOdysseyPainterEditorRasterSelection::HideSelectionHUD()
{
    mShowHUD = false;
    RefreshHUD();
}

void UOdysseyPainterEditorRasterSelection::ShowSelectionHUD()
{
    mShowHUD = true;
    RefreshHUD();
}

TArray<TArray<FVector2D>> UOdysseyPainterEditorRasterSelection::BuildContours(const TArray<FIntEdge>& edges)
{
    TMultiMap<FIntPoint, FIntPoint> adjacency;
    for( const FIntEdge& edge : edges )
    {
        adjacency.Add( edge.Start, edge.End );
        adjacency.Add( edge.End, edge.Start ); // Start -> End == End -> Start for our search
    }

    TSet<FIntEdge> used;
    TArray<TArray<FVector2D>> contours;

    for( const FIntEdge& edge : edges )
    {
        if( used.Contains( edge ) )
            continue;

        // Start a new contour
        TArray<FVector2D> contour;
        FIntPoint current = edge.Start;
        FIntPoint next = edge.End;

        contour.Add( FVector2D( current ) );
        used.Add( edge );

        while( true )
        {
            contour.Add( FVector2D( next ) );

            // Find next unused edge from next
            bool bFound = false;
            TArray<FIntPoint> connected;
            adjacency.MultiFind( next, connected );

            for( const FIntPoint& connectPoint : connected )
            {
                FIntEdge connectedEged{ next, connectPoint };
                FIntEdge connectedEdgeReverse{ connectPoint, next };

                if( !used.Contains( connectedEged ) && !used.Contains( connectedEdgeReverse ) )
                {
                    used.Add( connectedEged );
                    current = next;
                    next = connectPoint;
                    bFound = true;
                    break;
                }
            }

            if (!bFound || next == edge.Start) // Closed contour
                break;
        }

        if ( contour.Num() > 2 ) // We ignore single lines that can sometimes "float" in the selection if it crosses over itself a lot
            contours.Add( contour );
    }

    return contours;
}

void UOdysseyPainterEditorRasterSelection::OnBlockCommited(const TArray<::ULIS::FRectI>& iRects)
{
    RefreshHUD();
    mOnChanged.Broadcast();
}

::ULIS::FRectI
UOdysseyPainterEditorRasterSelection::ComputeBoundingRect(const TArray<FVector2D>& iPoints) const
{
    if (iPoints.IsEmpty())
        return ::ULIS::FRectI::FromXYWH(0, 0, 0, 0);

    int minX = FMath::FloorToFloat(iPoints[0].X) + 0.5f;
    int maxX = FMath::CeilToFloat(iPoints[0].X) + 0.5f;
    int minY = FMath::FloorToFloat(iPoints[0].Y) + 0.5f;
    int maxY = FMath::CeilToFloat(iPoints[0].Y) + 0.5f;

    for (int j = 1; j < iPoints.Num(); j++)
    {
        minX = FMath::Min(minX, FMath::FloorToFloat(iPoints[j].X) + 0.5f);
        maxX = FMath::Max(maxX, FMath::CeilToFloat(iPoints[j].X) + 0.5f);
        minY = FMath::Min(minY, FMath::FloorToFloat(iPoints[j].Y) + 0.5f);
        maxY = FMath::Max(maxY, FMath::CeilToFloat(iPoints[j].Y) + 0.5f);
    }

    return ::ULIS::FRectI::FromMinMax(minX, minY, maxX, maxY);
}
