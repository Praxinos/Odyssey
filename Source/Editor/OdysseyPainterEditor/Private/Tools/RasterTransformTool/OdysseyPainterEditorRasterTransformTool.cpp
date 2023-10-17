// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/RasterTransformTool/OdysseyPainterEditorRasterTransformTool.h"
#include "Tools/RasterTransformTool/Selection/OdysseyPainterEditorRasterRectangleSelection.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyHUDPolygon.h"
#include "GeomTools.h"
#include "OdysseyBrushTransform.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorRasterTransformTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorRasterTransformTool::~UOdysseyPainterEditorRasterTransformTool()
{
    if (mReferenceBlock)
    {
        mReferenceBlock.Reset();
        mReferenceBlock = nullptr;
    }
    if (mTransformedBlock)
    {
        mTransformedBlock.Reset();
        mTransformedBlock = nullptr;
    }
    mHandles.Empty();
    mHUD->EmptyHUDElements();
}

UOdysseyPainterEditorRasterTransformTool::UOdysseyPainterEditorRasterTransformTool() :
    SelectionShape(EOdysseySelectionShape::Rectangle),
    mSelection(NewObject<UOdysseyPainterEditorRasterRectangleSelection>()),
    mPaintEngine(),
    mTransformedBlock(nullptr),
    mReferenceBlock(nullptr),
    mTransformArea(nullptr),
    mTransformAreaSet(false),
    mRasterMutator(true),
    mAreaConstrain(EOdysseyTransformConstrain::Rectangle),
    mTransformCaptureMode(EOdysseyTransformCapture::NoCapture)
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.TransformTool32");
    mSelection->AddToRoot();
}

bool
UOdysseyPainterEditorRasterTransformTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaRaster>();
}

bool UOdysseyPainterEditorRasterTransformTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if( mSelection->OnMouseDown( iPointInTexture, iKey ) )
        return true;
/*
    if (!mTransformAreaSet) //Creating a zone for the transform
    {
        if( SelectionShape == EOdysseySelectionShape::Rectangle )
        {
            TArray<FVector2D> areaPoints;
            for (int i = 0; i < 4; i++)
            {
                areaPoints.Add(FVector2D(iPointInTexture.x, iPointInTexture.y));
            }

            mTransformArea = new FOdysseyHUDPolygon(FName("TransformArea"), areaPoints);
            mHUD->AddElement(mTransformArea);

            FOdysseyHUDHandle* handleTopLeft = new FOdysseyHUDHandle(FName("handleTopLeft"), mTransformArea, &(mTransformArea->GetPoints()[0]));
            FOdysseyHUDHandle* handleTopRight = new FOdysseyHUDHandle(FName("handleTopRight"), mTransformArea, &(mTransformArea->GetPoints()[1]));
            FOdysseyHUDHandle* handleBottomLeft = new FOdysseyHUDHandle(FName("handleBottomLeft"), mTransformArea, &(mTransformArea->GetPoints()[2]));
            FOdysseyHUDHandle* handleBottomRight = new FOdysseyHUDHandle(FName("handleBottomRight"), mTransformArea, &(mTransformArea->GetPoints()[3]));

            mTransformArea->AddElement(handleTopLeft);
            mTransformArea->AddElement(handleTopRight);
            mTransformArea->AddElement(handleBottomLeft);
            mTransformArea->AddElement(handleBottomRight);

            mHandles.Add(handleTopLeft);
            mHandles.Add(handleTopRight);
            mHandles.Add(handleBottomLeft);
            mHandles.Add(handleBottomRight);
        }
        else if( SelectionShape == EOdysseySelectionShape::FreeHand )
        {
            TArray<FVector2D> areaPoints;
            areaPoints.Add(FVector2D(iPointInTexture.x, iPointInTexture.y));
            mTransformArea = new FOdysseyHUDPolygon(FName("TransformArea"), areaPoints);
            mHUD->AddElement(mTransformArea);
            mAreaConstrain = EOdysseyTransformConstrain::NoConstrain;
            return true;
        }

    }*/
    
    if( mHUD->OnKeyDown(iPointInTexture, iKey) ) //Handling HUD events if needed
    {
        return true;
    }
    /*
    else if( FGeomTools2D::IsPointInPolygon( FVector2D( iPointInTexture.x, iPointInTexture.y ), mTransformArea->GetPoints() ) ) //Handling clicking inside the transform zone (for dragging it)
    {
        mTransformCaptureMode = EOdysseyTransformCapture::Inside;
        mMouseLastReferencePoint = FVector2D( iPointInTexture.x, iPointInTexture.y );
        return true;
    }*/


    return false;
}

