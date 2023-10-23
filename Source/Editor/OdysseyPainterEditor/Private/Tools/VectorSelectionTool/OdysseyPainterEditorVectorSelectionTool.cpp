// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorSelectionTool/OdysseyPainterEditorVectorSelectionTool.h"
#include "Tools/VectorSelectionTool/OdysseyPainterEditorVectorSelectionToolHUD.h"
#include "Tools/VectorSelectionTool/SOdysseyPainterEditorVectorSelectionToolTopTab.h"
#include "Tools/VectorSelectionTool/OdysseyPainterEditorVectorSelectionToolObjectContextMenu.h"
#include "Tools/VectorSelectionTool/OdysseyPainterEditorVectorSelectionToolVertexContextMenu.h"
#include "OdysseyPainterEditor.h"
#include "PainterEditor/OdysseyPainterEditorViewportTab.h"
#include "OdysseyMediaVector.h"

#include <chrono>

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorSelectionTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorSelectionTool::~UOdysseyPainterEditorVectorSelectionTool()
{
}

UOdysseyPainterEditorVectorSelectionTool::UOdysseyPainterEditorVectorSelectionTool()
    : SelectionShape( EOdysseyVectorSelectionShape::Freehand )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Lasso64");

    mPickHUD = new FOdysseyPainterEditorVectorSelectionToolHUD( this );
}

std::list<FOdysseyVectorObject*>&
UOdysseyPainterEditorVectorSelectionTool::GetFocusedObjectList( FOdysseyVectorScene* iScene )
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


bool
UOdysseyPainterEditorVectorSelectionTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
}

void
UOdysseyPainterEditorVectorSelectionTool::LoadVector( FOdysseyVectorEngine* iEngine
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

void
UOdysseyPainterEditorVectorSelectionTool::Load()
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

            LoadVector( vectorEngine, vectorScene );
        }
    }
}

void
UOdysseyPainterEditorVectorSelectionTool::UnloadVector( FOdysseyVectorEngine* iEngine
                                                      , FOdysseyVectorScene* iScene )
{
    iEngine->RemoveHUD( mPickHUD );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorSelectionTool::Unload()
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

            UnloadVector( vectorEngine, vectorScene );
        }
    }
}

TSharedRef<SWidget>
UOdysseyPainterEditorVectorSelectionTool::CreateTopTabWidget()
{
    return SNew(SOdysseyPainterEditorVectorSelectionToolTopTab, this);
}

bool
UOdysseyPainterEditorVectorSelectionTool::OnKeyDownVector( FOdysseyVectorEngine* iEngine
                                                         , FOdysseyVectorScene* iScene
                                                         , const FKey& iKey )
{
    // Note: this also calls iScene->Update(0)
    UOdysseyPainterEditorDefaultTool::OnKeyDownVector( iEngine, iScene, iKey );

    return false;
}

bool
UOdysseyPainterEditorVectorSelectionTool::OnKeyUpVector( FOdysseyVectorEngine* iEngine
                                                       , FOdysseyVectorScene* iScene
                                                       , const FKey& iKey )
{
    // Note: this also calls iScene->Update(0)
    UOdysseyPainterEditorDefaultTool::OnKeyUpVector( iEngine, iScene, iKey );

    return false;
}

bool
UOdysseyPainterEditorVectorSelectionTool::OnMouseDownVector( FOdysseyVectorEngine* iEngine
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

        iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                       | FOdysseyVectorEngine::SIGNAL_INTERACTIVE );
    }

    return true;
}

bool
UOdysseyPainterEditorVectorSelectionTool::OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

            return OnMouseDownVector( vectorEngine, vectorScene, iPointInTexture,iKey  );
        }
    }

    return false;
}

void
UOdysseyPainterEditorVectorSelectionTool::OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                                      , FOdysseyVectorScene* iScene
                                                      , const FOdysseyPoint& iPointInTexture )
{
    //::ULIS::FRectI redrawRegion = { 0, 0, 0, 0 };

    if( iPointInTexture.keysDown.Find( EKeys::LeftMouseButton ) != INDEX_NONE )
    {
        ::ULIS::FVec2D point = { iPointInTexture.x, iPointInTexture.y };

        switch( SelectionShape )
        {
            case EOdysseyVectorSelectionShape::Rectangle:
            case EOdysseyVectorSelectionShape::Circle :
            {
                ::ULIS::FVec2D downPoint = mPointArray[0];

                mPointArray.clear();
                mPointArray.push_back( downPoint );
                mPointArray.push_back( point );
            }
            break;

            case EOdysseyVectorSelectionShape::Freehand :
                mPointArray.push_back( point );
            break;

            default:
            break;
        }
    }

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_INTERACTIVE );

    //return redrawRegion; // unused;
}

