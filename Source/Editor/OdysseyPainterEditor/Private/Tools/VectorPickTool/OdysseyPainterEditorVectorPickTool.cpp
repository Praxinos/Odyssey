// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPickTool/OdysseyPainterEditorVectorPickTool.h"
#include "Tools/VectorPickTool/OdysseyPainterEditorVectorPickToolHUD.h"
#include <chrono>

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorPickTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPickTool::~UOdysseyPainterEditorVectorPickTool()
{
}

UOdysseyPainterEditorVectorPickTool::UOdysseyPainterEditorVectorPickTool()
    : EditionMode( EOdysseyVectorEditionMode::Object )
    , PickingMode( EOdysseyVectorPickingMode::Freehand )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Lasso64");

    mPickHUD = new FOdysseyPainterEditorVectorPickToolHUD( this );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorPickTool::UnloadVector( FOdysseyVectorEngine* iEngine
                                                 , FOdysseyVectorScene* iScene )
{
    iEngine->RemoveHUD( mPickHUD );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorPickTool::LoadVector( FOdysseyVectorEngine* iEngine
                                               , FOdysseyVectorScene* iScene )
{
    mPickHUD->Init( iEngine->GetBLImage()->width(), iEngine->GetBLImage()->height() );

    iEngine->ClearHUD();
    iEngine->AddHUD( mPickHUD );

    iEngine->ResetHUD();

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorPickTool::OnKeyDownVector( FOdysseyVectorEngine* iEngine
                                                    , FOdysseyVectorScene* iScene
                                                    , const FKey& iKey )
{
    // Note: this also calls iScene->Update(0)
    UOdysseyPainterEditorDefaultTool::OnKeyDownVector( iEngine, iScene, iKey );

    return false;
}

bool
UOdysseyPainterEditorVectorPickTool::OnKeyUpVector( FOdysseyVectorEngine* iEngine
                                                  , FOdysseyVectorScene* iScene
                                                  , const FKey& iKey )
{
    // Note: this also calls iScene->Update(0)
    UOdysseyPainterEditorDefaultTool::OnKeyUpVector( iEngine, iScene, iKey );

    return false;
}

bool
UOdysseyPainterEditorVectorPickTool::OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                                      , FOdysseyVectorScene* iScene
                                                      , const FOdysseyPoint& iPointInTexture
                                                      , const FKey& iKey )
{
/*
    if( iKey == EKeys::LeftMouseButton )
    {
        FSlateApplication::Get().PushMenu(
        GetEditorAs<FOdysseyPainterEditor>()->GetGUI()->GetViewportTab().Get()->GetViewport().Get()->GetViewportWidget().ToSharedRef(),
        FWidgetPath(),
        GetEditorAs<FOdysseyPainterEditor>()->GetGUI()->GetVectorContextMenu()->Widget().ToSharedRef(),
        FVector2D(iPointInTexture.x, iPointInTexture.y),
        FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu)
        );
    }
*/
    if( iKey == EKeys::LeftMouseButton )
    {
        mPressedMouseCoords.x = iPointInTexture.x;
        mPressedMouseCoords.y = iPointInTexture.y;

        mPointArray.clear();

        mPointArray.push_back( ::ULIS::FVec2D( iPointInTexture.x, iPointInTexture.y ) );

        iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
    }

    return true;
}

::ULIS::FRectI
UOdysseyPainterEditorVectorPickTool::OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                                      , FOdysseyVectorScene* iScene
                                                      , const FOdysseyPoint& iPointInTexture )
{
    ::ULIS::FRectI redrawRegion = { 0, 0, 0, 0 };

    if( iPointInTexture.keysDown.Find( EKeys::LeftMouseButton ) != INDEX_NONE )
    {
        ::ULIS::FVec2D point = { iPointInTexture.x, iPointInTexture.y };

        switch( PickingMode )
        {
            case EOdysseyVectorPickingMode::Rectangle:
            case EOdysseyVectorPickingMode::Circle :
            {
                ::ULIS::FVec2D downPoint = mPointArray[0];

                mPointArray.clear();
                mPointArray.push_back( downPoint );
                mPointArray.push_back( point );
            }
            break;

            case EOdysseyVectorPickingMode::Freehand :
                mPointArray.push_back( point );
            break;

            default:
            break;
        }
    }

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );

    return redrawRegion; // unused;
}

static void
SetSelectionSpace( FOdysseyVectorEngine* iVectorEngine, FOdysseyVectorObject* iSelectedObject )
{
    if( UOdysseyPainterEditorDefaultTool::DoubleClicked() == true )
    {
        FOdysseyVectorGroup* selectedGroup = nullptr;

        if( iSelectedObject )
        {
            if( iSelectedObject->HasBaseClass( FOdysseyVectorGroup::StaticClass() ) )
            {
                selectedGroup = static_cast<FOdysseyVectorGroup*>(iSelectedObject);
            }
        }

        iVectorEngine->SetSelectionSpace( selectedGroup );
    }
}

::ULIS::FRectD
UOdysseyPainterEditorVectorPickTool::GenerateMask( FOdysseyVectorEngine* iEngine )
{
    ::ULIS::FRectD roi = ::ULIS::FRectD::FromXYWH( 0, 0, 0, 0 );

    switch( PickingMode )
    {
        case EOdysseyVectorPickingMode::Rectangle:
            return iEngine->GenerateRectangleMask( mPointArray );
        break;

        case EOdysseyVectorPickingMode::Circle:
            return iEngine->GenerateCircleMask( mPointArray );
        break;

        case EOdysseyVectorPickingMode::Freehand:
            return iEngine->GenerateFreehandMask( mPointArray );
        break;

        default:
        break;
    }

    return roi;
}