void UOdysseyPainterEditorRasterTransformTool::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    //If is in polygon, change mouse cursor to hand
    //If is in other area, change mouse cursor accordingly
    mSelection->OnMouseHover( iPointInTexture );
    mHUD->MouseMove( iPointInTexture );
}

void UOdysseyPainterEditorRasterTransformTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    mSelection->OnMouseDrag(iPointInTexture);

    mHUD->CapturedMouseMove(iPointInTexture);

    /*
    if( !mTransformAreaSet )
    {
        if (SelectionShape == EOdysseySelectionShape::FreeHand)
        {
            mTransformArea->GetPoints().Add(FVector2D(iPointInTexture.x, iPointInTexture.y));
        }
    }
    else
    {
        if( mTransformCaptureMode == EOdysseyTransformCapture::Inside )
        {
            TArray<FVector2D>& points = mTransformArea->GetPoints();
            for (int i = 0; i < points.Num(); i++)
            {
                points[i] = points[i] - (mMouseLastReferencePoint - FVector2D(iPointInTexture.x, iPointInTexture.y));
            }
            mMouseLastReferencePoint = FVector2D(iPointInTexture.x, iPointInTexture.y);
            BlendTransformAreaToPaintBlock();
            return;
        }
    }

    switch (mAreaConstrain)
    {
    case EOdysseyTransformConstrain::Rectangle:
        ConstrainToRectangle(FVector2D(iPointInTexture.x, iPointInTexture.y));
        break;
    case EOdysseyTransformConstrain::Parallelogram:
        ConstrainToParallelogram(FVector2D(iPointInTexture.x, iPointInTexture.y));
        break;
    case EOdysseyTransformConstrain::NoConstrain:
        break;
    default:
        break;
    }

    CreateTransformBlockFromReferenceBlock();
    BlendTransformAreaToPaintBlock();*/
}

bool UOdysseyPainterEditorRasterTransformTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if( mSelection->OnMouseUp( iPointInTexture, iKey ))
        return true;


    bool isHandled = mHUD->OnKeyUp( iPointInTexture, iKey );
    /*
    mTransformCaptureMode = EOdysseyTransformCapture::NoCapture;
    if (!mTransformAreaSet)
    {
        TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaRaster>();
        if (mediaRasters.Num() <= 0)
            return isHandled;

        if (mediaRasters[0]->IsLocked())
            return isHandled;

        TSharedPtr<FOdysseyRasterBlock> rasterBlock = mediaRasters[0]->GetRasterBlock();
        mPaintEngine.RasterBlock(rasterBlock);
        TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> paintBlock = mPaintEngine.PaintBlock();

        if( mAreaConstrain == EOdysseyTransformConstrain::Rectangle )
        {
            ::ULIS::eFormat format = rasterBlock->GetFormat();
            ::ULIS::FRectI boundingBox = GetTransformAreaBoundingRect();

            if (!IsSelectionValid(boundingBox))
            {
                AbortTransform();
                return true;
            }

            int decalX = FMath::Min( boundingBox.x, 0 );
            int decalY = FMath::Min( boundingBox.y, 0 );

            mReferenceBlock = MakeShareable(new ::ULIS::FBlock(boundingBox.w, boundingBox.h, format));
            ClearBlock( mReferenceBlock );

            mRasterMutator.SetRasterBlock(rasterBlock);
            mRasterMutator.EditTilesFromRects(
                { boundingBox },
                FOdysseyRasterBlockMutator::FEditDelegate::CreateLambda(
                    [&, referenceBlock = mReferenceBlock](TSharedPtr<::ULIS::FBlock> iBlock, const FULISInvalidTileMap& iTileMap) -> TArray<::ULIS::FEvent>
                    {
                        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
                        ::ULIS::FEvent copyEvent, clearEvent, blendEvent;

                        ctx.Copy(
                            *iBlock,
                            *referenceBlock,
                            boundingBox,
                            ::ULIS::FVec2I(-decalX, -decalY),
                            ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
                            0,
                            nullptr,
                            &copyEvent
                        );

                        ctx.Clear(
                            *iBlock,
                            boundingBox & iBlock->Rect(),
                            ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
                            1,
                            &copyEvent,
                            &clearEvent);

                        ctx.Blend(
                            *referenceBlock,
                            *paintBlock,
                            referenceBlock->Rect(),
                            ::ULIS::FVec2I(boundingBox.x, boundingBox.y),
                            ::ULIS::Blend_Normal,
                            ::ULIS::Alpha_Normal,
                            1.f,
                            ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
                            1,
                            &clearEvent,
                            &blendEvent
                        );

                        return { blendEvent };
                    }
                )
            );

            paintBlock->Dirty();
            mPaintEngine.Update(FOdysseyBlendParameters());
        }
        else if ( mAreaConstrain == EOdysseyTransformConstrain::NoConstrain )
        {
            ::ULIS::eFormat format = rasterBlock->GetFormat();
            ::ULIS::FRectI boundingBox = GetTransformAreaBoundingRect();

            if ( !IsSelectionValid(boundingBox) )
            {
                AbortTransform();
                return true;
            }

            TArray<::ULIS::FRectI> rectangles = GetTransformAreaAsScanlines();

            mReferenceBlock = MakeShareable(new ::ULIS::FBlock(boundingBox.w, boundingBox.h, format));
            ClearBlock(mReferenceBlock);

            mRasterMutator.SetRasterBlock(rasterBlock);
            mRasterMutator.EditTilesFromRects(
                { boundingBox },
                FOdysseyRasterBlockMutator::FEditDelegate::CreateLambda(
                    [&, referenceBlock = mReferenceBlock](TSharedPtr<::ULIS::FBlock> iBlock, const FULISInvalidTileMap& iTileMap) -> TArray<::ULIS::FEvent>
                    {
                        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
                        ::ULIS::FEvent blendEvent;

                        for( int i = 0; i < rectangles.Num(); i++ )
                        {
                            int decalX = FMath::Min(rectangles[i].x, 0);
                            int decalY = FMath::Min(rectangles[i].y, 0);

                            ctx.Copy(
                                *iBlock,
                                *referenceBlock,
                                rectangles[i],
                                ::ULIS::FVec2I(-decalX - boundingBox.x + rectangles[i].x, -decalY - boundingBox.y + rectangles[i].y),
                                ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
                                0,
                                nullptr,
                                nullptr
                            );
                        }

                        ctx.Flush();

                        for (int i = 0; i < rectangles.Num(); i++)
                        {
                            if( (rectangles[i] & iBlock->Rect()).w > 0 ) //TODO: ULIS wtf... Why do I need to do this test to prevent a crash when I already use the intersection in the clear ?...
                            {
                                ctx.Clear(
                                    *iBlock,
                                    rectangles[i] & iBlock->Rect(),
                                    ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
                                    0,
                                    nullptr,
                                    nullptr);
                            }
                        }

                        ctx.Flush();

                        ctx.Blend(
                            *referenceBlock,
                            *paintBlock,
                            referenceBlock->Rect(),
                            ::ULIS::FVec2I(boundingBox.x, boundingBox.y),
                            ::ULIS::Blend_Normal,
                            ::ULIS::Alpha_Normal,
                            1.f,
                            ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
                            0,
                            nullptr,
                            &blendEvent
                        );

                        return { blendEvent };
                    }
                )
            );

            paintBlock->Dirty();
            mPaintEngine.Update(FOdysseyBlendParameters());
        }
        CreateTransformBlockFromReferenceBlock();
        mTransformAreaSet = true;
        ConstrainToRectangle(FVector2D(iPointInTexture.x, iPointInTexture.y));
        return true;
    }*/

    return isHandled;
}

bool UOdysseyPainterEditorRasterTransformTool::OnKeyUp(const FKey& iKey)
{
    if( iKey == EKeys::Enter || iKey == EKeys::SpaceBar )
    {
        CommitTransform();
        return true;
    }
    else if( iKey == EKeys::Escape )
    {
        AbortTransform();
        return true;
    }

    return false;
}

void UOdysseyPainterEditorRasterTransformTool::Load()
{
    mSelection->Init(mHUD, mEditor);
    mSelection->Load();
    UOdysseyPainterEditorTool::Load();
}

void UOdysseyPainterEditorRasterTransformTool::Unload()
{
    mSelection->Unload();
    AbortTransform();
    UOdysseyPainterEditorTool::Unload();
}

