// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/RasterTransformTool/OdysseyPainterEditorRasterTransformTool.h"
#include "Tools/RasterTransformTool/Selection/OdysseyPainterEditorRasterRectangleSelection.h"
#include "Tools/RasterTransformTool/Selection/OdysseyPainterEditorRasterEllipseSelection.h"
#include "Tools/RasterTransformTool/Selection/OdysseyPainterEditorRasterFreehandSelection.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyHUDPolygon.h"
#include "OdysseyBrushTransform.h"
#include "GeomTools.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorRasterTransformTool"

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
    SelectionShape(EOdysseySelectionShape::Rectangle),
    mSelection(NewObject<UOdysseyPainterEditorRasterRectangleSelection>()),
    mPaintEngine(),
    mTransformedBlock(nullptr),
    mRasterMutator(true),
    mTransformAreaConstrain(EOdysseyTransformConstrain::Rectangle),
    mTransformCaptureMode(EOdysseyTransformCapture::NoCapture),
    mTransformArea(nullptr)
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
    if (mHUD->OnKeyDown(iPointInTexture, iKey)) //Handling HUD events if needed
        return true;

    if (mSelection->OnMouseDown(iPointInTexture, iKey))
        return true;

    if( mTransformArea )
    {
        mTransformCaptureMode = DetectCaptureMode( FVector2D( iPointInTexture.x, iPointInTexture.y ));
        mMouseLastReferencePoint = FVector2D(iPointInTexture.x, iPointInTexture.y);
        if( mTransformCaptureMode != EOdysseyTransformCapture::NoCapture )
            return true;
    }
    else if( mSelection->IsInSelectionArea( FVector2D( iPointInTexture.x, iPointInTexture.y )))
    {
        mTransformCaptureMode = EOdysseyTransformCapture::Inside;
        mMouseLastReferencePoint = FVector2D(iPointInTexture.x, iPointInTexture.y);
        CreateTransformAreaFromSelection();
        CreateTransformBlockFromSelectionBlock();
        return true;
    }

    return false;
}

void UOdysseyPainterEditorRasterTransformTool::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    //If is in polygon, change mouse cursor to hand
    //If is in other area, change mouse cursor accordingly
    mHUD->MouseMove(iPointInTexture);
    mSelection->OnMouseHover( iPointInTexture );
}

void UOdysseyPainterEditorRasterTransformTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    mHUD->CapturedMouseMove(iPointInTexture);

    mSelection->OnMouseDrag(iPointInTexture);

    if (mTransformCaptureMode == EOdysseyTransformCapture::Inside)
    {
        TArray<FVector2D>& points = mTransformArea->GetPoints();
        for (int i = 0; i < points.Num(); i++)
        {
            points[i] = points[i] - (mMouseLastReferencePoint - FVector2D(iPointInTexture.x, iPointInTexture.y));
        }
        mMouseLastReferencePoint = FVector2D(iPointInTexture.x, iPointInTexture.y);
        //BlendTransformAreaToPaintBlock();
        return;
    }

    switch (mTransformAreaConstrain)
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

    CreateTransformBlockFromSelectionBlock();
    //BlendTransformAreaToPaintBlock();
}

bool UOdysseyPainterEditorRasterTransformTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    mTransformCaptureMode = EOdysseyTransformCapture::NoCapture;

    if( mHUD->OnKeyUp(iPointInTexture, iKey) )
        return true;

    if( mSelection->OnMouseUp( iPointInTexture, iKey ))
        return true;

    return false;
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
        ClearTransform();
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
    mSelection->Init(mHUD, mEditor);
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
                mSelection->Init(mHUD, mEditor);
                break;
            case EOdysseySelectionShape::FreeHand:
                mSelection = NewObject<UOdysseyPainterEditorRasterFreehandSelection>();
                mSelection->AddToRoot();
                mSelection->Init(mHUD, mEditor);
                break;
            case EOdysseySelectionShape::Ellipse:
                mSelection = NewObject<UOdysseyPainterEditorRasterEllipseSelection>();
                mSelection->AddToRoot();
                mSelection->Init(mHUD, mEditor);
                break;
            default:
                break;
        }

    }
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

    mTransformArea = new FOdysseyHUDPolygon(FName("TransformArea"), areaPoints);
    mHUD->EmptyHUDElements(); //Deleting the HUD of the selection to create the one for the transform
    mEditor->HUDSystem()->ClearHUDSurface();
    mHUD->AddElement(mTransformArea);

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

    return EOdysseyTransformCapture::NoCapture;
}

void UOdysseyPainterEditorRasterTransformTool::ConstrainToRectangle(FVector2D iPosition)
{
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

void UOdysseyPainterEditorRasterTransformTool::CreateTransformBlockFromSelectionBlock()
{
    if( !mSelection->GetSelectionBlock() )
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

    float scaleX = (float)mTransformedBlock->Width() / (float)mSelection->GetSelectionBlock()->Width();
    float scaleY = (float)mTransformedBlock->Height() / (float)mSelection->GetSelectionBlock()->Height();
    FOdysseyMatrix scale = UOdysseyTransformProxyLibrary::MakeScaleMatrix( scaleX, scaleY );
    
    ctx.TransformAffine(
        *mSelection->GetSelectionBlock()
        , *mTransformedBlock
        , mSelection->GetSelectionBlock()->Rect()
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
        GEditor->BeginTransaction(LOCTEXT("CommitTransform", "Transform"));
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
    mHandles.Empty();
    mHUD->EmptyHUDElements();
    mTransformArea = nullptr;
    mTransformAreaConstrain = EOdysseyTransformConstrain::Rectangle;
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

#undef LOCTEXT_NAMESPACE
