// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyVectorCell.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorLayer.h"
#include "HUD/OdysseyVectorHUD.h"

FOdysseyVectorCell::FOdysseyVectorCell( IOdysseyVectorCell* iCellInterface
                                      , FOdysseyVectorGroupPaint* iScene )
    : FOdysseyVectorObject("ROOT")
    , mCellInterface ( iCellInterface )
    , mScene ( nullptr )
    , mSelectionSpace( nullptr )
    //, mInvalidTileMap( 64, iCell->GetWidth(), iCell->GetHeight() )
    , mInvalidatedRect( 0, 0, 0, 0 )
    , mBLMask ( nullptr )
    , bPendingRedraw ( false )
{
    bSystem = true;

    SetScene( iScene );
}

bool
FOdysseyVectorCell::HasBaseClass( uint32 iBaseClassID )
{
    if( mStaticClass == iBaseClassID )
    {
        return true;
    }

    return FOdysseyVectorObject::HasBaseClass( iBaseClassID );
}

std::list<FOdysseyVectorObject*>&
FOdysseyVectorCell::GetSelectedObjectList()
{
    return mSelectedObjectList;
}

FOdysseyVectorObject*
FOdysseyVectorCell::GetLastSelectedObject()
{
    return ( mSelectedObjectList.empty() == true ) ? nullptr : mSelectedObjectList.back();
}

void
FOdysseyVectorCell::ClearObjectSelection()
{
    for( FOdysseyVectorObject *obj : mSelectedObjectList )
    {
        obj->SetSelected ( false );
    }

    mSelectedObjectList.clear();
}

void
FOdysseyVectorCell::UnselectObject( FOdysseyVectorObject* iVecObj )
{
    iVecObj->SetSelected( false );

    mSelectedObjectList.remove( iVecObj );
}

void
FOdysseyVectorCell::SelectObject( FOdysseyVectorObject* iVecObj )
{
    if( std::find( mSelectedObjectList.begin(), mSelectedObjectList.end(), iVecObj ) == mSelectedObjectList.end() )
    {
        iVecObj->SetSelected( true );

        mSelectedObjectList.push_back( iVecObj );
    }
}

int32
FOdysseyVectorCell::GetIndex()
{
    return mCellInterface->GetIndex();
}

uint32
FOdysseyVectorCell::GetLength()
{
    return mCellInterface->GetLength();
}

uint32
FOdysseyVectorCell::GetFrame()
{
    return mCellInterface->GetFrame();
}

FOdysseyVectorGroupPaint*
FOdysseyVectorCell::GetScene()
{
    return static_cast<FOdysseyVectorGroupPaint*>(mChildrenList.front());
}

FOdysseyVectorLayer*
FOdysseyVectorCell::GetLayer()
{
    return ( ( mParent )
          && ( mParent->GetClass() == FOdysseyVectorLayer::StaticClass() ) ) ? static_cast<FOdysseyVectorLayer*>(mParent)
                                                                             : nullptr;
}

IOdysseyVectorCell*
FOdysseyVectorCell::GetCellInterface()
{
    return mCellInterface;
}

BLImage*
FOdysseyVectorCell::GetBLMask()
{
    return mBLMask;
}

void
FOdysseyVectorCell::SetBLMask( BLImage* iBLMask )
{
    mBLMask = iBLMask;
}

bool
FOdysseyVectorCell::PendingRedraw()
{
    return bPendingRedraw;
}

void
FOdysseyVectorCell::SetPendingRedraw( bool iPendingRedraw )
{
    bPendingRedraw = iPendingRedraw;

    if( bPendingRedraw == false )
    {
        mInvalidatedRect = ::ULIS::FRectD( 0.0f, 0.0f, 0.0f, 0.0f );
    }
}

void
FOdysseyVectorCell::Update( uint32 iUpdateFlags )
{
    FOdysseyVectorObject::Update( iUpdateFlags );

    if( GetLayer() )
    {
        GetLayer()->InvalidateCell( this );
    }
}

void
FOdysseyVectorCell::Invalidate( uint64 iInvalidationFlags )
{
    FOdysseyVectorObject::Invalidate( iInvalidationFlags );
}

void
FOdysseyVectorCell::InvalidateRect()
{
    if( mParent )
    {
        mInvalidatedRect = ::ULIS::FRectD( 0.0f, 0.0f, GetLayer()->GetWidth(), GetLayer()->GetHeight() );
    }
}

