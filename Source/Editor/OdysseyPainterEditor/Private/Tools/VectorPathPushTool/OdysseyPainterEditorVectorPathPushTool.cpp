// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathPushTool/OdysseyPainterEditorVectorPathPushTool.h"
#include "Tools/VectorPathPushTool/OdysseyPainterEditorVectorPathPushToolHUD.h"
#include "Undo/OdysseyVectorUndoSegmentReshape.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorSource.h"
#include "OdysseyMediaVector.h"
#include "ISinglePropertyView.h"

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
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
}

uint64
UOdysseyPainterEditorVectorPathPushTool::LoadVector( FOdysseyVectorGroupPaint* iScene )
{
    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

     return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorPathPushTool::UnloadVector( FOdysseyVectorGroupPaint* iScene )
{
     return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
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

uint64
UOdysseyPainterEditorVectorPathPushTool::OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , const FKey& iKey )
{
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

        iEngine->Traverse
        ( iScene
        , 0
        , [ this
          , iScene
          , iEngine
          , &iPointInTexture
          , &pickedSegmentDistanceArray ]( FOdysseyVectorObject* object, uint64 traversalFlags ) -> uint64
          {
              if( iEngine->ObjectHasFocus( iScene, object, traversalFlags ) || ( RestrictToSelectedObjects == false ) )
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

                  return FOdysseyVectorEngine::TRAVERSE_OBJECT_ACCEPTED;
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
            FOdysseyVectorUndo *undo = new FOdysseyVectorUndoSegmentReshape( iScene, vertexArray );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
                
            TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();
    }

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;
}

uint64
UOdysseyPainterEditorVectorPathPushTool::OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                                           , const FOdysseyPoint& iPointInTexture )
{
    double diameter = Radius * 2.0f;
    ::ULIS::FRectI rect = { (int)iPointInTexture.x - (int)Radius
                          , (int)iPointInTexture.y - (int)Radius
                          , (int)diameter
                          , (int)diameter };

    mPathPushHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;
}

uint64
UOdysseyPainterEditorVectorPathPushTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                                          , const FOdysseyPoint& iPointInTexture )
{
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

        for( int i = 0; i < mPushedPointArray.size(); i++ )
        {
            FPushedPoint* pushedPoint = &mPushedPointArray[i];
            FOdysseyVectorPoint* point = pushedPoint->point;

            if( point->GetClass() == FOdysseyVectorHandleSegment::StaticClass() )
            {
                FOdysseyVectorHandleSegment* handle = static_cast<FOdysseyVectorHandleSegment*>(point);
                FOdysseyVectorSegment* segment = handle->GetOwner();
                FOdysseyVectorVertex* handleVertex = segment->GetVertex( handle->GetHandleID() );

                if( handleVertex->IsHandleAligned() )
                {
                    //FOdysseyVectorPath::SmoothSegments( vertex, true );
                    handleVertex->AlignHandles( handle );
                }
            }
        }

        // update vector scene and GUI widgets via delegates.
        iScene->Update( FOdysseyVectorObject::KEEPINVALIDATED );
    }

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;
}

uint64
UOdysseyPainterEditorVectorPathPushTool::OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                                        , const FOdysseyPoint& iPointInTexture
                                                        , const FKey& iKey )
{
    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();

        iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS ); // update invalidated objects

        vectorEngine->ResetHUD();
    }

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED;
}

TSharedRef<SWidget>
UOdysseyPainterEditorVectorPathPushTool::CreateTopTabWidget()
{
    FPropertyEditorModule& propertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FSinglePropertyParams defaultPropertyParams;
    const TSharedPtr<ISinglePropertyView> radiusPropertyView = propertyEditorModule.CreateSingleProperty(this, "Radius", defaultPropertyParams);
    TSharedPtr<class IPropertyHandle> radiusHandle = radiusPropertyView->GetPropertyHandle();

    return SNew(SUniformWrapPanel)
        .SlotPadding(FVector2D(3.f, 0.f))
        .EvenRowDistribution(true)
        .HAlign(HAlign_Left)
        + SUniformWrapPanel::Slot()
        [
            SNew( SOdysseyPainterEditorVectorEditionMode, GetEditor() )
        ]
        + SUniformWrapPanel::Slot()
        [
            CreatePropertyWidget(radiusHandle, radiusPropertyView).ToSharedRef()
        ];
}

FText
UOdysseyPainterEditorVectorPathPushTool::GetTooltip() const
{
    return LOCTEXT("vector-path-push-tool.tooltip", "Path Push Tool");
}

#undef LOCTEXT_NAMESPACE
