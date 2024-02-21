// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/RasterTransformTool/OdysseyPainterEditorRasterTransformTool.h"
#include "Tools/RasterTransformTool/Selection/OdysseyPainterEditorRasterRectangleSelection.h"
#include "Tools/RasterTransformTool/Selection/OdysseyPainterEditorRasterEllipseSelection.h"
#include "Tools/RasterTransformTool/Selection/OdysseyPainterEditorRasterFreehandSelection.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyMediaRaster.h"
#include "OdysseyHUDSystem.h"
#include "OdysseyHUDPolygon.h"
#include "OdysseyHUDHandle.h"
#include "OdysseyBrushTransform.h"
#include "GeomTools.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorRasterTransformTool::~UOdysseyPainterEditorRasterTransformTool()
{
    if (mTransformedBlock)
    {
        mTransformedBlock.Reset();
        mTransformedBlock = nullptr;
    }
    mHandles.Empty();
    mHUD->EmptyHUDElements();
}

UOdysseyPainterEditorRasterTransformTool::UOdysseyPainterEditorRasterTransformTool() :
    SelectionShape(EOdysseySelectionShape::Freehand),
    mSelection(NewObject<UOdysseyPainterEditorRasterFreehandSelection>()),
    mPaintEngine(),
    mTransformedBlock(nullptr),
    mRasterMutator(true),
    mTransformCaptureMode(EOdysseyTransformCapture::NoCapture),
    mTransformArea(nullptr)
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Transform32");
    mSelection->AddToRoot();
}

bool
UOdysseyPainterEditorRasterTransformTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaRaster>();
}

bool UOdysseyPainterEditorRasterTransformTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    FOdysseyPoint point = iPointInTexture;
    point.x = FMath::RoundToInt( point.x );
    point.y = FMath::RoundToInt( point.y );

    if (mHUD->OnKeyDown(point, iKey)) //Handling HUD events if needed
        return true;

    if (mSelection->OnMouseDown(iPointInTexture, iKey))
        return true;

    if( mTransformArea )
    {
        mTransformCaptureMode = DetectCaptureMode( FVector2D( iPointInTexture.x, iPointInTexture.y ));
        mMouseLastReferencePoint = FVector2D(point.x, point.y);
        if( mTransformCaptureMode != EOdysseyTransformCapture::NoCapture )
            return true;
    }
    else if( mSelection->IsInSelectionArea( FVector2D( iPointInTexture.x, iPointInTexture.y )))
    {
        mTransformCaptureMode = EOdysseyTransformCapture::Inside;
        mMouseLastReferencePoint = FVector2D(point.x, point.y);
        CreateTransformAreaFromSelection();
        CreateTransformBlockFromSelectionBlock();
        return true;
    }

    return false;
}

void UOdysseyPainterEditorRasterTransformTool::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    if( mSelection && !mTransformArea )
    {
        if( mSelection->IsInSelectionArea(FVector2D(iPointInTexture.x, iPointInTexture.y)))
            mMouseCursor = EMouseCursor::GrabHand;
        else 
            mMouseCursor = EMouseCursor::Crosshairs;
    }
    else
    {
        switch (DetectCaptureMode(FVector2D(iPointInTexture.x, iPointInTexture.y)))
        {
        case EOdysseyTransformCapture::Inside:
            mMouseCursor = EMouseCursor::GrabHand;
            break;
        default:
            mMouseCursor = EMouseCursor::Crosshairs;
        }
    }

    mHUD->MouseMove(iPointInTexture);
    mSelection->OnMouseHover( iPointInTexture );
}

void UOdysseyPainterEditorRasterTransformTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    FOdysseyPoint point = iPointInTexture;
    point.x = FMath::RoundToInt(point.x);
    point.y = FMath::RoundToInt(point.y);

    mHUD->CapturedMouseMove(point);

    mSelection->OnMouseDrag(iPointInTexture);

    if (mTransformCaptureMode == EOdysseyTransformCapture::Inside)
    {
        for (int i = 0; i < mHandles.Num(); i++)
        {
            mHandles[i]->SetPosition( mHandles[i]->GetPosition() - (mMouseLastReferencePoint - FVector2D(point.x, point.y)));
        }
        mMouseLastReferencePoint = FVector2D(point.x, point.y);
        //BlendTransformAreaToPaintBlock();
        return;
    }
    else if (mTransformCaptureMode == EOdysseyTransformCapture::Rotation)
    {
        ::ULIS::FRectI boundingBox = GetTransformAreaBoundingRect();
        int rotationDelta = GetRotationAngleFromLastReference(FVector2D(point.x, point.y)) - mLastReferenceRotation;
        mRotation+= rotationDelta;
        mLastReferenceRotation += rotationDelta;
        TArray<FVector2D>& points = mTransformArea->GetPoints();
        double cosAngle = FMath::Cos(::FMath::DegreesToRadians(-rotationDelta));
        double sinAngle = FMath::Sin(::FMath::DegreesToRadians(-rotationDelta));
        for (int i = 0; i < points.Num(); i++)
        {
            points[i] -= mPivot;
            FVector2D newPt = FVector2D( points[i].X * cosAngle - points[i].Y * sinAngle, points[i].X * sinAngle + points[i].Y * cosAngle );
            points[i] = mPivot + newPt;
        }
    }

    if( !Perspective )
        ConstrainToRectangle(FVector2D(point.x, point.y));

    CreateTransformBlockFromSelectionBlock();
    //BlendTransformAreaToPaintBlock();
}

bool UOdysseyPainterEditorRasterTransformTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    FOdysseyPoint point = iPointInTexture;
    point.x = FMath::RoundToInt(point.x);
    point.y = FMath::RoundToInt(point.y);

    mTransformCaptureMode = EOdysseyTransformCapture::NoCapture;

    if( mHUD->OnKeyUp(point, iKey) )
        return true;

    if( mSelection->OnMouseUp( iPointInTexture, iKey ))
        return true;

    return false;
}

bool UOdysseyPainterEditorRasterTransformTool::OnKeyDown(const FKey& iKey)
{
    mSelection->OnKeyDown( iKey );

    if (iKey == EKeys::LeftShift || iKey == EKeys::RightShift)
    {
        Uniform = !Uniform;
        return true;
    }
    return false;
}

bool UOdysseyPainterEditorRasterTransformTool::OnKeyUp(const FKey& iKey)
{
    mSelection->OnKeyUp(iKey);

    if (iKey == EKeys::LeftShift || iKey == EKeys::RightShift)
    {
        Uniform = !Uniform;
        return true;
    }
    else if( iKey == EKeys::Enter || iKey == EKeys::SpaceBar )
    {
        CommitTransform();
        return true;
    }
    else if( iKey == EKeys::Escape )
    {
        ClearTransform();
        return true;
    }

    return false;
}

void UOdysseyPainterEditorRasterTransformTool::Tick(float iDeltaTime)
{
    //Only blend on the tick to synchronize with the FPS of the Editor
    BlendTransformAreaToPaintBlock();
    mRotation = (mRotation + 1) % 360; 
}

void UOdysseyPainterEditorRasterTransformTool::Load()
{
    mSelection->Init(mHUD, mEditor, Uniform);
    mSelection->Load();
    UOdysseyPainterEditorTool::Load();
}

void UOdysseyPainterEditorRasterTransformTool::Unload()
{
    mSelection->Unload();
    ClearTransform();
    UOdysseyPainterEditorTool::Unload();
}

void UOdysseyPainterEditorRasterTransformTool::PostEditChangeProperty(FPropertyChangedEvent& iPropertyChangedEvent)
{
    Super::PostEditChangeProperty(iPropertyChangedEvent);

    if (iPropertyChangedEvent.GetPropertyName() == "SelectionShape")
    {
        ClearTransform();
        mSelection->RemoveFromRoot();

        switch( SelectionShape )
        {
            case EOdysseySelectionShape::Rectangle:
                mSelection = NewObject<UOdysseyPainterEditorRasterRectangleSelection>();
                mSelection->AddToRoot();
                mSelection->Init(mHUD, mEditor, Uniform);
                break;
            case EOdysseySelectionShape::Freehand:
                mSelection = NewObject<UOdysseyPainterEditorRasterFreehandSelection>();
                mSelection->AddToRoot();
                mSelection->Init(mHUD, mEditor, Uniform);
                break;
            case EOdysseySelectionShape::Ellipse:
                mSelection = NewObject<UOdysseyPainterEditorRasterEllipseSelection>();
                mSelection->AddToRoot();
                mSelection->Init(mHUD, mEditor, Uniform);
                break;
            default:
                break;
        }

    }
}

