// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/RasterLineTool/OdysseyPainterEditorRasterLineTool.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyHUDLine.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorRasterLineTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorRasterLineTool::~UOdysseyPainterEditorRasterLineTool()
{
}

UOdysseyPainterEditorRasterLineTool::UOdysseyPainterEditorRasterLineTool() :
    mPaintEngine(),
    mLine(nullptr),
    //mReferenceBlock(nullptr),
    mRasterMutator(true)
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.LineTool32" );
}

bool
UOdysseyPainterEditorRasterLineTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaRaster>();
}

bool UOdysseyPainterEditorRasterLineTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    mLine = new FOdysseyHUDLine(FName("Line"), FVector2D( iPointInTexture.x, iPointInTexture.y ), FVector2D( iPointInTexture.x, iPointInTexture.y ) );
    mHUD->AddElement(mLine);

    FOdysseyHUDHandle* handleStart = new FOdysseyHUDHandle(FName("handleStart"), mLine, &(mLine->mStartPoint));
    FOdysseyHUDHandle* handleFinish = new FOdysseyHUDHandle(FName("handleFinish"), mLine, &(mLine->mFinishPoint));

    mLine->AddElement(handleStart);
    mLine->AddElement(handleFinish);

    mHandles.Add(handleStart);
    mHandles.Add(handleFinish);
    
    mHUD->OnKeyDown(iPointInTexture, iKey);

    return true;
}

void UOdysseyPainterEditorRasterLineTool::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    mHUD->MouseMove( iPointInTexture );
}

void UOdysseyPainterEditorRasterLineTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    mHUD->CapturedMouseMove(iPointInTexture);

    if( mLine )
    {
        BlendLineToPaintBlock();
        return;
    }
}

bool UOdysseyPainterEditorRasterLineTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    bool isHandled = mHUD->OnKeyUp( iPointInTexture, iKey );

    if (mLine)
    {
        //TODO: create preview and blend it on the raster block
        /*
        TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaRaster>();
        if (mediaRasters.Num() <= 0)
            return isHandled;

        if (mediaRasters[0]->IsLocked())
            return isHandled;

        TSharedPtr<FOdysseyRasterBlock> rasterBlock = mediaRasters[0]->GetRasterBlock();
        mPaintEngine.RasterBlock(rasterBlock);
        TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> paintBlock = mPaintEngine.PaintBlock();

        if( mAreaConstrain == EOdysseyLineConstrain::Rectangle )
        {
            ::ULIS::eFormat format = rasterBlock->GetFormat();
            ::ULIS::FRectI boundingBox = GetLineAreaBoundingRect();
            
            if( mReferenceBlock )
                return true;

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
        return true;*/
    }

    return isHandled;
}

bool UOdysseyPainterEditorRasterLineTool::OnKeyUp(const FKey& iKey)
{
    if( iKey == EKeys::Escape )
    {
        AbortLine();
        return true;
    }

    return false;
}

void UOdysseyPainterEditorRasterLineTool::Load()
{
    UOdysseyPainterEditorTool::Load();
}

void UOdysseyPainterEditorRasterLineTool::Unload()
{
    AbortLine();
    UOdysseyPainterEditorTool::Unload();
}

::ULIS::FRectI UOdysseyPainterEditorRasterLineTool::GetLineBoundingRect()
{
    if( mLine )
    {   
        int minX = FMath::Min( mLine->mStartPoint.X, mLine->mFinishPoint.X );
        int maxX = FMath::Max( mLine->mStartPoint.X, mLine->mFinishPoint.X );
        int minY = FMath::Min( mLine->mStartPoint.Y, mLine->mFinishPoint.Y );
        int maxY = FMath::Max( mLine->mStartPoint.Y, mLine->mFinishPoint.Y );
        return ::ULIS::FRectI::FromMinMax( minX, minY, maxX, maxY );
    }
    return ::ULIS::FRectI::FromXYWH( 0,0,0,0 );
}

void UOdysseyPainterEditorRasterLineTool::BlendLineToPaintBlock()
{
    /*
    if( mLine )
    {
        TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> paintBlock = mPaintEngine.PaintBlock();

        if( !paintBlock || !mReferenceBlock )
            return;

        ::ULIS::eFormat format = paintBlock->Format();
        ::ULIS::FRectI boundingBox = GetLineBoundingRect();

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
    }*/
}

void UOdysseyPainterEditorRasterLineTool::CommitLine()
{
    if( mRasterMutator.GetRasterBlock() != nullptr )
    {
        GEditor->BeginTransaction(LOCTEXT("CommitLine", "Line"));
        mRasterMutator.Commit();
        mPaintEngine.Commit(FOdysseyBlendParameters());
        GEditor->EndTransaction();
        mRasterMutator.SetRasterBlock(nullptr);
    }
    ClearLine();
}

void UOdysseyPainterEditorRasterLineTool::AbortLine()
{
    if (mRasterMutator.GetRasterBlock() != nullptr)
    {
        mRasterMutator.Abort();
        mPaintEngine.Update(FOdysseyBlendParameters());
        mRasterMutator.SetRasterBlock(nullptr);
    }
    ClearLine();
}

void UOdysseyPainterEditorRasterLineTool::ClearLine()
{
    /*
    if (mReferenceBlock)
    {
        mReferenceBlock.Reset();
        mReferenceBlock = nullptr;
    }
    mHandles.Empty();
    mLine = nullptr;
    mEditor->HUDSystem()->ClearHUDSurface();*/
}

#undef LOCTEXT_NAMESPACE
