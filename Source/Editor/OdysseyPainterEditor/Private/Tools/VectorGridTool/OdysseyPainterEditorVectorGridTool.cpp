// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorGridTool/OdysseyPainterEditorVectorGridTool.h"
#include "Tools/VectorGridTool/OdysseyPainterEditorVectorGridToolHUD.h"
#include "OdysseyMediaVector.h"
#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorGridTool"

#ifndef M_PI
#define M_PI 3.14159265358979323846L
#endif

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorGridTool::~UOdysseyPainterEditorVectorGridTool()
{
}

UOdysseyPainterEditorVectorGridTool::UOdysseyPainterEditorVectorGridTool()
    : UOdysseyPainterEditorVectorSelectionTool( new FOdysseyPainterEditorVectorGridToolHUD( this ) )
    , mMultipleSelectionMode( false )
    , DivisionsX( 4 )
    , DivisionsY( 4 )
    , PickingRadius( 10.0f )
    , World ( false )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Grid64");

    mGridHUD = static_cast<FOdysseyPainterEditorVectorGridToolHUD*>( mBaseHUD );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

bool
UOdysseyPainterEditorVectorGridTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
}

uint64
UOdysseyPainterEditorVectorGridTool::UnloadVector( FOdysseyVectorScene* iScene )
{
    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorGridTool::LoadVector( FOdysseyVectorScene* iScene )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    mGridHUD->Export( mPointArray );

    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorGridTool::OnMouseDownVector( FOdysseyVectorScene* iScene
                                                      , const FOdysseyPoint& iPointInTexture
                                                      , const FKey& iKey )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("VectorGridTool","Vector Grid Tool"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoPointPosition( iScene, mPointArray );

            GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
        }
        GEditor->EndTransaction();

        // multiple selection mode
        if ( FSlateApplication::Get().GetModifierKeys().IsShiftDown() )
        {
            mMultipleSelectionMode = true;

            mGridHUD->StartSelectionRectangle( iPointInTexture.x, iPointInTexture.y );
        }
        else
        {
            bool picked;

            mGridHUD->GetSelection( mGridNodeArray );

            picked = mGridHUD->PickNodes( iPointInTexture.x
                                        , iPointInTexture.y
                                        , PickingRadius
                                        , FSlateApplication::Get().GetModifierKeys().IsControlDown() ? false : true );

            if( picked == true )
            {
                mGridHUD->GetSelection( mGridNodeArray );
            }
        }
    }

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;
}

uint64
UOdysseyPainterEditorVectorGridTool::OnMouseHoverVector( FOdysseyVectorScene* iScene
                                                       , const FOdysseyPoint& iPointInTexture )
{
    // TODO: highlight grid handles ?

    return 0;
}

uint64
UOdysseyPainterEditorVectorGridTool::OnMouseDragVector( FOdysseyVectorScene* iScene
                                                      , const FOdysseyPoint& iPointInTexture )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    if( iPointInTexture.keysDown.Find( EKeys::LeftMouseButton ) != INDEX_NONE )
    {
        if( mMultipleSelectionMode == true )
        {
            mGridHUD->DragSelectionRectangle( iPointInTexture.x, iPointInTexture.y );
        }
        else
        {
            FSelectionBox& selectionBox = mGridHUD->GetSelectionBox();

            if( selectionBox.rect.Area() )
            {
                BLPoint spaceDif = selectionBox.inverseWorldMatrix.mapVector( iPointInTexture.deltaPosition.X
                                                                            , iPointInTexture.deltaPosition.Y );

                for( int i = 0; i < mGridNodeArray.size(); i++ )
                {
                    mGridNodeArray[i]->Set( mGridNodeArray[i]->GetX() + spaceDif.x, mGridNodeArray[i]->GetY() + spaceDif.y );
                }

                mGridHUD->Deform();

                // update invalidated objects
                iScene->Update( FOdysseyVectorObject::KEEPINVALIDATED );
            }
        }
    }

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
          | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;
}

uint64
UOdysseyPainterEditorVectorGridTool::OnMouseUpVector( FOdysseyVectorScene* iScene
                                                    , const FOdysseyPoint& iPointInTexture
                                                    , const FKey& iKey )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        if( mMultipleSelectionMode == true )
        {
            mGridHUD->EndSelectionRectangle( FSlateApplication::Get().GetModifierKeys().IsControlDown() ? false : true );
        }

        iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

        mMultipleSelectionMode = false;
    }

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorGridTool::PropertyChangedVector( FOdysseyVectorScene* iScene
                                                          , const FName& iPropertyName )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    iEngine->ResetHUD();

    return UOdysseyPainterEditorVectorSelectionTool::PropertyChangedVector( iScene, iPropertyName )
         | FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

#undef LOCTEXT_NAMESPACE
