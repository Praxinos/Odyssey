// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPickTool/OdysseyPainterEditorVectorPickTool.h"
#include "Tools/VectorPickTool/OdysseyPainterEditorVectorPickToolHUD.h"
#include "Tools/VectorPickTool/SOdysseyPainterEditorVectorPickToolTopTab.h"
#include "Tools/VectorPickTool/OdysseyPainterEditorVectorPickToolObjectContextMenu.h"
#include "Tools/VectorPickTool/OdysseyPainterEditorVectorPickToolVertexContextMenu.h"
#include "OdysseyPainterEditor.h"
#include "PainterEditor/OdysseyPainterEditorViewportTab.h"
#include "OdysseyMediaVector.h"

#include <chrono>

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorPickTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPickTool::~UOdysseyPainterEditorVectorPickTool()
{
}

UOdysseyPainterEditorVectorPickTool::UOdysseyPainterEditorVectorPickTool()
    : PickingMode( EOdysseyVectorPickingMode::Freehand )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Lasso64");

    mPickHUD = new FOdysseyPainterEditorVectorPickToolHUD( this );
}

std::list<FOdysseyVectorObject*>&
UOdysseyPainterEditorVectorPickTool::GetFocusedObjectList( FOdysseyVectorScene* iScene )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();

    if( selectedObjectList.size() )
    {
        return selectedObjectList;
    }

    // return scene as list
    return iScene->GetEngine()->GetChildrenList();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorPickTool::Load()
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    UOdysseyPainterEditorVectorPickTool::LoadVector( vectorEngine, vectorScene );
}

void
UOdysseyPainterEditorVectorPickTool::Unload()
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    UOdysseyPainterEditorVectorPickTool::UnloadVector( vectorEngine, vectorScene );
}

bool
UOdysseyPainterEditorVectorPickTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
}

void
UOdysseyPainterEditorVectorPickTool::UnloadVector( FOdysseyVectorEngine* iEngine
                                                 , FOdysseyVectorScene* iScene )
{
    iEngine->RemoveHUD( mPickHUD );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorPickTool::LoadVector( FOdysseyVectorEngine* iEngine
                                               , FOdysseyVectorScene* iScene )
{
    mPickHUD->Load( iScene );

    iEngine->ClearHUD();
    iEngine->AddHUD( mPickHUD );

    iEngine->ResetHUD();

    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

TArray<TSharedPtr<SWidget>>
UOdysseyPainterEditorVectorPickTool::CreateTopTabWidgets()
{
    return {
        SNew(SOdysseyPainterEditorVectorPickToolTopTab, this)
    };
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
UOdysseyPainterEditorVectorPickTool::OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return false;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return false;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    return UOdysseyPainterEditorVectorPickTool::OnMouseDownVector( vectorEngine, vectorScene, iPointInTexture,iKey  );
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
        GetEditor()->GetGUI()->GetViewportTab().Get()->GetViewport().Get()->GetViewportWidget().ToSharedRef(),
        FWidgetPath(),
        GetEditor()->GetGUI()->GetVectorContextMenu()->Widget().ToSharedRef(),
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

        iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
    }

    return true;
}

void
UOdysseyPainterEditorVectorPickTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    UOdysseyPainterEditorVectorPickTool::OnMouseDragVector( vectorEngine, vectorScene, iPointInTexture );
}

void
UOdysseyPainterEditorVectorPickTool::OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                                      , FOdysseyVectorScene* iScene
                                                      , const FOdysseyPoint& iPointInTexture )
{
    //::ULIS::FRectI redrawRegion = { 0, 0, 0, 0 };

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

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );

    //return redrawRegion; // unused;
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

    iEngine->ClearMask();

    if( mPointArray.size() > 1 )
    {
        switch( PickingMode )
        {
            case EOdysseyVectorPickingMode::Rectangle:
            {
                double xmin = ::ULIS::FMath::Min( mPointArray[0].x, mPointArray[1].x );
                double ymin = ::ULIS::FMath::Min( mPointArray[0].y, mPointArray[1].y );
                double xmax = ::ULIS::FMath::Max( mPointArray[0].x, mPointArray[1].x );
                double ymax = ::ULIS::FMath::Max( mPointArray[0].y, mPointArray[1].y );
                ::ULIS::FRectD rect = ::ULIS::FRectD::FromMinMax( xmin, ymin, xmax, ymax );

                return iEngine->GenerateRectangleMask( rect );
            }
            break;

            case EOdysseyVectorPickingMode::Circle:
            {
                ::ULIS::FVec2D diagonal = ::ULIS::FVec2D( mPointArray[1] - mPointArray[0] );

                return iEngine->GenerateCircleMask( mPointArray[0].x, mPointArray[0].y, diagonal.Distance() );
            }
            break;

            case EOdysseyVectorPickingMode::Freehand:
                return iEngine->GenerateFreehandMask( mPointArray );
            break;

            default:
            break;
        }
    }

    return roi;
}

