// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathPushTool/OdysseyPainterEditorVectorPathPushTool.h"
#include "Tools/VectorPathPushTool/OdysseyPainterEditorVectorPathPushToolHUD.h"
#include "Undo/OdysseyVectorUndoSegmentReshape.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorSource.h"
#include "OdysseyMediaVector.h"
#include "ISinglePropertyView.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorRoot.h"
#include "OdysseyVectorSharedEnv.h"
#include "SOdysseySinglePropertyView.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathPushTool::~UOdysseyPainterEditorVectorPathPushTool()
{
}

UOdysseyPainterEditorVectorPathPushTool::UOdysseyPainterEditorVectorPathPushTool()
    : UOdysseyPainterEditorVectorBaseTool( new FOdysseyPainterEditorVectorPathPushToolHUD( this ),false )
    , Radius( 20.0f )
    , RestrictToSelectedObjects( false )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PathPush64");

    mPathPushHUD = static_cast<FOdysseyPainterEditorVectorPathPushToolHUD*>( mBaseHUD );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

bool
UOdysseyPainterEditorVectorPathPushTool::IsActivable() const
{
    uint64 HUDFlags = GetEditor()->GetVectorHUDFlags();

    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>()
          && ( HUDFlags & FOdysseyVectorHUD::HUD_MODE_OBJECT
            || HUDFlags & FOdysseyVectorHUD::HUD_MODE_VERTEX );
}

uint64
UOdysseyPainterEditorVectorPathPushTool::LoadVector( FOdysseyVectorGroupPaint* iScene )
{
    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    // side note: updating via root will request a redraw as well
    iScene->GetSharedEnv()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

     return 0;
}

uint64
UOdysseyPainterEditorVectorPathPushTool::UnloadVector( FOdysseyVectorGroupPaint* iScene )
{
    // side note: updating via root will request a redraw as well
    iScene->GetSharedEnv()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

     return 0;
}

FPushedPoint*
UOdysseyPainterEditorVectorPathPushTool::GetPushedPoint( FOdysseyVectorPoint* iPoint )
{
    for( int i = 0; i < mPushedPointArray.size(); i++ )
    {
        if( mPushedPointArray[i].point == iPoint )
        {
             return &mPushedPointArray[i];
        }
    }

    return nullptr;
}

bool
UOdysseyPainterEditorVectorPathPushTool::OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , const FKey& iKey
                                                          , uint64& oSignalFlags )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    uint64 notificationFlags = 0;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        FOdysseyVectorEngine* iEngine = iScene->GetEngine();
        std::vector<double> pickedSegmentDistanceArray;
        std::vector<FOdysseyVectorVertex*> vertexArray; // for undo/redo

        // this callback crashes if I dont reserve memory. I have no idea why. To troubleshoot later.
        vertexArray.reserve( 100 );
        mSegmentArray.reserve( 100 );
        pickedSegmentDistanceArray.reserve( 100 ); // unused for now

        mSegmentArray.clear();
        mPushedPointArray.clear();

        FOdysseyVectorObject::Traverse
        ( iScene
        , 0
        , [ this
          , iScene
          , &iPointInTexture
          , &pickedSegmentDistanceArray ]( FOdysseyVectorObject* object, uint64 traversalFlags ) -> uint64
          {
              if( iScene->GetRoot()->ObjectHasFocus( object, traversalFlags ) || ( RestrictToSelectedObjects == false ) )
              {
                  if( object->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
                  {
                      FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);

                      path->PickSegments( iPointInTexture.x
                                        , iPointInTexture.y
                                        , Radius
                                        , mSegmentArray
                                        , &pickedSegmentDistanceArray );
                  }

                  return FOdysseyVectorObject::TRAVERSE_OBJECT_ACCEPTED;
              }

              return 0;
          } );

        mMaxDistance = 0.0f;

        // First step: find farthest distance to mouse pointer
        for( int i = 0; i < mSegmentArray.size(); i++ )
        {
            FOdysseyVectorSegment* segment = mSegmentArray[i];
            FOdysseyVectorPath* path = segment->GetOwnerAsPath();
            BLMatrix2D& pathWorldMatrix = path->GetWorldMatrix();

            if( segment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
            {
                FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);
                FOdysseyVectorHandleSegment* handle0 = cubicSegment->GetHandle(0);
                FOdysseyVectorHandleSegment* handle1 = cubicSegment->GetHandle(1);
                FOdysseyVectorVertex* vertex0 = cubicSegment->GetVertex(0);
                FOdysseyVectorVertex* vertex1 = cubicSegment->GetVertex(1);
                ::ULIS::FVec2D& ctrlPoint0 = handle0->GetCoords();
                ::ULIS::FVec2D& ctrlPoint1 = handle1->GetCoords();
                ::ULIS::FVec2D& point0 = vertex0->GetCoords();
                ::ULIS::FVec2D& point1 = vertex1->GetCoords();
                BLPoint worldCtrlPoint0 = pathWorldMatrix.mapPoint( ctrlPoint0.x, ctrlPoint0.y );
                BLPoint worldCtrlPoint1 = pathWorldMatrix.mapPoint( ctrlPoint1.x, ctrlPoint1.y );
                BLPoint worldPoint0 = pathWorldMatrix.mapPoint( point0.x, point0.y );
                BLPoint worldPoint1 = pathWorldMatrix.mapPoint( point1.x, point1.y );
                double pointDistance0 = ::ULIS::FVec2D( iPointInTexture.x - worldPoint0.x
                                                      , iPointInTexture.y - worldPoint0.y ).Distance();
                double pointDistance1 = ::ULIS::FVec2D( iPointInTexture.x - worldPoint1.x
                                                      , iPointInTexture.y - worldPoint1.y ).Distance();
                double ctrlPointDistance0 = ::ULIS::FVec2D( iPointInTexture.x - worldCtrlPoint0.x
                                                          , iPointInTexture.y - worldCtrlPoint0.y ).Distance();
                double ctrlPointDistance1 = ::ULIS::FVec2D( iPointInTexture.x - worldCtrlPoint1.x
                                                          , iPointInTexture.y - worldCtrlPoint1.y ).Distance();

                if( pointDistance0 > mMaxDistance )
                {
                    mMaxDistance = pointDistance0;
                }

                if( pointDistance1 > mMaxDistance )
                {
                    mMaxDistance = pointDistance1;
                }

                if( ctrlPointDistance0 > mMaxDistance )
                {
                    mMaxDistance = ctrlPointDistance0;
                }

                if( ctrlPointDistance1 > mMaxDistance )
                {
                    mMaxDistance = ctrlPointDistance1;
                }

                mPushedPointArray.emplace_back( handle0, ctrlPointDistance0, nullptr );
                mPushedPointArray.emplace_back( handle1, ctrlPointDistance1, nullptr );

                if( GetPushedPoint( vertex0 ) == nullptr )
                {
                    mPushedPointArray.emplace_back( vertex0, pointDistance0, segment );
                    vertexArray.push_back( vertex0 );
                }

                if( GetPushedPoint( vertex1 ) == nullptr )
                {
                    mPushedPointArray.emplace_back( vertex1, pointDistance1, segment );
                    vertexArray.push_back( vertex1 );
                }
            }
        }

        // static call
        //FOdysseyVectorVertex::ArrayToSegmentArray( vertexArray, savedSegmentArray );

        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-path-push-tool.transaction.push-path","Vector Path Push Tool"));
        if( GUndo )
        {
            FOdysseyVectorUndo *undo = new FOdysseyVectorUndoSegmentReshape( iScene, vertexArray, notificationFlags );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();
    }

    oSignalFlags = notificationFlags;

    return true;
}

