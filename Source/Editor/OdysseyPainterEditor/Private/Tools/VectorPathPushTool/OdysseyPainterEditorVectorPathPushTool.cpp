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
    , PreserveSmoothness( true )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PathPushTool64");

    mPathPushHUD = new FOdysseyPainterEditorVectorPathPushToolHUD( this );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorPathPushTool::UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->RemoveHUD( mPathPushHUD );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorPathPushTool::LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->ClearHUD();
    iEngine->AddHUD( mPathPushHUD );

    mPathPushHUD->Reset( iScene );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorPathPushTool::HasVertex( FOdysseyVectorPoint* iPoint )
{
    for( int i = 0; i < mPushedPointArray.size(); i++ )
    {
        if( mPushedPointArray[i].point == iPoint )
        {
             return true;
        }
    }

    return false;
}

bool
UOdysseyPainterEditorVectorPathPushTool::OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                                          , FOdysseyVectorScene* iScene
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , const FKey& iKey )
{
    std::vector<double> pickedSegmentDistanceArray;
    std::vector<FOdysseyVectorVertex*> vertexArray;

    // this callback crashes if I dont reserve memory. I have no idea why. To troubleshoot later.
    vertexArray.reserve( 500 );
    mSegmentArray.reserve( 500 );
    pickedSegmentDistanceArray.reserve( 500 ); // unused for now

    mSegmentArray.clear();
    mPushedPointArray.clear();

    iEngine->PickSegments( iScene
                         , RestrictToSelection
                         , iPointInTexture.x
                         , iPointInTexture.y
                         , Radius
                         , mSegmentArray
                         , &pickedSegmentDistanceArray );

    for( int i = 0; i < mSegmentArray.size(); i++ )
    {
        FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>( mSegmentArray[i] );

        if( cubicSegment )
        {
            FOdysseyVectorPathCubic* cubicPath = static_cast<FOdysseyVectorPathCubic*>(cubicSegment->GetPath());
            BLPoint localPoint = cubicPath->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
            ::ULIS::FVec2D& ctrlPoint0 = cubicSegment->GetHandle(0)->GetCoords();
            ::ULIS::FVec2D& ctrlPoint1 = cubicSegment->GetHandle(1)->GetCoords();
            ::ULIS::FVec2D& point0 = cubicSegment->GetVertex(0)->GetCoords();
            ::ULIS::FVec2D& point1 = cubicSegment->GetVertex(1)->GetCoords();
            ::ULIS::FVec2D cp0Vec = { localPoint.x - ctrlPoint0.x, localPoint.y - ctrlPoint0.y };
            ::ULIS::FVec2D cp1Vec = { localPoint.x - ctrlPoint1.x, localPoint.y - ctrlPoint1.y };
            FOdysseyVectorVertex* vertex0 = cubicSegment->GetVertex(0);
            FOdysseyVectorVertex* vertex1 = cubicSegment->GetVertex(1);

            mPushedPointArray.push_back( FPushedPoint( cubicSegment->GetHandle(0), Radius / cp0Vec.Distance(), false ) );
            mPushedPointArray.push_back( FPushedPoint( cubicSegment->GetHandle(1), Radius / cp1Vec.Distance(), false ) );

            if( HasVertex( vertex0 ) == false )
            {
                ::ULIS::FVec2D p0Vec = { localPoint.x - point0.x, localPoint.y - point0.y };

                mPushedPointArray.push_back( FPushedPoint( cubicSegment->GetPoint(0), Radius / p0Vec.Distance(), vertex0->IsSmooth() ) );
                vertexArray.push_back( vertex0 );
            }

            if( HasVertex( vertex1 ) == false )
            {
                ::ULIS::FVec2D p1Vec = { localPoint.x - point1.x, localPoint.y - point1.y };

                mPushedPointArray.push_back( FPushedPoint( cubicSegment->GetPoint(1), Radius / p1Vec.Distance(), vertex1->IsSmooth() ) );
                vertexArray.push_back( vertex1 );
            }
        }
    }

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("VectorPathPushTool","Vector Path Push Tool"));
    if( GUndo )
    {
        // due to the "preserve smoothing" option, the segments that are altered could be more numerous than the one picked.
        // we have to include them from the picked vertices.
        std::vector<FOdysseyVectorSegment*> savedSegmentArray;
        FOdysseyVectorVertex::ArrayToSegmentArray( vertexArray, savedSegmentArray );
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoSegmentReshape( iScene, savedSegmentArray );

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );

    return true;
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

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorPathPushTool::OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                                          , FOdysseyVectorScene* iScene
                                                          , const FOdysseyPoint& iPointInTexture )
{
    mPathPushHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );

    for( int i = 0; i < mPushedPointArray.size(); i++ )
    {
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

        point->SetX( point->GetX() + ( delta.x * mPushedPointArray[i].ratio ) );
        point->SetY( point->GetY() + ( delta.y * mPushedPointArray[i].ratio ) );
    }

    for( int i = 0; i < mPushedPointArray.size(); i++ )
    {
        FOdysseyVectorPoint* point = mPushedPointArray[i].point;

        if( point->GetClass() == FOdysseyVectorVertex::StaticClass() )
        {
            FOdysseyVectorVertex* vertex = static_cast<FOdysseyVectorVertex*>(point);

            if( mPushedPointArray[i].isSmooth && PreserveSmoothness )
            {
                if( vertex )
                {
                    ::ULIS::FVec2D perpendicularVector = vertex->GetAverageVectorOnSegmentHandle(true);

                    // if the perpendicular vector is 0, use one of the segment's vector as a reference.
                    if( perpendicularVector.DistanceSquared() == 0.0f && vertex->GetFirstSegment() )
                    {
                        perpendicularVector = vertex->GetVectorOnSegment( vertex->GetFirstSegment(), true );
                        perpendicularVector = ::ULIS::FVec2D( perpendicularVector.y, -perpendicularVector.x );
                    }

                    FOdysseyVectorPathCubic::SmoothSegments(vertex,perpendicularVector,false,true);
                }
            }
        }
    }

    for( int i = 0; i < mSegmentArray.size(); i++ )
    {
        mSegmentArray[i]->Invalidate();
    }

    // update vector scene and GUI widgets via delegates.
    iScene->Update( FOdysseyVectorObject::FREQUENTUPDATES | FOdysseyVectorObject::KEEPINVALIDATED );
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorPathPushTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                                        , FOdysseyVectorScene* iScene
                                                        , const FOdysseyPoint& iPointInTexture
                                                        , const FKey& iKey )
{
    iScene->Update( 0 ); // update invalidated objects
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW | FOdysseyVectorScene::SIGNAL_OBJECT_MODIFIED );

    return false;
}

void
UOdysseyPainterEditorVectorPathPushTool::Commit()
{

}

void
UOdysseyPainterEditorVectorPathPushTool::PropertyChanged( const FName& iPropertyName )
{

}

#undef LOCTEXT_NAMESPACE
