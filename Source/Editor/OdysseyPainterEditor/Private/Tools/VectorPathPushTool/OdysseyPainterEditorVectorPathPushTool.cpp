// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathPushTool/OdysseyPainterEditorVectorPathPushTool.h"
#include "Tools/VectorPathPushTool/OdysseyPainterEditorVectorPathPushToolHUD.h"
#include "Undo/OdysseyVectorUndoSegmentReshape.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorPathPushTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathPushTool::~UOdysseyPainterEditorVectorPathPushTool()
{
}

UOdysseyPainterEditorVectorPathPushTool::UOdysseyPainterEditorVectorPathPushTool()
    : Radius( 20.0f )
    , PreserveSmoothness( false )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PathPushTool64");

    mPathPushHUD = new FOdysseyPainterEditorVectorPathPushToolHUD( this );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorPathPushTool::Load()
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    UOdysseyPainterEditorVectorPathPushTool::LoadVector( vectorEngine, vectorScene );
}

void
UOdysseyPainterEditorVectorPathPushTool::Unload()
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    UOdysseyPainterEditorVectorPathPushTool::UnloadVector( vectorEngine, vectorScene );
}

bool
UOdysseyPainterEditorVectorPathPushTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
}

void
UOdysseyPainterEditorVectorPathPushTool::UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->RemoveHUD( mPathPushHUD );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorPathPushTool::LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->ClearHUD();
    iEngine->AddHUD( mPathPushHUD );

    mPathPushHUD->Reset( iScene );

    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
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
UOdysseyPainterEditorVectorPathPushTool::OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return false;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return false;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    return UOdysseyPainterEditorVectorPathPushTool::OnMouseDownVector( vectorEngine, vectorScene, iPointInTexture,iKey  );
}

bool
UOdysseyPainterEditorVectorPathPushTool::OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                                          , FOdysseyVectorScene* iScene
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , const FKey& iKey )
{
    std::vector<double> pickedSegmentDistanceArray;
    std::vector<FOdysseyVectorPoint*> pointArray; // for undo/redo

    // this callback crashes if I dont reserve memory. I have no idea why. To troubleshoot later.
    pointArray.reserve( 100 );
    mSegmentArray.reserve( 100 );
    pickedSegmentDistanceArray.reserve( 100 ); // unused for now

    mSegmentArray.clear();
    mPushedPointArray.clear();

    iEngine->PickSegments( iScene
                         , RestrictToSelection
                         , iPointInTexture.x
                         , iPointInTexture.y
                         , Radius
                         , mSegmentArray
                         , &pickedSegmentDistanceArray );

    // First step: find farthest distance to mouse pointer
    for( int i = 0; i < mSegmentArray.size(); i++ )
    {
        FOdysseyVectorSegment* segment = mSegmentArray[i];
        FOdysseyVectorPath* path = segment->GetPath();
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
            double maxDistance = 0.0f;

            if( pointDistance0 > maxDistance )
            {
                maxDistance = pointDistance0;
            }

            if( pointDistance1 > maxDistance )
            {
                maxDistance = pointDistance1;
            }

            if( ctrlPointDistance0 > maxDistance )
            {
                maxDistance = ctrlPointDistance0;
            }

            if( ctrlPointDistance1 > maxDistance )
            {
                maxDistance = ctrlPointDistance1;
            }

            mPushedPointArray.emplace_back( handle0, ctrlPointDistance0 / maxDistance, false, nullptr );
            mPushedPointArray.emplace_back( handle1, ctrlPointDistance1 / maxDistance, false, nullptr );

            pointArray.push_back( handle0 );
            pointArray.push_back( handle1 );

            if( GetPushedPoint( vertex0 ) == nullptr )
            {
                double ratio = pointDistance0 / maxDistance;

                mPushedPointArray.emplace_back( vertex0, ratio, vertex0->IsSmooth(), segment );
                pointArray.push_back( vertex0 );
            }

            if( GetPushedPoint( vertex1 ) == nullptr )
            {
                double ratio = pointDistance1 / maxDistance;

                mPushedPointArray.emplace_back( vertex1, ratio, vertex1->IsSmooth(), segment );
                pointArray.push_back( vertex1 );
            }

        }
    }

    // second step : we also have to remember the position of neighbour handles, i.e handles taht are 
    // not per-se part of the picking but that will be influenced by the smoothing options.
    for( int i = 0; i < mSegmentArray.size(); i++ )
    {
        FOdysseyVectorSegment* segment = mSegmentArray[i];
        FOdysseyVectorVertex* vertex0 = segment->GetVertex(0);
        FOdysseyVectorVertex* vertex1 = segment->GetVertex(1);
        FOdysseyVectorHandleSegment* neighbourHandle0 = vertex0->GetOtherSegmentHandle( segment );
        FOdysseyVectorHandleSegment* neighbourHandle1 = vertex1->GetOtherSegmentHandle( segment );

        if( neighbourHandle0 && ( GetPushedPoint( neighbourHandle0 ) == nullptr ) )
        {
            pointArray.push_back( neighbourHandle0 );
        }

        if( neighbourHandle1 && ( GetPushedPoint( neighbourHandle1 ) == nullptr ) )
        {
            pointArray.push_back( neighbourHandle1 );
        }
    }

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("VectorPathPushTool","Vector Path Push Tool"));
    if( GUndo )
    {
        FOdysseyVectorUndo *undo = new FOdysseyVectorUndoPointPosition( iScene, pointArray );

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_INTERACTIVE );

    return true;
}

