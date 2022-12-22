// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorEditPathTool/OdysseyPainterEditorVectorEditPathTool.h"
#include "LayerStack/OdysseyTextureLayer.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "TextureEditor/OdysseyTextureEditor.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorPathBuilder.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorEditPathTool::~UOdysseyPainterEditorVectorEditPathTool()
{
}

UOdysseyPainterEditorVectorEditPathTool::UOdysseyPainterEditorVectorEditPathTool()
    : Size(1.0f)
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.DrawingTool64");
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorEditPathTool::Activate()
{
	//FOdysseyObjectEditorUtils::SetPropertyValue(BrushOptions, "Color", FOdysseyBrushColor(GetEditorAs<FOdysseyPainterEditor>()->PaintColor()));
}

bool
UOdysseyPainterEditorVectorEditPathTool::CanDraw()
{
    return IsActivable();
}

bool
UOdysseyPainterEditorVectorEditPathTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentVectorImageLayer();

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetVectorEngine();
        UOdysseyVectorObject* selectedObject = vectorEngine->GetScene()->GetLastSelected();

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
                    selectionFlags = UOdysseyVectorPath::PICK_HANDLE_SEGMENT | UOdysseyVectorPath::PICK_POINT;
/*
                }
*/
                picked = cubicPath->PickPoint( localCoords.x, localCoords.y, localRadius, selectionFlags );
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

    if( iPoint->GetClass() == UOdysseyVectorVertex::StaticClass() )
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
UOdysseyPainterEditorVectorEditPathTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentVectorImageLayer();
    static ::ULIS::FRectI oldInvalidatedArea = { 0, 0, 0, 0 };

    if( currentVectorLayer )
    {
        UOdysseyVectorObject* selectedObject = currentVectorLayer->GetVectorEngine()->GetScene()->GetLastSelected();

        if ( selectedObject )
        {
            if( selectedObject->GetClass() == UOdysseyVectorPathCubic::StaticClass() )
            {
                UOdysseyVectorPathCubic *cubicPath = Cast<UOdysseyVectorPathCubic>( selectedObject );
                BLPoint localCoords = cubicPath->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
                std::list<UOdysseyVectorPoint*> selectedPointList = cubicPath->GetSelectedPointList();
                ::ULIS::FRectD localInvalidatedArea = { 0, 0, 0, 0 };
                ::ULIS::FRectI invalidatedArea;
                ::ULIS::FRectI totalInvalidatedArea;
                BLPoint worldAreaP1;
                BLPoint worldAreaP2;
                bool inited = false;

                for( std::list<UOdysseyVectorPoint*>::iterator it = selectedPointList.begin(); it != selectedPointList.end(); ++it )
                {
                    UOdysseyVectorPoint *selectedPoint = *it;
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
                currentVectorLayer->GetVectorEngine()->GetScene()->Update();

                currentVectorLayer->GetVectorEngine()->InvalidateRegion( totalInvalidatedArea );

                RedrawCurrentLayer( { /*{ 0, 0, 0, 0 }*/ totalInvalidatedArea } );

                mOldLocalMouseX = localCoords.x;
                mOldLocalMouseY = localCoords.y;

                oldInvalidatedArea = invalidatedArea;
            }
        }
    }
}

bool
UOdysseyPainterEditorVectorEditPathTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentVectorImageLayer();

    if( currentVectorLayer )
    {

        return true;
    }

    return false;
}

void
UOdysseyPainterEditorVectorEditPathTool::Commit()
{

}