bool
UOdysseyPainterEditorVectorPickTool::OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{   
    bool ret = false;

/*if(FSlateApplication::Get().GetModifierKeys().IsControlDown())
{*/
    if( iKey == EKeys::RightMouseButton )
    {
        if( mEditor->GetVectorEditionMode() == eVectorEditionMode::Object )
        {
            TSharedPtr<SWidget> contextMenu = FOdysseyPainterEditorVectorPickToolObjectContextMenu::CreateWidget( GetEditor() );
            
            TSharedPtr<FOdysseyPainterEditorViewportTab> viewportTab = GetEditor()->FindTab<FOdysseyPainterEditorViewportTab>();
            FSlateApplication::Get().PushMenu( viewportTab->Widget().ToSharedRef(),
                                               FWidgetPath(),
                                               contextMenu.ToSharedRef(),
                                               FSlateApplication::Get().GetCursorPos(),
                                               FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu) );
        }

        if( mEditor->GetVectorEditionMode() == eVectorEditionMode::Vertex )
        {
            TSharedPtr<SWidget> contextMenu = FOdysseyPainterEditorVectorPickToolVertexContextMenu::CreateWidget( GetEditor() );

            TSharedPtr<FOdysseyPainterEditorViewportTab> viewportTab = GetEditor()->FindTab<FOdysseyPainterEditorViewportTab>();
            FSlateApplication::Get().PushMenu( viewportTab->Widget().ToSharedRef(),
                                               FWidgetPath(),
                                               contextMenu.ToSharedRef(),
                                               FSlateApplication::Get().GetCursorPos(),
                                               FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu) );
        }
    }
/*}*/
    else
    {
        bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
        if (!hasVector)
            return false;

        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
        if (mediaVectors.Num() <= 0)
            return false;

        FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
        FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

        return UOdysseyPainterEditorVectorPickTool::OnMouseUpVector( vectorEngine, vectorScene, iPointInTexture, iKey );
    }

    return ret;
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

    // deselect all if control key is not pressed
    if( FSlateApplication::Get().GetModifierKeys().IsControlDown() == false )
    {
        iPath->UnselectAllVertices();
    }

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
UOdysseyPainterEditorVectorPickTool::SelectBucketFromPaintGroup( FOdysseyVectorGroupPaint* iPaintGroup )
{
    std::vector<FOdysseyVectorBucket*> pickedBucketArray;

    pickedBucketArray.reserve( 50 );

    // deselect all if control key is not pressed
    if( FSlateApplication::Get().GetModifierKeys().IsControlDown() == false )
    {
        iPaintGroup->UnselectAllBuckets();
    }

    iPaintGroup->PickBucket( pickedBucketArray );

    for( int i = 0; i < pickedBucketArray.size(); i++ )
    {
        FOdysseyVectorBucket* bucket = pickedBucketArray[i];

        iPaintGroup->SelectBucket( bucket );
    }
}

void
UOdysseyPainterEditorVectorPickTool::OnMouseUpVectorVertexMode( FOdysseyVectorEngine* iEngine
                                                              , FOdysseyVectorScene* iScene
                                                              , const FOdysseyPoint& iPointInTexture
                                                              , const FKey& iKey )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = GetFocusedObjectList( iScene );

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
            SelectBucketFromPaintGroup( paintGroup );
        }
    }

    iEngine->ResetHUD(); // updates the current HUD (in most cases wil be this tool's HUD)
}

bool
UOdysseyPainterEditorVectorPickTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                                    , FOdysseyVectorScene* iScene
                                                    , const FOdysseyPoint& iPointInTexture
                                                    , const FKey& iKey )
{
    ::ULIS::FRectD roi;

    if( iKey == EKeys::LeftMouseButton )
    {
        roi = GenerateMask( iEngine );

        if( mEditor->GetVectorEditionMode() == eVectorEditionMode::Object )
        {
            OnMouseUpVectorObjectMode( iEngine, iScene, iPointInTexture, iKey );
        }

        if( mEditor->GetVectorEditionMode() == eVectorEditionMode::Vertex )
        {
            OnMouseUpVectorVertexMode( iEngine, iScene, iPointInTexture, iKey );
        }
    }

    mPointArray.clear();

    iScene->Update( 0 ); // update invalidated objects

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );

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
UOdysseyPainterEditorVectorPickTool::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
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
    PropertyChangedVector( vectorEngine, vectorScene, PropertyChangedEvent.GetPropertyName() );
}

void
UOdysseyPainterEditorVectorPickTool::PropertyChangedVector( FOdysseyVectorEngine* iEngine
                                                          , FOdysseyVectorScene* iScene
                                                          , const FName& iPropertyName )
{
    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

#undef LOCTEXT_NAMESPACE
