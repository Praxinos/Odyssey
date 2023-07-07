// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorGridTool/OdysseyPainterEditorVectorGridTool.h"
#include "Tools/VectorGridTool/OdysseyPainterEditorVectorGridToolHUD.h"
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
    : mMultipleSelectionMode( false )
    , DivisionsX( 4 )
    , DivisionsY( 4 )
    , PickingRadius( 10.0f )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Grid64");

    mGridHUD = new FOdysseyPainterEditorVectorGridToolHUD( this );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

bool
UOdysseyPainterEditorVectorGridTool::IsActivable() const
{
    return !!mToolContext->GetVectorEngine();
}

void
UOdysseyPainterEditorVectorGridTool::Load()
{
    FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();
    if( vectorEngine )
    {
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();
        UOdysseyPainterEditorVectorGridTool::LoadVector( vectorEngine, vectorScene );
    }
}

void
UOdysseyPainterEditorVectorGridTool::Unload()
{
    FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();
    if( vectorEngine )
    {
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();
        UOdysseyPainterEditorVectorGridTool::UnloadVector( vectorEngine, vectorScene );
    }
}

void
UOdysseyPainterEditorVectorGridTool::UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->RemoveHUD( mGridHUD );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorGridTool::LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->ClearHUD();
    iEngine->AddHUD( mGridHUD );

    mGridHUD->MakeGrid( iScene, DivisionsX, DivisionsY );
    mGridHUD->Export( mPointArray );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorGridTool::OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    bool ret = false;
    FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();
    if( vectorEngine )
    {
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();
        ret = UOdysseyPainterEditorVectorGridTool::OnMouseDownVector( vectorEngine, vectorScene, iPointInTexture, iKey );
    }

    return ret;
}

bool
UOdysseyPainterEditorVectorGridTool::OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                                      , FOdysseyVectorScene* iScene
                                                      , const FOdysseyPoint& iPointInTexture
                                                      , const FKey& iKey )
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

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );

    return true;
}

void
UOdysseyPainterEditorVectorGridTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();
    if( vectorEngine )
    {
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();
        UOdysseyPainterEditorVectorGridTool::OnMouseDragVector( vectorEngine, vectorScene, iPointInTexture );
    }
}

void
UOdysseyPainterEditorVectorGridTool::OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                                      , FOdysseyVectorScene* iScene
                                                      , const FOdysseyPoint& iPointInTexture )
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
            iScene->Update( FOdysseyVectorObject::FREQUENTUPDATES | FOdysseyVectorObject::KEEPINVALIDATED );
        }
    }

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorGridTool::OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    bool ret = false;

    FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();
    if( vectorEngine )
    {
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();
        ret = UOdysseyPainterEditorVectorGridTool::OnMouseUpVector( vectorEngine, vectorScene, iPointInTexture, iKey );
    }
    return ret;
}

bool
UOdysseyPainterEditorVectorGridTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                                    , FOdysseyVectorScene* iScene
                                                    , const FOdysseyPoint& iPointInTexture
                                                    , const FKey& iKey )
{
    if( mMultipleSelectionMode == true )
    {
        mGridHUD->EndSelectionRectangle( FSlateApplication::Get().GetModifierKeys().IsControlDown() ? false : true );
    }

    iScene->Update( 0 );
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );

    mMultipleSelectionMode = false;

    return true;
}

void
UOdysseyPainterEditorVectorGridTool::Commit()
{

}

void
UOdysseyPainterEditorVectorGridTool::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    
    FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();
    FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

    PropertyChangedVector( vectorEngine, vectorScene, PropertyChangedEvent.GetPropertyName());
}

void
UOdysseyPainterEditorVectorGridTool::PropertyChangedVector( FOdysseyVectorEngine* iEngine
                                                          , FOdysseyVectorScene* iScene
                                                          , const FName& iPropertyName )
{
    mGridHUD->MakeGrid( iScene, DivisionsX, DivisionsY );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

#undef LOCTEXT_NAMESPACE
