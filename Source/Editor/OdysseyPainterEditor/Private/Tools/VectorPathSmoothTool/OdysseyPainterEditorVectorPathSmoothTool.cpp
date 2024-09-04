// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathSmoothTool/OdysseyPainterEditorVectorPathSmoothTool.h"
#include "Tools/VectorPathSmoothTool/OdysseyPainterEditorVectorPathSmoothToolHUD.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorSource.h"
#include "OdysseyMediaVector.h"
// Vector engine
#include "OdysseyVectorGroupPaint.h"
#include "Undo/OdysseyVectorUndoSegmentReshape.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathSmoothTool::~UOdysseyPainterEditorVectorPathSmoothTool()
{
}

UOdysseyPainterEditorVectorPathSmoothTool::UOdysseyPainterEditorVectorPathSmoothTool()
    : UOdysseyPainterEditorVectorBaseTool( new FOdysseyPainterEditorVectorPathSmoothToolHUD( this ), false )
    , mUndoSegmentReshape( nullptr )
    , SmoothingMode( ePathSmoothingMode::Round )
    , PickingRadius( 20.0f )
    , PreserveHandleLength( false )
    , RestrictToSelectedObjects( false )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PathSmooth64");

    mPathSmoothHUD = static_cast<FOdysseyPainterEditorVectorPathSmoothToolHUD*>( mBaseHUD );
}

bool
UOdysseyPainterEditorVectorPathSmoothTool::IsActivable() const
{
    uint64 HUDFlags = GetEditor()->GetVectorHUDFlags();

    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>()
          && ( HUDFlags & FOdysseyVectorHUD::HUD_MODE_OBJECT
            || HUDFlags & FOdysseyVectorHUD::HUD_MODE_VERTEX );
}

uint64
UOdysseyPainterEditorVectorPathSmoothTool::UnloadVector( FOdysseyVectorGroupPaint* iScene )
{
    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorPathSmoothTool::LoadVector( FOdysseyVectorGroupPaint* iScene )
{
    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorPathSmoothTool::OnKeyDownVector( FOdysseyVectorGroupPaint* iScene
                                                          , const FKey& iKey )
{
    // backup SmoothingMode value
    SmoothingModeAtKeyDown = SmoothingMode;

    if ( FSlateApplication::Get().GetModifierKeys().IsShiftDown() )
    {
        // flip the value
        SmoothingMode = ( SmoothingMode == ePathSmoothingMode::Round ) ? ePathSmoothingMode::Sharp 
                                                                       : ePathSmoothingMode::Round;
    }

    return UOdysseyPainterEditorVectorBaseTool::OnKeyDownVector( iScene, iKey );
}

uint64
UOdysseyPainterEditorVectorPathSmoothTool::OnKeyUpVector( FOdysseyVectorGroupPaint* iScene
                                                        , const FKey& iKey )
{
    // restore SmoothingMode value
    SmoothingMode = SmoothingModeAtKeyDown;

    return UOdysseyPainterEditorVectorBaseTool::OnKeyUpVector( iScene, iKey );
}

uint64
UOdysseyPainterEditorVectorPathSmoothTool::OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                                            , const FOdysseyPoint& iPointInTexture
                                                            , const FKey& iKey )
{
    uint64 retFlags = FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-path-smooth-tool.transaction.smooth-path","Vector Path Smooth Tool"));
        if( GUndo )
        {
            mUndoSegmentReshape = new FOdysseyVectorUndoSegmentReshape( iScene, retFlags );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>( mUndoSegmentReshape ) );
                
            TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();
    }

    return retFlags | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;
}

uint64
UOdysseyPainterEditorVectorPathSmoothTool::OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
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
UOdysseyPainterEditorVectorPathSmoothTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
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

                // record vertex and connected segments segment for undos first
                if( mUndoSegmentReshape->HasVertex( vertex ) == false )
                {
                    mUndoSegmentReshape->RecordVertex( vertex );
                }

                // then sharp or smooth
                if( SmoothingMode == ePathSmoothingMode::Sharp )
                {
                    FOdysseyVectorPath::SharpSegments( vertex, PreserveHandleLength );

                    vertex->SetHandleAligned( false );
                }

                if( SmoothingMode == ePathSmoothingMode::Round )
                {
                    FOdysseyVectorPath::SmoothSegments( vertex, PreserveHandleLength );

                    vertex->SetHandleAligned( true );
                }
            }
        }

        iScene->Update( FOdysseyVectorObject::UPDATE_INTERACTIVE );
    }

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;
}

uint64
UOdysseyPainterEditorVectorPathSmoothTool::OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
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

        iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS ); // update invalidated objects

        vectorEngine->ResetHUD();
    }

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

FText
UOdysseyPainterEditorVectorPathSmoothTool::GetTooltip() const
{
    return LOCTEXT("vector-path-smooth-tool.tooltip", "Path Smooth Tool");
}

#undef LOCTEXT_NAMESPACE
