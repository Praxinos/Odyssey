// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathEditTool/OdysseyPainterEditorVectorPathEditTool.h"
#include "LayerStack/OdysseyTextureLayer.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "TextureEditor/OdysseyTextureEditor.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorPathBuilder.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathEditTool::~UOdysseyPainterEditorVectorPathEditTool()
{
}

UOdysseyPainterEditorVectorPathEditTool::UOdysseyPainterEditorVectorPathEditTool()
    : Size(1.0f)
    , mCubicPathHUD( FOdysseyVectorHUDPathCubic::VIEW_PATH
                   | FOdysseyVectorHUDPathCubic::VIEW_POINT/*
                   | FOdysseyVectorHUDPathCubic::VIEW_HANDLE_POINT
                   | FOdysseyVectorHUDPathCubic::VIEW_HANDLE_SEGMENT*/ )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.VectoEdit64");
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorPathEditTool::Activate()
{
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();

    if(currentVectorLayer)
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();

        vectorEngine->ClearHUD();
        vectorEngine->AddHUD(&mCubicPathHUD);

        currentVectorLayer->RenderImageChanged(false);
    }
}

bool
UOdysseyPainterEditorVectorPathEditTool::CanDraw()
{
    return IsActivable();
}

bool
UOdysseyPainterEditorVectorPathEditTool::OnKeyDown(const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    /*
        if (!CanDraw())
            return false;
    */

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);

        if ( ( iKey == EKeys::LeftControl ) || ( iKey == EKeys::RightControl ) )
        {
            mCubicPathHUD.SetDisplayMode( FOdysseyVectorHUDPathCubic::VIEW_PATH
                                        | FOdysseyVectorHUDPathCubic::VIEW_HANDLE_SEGMENT );
        }

        if ( ( iKey == EKeys::LeftShift ) || ( iKey == EKeys::LeftShift ) )
        {
            mCubicPathHUD.SetDisplayMode( FOdysseyVectorHUDPathCubic::VIEW_PATH
                                        | FOdysseyVectorHUDPathCubic::VIEW_HANDLE_POINT );
        }

        currentVectorLayer->RenderImageChanged( true );
    }

    // TODO: find out the diference between returning true or false
    return false;
}

bool
UOdysseyPainterEditorVectorPathEditTool::OnKeyUp(const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

/*
    if (!CanDraw())
        return false;
*/

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);

        mCubicPathHUD.SetDisplayMode( FOdysseyVectorHUDPathCubic::VIEW_PATH
                                    | FOdysseyVectorHUDPathCubic::VIEW_POINT );

        currentVectorLayer->RenderImageChanged( true );
    }

    return false;
}

bool
UOdysseyPainterEditorVectorPathEditTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();

    mPickedPointArray.clear();

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        UOdysseyVectorObject* selectedObject = currentVectorLayer->GetScene()->GetLastSelected();

        if ( selectedObject )
        {
            bool picked = false;

            if ( selectedObject->GetClass() == UOdysseyVectorPathCubic::StaticClass() )
            {
                double radius = 10.0f;
                UOdysseyVectorPathCubic *cubicPath = Cast<UOdysseyVectorPathCubic>( selectedObject );
                BLPoint localCoords = cubicPath->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
                BLPoint localSize = cubicPath->GetInverseWorldMatrix().mapVector( radius, radius );
                double localRadius;

                uint64 selectionFlags = 0;

                localRadius = sqrt ( ( localSize.x * localSize.x ) + ( localSize.y * localSize.y ) );

                mOldLocalMouseX = localCoords.x;
                mOldLocalMouseY = localCoords.y;

                cubicPath->Unselect( nullptr );
/*
                if ( QApplication::keyboardModifiers().testFlag( Qt::ControlModifier ) == true )
                {
                    selectionFlags = FVectorPath::PICK_HANDLE_POINT;
                }
                else
                {
*/
                 if( mCubicPathHUD.GetDisplayMode() & FOdysseyVectorHUDPathCubic::VIEW_HANDLE_SEGMENT )
                 {
                    selectionFlags = UOdysseyVectorPath::PICK_HANDLE_SEGMENT;
                 }

                 if( mCubicPathHUD.GetDisplayMode() & FOdysseyVectorHUDPathCubic::VIEW_HANDLE_POINT )
                 {
                    selectionFlags = UOdysseyVectorPath::PICK_HANDLE_POINT;
                 }

                 if( mCubicPathHUD.GetDisplayMode() & FOdysseyVectorHUDPathCubic::VIEW_POINT )
                 {
                    selectionFlags = UOdysseyVectorPath::PICK_POINT;
                 }
/*
                }
*/
                cubicPath->PickPoint( localCoords.x, localCoords.y, localRadius, mPickedPointArray, selectionFlags );

                cubicPath->Invalidate();

/*
                if ( picked == false )
                {
                    PickObject (event);
                }
*/
            }
        }
/*
        else
        {
            PickObject ( event );
        }
*/
    }

    return true;
}

