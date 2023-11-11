// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathSmoothTool/OdysseyPainterEditorVectorPathSmoothTool.h"
#include "Tools/VectorPathSmoothTool/OdysseyPainterEditorVectorPathSmoothToolHUD.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyMediaVector.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorPathSmoothTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathSmoothTool::~UOdysseyPainterEditorVectorPathSmoothTool()
{
}

UOdysseyPainterEditorVectorPathSmoothTool::UOdysseyPainterEditorVectorPathSmoothTool()
    : UOdysseyPainterEditorVectorBaseTool( new FOdysseyPainterEditorVectorPathSmoothToolHUD( this ) )
    , mUndoSegmentReshape( nullptr )
    , SmoothingMode( ePathSmoothingMode::Round )
    , PickingRadius( 20.0f )
    , PreserveHandleLength( false )
    , RestrictToSelectedObjects( false )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PathSmoothTool64");

    mPathSmoothHUD = static_cast<FOdysseyPainterEditorVectorPathSmoothToolHUD*>( mBaseHUD );
}

bool
UOdysseyPainterEditorVectorPathSmoothTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
}

uint64
UOdysseyPainterEditorVectorPathSmoothTool::UnloadVector( FOdysseyVectorScene* iScene )
{
    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorPathSmoothTool::LoadVector( FOdysseyVectorScene* iScene )
{
    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorPathSmoothTool::OnKeyDownVector( FOdysseyVectorScene* iScene
                                                          , const FKey& iKey )
{
    //FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();
    //FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

    // backup SmoothingMode value
    SmoothingModeAtKeyDown = SmoothingMode;

    if ( FSlateApplication::Get().GetModifierKeys().IsShiftDown() )
    {
        // flip the value
        SmoothingMode = ( SmoothingMode == ePathSmoothingMode::Round ) ? ePathSmoothingMode::Sharp 
                                                                       : ePathSmoothingMode::Round;
    }

    //UOdysseyPainterEditorDefaultTool::OnKeyDownVector( vectorEngine, vectorScene, iKey );
    //iScene->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );

    return UOdysseyPainterEditorVectorBaseTool::OnKeyDownVector( iScene, iKey )
         | FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorPathSmoothTool::OnKeyUpVector( FOdysseyVectorScene* iScene
                                                        , const FKey& iKey )
{
    //FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();
    //FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

    // restore SmoothingMode value
    SmoothingMode = SmoothingModeAtKeyDown;

    //UOdysseyPainterEditorDefaultTool::OnKeyUpVector( vectorEngine, vectorScene, iKey );
    //iScene->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );

    return UOdysseyPainterEditorVectorBaseTool::OnKeyUpVector( iScene, iKey )
         | FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorPathSmoothTool::OnMouseDownVector( FOdysseyVectorScene* iScene
                                                            , const FOdysseyPoint& iPointInTexture
                                                            , const FKey& iKey )
{
    /*mPickedPointArray.clear();

    iEngine->PickPoints( iScene
                       , RestrictToSelection
                       , iPointInTexture.x
                       , iPointInTexture.y
                       , Radius
                       , mPickedPointArray
                       , FOdysseyVectorPath::PICK_POINT );*/

    //FOdysseyVectorPoint::ArrayToVertexArray( pickedPointArray, vertexArray );
    //FOdysseyVectorVertex::ArrayToSegmentArray( vertexArray, segmentArray );

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("VectorPathSmoothTool","Vector Path Smooth Tool"));
        if( GUndo )
        {
            mUndoSegmentReshape = new FOdysseyVectorUndoSegmentReshape( iScene );

            GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>( mUndoSegmentReshape ) );
        }
        GEditor->EndTransaction();
    }

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;
}

uint64
UOdysseyPainterEditorVectorPathSmoothTool::OnMouseHoverVector( FOdysseyVectorScene* iScene
                                                             , const FOdysseyPoint& iPointInTexture )
{
    double diameter = PickingRadius * 2.0f;
    ::ULIS::FRectI rect = { (int)iPointInTexture.x - (int)PickingRadius
                          , (int)iPointInTexture.y - (int)PickingRadius
                          , (int)diameter
                          , (int)diameter };

    mPathSmoothHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;
}

uint64
UOdysseyPainterEditorVectorPathSmoothTool::OnMouseDragVector( FOdysseyVectorScene* iScene
                                                            , const FOdysseyPoint& iPointInTexture )
{
    // Left mouse button clicked
    if( iPointInTexture.keysDown.Find( EKeys::LeftMouseButton ) != INDEX_NONE )
    {
        std::vector<FOdysseyVectorPoint*> pickedPointArray;

        mPathSmoothHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );

        pickedPointArray = mPathSmoothHUD->GetPickedPointArray();

        for( int i = 0; i < pickedPointArray.size(); i++ )
        {
            if( pickedPointArray[i]->GetClass() == FOdysseyVectorVertex::StaticClass() )
            {
                FOdysseyVectorVertex* vertex = static_cast<FOdysseyVectorVertex*>(pickedPointArray[i]);
                FOdysseyVectorSegment* segment[2] = { vertex->GetFirstSegment()
                                                    , vertex->GetLastSegment() };

                // record segment for undos first
                if( segment[0] && mUndoSegmentReshape->HasSegment( segment[0] ) == false )
                {
                    mUndoSegmentReshape->RecordSegment( segment[0] );
                }

                if( segment[1] && mUndoSegmentReshape->HasSegment( segment[1] ) == false )
                {
                    mUndoSegmentReshape->RecordSegment( segment[1] );
                }

                // then sharp or smooth
                if( SmoothingMode == ePathSmoothingMode::Sharp )
                {
                    FOdysseyVectorPath::SharpSegments( vertex, PreserveHandleLength );
                }

                if( SmoothingMode == ePathSmoothingMode::Round )
                {
                    FOdysseyVectorPath::SmoothSegments( vertex, PreserveHandleLength );
                }
            }
        }

        iScene->Update( FOdysseyVectorObject::KEEPINVALIDATED );
    }

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;
}

uint64
UOdysseyPainterEditorVectorPathSmoothTool::OnMouseUpVector( FOdysseyVectorScene* iScene
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , const FKey& iKey )
{
    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        std::vector<FOdysseyVectorVertex*> vertexArray;
        std::vector<FOdysseyVectorSegment*> segmentArray;
        std::vector<FOdysseyVectorPoint*> pickedPointArray = mPathSmoothHUD->GetPickedPointArray();
        FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();

        iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS ); // update invalidated objects

        vectorEngine->ResetHUD();
    }

/*
    FOdysseyVectorPoint::ArrayToVertexArray( pickedPointArray, vertexArray );
    FOdysseyVectorVertex::ArrayToSegmentArray( vertexArray, segmentArray );

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("VectorPathSmoothTool","Vector Path Smooth Tool"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoSegmentReshape( iScene, segmentArray );

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();
*/
    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

#undef LOCTEXT_NAMESPACE
