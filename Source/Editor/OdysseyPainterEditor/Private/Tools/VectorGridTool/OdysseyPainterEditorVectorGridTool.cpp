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
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
}

void
UOdysseyPainterEditorVectorGridTool::Load()
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    UOdysseyPainterEditorVectorGridTool::LoadVector( vectorEngine, vectorScene );
}

void
UOdysseyPainterEditorVectorGridTool::Unload()
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    UOdysseyPainterEditorVectorGridTool::UnloadVector( vectorEngine, vectorScene );
}

void
UOdysseyPainterEditorVectorGridTool::UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->RemoveHUD( mGridHUD );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorGridTool::LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->ClearHUD();
    iEngine->AddHUD( mGridHUD );

    mGridHUD->MakeGrid( iScene, DivisionsX, DivisionsY );
    mGridHUD->Export( mPointArray );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorGridTool::OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return false;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return false;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    return UOdysseyPainterEditorVectorGridTool::OnMouseDownVector( vectorEngine, vectorScene, iPointInTexture, iKey );
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

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );

    return true;
}

void
UOdysseyPainterEditorVectorGridTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    UOdysseyPainterEditorVectorGridTool::OnMouseDragVector( vectorEngine, vectorScene, iPointInTexture );
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

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorGridTool::OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return false;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return false;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    return UOdysseyPainterEditorVectorGridTool::OnMouseUpVector( vectorEngine, vectorScene, iPointInTexture, iKey );
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

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );

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

    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

    PropertyChangedVector( vectorEngine, vectorScene, PropertyChangedEvent.GetPropertyName());
}

void
UOdysseyPainterEditorVectorGridTool::PropertyChangedVector( FOdysseyVectorEngine* iEngine
                                                          , FOdysseyVectorScene* iScene
                                                          , const FName& iPropertyName )
{
    mGridHUD->MakeGrid( iScene, DivisionsX, DivisionsY );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

#undef LOCTEXT_NAMESPACE
