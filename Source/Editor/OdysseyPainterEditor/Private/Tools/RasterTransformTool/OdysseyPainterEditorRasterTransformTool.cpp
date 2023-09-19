// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/RasterTransformTool/OdysseyPainterEditorRasterTransformTool.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyHUDPolygon.h"
#include "OdysseyHUDRectangle.h"
#include "GeomTools.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorRasterTransformTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorRasterTransformTool::~UOdysseyPainterEditorRasterTransformTool()
{
}

UOdysseyPainterEditorRasterTransformTool::UOdysseyPainterEditorRasterTransformTool() :
    mPaintEngine(),
    mTransformArea(nullptr),
    mMouseReferencePoint( -1, -1 )
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
    //Create widget slate when creating this
    if (!mTransformArea) 
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

        mAreaConstrain = EOdysseyTransformConstrain::Rectangle;
    }
    else if( FGeomTools2D::IsPointInPolygon( FVector2D( iPointInTexture.x, iPointInTexture.y ), mTransformArea->mPoints ) )
    {
        mMouseReferencePoint = FVector2D( iPointInTexture.x, iPointInTexture.y );
        return true;
    }

    mMouseReferencePoint = FVector2D( -1, -1 );
    return mHUD->OnKeyDown(iPointInTexture, iKey);
}

void UOdysseyPainterEditorRasterTransformTool::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    mHUD->MouseMove( iPointInTexture );
}

void UOdysseyPainterEditorRasterTransformTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    if( mTransformArea )
    {
        switch(mAreaConstrain)
        {
            case EOdysseyTransformConstrain::Rectangle:
                ConstrainToRectangle( FVector2D( iPointInTexture.x, iPointInTexture.y ) );
            break;
            case EOdysseyTransformConstrain::Parallelogram:
                ConstrainToParallelogram( FVector2D( iPointInTexture.x, iPointInTexture.y ) );
            break;
            case EOdysseyTransformConstrain::NoConstrain:
                for (int i = 0; i < mHandles.Num(); i++)
                {
                    mHandles[i]->SetPosition(mHandles[i]->GetPosition() - (mMouseReferencePoint - FVector2D(iPointInTexture.x, iPointInTexture.y)));
                }
            break;
            default:
            break;
        }
        mMouseReferencePoint = FVector2D(iPointInTexture.x, iPointInTexture.y);
        return;
    }

    mHUD->CapturedMouseMove( iPointInTexture );
}

bool UOdysseyPainterEditorRasterTransformTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    bool isHandled = mHUD->OnKeyUp( iPointInTexture, iKey );

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

        /*::ULIS::eFormat format = paintBlock->Format();

        mReferenceBlock = MakeShareable(new ::ULIS::FBlock(100, 100, format));

        ::ULIS::FEvent copyEvent, clearEvent, blendEvent;

	    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);

        ctx.Copy(
            *paintBlock,
            *mReferenceBlock,
            mReferenceBlock->Rect(),
            ::ULIS::FVec2I(0,0),
            ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
            0,
            nullptr,
            &copyEvent
            );

        ctx.Clear(
            *paintBlock,
            mReferenceBlock->Rect(),
            ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
            1,
            &copyEvent,
            &clearEvent);

        ctx.Blend(
            *mReferenceBlock,
            *paintBlock,
            mReferenceBlock->Rect(),
            ::ULIS::FVec2I(100, 100),
            ::ULIS::Blend_Normal,
            ::ULIS::Alpha_Normal,
            1.f,
            ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
            1, //On attend sur 0 event
            &clearEvent, // Event sur lequel attendre
            &blendEvent //Event qui se passe là maintenant tout de suite
        );

	    ctx.Finish();

	    paintBlock->Dirty();

        mPaintEngine.Update(FOdysseyBlendParameters());*/


        //ctx.Flush(); //Demarre la liste d'events
        //ctx.Finish(); //Flush + finish

        //blockToMove->Dirty();
    }

    return isHandled;
}

void UOdysseyPainterEditorRasterTransformTool::ConstrainToRectangle(FVector2D iPosition)
{
    for( int i = 0; i < mHandles.Num(); i++ )
    {
        if( mHandles[i]->IsCaptured() )
        {
            UE_LOG(LogTemp, Display, TEXT("%d"), i );
        }
    }
}

void UOdysseyPainterEditorRasterTransformTool::ConstrainToParallelogram(FVector2D iPosition)
{

}

#undef LOCTEXT_NAMESPACE
