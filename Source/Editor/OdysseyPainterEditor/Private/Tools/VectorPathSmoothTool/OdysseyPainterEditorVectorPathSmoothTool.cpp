// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathSmoothTool/OdysseyPainterEditorVectorPathSmoothTool.h"
#include "Tools/VectorPathSmoothTool/OdysseyPainterEditorVectorPathSmoothToolHUD.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorSource.h"
#include "OdysseyMediaVector.h"

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
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
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
    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

bool
UOdysseyPainterEditorVectorPathSmoothTool::OnKeyDownVector( FOdysseyVectorGroupPaint* iScene
                                                          , const FKey& iKey
                                                          , uint64& oSignalFlags )
{
    // backup SmoothingMode value
    SmoothingModeAtKeyDown = SmoothingMode;

    if ( FSlateApplication::Get().GetModifierKeys().IsShiftDown() )
    {
        // flip the value
        SmoothingMode = ( SmoothingMode == ePathSmoothingMode::Round ) ? ePathSmoothingMode::Sharp
                                                                       : ePathSmoothingMode::Round;
        return true;
    }

    return UOdysseyPainterEditorVectorBaseTool::OnKeyDownVector( iScene, iKey, oSignalFlags );
}

bool
UOdysseyPainterEditorVectorPathSmoothTool::OnKeyUpVector( FOdysseyVectorGroupPaint* iScene
                                                        , const FKey& iKey
                                                        , uint64& oSignalFlags )
{
    // restore SmoothingMode value
    SmoothingMode = SmoothingModeAtKeyDown;

    return UOdysseyPainterEditorVectorBaseTool::OnKeyUpVector( iScene, iKey, oSignalFlags );
}

bool
UOdysseyPainterEditorVectorPathSmoothTool::OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                                            , const FOdysseyPoint& iPointInTexture
                                                            , const FKey& iKey
                                                            , uint64& oSignalFlags )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-path-smooth-tool.transaction.smooth-path","Vector Path Smooth Tool"));
        if( GUndo )
        {
            mUndoSegmentReshape = new FOdysseyVectorUndoSegmentReshape( iScene );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>( mUndoSegmentReshape ) );

            TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();
    }

    oSignalFlags = FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;

    return true;
}

void
UOdysseyPainterEditorVectorPathSmoothTool::OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                                             , const FOdysseyPoint& iPointInTexture
                                                             , uint64& oSignalFlags )
{
    double diameter = PickingRadius * 2.0f;
    ::ULIS::FRectI rect = { (int)iPointInTexture.x - (int)PickingRadius
                          , (int)iPointInTexture.y - (int)PickingRadius
                          , (int)diameter
                          , (int)diameter };

    mPathSmoothHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );

    oSignalFlags = FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;
}

void
UOdysseyPainterEditorVectorPathSmoothTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                                            , const FOdysseyPoint& iPointInTexture
                                                            , uint64& oSignalFlags )
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

        iScene->Update( FOdysseyVectorObject::KEEPINVALIDATED );
    }

    oSignalFlags = FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;
}

bool
UOdysseyPainterEditorVectorPathSmoothTool::OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , const FKey& iKey
                                                          , uint64& oSignalFlags )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

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

    oSignalFlags = FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
    return true;
}

FText
UOdysseyPainterEditorVectorPathSmoothTool::GetTooltip() const
{
    return LOCTEXT("vector-path-smooth-tool.tooltip", "Path Smooth Tool");
}

#undef LOCTEXT_NAMESPACE
