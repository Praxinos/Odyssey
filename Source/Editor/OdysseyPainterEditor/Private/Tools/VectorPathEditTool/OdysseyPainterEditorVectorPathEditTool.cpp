// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathEditTool/OdysseyPainterEditorVectorPathEditTool.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathEditTool::~UOdysseyPainterEditorVectorPathEditTool()
{
}

UOdysseyPainterEditorVectorPathEditTool::UOdysseyPainterEditorVectorPathEditTool()
    : Radius(10.0f)
    , mCubicPathHUD( FOdysseyVectorHUDPathCubic::VIEW_PATH
                   | FOdysseyVectorHUDPathCubic::VIEW_POINT/*
                   | FOdysseyVectorHUDPathCubic::VIEW_HANDLE_POINT
                   | FOdysseyVectorHUDPathCubic::VIEW_HANDLE_SEGMENT*/ )

{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.VectoEdit64");

    mPickingHUD.SetRadius( Radius );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorPathEditTool::Activate( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->ClearHUD();
    iEngine->AddHUD(&mCubicPathHUD);
    iEngine->AddHUD(&mPickingHUD);
}

bool
UOdysseyPainterEditorVectorPathEditTool::OnKeyDown( FOdysseyVectorEngine* iEngine
                                                  , FOdysseyVectorScene* iScene
                                                  , const FKey& iKey )
{
    if ( ( iKey == EKeys::LeftControl ) || ( iKey == EKeys::RightControl ) )
    {
        mCubicPathHUD.SetDisplayMode( FOdysseyVectorHUDPathCubic::VIEW_PATH
                                    | FOdysseyVectorHUDPathCubic::VIEW_HANDLE_SEGMENT );
    }

    if ( ( iKey == EKeys::LeftShift ) || ( iKey == EKeys::RightShift ) )
    {
        mCubicPathHUD.SetDisplayMode( FOdysseyVectorHUDPathCubic::VIEW_PATH
                                    | FOdysseyVectorHUDPathCubic::VIEW_HANDLE_POINT );
    }

    return false;
}

bool
UOdysseyPainterEditorVectorPathEditTool::OnKeyUp( FOdysseyVectorEngine* iEngine
                                                , FOdysseyVectorScene* iScene
                                                , const FKey& iKey )
{
    mCubicPathHUD.SetDisplayMode( FOdysseyVectorHUDPathCubic::VIEW_PATH
                                | FOdysseyVectorHUDPathCubic::VIEW_POINT );

    return false;
}

bool
UOdysseyPainterEditorVectorPathEditTool::OnMouseDown( FOdysseyVectorEngine* iEngine
                                                    , FOdysseyVectorScene* iScene
                                                    , FOdysseyVectorUndo** iUndo
                                                    , const FOdysseyPoint& iPointInTexture
                                                    , const FKey& iKey )
{
    FOdysseyVectorObject* selectedObject = iScene->GetLastSelected();

    mPickedPointArray.clear();

    if ( selectedObject )
    {
        bool picked = false;

        if ( selectedObject->GetClass() == FOdysseyVectorPathCubic::StaticClass() )
        {
            FOdysseyVectorPathCubic *cubicPath = static_cast<FOdysseyVectorPathCubic*>( selectedObject );
            BLPoint localCoords = cubicPath->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
            uint64 selectionFlags = 0;

            mOldLocalMouseX = localCoords.x;
            mOldLocalMouseY = localCoords.y;

            cubicPath->Unselect( nullptr );

            if( mCubicPathHUD.GetDisplayMode() & FOdysseyVectorHUDPathCubic::VIEW_HANDLE_SEGMENT )
            {
                selectionFlags = FOdysseyVectorPath::PICK_HANDLE_SEGMENT;
            }

            if( mCubicPathHUD.GetDisplayMode() & FOdysseyVectorHUDPathCubic::VIEW_HANDLE_POINT )
            {
                selectionFlags = FOdysseyVectorPath::PICK_HANDLE_POINT;
            }

            if( mCubicPathHUD.GetDisplayMode() & FOdysseyVectorHUDPathCubic::VIEW_POINT )
            {
                selectionFlags = FOdysseyVectorPath::PICK_POINT;
            }

            cubicPath->PickPoint( iPointInTexture.x, iPointInTexture.y, Radius, mPickedPointArray, selectionFlags );

            // TODO: put this in a function or something
            // Control point must move with the point. Store it in the mPickedPointArray as well
            for( int i = 0; i < mPickedPointArray.size(); i++ )
            {
                if( mPickedPointArray[i]->GetClass() == FOdysseyVectorVertexCubic::StaticClass() )
                {
                    FOdysseyVectorVertexCubic* cubicVertex = static_cast<FOdysseyVectorVertexCubic*>( mPickedPointArray[i] );
                    std::list<FOdysseyVectorSegment*> segmentList = cubicVertex->GetSegmentList();

                    for( std::list<FOdysseyVectorSegment*>::iterator segit = segmentList.begin(); segit != segmentList.end(); ++segit )
                    {
                        FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(*segit);
                        FOdysseyVectorHandleSegment* handle = ( cubicVertex == cubicSegment->GetVertex( 0 ) ) ? cubicSegment->GetHandle( 0 ) :
                                                                                                                cubicSegment->GetHandle( 1 );

                        mPickedPointArray.push_back( handle );
                    }
                }
            }
            // End-of TODO

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
    // BeginTransaction() must be called for GUndo to have a value. Please do it in the caller function.
    if( iUndo && GUndo )
    {
        (*iUndo) = new FOdysseyVectorUndoPointPosition( iScene, mPickedPointArray );

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(*iUndo) );
    }

    return true;
}

void
UOdysseyPainterEditorVectorPathEditTool::OnMouseHover( FOdysseyVectorEngine* iEngine
                                                     , FOdysseyVectorScene* iScene
                                                     , const FOdysseyPoint& iPointInTexture )
{
/*
    ::ULIS::FRectI formerRegion = ::ULIS::FRectI( iPointInTexture.x - iPointInTexture.deltaPosition.X - Radius
                                                , iPointInTexture.y - iPointInTexture.deltaPosition.Y - Radius
                                                , Radius * 2
                                                , Radius * 2 );
    ::ULIS::FRectI currentRegion = ::ULIS::FRectI( iPointInTexture.x - Radius
                                                 , iPointInTexture.y - Radius
                                                 , Radius * 2
                                                 , Radius * 2 );
    ::ULIS::FRectI finalRegion = currentRegion | formerRegion;
*/
    mPickingHUD.SetPosition( iPointInTexture.x, iPointInTexture.y );
}

static ::ULIS::FRectD
DragPoint( double iLocalX
         , double iLocalY
         , double iOldLocalX
         , double iOldLocalY
         , FOdysseyVectorPoint *iPoint )
{
    double difx = iLocalX - iOldLocalX;
    double dify = iLocalY - iOldLocalY;

    if ( iPoint->GetClass() == FOdysseyVectorHandlePoint::StaticClass() )
    {
        FOdysseyVectorHandlePoint* pointHandle = static_cast<FOdysseyVectorHandlePoint*>( iPoint );
        FOdysseyVectorVertexCubic* cubicVertex = static_cast<FOdysseyVectorVertexCubic*>(pointHandle->GetParent());
        ::ULIS::FVec2D dif = { cubicVertex->GetX() - iLocalX, cubicVertex->GetY() - iLocalY };

        cubicVertex->SetRadius( dif.Distance() );

        return cubicVertex->GetBoundingBox( false );
    }

    if( iPoint->GetClass() == FOdysseyVectorHandleSegment::StaticClass() )
    {
        FOdysseyVectorHandleSegment* segmentHandle = static_cast<FOdysseyVectorHandleSegment*>( iPoint );
        FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segmentHandle->GetParent());

        iPoint->Set( iPoint->GetX() + difx
                   , iPoint->GetY() + dify );

        return cubicSegment->GetBoundingBox( false );
    }

    if( iPoint->GetClass() == FOdysseyVectorVertexCubic::StaticClass() )
    {
        FOdysseyVectorVertexCubic* cubicVertex = static_cast<FOdysseyVectorVertexCubic*>( iPoint );
        std::list<FOdysseyVectorSegment*> segmentList = cubicVertex->GetSegmentList();

        cubicVertex->Set( iPoint->GetX() + difx
                        , iPoint->GetY() + dify );

        return cubicVertex->GetBoundingBox( false );
    }

    return { 0, 0, 0, 0 };
}

void
UOdysseyPainterEditorVectorPathEditTool::OnMouseDrag( FOdysseyVectorEngine* iEngine
                                                    , FOdysseyVectorScene* iScene
                                                    , const FOdysseyPoint& iPointInTexture )
{
    FOdysseyVectorObject* selectedObject = iScene->GetLastSelected();
    static ::ULIS::FRectI oldInvalidatedArea = { 0, 0, 0, 0 };

    mPickingHUD.SetPosition( iPointInTexture.x, iPointInTexture.y );

    if ( selectedObject )
    {
        if( selectedObject->GetClass() == FOdysseyVectorPathCubic::StaticClass() )
        {
            FOdysseyVectorPathCubic *cubicPath = static_cast<FOdysseyVectorPathCubic*>( selectedObject );
            BLPoint localCoords = cubicPath->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
            ::ULIS::FRectD localInvalidatedArea = { 0, 0, 0, 0 };
            ::ULIS::FRectI invalidatedArea;
            ::ULIS::FRectI totalInvalidatedArea;
            BLPoint worldAreaP1;
            BLPoint worldAreaP2;
            bool inited = false;

            for( int i = 0; i < mPickedPointArray.size(); i++ )
            {
                FOdysseyVectorPoint *selectedPoint = mPickedPointArray[i];
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
            iScene->Update( FOdysseyVectorObject::FREQUENTUPDATES | FOdysseyVectorObject::KEEPINVALIDATED );

            mOldLocalMouseX = localCoords.x;
            mOldLocalMouseY = localCoords.y;

            oldInvalidatedArea = invalidatedArea;
        }
    }
}

bool
UOdysseyPainterEditorVectorPathEditTool::OnMouseUp( FOdysseyVectorEngine* iEngine
                                                  , FOdysseyVectorScene* iScene
                                                  , const FOdysseyPoint& iPointInTexture
                                                  , const FKey& iKey )
{
    iScene->Update( 0 );

    return true;
}

void
UOdysseyPainterEditorVectorPathEditTool::Commit()
{

}

void
UOdysseyPainterEditorVectorPathEditTool::PropertyChanged( const FName& iPropertyName )
{
    mPickingHUD.SetRadius( Radius );
}
