// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/RasterTransformTool/OdysseyPainterEditorRasterTransformTool.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyMediaRaster.h"
#include "OdysseyHUDSystem.h"
#include "OdysseyHUDPolygon.h"
#include "OdysseyHUDHandle.h"
#include "OdysseyBrushTransform.h"
#include "GeomTools.h"
#include "PainterEditor/OdysseyPainterEditorSource.h"
#include "PainterEditor/OdysseyPainterEditorRasterSelection.h"
#include "ULISEventBuilder.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorRasterTransformTool::~UOdysseyPainterEditorRasterTransformTool()
{
    if (mTransformSelectionBlock)
    {
        mTransformSelectionBlock.Reset();
        mTransformSelectionBlock = nullptr;
    }

    if (mSelectionBlock)
    {
        mSelectionBlock.Reset();
        mSelectionBlock = nullptr;
    }

    mHandles.Empty();
    mTransformHUD->EmptyElements();
}

UOdysseyPainterEditorRasterTransformTool::UOdysseyPainterEditorRasterTransformTool()
    : mPaintEngine()
    , mRasterMutator(true)
    , mTransformCaptureMode(EOdysseyTransformCapture::NoCapture)
    , mTransformHUD(MakeShared<FOdysseyHUDElement>())
    , mTransformArea(nullptr)
    , mMouseCursor(EMouseCursor::Crosshairs)
    , mSelectionBlock(nullptr)
    , mTransformSelectionBlock(nullptr)
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Transform32");
}

bool
UOdysseyPainterEditorRasterTransformTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaRaster>();
}

bool UOdysseyPainterEditorRasterTransformTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if (UOdysseyPainterEditorRasterBaseTool::OnMouseDown(iPointInTexture, iKey))
        return true;

    if( !mTransformArea )
        return false;

    FOdysseyPoint point = iPointInTexture;
    point.x = FMath::RoundToInt( point.x );
    point.y = FMath::RoundToInt( point.y );

    mTransformCaptureMode = DetectCaptureMode( FVector2D( iPointInTexture.x, iPointInTexture.y ));
    mMouseLastReferencePoint = FVector2D(point.x, point.y);
    if( mTransformCaptureMode != EOdysseyTransformCapture::NoCapture )
        return true;

    return false;
}

void UOdysseyPainterEditorRasterTransformTool::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    if (!mTransformArea)
        return;

    switch (DetectCaptureMode(FVector2D(iPointInTexture.x, iPointInTexture.y)))
    {
    case EOdysseyTransformCapture::Inside:
        mMouseCursor = EMouseCursor::GrabHand;
        break;
    default:
        mMouseCursor = EMouseCursor::Crosshairs;
    }
}

void UOdysseyPainterEditorRasterTransformTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    if (!mTransformArea)
        return;

    FOdysseyPoint point = iPointInTexture;
    point.x = FMath::RoundToInt(point.x);
    point.y = FMath::RoundToInt(point.y);

    if (mTransformCaptureMode == EOdysseyTransformCapture::Inside)
    {
        for (int i = 0; i < mHandles.Num(); i++)
        {
            mHandles[i]->SetPosition(mHandles[i]->GetPosition() - (mMouseLastReferencePoint - FVector2D(point.x, point.y)));
        }
        TArray<FVector2D>& points = mTransformArea->GetPoints();
        for (int i = 0; i < points.Num(); i++)
        {
            points[i] = mHandles[i]->GetPosition();
        }

        mMouseLastReferencePoint = FVector2D(point.x, point.y);
    }
    else if (mTransformCaptureMode == EOdysseyTransformCapture::Rotation)
    {
        ::ULIS::FRectI boundingBox = GetTransformAreaBoundingRect();
        int rotationDelta = GetRotationAngleFromLastReference(FVector2D(point.x, point.y)) - mLastReferenceRotation;
        mLastReferenceRotation += rotationDelta;
        TArray<FVector2D>& points = mTransformArea->GetPoints();
        double cosAngle = FMath::Cos(::FMath::DegreesToRadians(-rotationDelta));
        double sinAngle = FMath::Sin(::FMath::DegreesToRadians(-rotationDelta));
        for (int i = 0; i < points.Num(); i++)
        {
            points[i] -= mHandles[4]->GetPosition();
            FVector2D newPt = FVector2D( points[i].X * cosAngle - points[i].Y * sinAngle, points[i].X * sinAngle + points[i].Y * cosAngle );
            points[i] = mHandles[4]->GetPosition() + newPt;
            mHandles[i]->SetPosition( points[i] );
        }
    }

    if( !Perspective )
        ConstrainToRectangle(FVector2D(point.x, point.y));

    CreateTransformBlockFromSelectionBlock();
}

bool UOdysseyPainterEditorRasterTransformTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if (UOdysseyPainterEditorRasterBaseTool::OnMouseUp(iPointInTexture, iKey))
        return true;

    if (!mTransformArea)
        return false;

    FOdysseyPoint point = iPointInTexture;
    point.x = FMath::RoundToInt(point.x);
    point.y = FMath::RoundToInt(point.y);

    mTransformCaptureMode = EOdysseyTransformCapture::NoCapture;

    return true;
}

bool UOdysseyPainterEditorRasterTransformTool::OnKeyDown(const FKey& iKey)
{
    if (iKey == EKeys::LeftShift || iKey == EKeys::RightShift)
    {
        Uniform = !Uniform;
        return true;
    }

    return false;
}

bool UOdysseyPainterEditorRasterTransformTool::OnKeyUp(const FKey& iKey)
{
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
        CreateTransformAreaFromSelection();
        return true;
    }

    return false;
}

void UOdysseyPainterEditorRasterTransformTool::Tick(float iDeltaTime)
{
    //Only blend on the tick to synchronize with the FPS of the Editor
    BlendTransformAreaToPaintBlock();
}

void UOdysseyPainterEditorRasterTransformTool::Load()
{
    TSharedPtr<FOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();
    rasterSelection->OnChanged().AddUObject(this, &UOdysseyPainterEditorRasterTransformTool::OnRasterSelectionChanged);
    mHUD->AddElement(rasterSelection->GetHUD());
    mHUD->AddElement(mTransformHUD);

    UpdateRasterSelection();
    
    CreateTransformAreaFromSelection();
    UOdysseyPainterEditorTool::Load();
}

void UOdysseyPainterEditorRasterTransformTool::Unload()
{
    TSharedPtr<FOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();
    rasterSelection->OnChanged().RemoveAll(this);
    mHUD->RemoveElement(rasterSelection->GetHUD());
    mHUD->RemoveElement(mTransformHUD);

    ClearTransform();
    
    mSelectionBlock = nullptr;
    UOdysseyPainterEditorTool::Unload();
}

EMouseCursor::Type UOdysseyPainterEditorRasterTransformTool::GetMouseCursor() const
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return EMouseCursor::SlashedCircle;

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

    FVector2D refPoint = mHandles[4]->GetPosition() - point;

    return FMath::RadiansToDegrees(FMath::Atan2(refPoint.X, refPoint.Y));
}