void
FOdysseyVectorCell::InvalidateRect( const ::ULIS::FRectD& iRect )
{
    ::ULIS::FRectD extendedRect = iRect;

    // extended the rectangle a bit in order to be sure to invalidate the borders
    extendedRect.x -= 2;
    extendedRect.y -= 2;
    extendedRect.w += 4;
    extendedRect.h += 4;

    if( mInvalidatedRect.Area() == 0.0f )
    {
        mInvalidatedRect = extendedRect;
    }
    else // combine the rectangles
    {
        mInvalidatedRect = mInvalidatedRect | extendedRect;
    }
}

void
FOdysseyVectorCell::SetSelectionSpace( FOdysseyVectorGroup* iSelectionSpace )
{
   mSelectionSpace = iSelectionSpace;
}

FOdysseyVectorGroup*
FOdysseyVectorCell::GetSelectionSpace()
{
    return mSelectionSpace;
}

// static
void
FOdysseyVectorCell::RecursivePick( FOdysseyVectorGroup* iSelectionSpace
                                 , FOdysseyVectorObject* iObj
                                 , std::vector<FOdysseyVectorObject*>& oSelectedObjectArray
                                 , const ::ULIS::FRectD& iRoi
                                 , uint32 iSelectionFlags )
{
    FOdysseyVectorObject* pickedObject = ( iObj != iSelectionSpace ) ? iObj->Pick( iSelectionSpace, iRoi, iSelectionFlags ) : nullptr;

    for( FOdysseyVectorObject* child : iObj->GetChildrenList() )
    {
        RecursivePick( iSelectionSpace, child, oSelectedObjectArray, iRoi, iSelectionFlags );
    }

    if( pickedObject )
    {
        oSelectedObjectArray.push_back( pickedObject );
    }
}

void
FOdysseyVectorCell::Pick( FOdysseyVectorGroupPaint* iScene
                          , const ::ULIS::FRectD& iRoi
                          , std::vector<FOdysseyVectorObject*>& oPickedObjectArray
                          , uint32 iSelectionFlags )
{
    RecursivePick( mSelectionSpace ? mSelectionSpace : iScene, iScene, oPickedObjectArray, iRoi, iSelectionFlags );
/*
    if( iSelectionFlags & FOdysseyVectorObject::PICK_MASK_BASED )
    {
        UseColorImage();
    }
*/
}

::ULIS::FRectD
FOdysseyVectorCell::GetInvalidatedRect()
{
    return mInvalidatedRect;
}

void
FOdysseyVectorCell::SelectAllInSelectionSpace()
{
    FOdysseyVectorGroupPaint* scene = GetScene();

    // TODO: set scene as the default selection space
    FOdysseyVectorGroup* selectionSpace = mSelectionSpace ? mSelectionSpace : scene;

    ClearObjectSelection();

    for( FOdysseyVectorObject *child : selectionSpace->GetChildrenList() )
    {
        SelectObject( child );
    }
}

void
FOdysseyVectorCell::GetSelectedVerticesFromFocusedObjects( std::vector<FOdysseyVectorVertex*>& oVertexArray )
{
    FOdysseyVectorGroupPaint* scene = GetScene();

    FOdysseyVectorObject::Traverse(
        scene
      , 0
      , [ scene
        , this
        , &oVertexArray ]( FOdysseyVectorObject* object, uint64 traversalFlags ) -> uint64
        {
            if( ObjectHasFocus( object, traversalFlags ) )
            {
                if( object->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
                {
                    FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);

                    for( FOdysseyVectorVertex* vertex : path->GetSelectedVertexList() )
                    {
                        oVertexArray.push_back( vertex );
                    }
                }

                return FOdysseyVectorObject::TRAVERSE_OBJECT_ACCEPTED;
            }

            return 0;
        } );
}

FOdysseyVectorCell::FRequestRedrawDelegate&
FOdysseyVectorCell::OnRequestRedrawDelegate()
{
    return mOnRequestRedrawDelegate;
}

void
FOdysseyVectorCell::SetScene( FOdysseyVectorGroupPaint* iScene )
{
    if( mScene )
    {
        RemoveChild( mScene );
    }

    AppendChild( iScene );

    ClearObjectSelection();

    //mEngine.ResetHUD();

    mScene = iScene;
    mScene->UpdateMatrix();
}

void
FOdysseyVectorCell::GetSelectedInbetweenerTagList( std::list<FOdysseyVectorTagInbetweener*>& oSelectedInbetweenerTagList )
{
    for( FOdysseyVectorObject* selectedObject : mSelectedObjectList )
    {
        FOdysseyVectorTag* tag = selectedObject->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() );

        if( tag )
        {
            FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);

            oSelectedInbetweenerTagList.push_back( inbetweenerTag );
        }
    }
}