void UOdysseyPainterEditorRasterTransformTool::ConstrainToRectangle(FVector2D iPosition)
{
    if( mTransformAreaSet && mTransformArea->GetPoints().Num() != 4 )
    {
        ::ULIS::FRectI boundingBox = GetTransformAreaBoundingRect();
        mEditor->HUDSystem()->ClearHUDSurface();
        mHandles.Empty();
        mHUD->EmptyHUDElements();

        TArray<FVector2D> areaPoints;
        mTransformArea = new FOdysseyHUDPolygon(FName("TransformArea"), areaPoints);
        mHUD->AddElement(mTransformArea);
        mTransformArea->GetPoints().Add( FVector2D( boundingBox.x, boundingBox.y ));
        mTransformArea->GetPoints().Add( FVector2D( boundingBox.x + boundingBox.w, boundingBox.y ));
        mTransformArea->GetPoints().Add( FVector2D( boundingBox.x + boundingBox.w, boundingBox.y + boundingBox.h ));
        mTransformArea->GetPoints().Add( FVector2D( boundingBox.x , boundingBox.y + boundingBox.h));

        FOdysseyHUDHandle* handleTopLeft = new FOdysseyHUDHandle(FName("handleTopLeft"), mTransformArea, &(mTransformArea->GetPoints()[0]));
        FOdysseyHUDHandle* handleTopRight = new FOdysseyHUDHandle(FName("handleTopRight"), mTransformArea, &(mTransformArea->GetPoints()[1]));
        FOdysseyHUDHandle* handleBottomRight = new FOdysseyHUDHandle(FName("handleBottomRight"), mTransformArea, &(mTransformArea->GetPoints()[2]));
        FOdysseyHUDHandle* handleBottomLeft = new FOdysseyHUDHandle(FName("handleBottomLeft"), mTransformArea, &(mTransformArea->GetPoints()[3]));

        mTransformArea->AddElement(handleTopLeft);
        mTransformArea->AddElement(handleTopRight);
        mTransformArea->AddElement(handleBottomRight);
        mTransformArea->AddElement(handleBottomLeft);

        mHandles.Add(handleTopLeft);
        mHandles.Add(handleTopRight);
        mHandles.Add(handleBottomRight);
        mHandles.Add(handleBottomLeft);
        mAreaConstrain = EOdysseyTransformConstrain::Rectangle;
        return;
    }

    int next;
    int opposite;
    int previous;
    for( int i = 0; i < mHandles.Num(); i++ )
    {
        if( mHandles[i]->IsCaptured() )
        {
            next = (i + 1) % mHandles.Num();
            opposite = (i + 2) % mHandles.Num();
            previous = (i + 3) % mHandles.Num();

            mHandles[i]->SetPosition( iPosition );
            mHandles[next]->SetPosition( FVector2D( iPosition.X, mHandles[opposite]->GetPosition().Y ) );
            mHandles[previous]->SetPosition( FVector2D( mHandles[opposite]->GetPosition().X, iPosition.Y ) );
        }
    }
}

void UOdysseyPainterEditorRasterTransformTool::ConstrainToParallelogram(FVector2D iPosition)
{

}

void UOdysseyPainterEditorRasterTransformTool::CreateTransformBlockFromReferenceBlock()
{
    if( !mReferenceBlock )
        return;

    ::ULIS::eFormat format = mReferenceBlock->Format();
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
    ::ULIS::FEvent transformEvent;

    if( mTransformedBlock )
    {
        mTransformedBlock.Reset();
        mTransformedBlock = nullptr;
    }
    
    ::ULIS::FRectI boundingBox = GetTransformAreaBoundingRect();
    mTransformedBlock = MakeShareable(new ::ULIS::FBlock(boundingBox.w, boundingBox.h, format));
    ClearBlock( mTransformedBlock );

    float scaleX = (float)mTransformedBlock->Width() / (float)mReferenceBlock->Width();
    float scaleY = (float)mTransformedBlock->Height() / (float)mReferenceBlock->Height();
    FOdysseyMatrix scale = UOdysseyTransformProxyLibrary::MakeScaleMatrix( scaleX, scaleY );
    
    ctx.TransformAffine(
        *mReferenceBlock
        , *mTransformedBlock
        , mReferenceBlock->Rect()
        , scale.m
        , ::ULIS::eResamplingMethod::Resampling_Bilinear
        , ::ULIS::eBorderMode::Border_Transparent
        , ::ULIS::FColor::Transparent
        , ::ULIS::FSchedulePolicy::AsyncCacheEfficient
        , 0
        , nullptr
        , &transformEvent
    );
    ctx.Finish();
}