void
UOdysseyPainterEditorVectorPathPushTool::OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                                           , const FOdysseyPoint& iPointInTexture
                                                           , uint64& oSignalFlags )
{
    double diameter = Radius * 2.0f;
    ::ULIS::FRectI rect = { (int)iPointInTexture.x - (int)Radius
                          , (int)iPointInTexture.y - (int)Radius
                          , (int)diameter
                          , (int)diameter };
    uint64 notificationFlags = 0;

    mPathPushHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );

    // force invalidation for redrawal
    iScene->Invalidate(0);
    // Update and request for redraw
    iScene->GetSharedEnv()->Update( FOdysseyVectorObject::UPDATE_INTERACTIVE );

    oSignalFlags = notificationFlags;
}

void
UOdysseyPainterEditorVectorPathPushTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , uint64& oSignalFlags )
{
    uint64 notificationFlags = 0;

    // Left mouse button clicked
    if( iPointInTexture.keysDown.Find( EKeys::LeftMouseButton ) != INDEX_NONE )
    {
        mPathPushHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );

        for( int i = 0; i < mPushedPointArray.size(); i++ )
        {
            double ratio = 1.0f - ( mPushedPointArray[i].distance / mMaxDistance );
            FOdysseyVectorPoint* point = mPushedPointArray[i].point;
            FOdysseyVectorPath* path;

            if( point->GetClass() == FOdysseyVectorHandleSegment::StaticClass() )
            {
                FOdysseyVectorHandleSegment* handleSegment = static_cast<FOdysseyVectorHandleSegment*>(point);

                path = handleSegment->GetOwner()->GetOwnerAsPath();
            }
            else
            {
                FOdysseyVectorVertex* vertex = static_cast<FOdysseyVectorVertex*>(point);

                path = vertex->GetOwnerAsPath();
            }

            BLPoint delta = path->GetInverseWorldMatrix().mapVector( iPointInTexture.deltaPosition.X
                                                                   , iPointInTexture.deltaPosition.Y );

            point->SetX( point->GetX() + ( delta.x * ratio ) );
            point->SetY( point->GetY() + ( delta.y * ratio ) );
        }

        // update vector scene. It will request a redraw as well
        iScene->GetSharedEnv()->Update( FOdysseyVectorObject::UPDATE_INTERACTIVE
                                      | FOdysseyVectorObject::UPDATE_NOINBETWEENING );
    }

    oSignalFlags = notificationFlags;
}

bool
UOdysseyPainterEditorVectorPathPushTool::OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                                        , const FOdysseyPoint& iPointInTexture
                                                        , const FKey& iKey
                                                        , uint64& oSignalFlags )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_HUD;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        //iScene->GetSharedEnv()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS ); // update invalidated objects
    }

    oSignalFlags = notificationFlags;

    return true;
}

void
UOdysseyPainterEditorVectorPathPushTool::ExtendToolbar( FToolBarBuilder& iBuilder )
{
    Super::ExtendToolbar(iBuilder);

    iBuilder.BeginSection( NAME_None );

    iBuilder.AddWidget(
        SNew(SBox)
        .Padding(10.f, 0.f, 10.f, 0.f)
        [
            SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorPathPushTool, Radius ), FSinglePropertyParams())
            .InnerPadding(10.f)
            .ValueWidthOverride(100.f)
        ]
    );

    iBuilder.EndSection();
}

FText
UOdysseyPainterEditorVectorPathPushTool::GetTooltip() const
{
    return LOCTEXT("vector-path-push-tool.tooltip", "Path Push Tool");
}

#undef LOCTEXT_NAMESPACE