void
UOdysseyPainterEditorVectorPickTool::OnMouseUpVectorObjectMode( FOdysseyVectorEngine* iEngine
                                                              , FOdysseyVectorScene* iScene
                                                              , const FOdysseyPoint& iPointInTexture
                                                              , const FKey& iKey )
{
    std::vector<FOdysseyVectorObject*> pickedObjectArray;
    ::ULIS::FRectD roi;

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("VectorObjectPickTool","Vector Object Pick Tool"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoSelect( iScene );

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

    // deselect all if control key is not pressed
    if( FSlateApplication::Get().GetModifierKeys().IsControlDown() == false )
    {
        iScene->ClearSelection();
    }

    // dragging occured
    if ( mPointArray.size() > 1 )
    {
        roi = GenerateMask( iEngine );

        iEngine->Pick( iScene, roi, pickedObjectArray, FOdysseyVectorObject::PICK_MASK_BASED );

        // when dragging occured, we select all objects lying in the selection area.
        for ( int i = 0; i < pickedObjectArray.size(); i++ )
        {
            iScene->Select( pickedObjectArray[i] );
        }
    }

    // no dragging occured
    if ( mPointArray.size() == 1 )
    {
        roi.x = mPointArray[0].x;
        roi.y = mPointArray[0].y;

        iEngine->Pick( iScene, roi, pickedObjectArray, FOdysseyVectorObject::PICK_MATH_BASED );

        // if no dragging occured, we only select the object that is the most forward
        if( pickedObjectArray.size() )
        {
            iScene->Select( pickedObjectArray.back() );
        }
    }

    SetSelectionSpace( iEngine, iScene->GetLastSelected() );

    iEngine->ResetHUD(); // updates the current HUD (in most cases wil be this tool's HUD)
}

void
UOdysseyPainterEditorVectorPickTool::SelectVertexFromPath( FOdysseyVectorPath* iPath )
{
    std::vector<FOdysseyVectorVertex*> pickedVertexArray;

    pickedVertexArray.reserve( 50 );

    iPath->UnselectAllVertices();
    // Pick from mask image
    iPath->PickVertex(  pickedVertexArray );

    for( int i = 0; i < pickedVertexArray.size(); i++ )
    {
        FOdysseyVectorVertex* vertex = pickedVertexArray[i];

        if( vertex->IsSelected() == false )
        {
            iPath->SelectVertex( vertex );
        }
    }
}

void
UOdysseyPainterEditorVectorPickTool::SelectVertexFromPaintGroup( FOdysseyVectorGroupPaint* iPaintGroup )
{
    std::list<FOdysseyVectorObject*>& childrenObjectList = iPaintGroup->GetChildrenList();
    std::list<FOdysseyVectorObject*>::iterator it;

    for( it = childrenObjectList.begin(); it != childrenObjectList.end(); ++it )
    {
        FOdysseyVectorObject* child = (*it);

        if( child->HasBaseClass(FOdysseyVectorPath::StaticClass()) )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(child);

            SelectVertexFromPath( path );
        }
    }
}

void
UOdysseyPainterEditorVectorPickTool::OnMouseUpVectorVertexMode( FOdysseyVectorEngine* iEngine
                                                              , FOdysseyVectorScene* iScene
                                                              , const FOdysseyPoint& iPointInTexture
                                                              , const FKey& iKey )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();
    ::ULIS::FRectD roi;

    // dragging occured
    if ( mPointArray.size() > 1 )
    {
        roi = GenerateMask( iEngine );

        for( std::list<FOdysseyVectorObject*>::iterator it = selectedObjectList.begin(); it != selectedObjectList.end(); ++it )
        {
            FOdysseyVectorObject* selectedObject  = *it;

            if( selectedObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
            {
                FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(selectedObject);

                SelectVertexFromPath( path );
            }

            if( selectedObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
            {
                FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(selectedObject);

                SelectVertexFromPaintGroup( paintGroup );
            }
        }
    }
}

bool
UOdysseyPainterEditorVectorPickTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                                    , FOdysseyVectorScene* iScene
                                                    , const FOdysseyPoint& iPointInTexture
                                                    , const FKey& iKey )
{
    if( iKey == EKeys::LeftMouseButton )
    {
        if( EditionMode == EOdysseyVectorEditionMode::Object )
        {
            OnMouseUpVectorObjectMode( iEngine, iScene, iPointInTexture, iKey );
        }

        if( EditionMode == EOdysseyVectorEditionMode::Vertex )
        {
            OnMouseUpVectorVertexMode( iEngine, iScene, iPointInTexture, iKey );
        }
    }

    mPointArray.clear();

    iScene->Update( 0 ); // update invalidated objects
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW | FOdysseyVectorScene::SIGNAL_OBJECT_SELECTED );

    return true;
}

std::vector<::ULIS::FVec2D>&
UOdysseyPainterEditorVectorPickTool::GetPointArray()
{
    return mPointArray;
}

EOdysseyVectorPickingMode
UOdysseyPainterEditorVectorPickTool::GetPickingMode()
{
    return PickingMode;
}

void
UOdysseyPainterEditorVectorPickTool::Commit()
{

}

void
UOdysseyPainterEditorVectorPickTool::PropertyChangedVector( FOdysseyVectorEngine* iEngine
                                                          , FOdysseyVectorScene* iScene
                                                          , const FName& iPropertyName )
{
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

#undef LOCTEXT_NAMESPACE