::ULIS::FRectI UOdysseyPainterEditorRasterTransformTool::GetTransformAreaBoundingRect()
{
    if( mTransformArea )
    {   
        int minX = mTransformArea->GetPoints()[0].X;
        int maxX = mTransformArea->GetPoints()[0].X;
        int minY = mTransformArea->GetPoints()[0].Y;
        int maxY = mTransformArea->GetPoints()[0].Y;
        for( int i = 1; i < mTransformArea->GetPoints().Num(); i++ )
        {
            minX = FMath::Min( minX, mTransformArea->GetPoints()[i].X );
            maxX = FMath::Max( maxX, mTransformArea->GetPoints()[i].X );
            minY = FMath::Min( minY, mTransformArea->GetPoints()[i].Y );
            maxY = FMath::Max( maxY, mTransformArea->GetPoints()[i].Y );
        }
        return ::ULIS::FRectI::FromMinMax(minX, minY, maxX, maxY);
    }
    return ::ULIS::FRectI::FromXYWH( 0,0,0,0 );
}

TArray<::ULIS::FRectI> UOdysseyPainterEditorRasterTransformTool::GetTransformAreaAsScanlines()
{
    TArray<::ULIS::FRectI> rectangles;
    ::ULIS::FRectI boundingBox = GetTransformAreaBoundingRect();
    int maxX = boundingBox.x + boundingBox.w;
    int maxY = boundingBox.y + boundingBox.h;
    int minX = boundingBox.x;
    int minY = boundingBox.y;

    TArray<FVector2D>& points = mTransformArea->GetPoints();

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

                rectangles.Add( ::ULIS::FRectI::FromXYWH(nodesX[i], y, nodesX[i + 1] - nodesX[i], 1) );
            }
        }
    }

    return rectangles;
}

void UOdysseyPainterEditorRasterTransformTool::BlendTransformAreaToPaintBlock()
{
    if( mTransformArea )
    {
        TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> paintBlock = mPaintEngine.PaintBlock();

        if( !paintBlock || !mReferenceBlock || !mTransformedBlock )
            return;

        ::ULIS::eFormat format = paintBlock->Format();
        ::ULIS::FRectI boundingBox = GetTransformAreaBoundingRect();

        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
        ::ULIS::FEvent clearEvent, blendEvent;

        ctx.Clear(
            *paintBlock,
            paintBlock->Rect(),
            ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
            0,
            nullptr,
            &clearEvent);

        ctx.Blend(
            *mTransformedBlock,
            *paintBlock,
            mTransformedBlock->Rect(),
            ::ULIS::FVec2I(boundingBox.x, boundingBox.y),
            ::ULIS::Blend_Normal,
            ::ULIS::Alpha_Normal,
            1.f,
            ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
            1,
            &clearEvent,
            &blendEvent
        );

        ctx.Finish();

        paintBlock->Dirty();
        mPaintEngine.Update( FOdysseyBlendParameters() );
    }
}

void UOdysseyPainterEditorRasterTransformTool::ClearBlock(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock)
{
    if( !iBlock )
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

bool UOdysseyPainterEditorRasterTransformTool::IsSelectionValid(::ULIS::FRectI iSelectionArea)
{
    if( iSelectionArea.w > 8192 || iSelectionArea.h > 8192 ) //Unreal limitations + very slow in ULIS at these sizes
        return false;

    return true;
}

void UOdysseyPainterEditorRasterTransformTool::CommitTransform()
{
    if( mRasterMutator.GetRasterBlock() != nullptr )
    {
        GEditor->BeginTransaction(LOCTEXT("CommitTransform", "Transform"));
        mRasterMutator.Commit();
        mPaintEngine.Commit(FOdysseyBlendParameters());
        GEditor->EndTransaction();
        mRasterMutator.SetRasterBlock(nullptr);
    }
    ClearTransform();
}

void UOdysseyPainterEditorRasterTransformTool::AbortTransform()
{
    if (mRasterMutator.GetRasterBlock() != nullptr)
    {
        mPaintEngine.Abort();
        mRasterMutator.Abort();
        mRasterMutator.SetRasterBlock(nullptr);
    }
    ClearTransform();
}

void UOdysseyPainterEditorRasterTransformTool::ClearTransform()
{
    if (mReferenceBlock)
    {
        mReferenceBlock.Reset();
        mReferenceBlock = nullptr;
    }
    if (mTransformedBlock)
    {
        mTransformedBlock.Reset();
        mTransformedBlock = nullptr;
    }
    mHandles.Empty();
    mHUD->EmptyHUDElements();
    mTransformAreaSet = false;
    mTransformArea = nullptr;
    mAreaConstrain = EOdysseyTransformConstrain::Rectangle;
    mTransformCaptureMode = EOdysseyTransformCapture::NoCapture;
    mEditor->HUDSystem()->ClearHUDSurface();
}

#undef LOCTEXT_NAMESPACE