void
UOdysseyPainterEditorVectorPathPushTool::OnMouseHover( const FOdysseyPoint& iPointInTexture )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    UOdysseyPainterEditorVectorPathPushTool::OnMouseHoverVector( vectorEngine, vectorScene, iPointInTexture );
}

void
UOdysseyPainterEditorVectorPathPushTool::OnMouseHoverVector( FOdysseyVectorEngine* iEngine
                                                           , FOdysseyVectorScene* iScene
                                                           , const FOdysseyPoint& iPointInTexture )
{
    double diameter = Radius * 2.0f;
    ::ULIS::FRectI rect = { (int)iPointInTexture.x - (int)Radius
                          , (int)iPointInTexture.y - (int)Radius
                          , (int)diameter
                          , (int)diameter };

    mPathPushHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_INTERACTIVE );
}

void
UOdysseyPainterEditorVectorPathPushTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    UOdysseyPainterEditorVectorPathPushTool::OnMouseDragVector( vectorEngine, vectorScene, iPointInTexture );
}

void
UOdysseyPainterEditorVectorPathPushTool::OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                                          , FOdysseyVectorScene* iScene
                                                          , const FOdysseyPoint& iPointInTexture )
{
    mPathPushHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );

    for( int i = 0; i < mPushedPointArray.size(); i++ )
    {
        double ratio = 1.0f - mPushedPointArray[i].ratio;
        FOdysseyVectorPoint* point = mPushedPointArray[i].point;
        FOdysseyVectorPath* path;

        if( point->GetClass() == FOdysseyVectorHandleSegment::StaticClass() )
        {
            FOdysseyVectorHandleSegment* handleSegment = static_cast<FOdysseyVectorHandleSegment*>(point);

            path = handleSegment->GetOwner()->GetPath();
        }
        else
        {
            FOdysseyVectorVertex* vertex = static_cast<FOdysseyVectorVertex*>(point);

            path = vertex->GetPath();
        }

        BLPoint delta = path->GetInverseWorldMatrix().mapVector( iPointInTexture.deltaPosition.X
                                                               , iPointInTexture.deltaPosition.Y );

        point->SetX( point->GetX() + ( delta.x * ratio ) );
        point->SetY( point->GetY() + ( delta.y * ratio ) );
    }

    if( PreserveSmoothness )
    {
        for( int i = 0; i < mPushedPointArray.size(); i++ )
        {
            FPushedPoint* pushedPoint = &mPushedPointArray[i];
            FOdysseyVectorPoint* point = pushedPoint->point;

            if( pushedPoint->isSmooth )
            {
                if( point->GetClass() == FOdysseyVectorVertex::StaticClass() )
                {
                    FOdysseyVectorVertex* vertex = static_cast<FOdysseyVectorVertex*>(point);
                    ::ULIS::FVec2D smoothingGuideSegmentVector = pushedPoint->smoothingGuideSegment->GetHandleVector(vertex, true);
                    FOdysseyVectorSegment* otherSegment = vertex->GetOtherSegment( pushedPoint->smoothingGuideSegment );
                    FOdysseyVectorHandleSegment* otherSegmentHandle = otherSegment->GetHandle(vertex);
                    ::ULIS::FVec2D otherSegmentHandleVector = otherSegment->GetHandleVector( vertex, false );
                    double length = otherSegmentHandleVector.Distance();

                    otherSegmentHandle->Set( vertex->GetX() - ( smoothingGuideSegmentVector.x * length )
                                           , vertex->GetY() - ( smoothingGuideSegmentVector.y * length ) );
                }
            }
        }
    }

    // update vector scene and GUI widgets via delegates.
    iScene->Update( FOdysseyVectorObject::KEEPINVALIDATED );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_INTERACTIVE );
}

bool
UOdysseyPainterEditorVectorPathPushTool::OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return false;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return false;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    return UOdysseyPainterEditorVectorPathPushTool::OnMouseUpVector( vectorEngine, vectorScene, iPointInTexture, iKey );
}

bool
UOdysseyPainterEditorVectorPathPushTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                                        , FOdysseyVectorScene* iScene
                                                        , const FOdysseyPoint& iPointInTexture
                                                        , const FKey& iKey )
{
    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS ); // update invalidated objects

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );

    return false;
}

void
UOdysseyPainterEditorVectorPathPushTool::Commit()
{

}

void
UOdysseyPainterEditorVectorPathPushTool::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;
    
    PropertyChanged( PropertyChangedEvent.GetPropertyName() );
    
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

    vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorPathPushTool::PropertyChanged( const FName& iPropertyName )
{

}

#undef LOCTEXT_NAMESPACE