EMouseCursor::Type UOdysseyPainterEditorRasterTransformTool::GetMouseCursor()
{
    if( mSelection && !mSelection->IsSelectionAreaSet() )
        return mSelection->GetMouseCursor();
    
    return mMouseCursor;
}

int
UOdysseyPainterEditorRasterTransformTool::GetRotationAngleFromLastReference( FVector2D iPointInTexture )
{
    FVector2D point = iPointInTexture;
    point.X = FMath::RoundToInt(point.X);
    point.Y = FMath::RoundToInt(point.Y);

    if( !mTransformArea )
        return 0;

    FVector2D refPoint = mPivot - point;

    return FMath::RadiansToDegrees(FMath::Atan2(refPoint.X, refPoint.Y));
}

void UOdysseyPainterEditorRasterTransformTool::CreateTransformAreaFromSelection()
{
    if( !mSelection->IsSelectionAreaSet() )
        return;

    TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaRaster>();
    if (mediaRasters.Num() <= 0)
        return;

    if (mediaRasters[0]->IsLocked())
        return;

    ::ULIS::FRectI boundingBox = mSelection->GetSelectionAreaBoundingRect();

    //Creating the HUD
    TArray<FVector2D> areaPoints;
    areaPoints.Add( FVector2D( boundingBox.x, boundingBox.y ) );
    areaPoints.Add( FVector2D( boundingBox.x + boundingBox.w, boundingBox.y ) );
    areaPoints.Add( FVector2D( boundingBox.x + boundingBox.w, boundingBox.y + boundingBox.h ) );
    areaPoints.Add( FVector2D( boundingBox.x, boundingBox.y + boundingBox.h ) );
    mPivot = FVector2D(boundingBox.x + boundingBox.w / 2, boundingBox.y + boundingBox.h / 2);

    mTransformArea = new FOdysseyHUDPolygon(FName("TransformArea"), areaPoints);
    mHUD->EmptyHUDElements(); //Deleting the HUD of the selection to create the one for the transform
    mEditor->HUDSystem()->ClearHUDSurface();
    mHUD->AddElement(mTransformArea);

    FOdysseyHUDHandle* handleTopLeft = new FOdysseyHUDHandle(FName("handleTopLeft"), mTransformArea, &(mTransformArea->GetPoints()[0]));
    FOdysseyHUDHandle* handleTopRight = new FOdysseyHUDHandle(FName("handleTopRight"), mTransformArea, &(mTransformArea->GetPoints()[1]));
    FOdysseyHUDHandle* handleBottomRight = new FOdysseyHUDHandle(FName("handleBottomRight"), mTransformArea, &(mTransformArea->GetPoints()[2]));
    FOdysseyHUDHandle* handleBottomLeft = new FOdysseyHUDHandle(FName("handleBottomLeft"), mTransformArea, &(mTransformArea->GetPoints()[3]));
    FOdysseyHUDHandle* pivot = new FOdysseyHUDHandle(FName("pivot"), mTransformArea, &mPivot);

    mTransformArea->AddElement(handleTopLeft);
    mTransformArea->AddElement(handleTopRight);
    mTransformArea->AddElement(handleBottomRight);
    mTransformArea->AddElement(handleBottomLeft);
    mTransformArea->AddElement(pivot);

    mHandles.Add(handleTopLeft);
    mHandles.Add(handleTopRight);
    mHandles.Add(handleBottomRight);
    mHandles.Add(handleBottomLeft);
    mHandles.Add(pivot);

    //Creating the data
    TSharedPtr<FOdysseyRasterBlock> rasterBlock = mediaRasters[0]->GetRasterBlock();
    mPaintEngine.RasterBlock(rasterBlock);
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> paintBlock = mPaintEngine.PaintBlock();

    ::ULIS::eFormat format = rasterBlock->GetFormat();

     int decalX = FMath::Min(boundingBox.x, 0);
     int decalY = FMath::Min(boundingBox.y, 0);
     
     mRasterMutator.SetRasterBlock(rasterBlock);
     mRasterMutator.EditTilesFromRects(
        { boundingBox },
        FOdysseyRasterBlockMutator::FEditDelegate::CreateLambda(
            [&, referenceBlock = mSelection->GetSelectionBlock()](TSharedPtr<::ULIS::FBlock> iBlock, const FULISInvalidTileMap& iTileMap) -> TArray<::ULIS::FEvent>
            {
                ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
                ::ULIS::FEvent blendEvent;

                ctx.Blend(
                    *referenceBlock,
                    *iBlock,
                    referenceBlock->Rect(),
                    ::ULIS::FVec2I(boundingBox.x, boundingBox.y),
                    ::ULIS::Blend_Normal,
                    ::ULIS::Alpha_Sub,
                    1.f,
                    ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
                    0,
                    nullptr,
                    nullptr
                );

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


EOdysseyTransformCapture UOdysseyPainterEditorRasterTransformTool::DetectCaptureMode( FVector2D iPoint )
{
    if (FGeomTools2D::IsPointInPolygon( iPoint, mTransformArea->GetPoints()))
    {
        return EOdysseyTransformCapture::Inside;
    }
    else
    {  
        mLastReferenceRotation = GetRotationAngleFromLastReference( iPoint );
        return EOdysseyTransformCapture::Rotation;
    }

    return EOdysseyTransformCapture::NoCapture;
}

void UOdysseyPainterEditorRasterTransformTool::ConstrainToRectangle(FVector2D iPosition)
{
    if( !mTransformArea )
        return;

    if (Uniform)
    {
        int opposite = 0;
        for (int i = 0; i < 4; i++)
        {
            if (mHandles[i]->IsCaptured())
            {
                opposite = (i + 2) % 4;
            }
        }
        int shiftX = iPosition.X - mHandles[opposite]->GetPosition().X;
        int shiftY = iPosition.Y - mHandles[opposite]->GetPosition().Y;

        int signX = shiftX < 0 ? -1 : 1;
        int signY = shiftY < 0 ? -1 : 1;

        int mult = signX == signY ? 1 : -1;

        if (FMath::Abs(shiftX) > FMath::Abs(shiftY))
        {
            iPosition.X = mHandles[opposite]->GetPosition().X + shiftX;
            iPosition.Y = mHandles[opposite]->GetPosition().Y + shiftX * mult;
        }
        else
        {
            iPosition.X = mHandles[opposite]->GetPosition().X + shiftY * mult;
            iPosition.Y = mHandles[opposite]->GetPosition().Y + shiftY;
        }
    }

    int next;
    int opposite;
    int previous;
    for( int i = 0; i < 4; i++ )
    {
        if( mHandles[i]->IsCaptured() )
        {
            next = (i + 1) % 4;
            opposite = (i + 2) % 4;
            previous = (i + 3) % 4;

            if( i % 2 == 0 )
            {
                mHandles[i]->SetPosition(iPosition);
                mHandles[next]->SetPosition(FVector2D(mHandles[opposite]->GetPosition().X, iPosition.Y));
                mHandles[previous]->SetPosition(FVector2D(iPosition.X, mHandles[opposite]->GetPosition().Y));

            }
            else
            {
                mHandles[i]->SetPosition(iPosition);
                mHandles[next]->SetPosition(FVector2D(iPosition.X, mHandles[opposite]->GetPosition().Y));
                mHandles[previous]->SetPosition(FVector2D(mHandles[opposite]->GetPosition().X, iPosition.Y));
            }

            mHandles[4]->SetPosition( ( mTransformArea->GetPoints()[0] + mTransformArea->GetPoints()[2] ) / 2 );
        }
    }
}

void UOdysseyPainterEditorRasterTransformTool::ConstrainToParallelogram(FVector2D iPosition)
{

}

void UOdysseyPainterEditorRasterTransformTool::CreateTransformBlockFromSelectionBlock()
{
    if( !mSelection->GetSelectionBlock() || !mTransformArea )
        return;

    ::ULIS::eFormat format = mSelection->GetSelectionBlock()->Format();
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

    TArray<FVector2D> basePoints;
    basePoints.Add(FVector2D(0, 0));
    basePoints.Add(FVector2D(mSelection->GetSelectionBlock()->Width(), 0));
    basePoints.Add(FVector2D(mSelection->GetSelectionBlock()->Width(), mSelection->GetSelectionBlock()->Height()));
    basePoints.Add(FVector2D(0, mSelection->GetSelectionBlock()->Height()));

    TArray<FVector2D> shiftedVertices = mTransformArea->GetPoints();
    for( int i = 0; i < shiftedVertices.Num(); i++ )
    {
        shiftedVertices[i] -= FVector2D( boundingBox.x, boundingBox.y );
    }

    if( !IsPolygonConvex(shiftedVertices) )
        return;

    FOdysseyMatrix transformation = UOdysseyTransformProxyLibrary::MakePerspectiveMatrix(
        basePoints[0],
        basePoints[1],
        basePoints[2],
        basePoints[3],
        shiftedVertices[0],
        shiftedVertices[1],
        shiftedVertices[2],
        shiftedVertices[3]
    );
    
    ctx.TransformPerspective(
        *mSelection->GetSelectionBlock()
        , *mTransformedBlock
        , ::ULIS::FRectI::Auto
        , transformation.m
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

void UOdysseyPainterEditorRasterTransformTool::BlendTransformAreaToPaintBlock()
{
    if( mTransformArea )
    {
        TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> paintBlock = mPaintEngine.PaintBlock();

        if( !paintBlock || !mSelection->GetSelectionBlock() || !mTransformedBlock )
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

void UOdysseyPainterEditorRasterTransformTool::CommitTransform()
{
    if( mRasterMutator.GetRasterBlock() != nullptr )
    {
        GEditor->BeginTransaction(LOCTEXT("raster-transform-tool.transaction.transform", "Transform"));
        mRasterMutator.Commit();
        mPaintEngine.Commit(FOdysseyBlendParameters());
        GEditor->EndTransaction();
        mRasterMutator.SetRasterBlock(nullptr);
    }
    ClearTransform();
}

void UOdysseyPainterEditorRasterTransformTool::ClearTransform()
{
    if (mRasterMutator.GetRasterBlock() != nullptr)
    {
        mPaintEngine.Abort();
        mRasterMutator.Abort();
        mRasterMutator.SetRasterBlock(nullptr);
    }
    if (mTransformedBlock)
    {
        mTransformedBlock.Reset();
        mTransformedBlock = nullptr;
    }
    mRotation = 0;
    mLastReferenceRotation = 0;
    mHandles.Empty();
    mHUD->EmptyHUDElements();
    mTransformArea = nullptr;
    mTransformCaptureMode = EOdysseyTransformCapture::NoCapture;
    mEditor->HUDSystem()->ClearHUDSurface();
    mSelection->ClearSelection();
}

void UOdysseyPainterEditorRasterTransformTool::ClearBlock(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock)
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


bool UOdysseyPainterEditorRasterTransformTool::IsPolygonConvex(const TArray<FVector2D>& Points)
{
    const int PointCount = Points.Num();
    float Sign = 0;
    for (int32 PointIndex = 0; PointIndex < PointCount; ++PointIndex)
    {
        const FVector2D& A = Points[PointIndex];
        const FVector2D& B = Points[(PointIndex + 1) % PointCount];
        const FVector2D& C = Points[(PointIndex + 2) % PointCount];
        float Det = (B.X - A.X) * (C.Y - B.Y) - (B.Y - A.Y) * (C.X - B.X);
        float DetSign = FMath::Sign(Det);
        if (DetSign != 0)
        {
            if (Sign == 0)
            {
                Sign = DetSign;
            }
            else if (Sign != DetSign)
            {
                return false;
            }
        }
    }

    return true;
}

#undef LOCTEXT_NAMESPACE