static ::ULIS::FRectD
DragPoint( double iLocalX
         , double iLocalY
         , double iOldLocalX
         , double iOldLocalY
         , UOdysseyVectorPoint *iPoint )
{
    double difx = iLocalX - iOldLocalX;
    double dify = iLocalY - iOldLocalY;

    if ( iPoint->GetClass() == UOdysseyVectorHandlePoint::StaticClass() )
    {
        UOdysseyVectorHandlePoint* pointHandle = static_cast<UOdysseyVectorHandlePoint*>( iPoint );
        UOdysseyVectorVertexCubic* cubicVertex = static_cast<UOdysseyVectorVertexCubic*>(pointHandle->GetParent());
        ::ULIS::FVec2D dif = { cubicVertex->GetX() - iLocalX, cubicVertex->GetY() - iLocalY };

        cubicVertex->SetRadius( dif.Distance(), true );

        return cubicVertex->GetRectangle();
    }

    if( iPoint->GetClass() == UOdysseyVectorHandleSegment::StaticClass() )
    {
        UOdysseyVectorHandleSegment* segmentHandle = static_cast<UOdysseyVectorHandleSegment*>( iPoint );
        UOdysseyVectorSegmentCubic* cubicSegment = static_cast<UOdysseyVectorSegmentCubic*>(segmentHandle->GetParent());

        iPoint->Set( iPoint->GetX() + difx
                   , iPoint->GetY() + dify );

        return cubicSegment->GetBoundingBox();
    }

    if( iPoint->GetClass() == UOdysseyVectorVertexCubic::StaticClass() )
    {
        UOdysseyVectorVertexCubic* cubicVertex = static_cast<UOdysseyVectorVertexCubic*>( iPoint );
        std::list<UOdysseyVectorSegment*> segmentList = cubicVertex->GetSegmentList();

        cubicVertex->Set( iPoint->GetX() + difx
                        , iPoint->GetY() + dify, false );

        // Control point must move with the point
        for( std::list<UOdysseyVectorSegment*>::iterator segit = segmentList.begin(); segit != segmentList.end(); ++segit )
        {
            UOdysseyVectorSegmentCubic* cubicSegment = static_cast<UOdysseyVectorSegmentCubic*>(*segit);
            UOdysseyVectorHandleSegment* ctrlPoint = ( cubicSegment->GetPoint(0) == iPoint ) ? static_cast<UOdysseyVectorHandleSegment*>( cubicSegment->GetControlPoint( 0 ) ) :
                                                                                               static_cast<UOdysseyVectorHandleSegment*>( cubicSegment->GetControlPoint( 1 ) );

            ctrlPoint->Set( ctrlPoint->GetX() + difx
                          , ctrlPoint->GetY() + dify );
        }

        return cubicVertex->GetRectangle();
    }

    return { 0, 0, 0, 0 };
}

void
UOdysseyPainterEditorVectorPathEditTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();
    static ::ULIS::FRectI oldInvalidatedArea = { 0, 0, 0, 0 };

    if( currentVectorLayer )
    {
        UOdysseyVectorObject* selectedObject = currentVectorLayer->GetScene()->GetLastSelected();

        if ( selectedObject )
        {
            if( selectedObject->GetClass() == UOdysseyVectorPathCubic::StaticClass() )
            {
                UOdysseyVectorPathCubic *cubicPath = Cast<UOdysseyVectorPathCubic>( selectedObject );
                BLPoint localCoords = cubicPath->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
                ::ULIS::FRectD localInvalidatedArea = { 0, 0, 0, 0 };
                ::ULIS::FRectI invalidatedArea;
                ::ULIS::FRectI totalInvalidatedArea;
                BLPoint worldAreaP1;
                BLPoint worldAreaP2;
                bool inited = false;

                for( int i = 0; i < mPickedPointArray.size(); i++ )
                {
                    UOdysseyVectorPoint *selectedPoint = mPickedPointArray[i];
                    ::ULIS::FRectD rect;

                    rect = DragPoint( localCoords.x, localCoords.y, mOldLocalMouseX, mOldLocalMouseY, selectedPoint );

                    localInvalidatedArea = ( inited == false ) ? rect : localInvalidatedArea | rect;

                    inited = true;
                }

                worldAreaP1 = cubicPath->GetWorldMatrix().mapPoint( localInvalidatedArea.x, localInvalidatedArea.y );
                worldAreaP2 = cubicPath->GetWorldMatrix().mapPoint( localInvalidatedArea.x + localInvalidatedArea.w
                                                                  , localInvalidatedArea.y + localInvalidatedArea.h );

                invalidatedArea = ::ULIS::FRectI::FromMinMax( ::ULIS::FMath::Min(worldAreaP1.x,worldAreaP2.x)
                                                            , ::ULIS::FMath::Min(worldAreaP1.y,worldAreaP2.y)
                                                            , ::ULIS::FMath::Max(worldAreaP1.x,worldAreaP2.x)
                                                            , ::ULIS::FMath::Max(worldAreaP1.y,worldAreaP2.y) );

                totalInvalidatedArea = invalidatedArea | oldInvalidatedArea;

//UE_LOG(LogTemp, Warning, TEXT("%d %d %d %d"), invalidatedArea.x, invalidatedArea.y, invalidatedArea.w, invalidatedArea.h );
                currentVectorLayer->GetScene()->Update( UOdysseyVectorObject::FREQUENTUPDATES
                                                      | UOdysseyVectorObject::KEEPINVALIDATED );

                currentVectorLayer->RenderImageChanged( /*{ totalInvalidatedArea },*/ true );

                mOldLocalMouseX = localCoords.x;
                mOldLocalMouseY = localCoords.y;

                oldInvalidatedArea = invalidatedArea;
            }
        }
    }
}

bool
UOdysseyPainterEditorVectorPathEditTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();

    if( currentVectorLayer )
    {
        currentVectorLayer->GetScene()->Update( 0 );

        currentVectorLayer->RenderImageChanged(false);

        return true;
    }

    return false;
}

void
UOdysseyPainterEditorVectorPathEditTool::Commit()
{

}
