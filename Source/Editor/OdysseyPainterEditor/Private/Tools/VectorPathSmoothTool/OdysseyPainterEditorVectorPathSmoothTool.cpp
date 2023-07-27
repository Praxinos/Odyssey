// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathSmoothTool/OdysseyPainterEditorVectorPathSmoothTool.h"
#include "Tools/VectorPathSmoothTool/OdysseyPainterEditorVectorPathSmoothToolHUD.h"


#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorPathSmoothTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathSmoothTool::~UOdysseyPainterEditorVectorPathSmoothTool()
{
}

UOdysseyPainterEditorVectorPathSmoothTool::UOdysseyPainterEditorVectorPathSmoothTool()
    : mUndoSegmentReshape( nullptr )
    , SmoothingMode( ePathSmoothingMode::Round )
    , PickingRadius( 20.0f )
    , RestrictToSelection( false )
    , PreserveHandleLength( false )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PathSmoothTool64");

    mPathSmoothHUD = new FOdysseyPainterEditorVectorPathSmoothToolHUD( this );
}

void
UOdysseyPainterEditorVectorPathSmoothTool::Load()
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    UOdysseyPainterEditorVectorPathSmoothTool::LoadVector( vectorEngine, vectorScene );
}

void
UOdysseyPainterEditorVectorPathSmoothTool::Unload()
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    UOdysseyPainterEditorVectorPathSmoothTool::UnloadVector( vectorEngine, vectorScene );
}

bool
UOdysseyPainterEditorVectorPathSmoothTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
}

void
UOdysseyPainterEditorVectorPathSmoothTool::UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->RemoveHUD( mPathSmoothHUD );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorPathSmoothTool::LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->ClearHUD();
    iEngine->AddHUD( mPathSmoothHUD );

    iEngine->ResetHUD();

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorPathSmoothTool::OnKeyDown( const FKey& iKey )
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

    return false;
}

bool
UOdysseyPainterEditorVectorPathSmoothTool::OnKeyUp( const FKey& iKey )
{
    //FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();
    //FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

    // restore SmoothingMode value
    SmoothingMode = SmoothingModeAtKeyDown;

    //UOdysseyPainterEditorDefaultTool::OnKeyUpVector( vectorEngine, vectorScene, iKey );
    //iScene->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );

    return false;
}

bool
UOdysseyPainterEditorVectorPathSmoothTool::OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return false;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return false;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    return UOdysseyPainterEditorVectorPathSmoothTool::OnMouseDownVector( vectorEngine, vectorScene, iPointInTexture,iKey  );
}

bool
UOdysseyPainterEditorVectorPathSmoothTool::OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                                            , FOdysseyVectorScene* iScene
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

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("VectorPathSmoothTool","Vector Path Smooth Tool"));
    if( GUndo )
    {
        mUndoSegmentReshape = new FOdysseyVectorUndoSegmentReshape( iScene );

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>( mUndoSegmentReshape ) );
    }
    GEditor->EndTransaction();

    return true;
}

void
UOdysseyPainterEditorVectorPathSmoothTool::OnMouseHover( const FOdysseyPoint& iPointInTexture )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    UOdysseyPainterEditorVectorPathSmoothTool::OnMouseHoverVector( vectorEngine, vectorScene, iPointInTexture );
}

void
UOdysseyPainterEditorVectorPathSmoothTool::OnMouseHoverVector( FOdysseyVectorEngine* iEngine
                                                             , FOdysseyVectorScene* iScene
                                                             , const FOdysseyPoint& iPointInTexture )
{
    double diameter = PickingRadius * 2.0f;
    ::ULIS::FRectI rect = { (int)iPointInTexture.x - (int)PickingRadius
                          , (int)iPointInTexture.y - (int)PickingRadius
                          , (int)diameter
                          , (int)diameter };

    mPathSmoothHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorPathSmoothTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    UOdysseyPainterEditorVectorPathSmoothTool::OnMouseDragVector( vectorEngine, vectorScene, iPointInTexture );
}

void
UOdysseyPainterEditorVectorPathSmoothTool::OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                                            , FOdysseyVectorScene* iScene
                                                            , const FOdysseyPoint& iPointInTexture )
{
    std::vector<FOdysseyVectorPoint*> pickedPointArray;

    mPathSmoothHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );

    pickedPointArray = mPathSmoothHUD->GetPickedPointArray();

    for( int i = 0; i < pickedPointArray.size(); i++ )
    {
        if( pickedPointArray[i]->GetClass() == FOdysseyVectorVertex::StaticClass() )
        {
            FOdysseyVectorVertex* vertex = static_cast<FOdysseyVectorVertex*>(pickedPointArray[i]);

            if( vertex->GetSegmentCount() == 2 )
            {
                FOdysseyVectorSegment* segment[2] = { vertex->GetFirstSegment()
                                                    , vertex->GetLastSegment() };

                // record segment for undos first
                if( mUndoSegmentReshape->HasSegment( segment[0] ) == false )
                {
                    mUndoSegmentReshape->RecordSegment( segment[0] );
                }

                if( mUndoSegmentReshape->HasSegment( segment[1] ) == false )
                {
                    mUndoSegmentReshape->RecordSegment( segment[1] );
                }

                // then sharp or smooth
                if( SmoothingMode == ePathSmoothingMode::Sharp )
                {
                    FOdysseyVectorPath::SharpSegments( vertex, false, PreserveHandleLength );
                }

                if( SmoothingMode == ePathSmoothingMode::Round )
                {
                    FOdysseyVectorPath::SmoothSegments( vertex, false, PreserveHandleLength );
                }
            }
        }
    }

    iScene->Update( FOdysseyVectorObject::KEEPINVALIDATED );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorPathSmoothTool::OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return false;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return false;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    return UOdysseyPainterEditorVectorPathSmoothTool::OnMouseUpVector( vectorEngine, vectorScene, iPointInTexture, iKey );
}

bool
UOdysseyPainterEditorVectorPathSmoothTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                                          , FOdysseyVectorScene* iScene
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , const FKey& iKey )
{
    std::vector<FOdysseyVectorVertex*> vertexArray;
    std::vector<FOdysseyVectorSegment*> segmentArray;
    std::vector<FOdysseyVectorPoint*> pickedPointArray = mPathSmoothHUD->GetPickedPointArray();

    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS ); // update invalidated objects

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );

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
    return false;
}

void
UOdysseyPainterEditorVectorPathSmoothTool::Commit()
{

}

void
UOdysseyPainterEditorVectorPathSmoothTool::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
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
UOdysseyPainterEditorVectorPathSmoothTool::PropertyChanged( const FName& iPropertyName )
{
    //mPickingHUD.SetRadius( Radius );
}

#undef LOCTEXT_NAMESPACE
