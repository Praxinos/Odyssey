// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/RasterTransformTool/OdysseyPainterEditorRasterTransformTool.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyHUDPolygon.h"
#include "GeomTools.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorRasterTransformTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorRasterTransformTool::~UOdysseyPainterEditorRasterTransformTool()
{
}

UOdysseyPainterEditorRasterTransformTool::UOdysseyPainterEditorRasterTransformTool() :
    mPaintEngine(),
    mReferenceBlock(nullptr),
    mTransformArea(nullptr),
    mRasterMutator(true),
    mAreaConstrain(EOdysseyTransformConstrain::Rectangle),
    mTransformCaptureMode(EOdysseyTransformCapture::NoCapture)
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.TransformTool32");
}

bool
UOdysseyPainterEditorRasterTransformTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaRaster>();
}

bool UOdysseyPainterEditorRasterTransformTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if (!mTransformArea) //Creating a zone for the transform
    {
        mTransformArea = NewObject<UOdysseyHUDPolygon>(GetTransientPackage(), NAME_None, RF_Transient);
        mTransformArea->AddToRoot();
        mHUD->AddElement(mTransformArea);

        TArray<FVector2D> areaPoints;
        for( int i = 0; i < 4; i++ )
        {
            areaPoints.Add( FVector2D( iPointInTexture.x, iPointInTexture.y ));
        }

        mTransformArea->Init(FName("TransformArea"), areaPoints );

        UOdysseyHUDHandle* handleTopLeft = NewObject<UOdysseyHUDHandle>(GetTransientPackage(), NAME_None, RF_Transient);
        handleTopLeft->AddToRoot();
        handleTopLeft->Init(FName("handleTopLeft"), mTransformArea, &(mTransformArea->mPoints[0]));

        UOdysseyHUDHandle* handleTopRight = NewObject<UOdysseyHUDHandle>(GetTransientPackage(), NAME_None, RF_Transient);
        handleTopRight->AddToRoot();
        handleTopRight->Init(FName("handleTopRight"), mTransformArea, &(mTransformArea->mPoints[1]));

        UOdysseyHUDHandle* handleBottomLeft = NewObject<UOdysseyHUDHandle>(GetTransientPackage(), NAME_None, RF_Transient);
        handleBottomLeft->AddToRoot();
        handleBottomLeft->Init(FName("handleBottomLeft"), mTransformArea, &(mTransformArea->mPoints[2]));

        UOdysseyHUDHandle* handleBottomRight = NewObject<UOdysseyHUDHandle>(GetTransientPackage(), NAME_None, RF_Transient);
        handleBottomRight->AddToRoot();
        handleBottomRight->Init(FName("handleBottomRight"), mTransformArea, &(mTransformArea->mPoints[3]));

        mTransformArea->AddElement(handleTopLeft);
        mTransformArea->AddElement(handleTopRight);
        mTransformArea->AddElement(handleBottomLeft);
        mTransformArea->AddElement(handleBottomRight);

        mHandles.Add(handleTopLeft);
        mHandles.Add(handleTopRight);
        mHandles.Add(handleBottomLeft);
        mHandles.Add(handleBottomRight);
    }
    
    
    if( mHUD->OnKeyDown(iPointInTexture, iKey) ) //Handling HUD events if needed
    {
        return true;
    }
    else if( FGeomTools2D::IsPointInPolygon( FVector2D( iPointInTexture.x, iPointInTexture.y ), mTransformArea->mPoints ) ) //Handling clicking inside the transform zone (for dragging it)
    {
        mTransformCaptureMode = EOdysseyTransformCapture::Inside;
        mMouseLastReferencePoint = FVector2D( iPointInTexture.x, iPointInTexture.y );
        return true;
    }


    return false;
}

void UOdysseyPainterEditorRasterTransformTool::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    mHUD->MouseMove( iPointInTexture );
}

void UOdysseyPainterEditorRasterTransformTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    mHUD->CapturedMouseMove(iPointInTexture);

    if( mTransformArea )
    {
        if( mTransformCaptureMode == EOdysseyTransformCapture::Inside )
        {
            for (int i = 0; i < mHandles.Num(); i++)
            {
                mHandles[i]->SetPosition(mHandles[i]->GetPosition() - (mMouseLastReferencePoint - FVector2D(iPointInTexture.x, iPointInTexture.y)));
            }
            mMouseLastReferencePoint = FVector2D(iPointInTexture.x, iPointInTexture.y);
            
            BlendTransformAreaToPaintBlock();
            
            return;
        }

        switch(mAreaConstrain)
        {
            case EOdysseyTransformConstrain::Rectangle:
                ConstrainToRectangle( FVector2D( iPointInTexture.x, iPointInTexture.y ) );
            break;
            case EOdysseyTransformConstrain::Parallelogram:
                ConstrainToParallelogram( FVector2D( iPointInTexture.x, iPointInTexture.y ) );
            break;
            case EOdysseyTransformConstrain::NoConstrain:
            break;
            default:
            break;
        }
    }
}

bool UOdysseyPainterEditorRasterTransformTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    bool isHandled = mHUD->OnKeyUp( iPointInTexture, iKey );
    mTransformCaptureMode = EOdysseyTransformCapture::NoCapture;

    if (mTransformArea)
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
            
            if( mReferenceBlock )
            {
                return true;
                //mReferenceBlock.Reset();
                //mReferenceBlock = nullptr;
            }

            mReferenceBlock = MakeShareable(new ::ULIS::FBlock(boundingBox.w, boundingBox.h, format));

            if( mRasterMutator.GetRasterBlock() == rasterBlock )
                return isHandled;

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
                            ::ULIS::FVec2I(0, 0),
                            ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
                            0,
                            nullptr,
                            &copyEvent
                        );

                        ctx.Clear(
                            *iBlock,
                            boundingBox,
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
        return true;
    }

    return isHandled;
}

void UOdysseyPainterEditorRasterTransformTool::Load()
{
    UOdysseyPainterEditorTool::Load();
}

void UOdysseyPainterEditorRasterTransformTool::Unload()
{
    if (mReferenceBlock)
    {
        mReferenceBlock.Reset();
        mReferenceBlock = nullptr;
    }
    mHandles.Empty();
    mTransformArea = nullptr;
    mAreaConstrain = EOdysseyTransformConstrain::Rectangle;
    mTransformCaptureMode = EOdysseyTransformCapture::NoCapture;
    CommitTransform();
    UOdysseyPainterEditorTool::Unload();
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

::ULIS::FRectI UOdysseyPainterEditorRasterTransformTool::GetTransformAreaBoundingRect()
{
    if( mTransformArea )
    {   
        int minX = mTransformArea->mPoints[0].X;
        int maxX = mTransformArea->mPoints[0].X;
        int minY = mTransformArea->mPoints[0].Y;
        int maxY = mTransformArea->mPoints[0].Y;
        for( int i = 1; i < mTransformArea->mPoints.Num(); i++ )
        {
            minX = FMath::Min( minX, mTransformArea->mPoints[i].X );
            maxX = FMath::Max( maxX, mTransformArea->mPoints[i].X );
            minY = FMath::Min( minY, mTransformArea->mPoints[i].Y );
            maxY = FMath::Max( maxY, mTransformArea->mPoints[i].Y );
        }
        return ::ULIS::FRectI::FromMinMax( minX, minY, maxX, maxY );
    }
    return ::ULIS::FRectI::FromXYWH( 0,0,0,0 );
}

void UOdysseyPainterEditorRasterTransformTool::BlendTransformAreaToPaintBlock()
{
    if( mTransformArea )
    {
        TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> paintBlock = mPaintEngine.PaintBlock();

        if( !paintBlock || !mReferenceBlock )
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
            *mReferenceBlock,
            *paintBlock,
            mReferenceBlock->Rect(),
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
    GEditor->BeginTransaction(LOCTEXT("CommitTransform", "Transform"));
    mRasterMutator.Commit();
    mPaintEngine.Commit( FOdysseyBlendParameters() );
    GEditor->EndTransaction();
    mRasterMutator.SetRasterBlock( nullptr );
}

#undef LOCTEXT_NAMESPACE