void
FOdysseyVectorCell::GetFocusedInbetweenerTagList( std::list<FOdysseyVectorTagInbetweener*>& oFocusedInbetweenerTagList )
{
    FOdysseyVectorGroupPaint* scene = GetScene();

    FOdysseyVectorObject::Traverse
    ( scene
    , 0
    , [ this
      , scene
      , &oFocusedInbetweenerTagList ]( FOdysseyVectorObject* object
                                     , uint64 travesalFlags ) -> uint64
      {
          if( ObjectHasFocus( object, travesalFlags ) )
          {
              FOdysseyVectorTag* tag = object->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() );

              if( tag )
              {
                  FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);

                  oFocusedInbetweenerTagList.push_back( inbetweenerTag );

                  return FOdysseyVectorObject::TRAVERSE_OBJECT_ACCEPTED;
              }
          }

          return 0;
      } );
}

bool
FOdysseyVectorCell::ObjectHasFocus( FOdysseyVectorObject* iObject
                                  , uint64 iTraversalFlags )
{
    if( iObject->IsSelected() )
    {
        return true;
    }

    if( GetSelectedObjectList().size() == 0 )
    {
        return true;
    }

    if( iTraversalFlags & FOdysseyVectorObject::TRAVERSE_PARENT_HASFOCUS )
    {
        return true;
    }

    return false;
}

void
FOdysseyVectorCell::GetFocusedAncestorList( std::list<FOdysseyVectorObject*>& oObjectList )
{
    FOdysseyVectorGroupPaint* scene = GetScene();

    FOdysseyVectorObject::Traverse
    ( scene
    , 0
    , [ this
      , scene
      , &oObjectList ]( FOdysseyVectorObject* object, uint64 traversalFlags ) -> uint64
      {
          if( ObjectHasFocus( object, traversalFlags ) )
          {
              oObjectList.push_back( object );

              return FOdysseyVectorObject::TRAVERSE_OBJECT_IGNORE_CHILDREN;
          }

          return 0;
      } );
}

void
FOdysseyVectorCell::GetFocusedObjectList( std::list<FOdysseyVectorObject*>& oObjectList )
{
    FOdysseyVectorGroupPaint* scene = GetScene();

    FOdysseyVectorObject::Traverse
    ( scene
    , 0
    , [ this
      , scene
      , &oObjectList ]( FOdysseyVectorObject* object, uint64 traversalFlags ) -> uint64
      {
          if( ObjectHasFocus( object, traversalFlags ) )
          {
              oObjectList.push_back( object );

              return FOdysseyVectorObject::TRAVERSE_OBJECT_ACCEPTED;
          }

          return 0;
      } );
}

void
FOdysseyVectorCell::PickPathPoints( FOdysseyVectorGroupPaint* iScene
                                  , double iWorldX
                                  , double iWorldY
                                  , double iWorldRadius
                                  , uint64 iPickingFlags
                                  , bool iStopAtFirstSuccess
                                  , std::vector<FOdysseyVectorVertex*>& oPickedVertexArray
                                  , std::vector<FOdysseyVectorHandleSegment*>& oPickedHandleArray )
{
    FOdysseyVectorGroupPaint* scene = GetScene();

    FOdysseyVectorObject::Traverse
    ( scene
    , 0
    , [ this
      , scene
      , iScene
      , &iWorldX
      , &iWorldY
      , &iWorldRadius
      , &iPickingFlags
      , &iStopAtFirstSuccess
      , &oPickedVertexArray
      , &oPickedHandleArray ]( FOdysseyVectorObject* object, uint64 traversalFlags ) -> uint64
      {
          if( ObjectHasFocus( object, traversalFlags ) )
          {
              if( object->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
              {
                  FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);

                  if( path->PickPoint( iWorldX
                                     , iWorldY
                                     , iWorldRadius
                                     , oPickedVertexArray
                                     , oPickedHandleArray
                                     , iPickingFlags ) )
                  {
                      if( iStopAtFirstSuccess )
                      {
                          return FOdysseyVectorObject::TRAVERSE_STOP;
                      }
                  }
              }

              return FOdysseyVectorObject::TRAVERSE_OBJECT_ACCEPTED;
          }

          return 0;
      } );
}