void UOdysseyPainterEditorRasterTransformTool::CreateTransformAreaFromSelection()
{
    if( !mSelectionBlock )
        return;

    FOdysseyMediaProvider mediaProvider = mEditor->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return;

    TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters = mediaProvider.GetOrCreateMedias<FOdysseyMediaRaster>();
    if (mediaRasters.Num() <= 0)
        return;

    TSharedPtr<FOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();
    ::ULIS::FRectI boundingBox = rasterSelection->GetMaskBoundingRect();

    if (boundingBox.Area() <= 0)
        boundingBox = ::ULIS::FRectI::FromMinMax(0, 0, mediaRasters[0]->GetRasterBlock()->GetWidth(), mediaRasters[0]->GetRasterBlock()->GetHeight());

    mTransformArea = MakeShared<FOdysseyHUDPolygon>();
    TArray<FVector2D>& areaPoints = mTransformArea->GetPoints();
    areaPoints.Add( FVector2D( boundingBox.x, boundingBox.y ) );
    areaPoints.Add( FVector2D( boundingBox.x + boundingBox.w, boundingBox.y ) );
    areaPoints.Add( FVector2D( boundingBox.x + boundingBox.w, boundingBox.y + boundingBox.h ) );
    areaPoints.Add( FVector2D( boundingBox.x, boundingBox.y + boundingBox.h ) );

    mTransformHUD->EmptyElements(); //Deleting the HUD of the selection to create the one for the transform
    mTransformHUD->AddElement(mTransformArea);

    TSharedPtr<FOdysseyHUDHandle> handleTopLeft = MakeShared<FOdysseyHUDHandle>(areaPoints[0]);
    TSharedPtr<FOdysseyHUDHandle> handleTopRight = MakeShared<FOdysseyHUDHandle>(areaPoints[1]);
    TSharedPtr<FOdysseyHUDHandle> handleBottomRight = MakeShared<FOdysseyHUDHandle>(areaPoints[2]);
    TSharedPtr<FOdysseyHUDHandle> handleBottomLeft = MakeShared<FOdysseyHUDHandle>(areaPoints[3]);
    TSharedPtr<FOdysseyHUDHandle> pivot = MakeShared<FOdysseyHUDHandle>(FVector2D(boundingBox.x + boundingBox.w / 2, boundingBox.y + boundingBox.h / 2));

    handleTopLeft->OnDragged().AddUObject(this, &UOdysseyPainterEditorRasterTransformTool::OnTopLeftHandleDragged);
    handleTopRight->OnDragged().AddUObject(this, &UOdysseyPainterEditorRasterTransformTool::OnTopRightHandleDragged);
    handleBottomRight->OnDragged().AddUObject(this, &UOdysseyPainterEditorRasterTransformTool::OnBottomRightHandleDragged);
    handleBottomLeft->OnDragged().AddUObject(this, &UOdysseyPainterEditorRasterTransformTool::OnBottomLeftHandleDragged);
    pivot->OnDragged().AddUObject(this, &UOdysseyPainterEditorRasterTransformTool::OnPivotHandleDragged);

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

    mRasterMutator.SetRasterBlock(rasterBlock);
    mRasterMutator.EditTilesFromRects(
        { boundingBox },
		[&, referenceBlock = mSelectionBlock](TSharedPtr<::ULIS::FBlock> iBlock, const FULISInvalidTileMap& iTileMap) -> TArray<::ULIS::FEvent>
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

			ctx.Finish();

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

        float ratio = FMath::Abs(float(mSelectionBlock->Height()) / float(mSelectionBlock->Width()));

        int signX = shiftX < 0 ? -1 : 1;
        int signY = shiftY < 0 ? -1 : 1;

        int mult = signX == signY ? 1 : -1;

        if (FMath::Abs(shiftX * ratio) > FMath::Abs(shiftY))
        {
            iPosition.X = mHandles[opposite]->GetPosition().X + shiftX;
            iPosition.Y = mHandles[opposite]->GetPosition().Y + shiftX * mult * ratio;
        }
        else
        {
            iPosition.X = mHandles[opposite]->GetPosition().X + shiftY * mult * (1.0 / ratio);
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
                mTransformArea->GetPoints()[i] = mHandles[i]->GetPosition();
                mHandles[next]->SetPosition(FVector2D(mHandles[opposite]->GetPosition().X, iPosition.Y));
                mTransformArea->GetPoints()[next] = mHandles[next]->GetPosition();
                mHandles[previous]->SetPosition(FVector2D(iPosition.X, mHandles[opposite]->GetPosition().Y));
                mTransformArea->GetPoints()[previous] = FVector2D(iPosition.X, mHandles[opposite]->GetPosition().Y);
            }
            else
            {
                mHandles[i]->SetPosition(iPosition);
                mTransformArea->GetPoints()[i] = mHandles[i]->GetPosition();
                mHandles[next]->SetPosition(FVector2D(iPosition.X, mHandles[opposite]->GetPosition().Y));
                mTransformArea->GetPoints()[next] = mHandles[next]->GetPosition();
                mHandles[previous]->SetPosition(FVector2D(mHandles[opposite]->GetPosition().X, iPosition.Y));
                mTransformArea->GetPoints()[previous] = mHandles[previous]->GetPosition();
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
    if( !mSelectionBlock || !mTransformArea )
        return;

    ::ULIS::eFormat format = mSelectionBlock->Format();
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);

    mTransformSelectionBlock = nullptr;
    
    ::ULIS::FRectI boundingBox = GetTransformAreaBoundingRect();

    TSharedPtr<FOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();
    if (mSelectionBlock)
    {
        mTransformSelectionBlock = MakeShareable(new ::ULIS::FBlock(boundingBox.w, boundingBox.h, mSelectionBlock->Format()));
        ClearBlock( mTransformSelectionBlock );
    }

    TArray<FVector2D> basePoints;
    basePoints.Add(FVector2D(0, 0));
    basePoints.Add(FVector2D(mSelectionBlock->Width(), 0));
    basePoints.Add(FVector2D(mSelectionBlock->Width(), mSelectionBlock->Height()));
    basePoints.Add(FVector2D(0, mSelectionBlock->Height()));

    TArray<FVector2D> vertices = mTransformArea->GetPoints();
    for( int i = 0; i < vertices.Num(); i++ )
    {
        vertices[i] -= FVector2D( boundingBox.x, boundingBox.y );
    }

    if( !IsPolygonConvex(vertices) )
        return;

    FOdysseyMatrix transformation = UOdysseyTransformProxyLibrary::MakePerspectiveMatrix(
        basePoints[0],
        basePoints[1],
        basePoints[2],
        basePoints[3],
        vertices[0],
        vertices[1],
        vertices[2],
        vertices[3]
    );
    
    ctx.TransformPerspective(
        *mSelectionBlock
        , *mTransformSelectionBlock
        , ::ULIS::FRectI::Auto
        , transformation.m
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

        if( !paintBlock || !mSelectionBlock || !mTransformSelectionBlock )
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
            *mTransformSelectionBlock,
            *paintBlock,
            mTransformSelectionBlock->Rect(),
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

        if (mTransformSelectionBlock)
        {
            TSharedPtr<FOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();
            ClearBlock(rasterSelection->GetBlock());

            ::ULIS::FContext& selectionCtx = IULISLoaderModule::StaticFindOrAddContext(mSelectionBlock->Format());
            selectionCtx.Blend(
                *mTransformSelectionBlock,
                *rasterSelection->GetBlock(),
                mTransformSelectionBlock->Rect(),
                ::ULIS::FVec2I(boundingBox.x, boundingBox.y)
            );
            selectionCtx.Finish();

            rasterSelection->RefreshHUD();
        }

        paintBlock->Dirty();
        mPaintEngine.Update( FOdysseyBlendParameters() );
    }
}

void UOdysseyPainterEditorRasterTransformTool::CommitTransform()
{
    if( !mTransformArea )
        return;

    if( mRasterMutator.GetRasterBlock() != nullptr )
    {
        GEditor->BeginTransaction(LOCTEXT("raster-transform-tool.transaction.transform", "Transform"));
        mRasterMutator.Commit();
        mPaintEngine.Commit(FOdysseyBlendParameters());

        FOdysseyPainterEditor* editor = GetEditor();
        TSharedPtr<FOdysseyPainterEditorSource> source = editor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();

        GEditor->EndTransaction();
        mRasterMutator.SetRasterBlock(nullptr);

        TSharedPtr<FOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();
        if (!rasterSelection->IsEmpty())
            rasterSelection->OnChanged().Broadcast();
    }
    ClearTransform();
    CreateTransformAreaFromSelection();
}

void UOdysseyPainterEditorRasterTransformTool::ClearTransform()
{
    if (mRasterMutator.GetRasterBlock() != nullptr)
    {
        mPaintEngine.Abort();
        mRasterMutator.Abort();
        mRasterMutator.SetRasterBlock(nullptr);
    }

    mSelectionBlock = nullptr;
    mTransformSelectionBlock = nullptr;
    mLastReferenceRotation = 0;
    mHandles.Empty();
    mTransformHUD->EmptyElements();
    mTransformArea = nullptr;
    mTransformCaptureMode = EOdysseyTransformCapture::NoCapture;
    ResetRasterSelection();
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

FText
UOdysseyPainterEditorRasterTransformTool::GetTooltip() const
{
    return LOCTEXT("raster-transform-tool.tooltip", "Transform Tool");
}

void
UOdysseyPainterEditorRasterTransformTool::OnTopLeftHandleDragged()
{
    if( !Perspective )
        ConstrainToRectangle( mHandles[0]->GetPosition() );
    else
        mTransformArea->GetPoints()[0] = mHandles[0]->GetPosition();

    CreateTransformBlockFromSelectionBlock();
}

void
UOdysseyPainterEditorRasterTransformTool::OnTopRightHandleDragged()
{
    if( !Perspective )
        ConstrainToRectangle( mHandles[1]->GetPosition() );
    else
        mTransformArea->GetPoints()[1] = mHandles[1]->GetPosition();

    CreateTransformBlockFromSelectionBlock();
}

void
UOdysseyPainterEditorRasterTransformTool::OnBottomRightHandleDragged()
{
    if( !Perspective )
        ConstrainToRectangle( mHandles[2]->GetPosition() );
    else
        mTransformArea->GetPoints()[2] = mHandles[2]->GetPosition();

    CreateTransformBlockFromSelectionBlock();
}

void
UOdysseyPainterEditorRasterTransformTool::OnBottomLeftHandleDragged()
{
    if( !Perspective )
        ConstrainToRectangle( mHandles[3]->GetPosition() );
    else
        mTransformArea->GetPoints()[3] = mHandles[3]->GetPosition();

    CreateTransformBlockFromSelectionBlock();
}

void
UOdysseyPainterEditorRasterTransformTool::OnPivotHandleDragged()
{
}

void
UOdysseyPainterEditorRasterTransformTool::UpdateRasterSelection()
{
    FOdysseyMediaProvider mediaProvider = mEditor->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return;

    TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters = mediaProvider.GetOrCreateMedias<FOdysseyMediaRaster>();
    if (mediaRasters.Num() <= 0)
        return;

    TSharedPtr<FOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();
    if (rasterSelection->IsEmpty())
    {
        mSelectionBlock = nullptr;
        mTransformSelectionBlock = nullptr;
        return;
    }

    ::ULIS::FRectI boundingBox = rasterSelection->GetMaskBoundingRect();

    TSharedPtr<FOdysseyRasterBlock> rasterBlock = mediaRasters[0]->GetRasterBlock();
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = rasterBlock->GetBlock();

    mSelectionBlock = MakeShared<::ULIS::FBlock>(boundingBox.w, boundingBox.h, rasterBlock->GetFormat());

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(rasterBlock->GetFormat());
    ::ULIS::FEvent clearEvent, copyEvent;
    ctx.Clear(*mSelectionBlock);
    ctx.Finish();

    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> maskBlock = mEditor->RasterSelection()->GetBlock();
    
    if (maskBlock)
    {
        ctx.Copy(
            *block,
            *mSelectionBlock,
            boundingBox,
            ::ULIS::FVec2I(0,0),
            ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
            0,
            nullptr,
            &copyEvent
        );

        ctx.FilterInto(
            [](const ::ULIS::FPixel& iSrcPixel, ::ULIS::FPixel& iDstPixel, uint64 iNumPixels)
            {
                for (int i = 0; i < iNumPixels; i++, iSrcPixel.Next(), iDstPixel.Next())
                {
                    iDstPixel.SetAlphaF(iDstPixel.AlphaF() * iSrcPixel.GreyF());
                }
            }
                , *maskBlock
                , *mSelectionBlock
                , boundingBox
                , ::ULIS::FVec2I(0,0)
                , ::ULIS::FSchedulePolicy::MultiScanlines
                , 1
                , &copyEvent
                , nullptr
                );

        ctx.Finish();
    }
}

void
UOdysseyPainterEditorRasterTransformTool::ResetRasterSelection()
{
    if (!mSelectionBlock)
        return;

    TSharedPtr<FOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(mSelectionBlock->Format());

    ctx.Clear(*rasterSelection->GetBlock());
    ctx.Finish();

    ctx.Copy(*mSelectionBlock, *rasterSelection->GetBlock(), ::ULIS::FRectI::Auto, ::ULIS::FVec2I(mSelectionBoundingBox.x, mSelectionBoundingBox.y));
    ctx.Finish();

    rasterSelection->RefreshHUD(); 
}

void
UOdysseyPainterEditorRasterTransformTool::OnRasterSelectionChanged()
{
    UpdateRasterSelection();
    ClearTransform();
    CreateTransformAreaFromSelection();
}


#undef LOCTEXT_NAMESPACE