void
UOdysseyPainterEditorVectorSelectionTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

            OnMouseDragVector( vectorEngine, vectorScene, iPointInTexture );
        }
    }
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
UOdysseyPainterEditorVectorSelectionTool::GenerateMask( FOdysseyVectorEngine* iEngine )
{
    ::ULIS::FRectD roi = ::ULIS::FRectD::FromXYWH( 0, 0, 0, 0 );

    mPickHUD->ClearMask();

    if( mPointArray.size() > 1 )
    {
        switch( SelectionShape )
        {
            case EOdysseyVectorSelectionShape::Rectangle:
            {
                double xmin = ::ULIS::FMath::Min( mPointArray[0].x, mPointArray[1].x );
                double ymin = ::ULIS::FMath::Min( mPointArray[0].y, mPointArray[1].y );
                double xmax = ::ULIS::FMath::Max( mPointArray[0].x, mPointArray[1].x );
                double ymax = ::ULIS::FMath::Max( mPointArray[0].y, mPointArray[1].y );
                ::ULIS::FRectD rect = ::ULIS::FRectD::FromMinMax( xmin, ymin, xmax, ymax );

                return mPickHUD->GenerateRectangleMask( rect );
            }
            break;

            case EOdysseyVectorSelectionShape::Circle:
            {
                ::ULIS::FVec2D diagonal = ::ULIS::FVec2D( mPointArray[1] - mPointArray[0] );

                return mPickHUD->GenerateCircleMask( mPointArray[0].x, mPointArray[0].y, diagonal.Distance() );
            }
            break;

            case EOdysseyVectorSelectionShape::Freehand:
                return mPickHUD->GenerateFreehandMask( mPointArray );
            break;

            default:
            break;
        }
    }

    return roi;
}

void
UOdysseyPainterEditorVectorSelectionTool::OnMouseUpVectorObjectMode( FOdysseyVectorEngine* iEngine
                                                              , FOdysseyVectorScene* iScene
                                                              , const FOdysseyPoint& iPointInTexture
                                                              , const FKey& iKey )
{
    std::vector<FOdysseyVectorObject*> pickedObjectArray;
    ::ULIS::FRectD roi;

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("VectorObjectSelectionTool","Vector Object Pick Tool"));
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
UOdysseyPainterEditorVectorSelectionTool::SelectVertexFromPath( FOdysseyVectorPath* iPath )
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
UOdysseyPainterEditorVectorSelectionTool::SelectVertexFromPaintGroup( FOdysseyVectorGroupPaint* iPaintGroup )
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
UOdysseyPainterEditorVectorSelectionTool::SelectBucketFromPaintGroup( FOdysseyVectorGroupPaint* iPaintGroup )
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
UOdysseyPainterEditorVectorSelectionTool::OnMouseUpVectorVertexMode( FOdysseyVectorEngine* iEngine
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
UOdysseyPainterEditorVectorSelectionTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                                    , FOdysseyVectorScene* iScene
                                                    , const FOdysseyPoint& iPointInTexture
                                                    , const FKey& iKey )
{
    ::ULIS::FRectD roi;

    if( iKey == EKeys::LeftMouseButton )
    {
        roi = GenerateMask( iEngine );

        // TODO: pass the mask image as arg to Pick function
        iEngine->SetBLMask( mPickHUD->GetMask() );
        if( mEditor->GetVectorEditionMode() == eVectorEditionMode::Object )
        {
            OnMouseUpVectorObjectMode( iEngine, iScene, iPointInTexture, iKey );
        }

        if( mEditor->GetVectorEditionMode() == eVectorEditionMode::Vertex )
        {
            OnMouseUpVectorVertexMode( iEngine, iScene, iPointInTexture, iKey );
        }
        iEngine->SetBLMask( nullptr );
    }

    mPointArray.clear();

    iScene->Update( 0 ); // update invalidated objects

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );

    return true;
}


bool
UOdysseyPainterEditorVectorSelectionTool::OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{   
    bool ret = false;

/*if(FSlateApplication::Get().GetModifierKeys().IsControlDown())
{*/
    if( iKey == EKeys::RightMouseButton )
    {
        if( mEditor->GetVectorEditionMode() == eVectorEditionMode::Object )
        {
            TSharedPtr<SWidget> contextMenu = FOdysseyPainterEditorVectorSelectionToolObjectContextMenu::CreateWidget( GetEditor() );
            
            TSharedPtr<FOdysseyPainterEditorViewportTab> viewportTab = GetEditor()->FindTab<FOdysseyPainterEditorViewportTab>();
            FSlateApplication::Get().PushMenu( viewportTab->Widget().ToSharedRef(),
                                               FWidgetPath(),
                                               contextMenu.ToSharedRef(),
                                               FSlateApplication::Get().GetCursorPos(),
                                               FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu) );
        }

        if( mEditor->GetVectorEditionMode() == eVectorEditionMode::Vertex )
        {
            TSharedPtr<SWidget> contextMenu = FOdysseyPainterEditorVectorSelectionToolVertexContextMenu::CreateWidget( GetEditor() );

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

        if( hasVector )
        {
            TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();

            if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
            {
                FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
                FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

                return OnMouseUpVector( vectorEngine, vectorScene, iPointInTexture, iKey );
            }
        }
    }

    return ret;
}

std::vector<::ULIS::FVec2D>&
UOdysseyPainterEditorVectorSelectionTool::GetPointArray()
{
    return mPointArray;
}

EOdysseyVectorSelectionShape
UOdysseyPainterEditorVectorSelectionTool::GetSelectionShape()
{
    return SelectionShape;
}

void
UOdysseyPainterEditorVectorSelectionTool::Commit()
{

}

void
UOdysseyPainterEditorVectorSelectionTool::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    // Redraw
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

            vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
        }
    }
}
/*
void
UOdysseyPainterEditorVectorSelectionTool::PropertyChangedVector( FOdysseyVectorEngine* iEngine
                                                               , FOdysseyVectorScene* iScene
                                                               , const FName& iPropertyName )
{
    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}
*/

#undef LOCTEXT_NAMESPACE