// static
FOdysseyVectorVertex*
FOdysseyVectorCell::Stitch( FOdysseyVectorVertex* iVertexA
                          , FOdysseyVectorVertex* iVertexB
                          , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray
                          , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray
                          , bool iSmooth )
{
    if( ( iVertexA->GetSegmentCount() == 1 )
     && ( iVertexB->GetSegmentCount() == 1 )
     && ( iVertexA->GetOwnerAsPath() == iVertexB->GetOwnerAsPath() ) )
    {
        ::ULIS::FVec2D& vertexACoords = iVertexA->GetCoords();
        ::ULIS::FVec2D& vertexBCoords = iVertexB->GetCoords();
        ::ULIS::FVec2D averageCoords = ( vertexACoords + vertexBCoords ) * 0.5f;
        double vertexARadius = iVertexA->GetRadius();
        double vertexBRadius = iVertexB->GetRadius();
        double averageRadius = ( vertexARadius + vertexBRadius ) * 0.5f;
        FOdysseyVectorPath* path = iVertexA->GetOwnerAsPath();

        if( path->GetClass() == FOdysseyVectorPath::StaticClass() )
        {
            FOdysseyVectorPath* cubicPath = static_cast<FOdysseyVectorPath*>(path);
            FOdysseyVectorSegmentCubic* vertexBSegment = static_cast<FOdysseyVectorSegmentCubic*>(iVertexB->GetFirstSegment());
            FOdysseyVectorSegmentCubic* vertexASegment = static_cast<FOdysseyVectorSegmentCubic*>(iVertexA->GetFirstSegment());
            FOdysseyVectorVertex* prevVertex = static_cast<FOdysseyVectorVertex*>(vertexASegment->GetOtherVertex( iVertexA ));
            FOdysseyVectorVertex* nextVertex = static_cast<FOdysseyVectorVertex*>(vertexBSegment->GetOtherVertex( iVertexB ));
            FOdysseyVectorVertex* knotVertex = new FOdysseyVectorVertex( averageCoords.x, averageCoords.y, averageRadius );
            FOdysseyVectorSegmentCubic* newCubicSegment[2] = { new FOdysseyVectorSegmentCubic( cubicPath
                                                                                            ,  prevVertex
                                                                                            ,  knotVertex
                                                                                            ,  true ),
                                                               new FOdysseyVectorSegmentCubic( cubicPath
                                                                                            ,  knotVertex
                                                                                            ,  nextVertex
                                                                                            ,  true ) };
            ::ULIS::FVec2D vertexAToHandle = vertexASegment->GetHandle( iVertexA  )->GetCoords() - iVertexA->GetCoords();
            ::ULIS::FVec2D vertexBToHandle = vertexBSegment->GetHandle( iVertexB  )->GetCoords() - iVertexB->GetCoords();

            newCubicSegment[0]->GetHandle(0)->Set( vertexASegment->GetHandle(prevVertex)->GetCoords() );
            newCubicSegment[0]->GetHandle(1)->Set( knotVertex->GetCoords() + vertexAToHandle );
            newCubicSegment[1]->GetHandle(0)->Set( knotVertex->GetCoords() + vertexBToHandle );
            newCubicSegment[1]->GetHandle(1)->Set( vertexBSegment->GetHandle(nextVertex)->GetCoords() );

            path->RemoveSegment( vertexASegment );
            path->RemoveSegment( vertexBSegment );
            path->RemoveVertex( iVertexA );
            path->RemoveVertex( iVertexB );

            oRemovedSegmentArray.push_back( vertexASegment );
            oRemovedSegmentArray.push_back( vertexBSegment );

            path->AddVertex( knotVertex );
            path->AddSegment( newCubicSegment[0] );
            path->AddSegment( newCubicSegment[1] );

            oAddedSegmentArray.push_back( newCubicSegment[0] );
            oAddedSegmentArray.push_back( newCubicSegment[1] );
/*
            if( iSmooth )
            {
                FOdysseyVectorPath::SmoothSegments( knotVertex, false, true );
            }
*/
            path->InvalidateAllSegments();

            //mScene->Update( 0 );

            return knotVertex;
        }
    }

    return nullptr;
}

void
FOdysseyVectorCell::RemoveObjects( const std::list<FOdysseyVectorObject*>& iObjectList
                                 , std::vector<FOdysseyVectorObject*>& oRemovedObjectArray )
{
    for( FOdysseyVectorObject* vectorObject : iObjectList )
    {
        if( vectorObject != GetScene() )
        {
            if( vectorObject->GetParent()->RemoveChild( vectorObject ) == FOdysseyVectorObject::HIERARCHY_CHANGE_SUCCESS )
            {
                oRemovedObjectArray.push_back( vectorObject );
            }
        }
    }
}
