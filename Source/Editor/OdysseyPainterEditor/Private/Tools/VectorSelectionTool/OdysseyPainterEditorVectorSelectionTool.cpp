// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorSelectionTool/OdysseyPainterEditorVectorSelectionTool.h"
#include "Tools/VectorSelectionTool/OdysseyPainterEditorVectorSelectionToolHUD.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseToolHUD.h"
#include "OdysseyPainterEditor.h"
#include "PainterEditor/OdysseyPainterEditorViewportTab.h"
#include "OdysseyMediaVector.h"

#include "Undo/OdysseyVectorUndoSelectObject.h"
#include "Undo/OdysseyVectorUndoSelectVertex.h"

#include <chrono>

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorSelectionTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorSelectionTool::~UOdysseyPainterEditorVectorSelectionTool()
{
}

UOdysseyPainterEditorVectorSelectionTool::UOdysseyPainterEditorVectorSelectionTool()
    : UOdysseyPainterEditorVectorBaseTool( new FOdysseyPainterEditorVectorSelectionToolHUD( this ) )
    , SelectionShape( EOdysseyVectorSelectionShape::Freehand )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Lasso64");

    mPickHUD = static_cast<FOdysseyPainterEditorVectorSelectionToolHUD*>( mBaseHUD );
}

UOdysseyPainterEditorVectorSelectionTool::UOdysseyPainterEditorVectorSelectionTool( FOdysseyPainterEditorVectorBaseToolHUD* iHUD )
    : UOdysseyPainterEditorVectorBaseTool( iHUD )
    , SelectionShape( EOdysseyVectorSelectionShape::Freehand )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Lasso64");

    mPickHUD = static_cast<FOdysseyPainterEditorVectorSelectionToolHUD*>( mBaseHUD );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides


bool
UOdysseyPainterEditorVectorSelectionTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
}

uint64
UOdysseyPainterEditorVectorSelectionTool::LoadVector( FOdysseyVectorGroupPaint* iScene )
{
    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorSelectionTool::UnloadVector( FOdysseyVectorGroupPaint* iScene )
{
    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorSelectionTool::OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                                           , const FOdysseyPoint& iPointInTexture
                                                           , const FKey& iKey )
{
    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        mPressedMouseCoords.x = iPointInTexture.x;
        mPressedMouseCoords.y = iPointInTexture.y;

        mPointArray.clear();

        mPointArray.push_back( ::ULIS::FVec2D( iPointInTexture.x, iPointInTexture.y ) );
    }

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;
}

uint64
UOdysseyPainterEditorVectorSelectionTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
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

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;

    //return redrawRegion; // unused;
}


static void
SetSelectionSpace( FOdysseyVectorEngine* iVectorEngine, FOdysseyVectorObject* iSelectedObject )
{
    if( UOdysseyPainterEditorVectorBaseTool::DoubleClicked() == true )
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
UOdysseyPainterEditorVectorSelectionTool::GenerateMask()
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
UOdysseyPainterEditorVectorSelectionTool::OnMouseUpVectorObjectMode( FOdysseyVectorGroupPaint* iScene
                                                                   , const FOdysseyPoint& iPointInTexture
                                                                   , const FKey& iKey )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();
    std::vector<FOdysseyVectorObject*> pickedObjectArray;
    ::ULIS::FRectD roi;

    if( UOdysseyPainterEditorVectorBaseTool::DoubleClicked() == true )
    {
        FOdysseyVectorGroup* pickedGroup = nullptr;

        roi.x = iPointInTexture.x;
        roi.y = iPointInTexture.y;

        iEngine->Pick( iScene, roi, pickedObjectArray, FOdysseyVectorObject::PICK_MATH_BASED );

        if( pickedObjectArray.size() )
        {
            FOdysseyVectorObject* pickedObject = pickedObjectArray.front();

            if( pickedObject->HasBaseClass( FOdysseyVectorGroup::StaticClass() ) )
            {
                pickedGroup = static_cast<FOdysseyVectorGroup*>(pickedObject);
            }
        }

        iEngine->SetSelectionSpace( pickedGroup );
    }
    else
    {
        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("VectorObjectSelectionTool","Vector Object Pick Tool"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoSelectObject( iScene );

            GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
        }
        GEditor->EndTransaction();

        // deselect all if control key is not pressed
        if( FSlateApplication::Get().GetModifierKeys().IsControlDown() == false )
        {
            iEngine->ClearObjectSelection();
        }

        // dragging occured
        if ( mPointArray.size() > 1 )
        {
            iEngine->Pick( iScene, roi, pickedObjectArray, FOdysseyVectorObject::PICK_MASK_BASED );

            // when dragging occured, we select all objects lying in the selection area.
            for ( int i = 0; i < pickedObjectArray.size(); i++ )
            {
                iEngine->SelectObject( pickedObjectArray[i] );
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
                iEngine->SelectObject( pickedObjectArray.back() );
            }
        }
    }

    iEngine->ResetHUD(); // updates the current HUD (in most cases wil be this tool's HUD)
}

void
UOdysseyPainterEditorVectorSelectionTool::OnMouseUpVectorVertexMode( FOdysseyVectorGroupPaint* iScene
                                                                   , const FOdysseyPoint& iPointInTexture
                                                                   , const FKey& iKey )
{
    std::list<FOdysseyVectorObject*> objectList;
    std::vector<FOdysseyVectorVertex*> pickedVertexArray;
    std::vector<FOdysseyVectorBucket*> pickedBucketArray;
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    // run lambda on object tree
    iEngine->Traverse
    ( iScene
    , iScene
    , 0
    , [ this
      , iEngine
      , iScene
      , &objectList
      , &pickedVertexArray
      , &pickedBucketArray ]( FOdysseyVectorObject* object, uint64 traversalFlags ) -> uint64
    {
        if( iEngine->HasFocus( iScene, object, traversalFlags ) )
        {
            // all focused object are concerned, as their selection
            // might be cleared in case no vertex is selected.
            objectList.push_back( object );

            if( object->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
            {
                FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);

                path->PickVertex( pickedVertexArray );
            }

            if( object->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
            {
                FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(object);

                paintGroup->PickBucket( pickedBucketArray );
            }

            return FOdysseyVectorEngine::TRAVERSE_OBJECT_ACCEPTED; // keep traversing
        }

        return 0;
    } );

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("VectorObjectSelectionTool","Vector Vertex Pick Tool"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoSelectVertex( iScene, objectList );

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

    // the actual selection

    // deselect all if control key is not pressed
    for( FOdysseyVectorObject* object : objectList )
    {
        if( object->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);

            if( FSlateApplication::Get().GetModifierKeys().IsControlDown() == false )
            {
                path->UnselectAllVertices();
            }
        }

        if( object->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(object);

            if( FSlateApplication::Get().GetModifierKeys().IsControlDown() == false )
            {
                paintGroup->UnselectAllBuckets();
            }
        }
    }

    // select
    for( FOdysseyVectorVertex* vertex : pickedVertexArray )
    {
        if( vertex->IsSelected() == false )
        {
            vertex->GetPath()->SelectVertex( vertex );
        }
    }

    for( int i = 0; i < pickedBucketArray.size(); i++ )
    {
        FOdysseyVectorBucket* bucket = pickedBucketArray[i];
        FOdysseyVectorObject* ownerObject = bucket->GetOwner();

        if( ownerObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(ownerObject);

            //note:  bucket selection is checked in SelectBucket()
            paintGroup->SelectBucket( bucket );
        }
    }


    iEngine->ResetHUD(); // updates the current HUD (in most cases wil be this tool's HUD)
}

uint64
UOdysseyPainterEditorVectorSelectionTool::OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                                         , const FOdysseyPoint& iPointInTexture
                                                         , const FKey& iKey )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();
    ::ULIS::FRectD roi;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        roi = GenerateMask();

        // TODO: pass the mask image as arg to Pick function
        iEngine->SetBLMask( mPickHUD->GetMask() );
        if( mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_OBJECT )
        {
            OnMouseUpVectorObjectMode( iScene, iPointInTexture, iKey );
        }

        if( mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_VERTEX )
        {
            OnMouseUpVectorVertexMode( iScene, iPointInTexture, iKey );
        }
        iEngine->SetBLMask( nullptr );

        mPointArray.clear();

        iScene->Update( 0 ); // update invalidated objects
/*
        iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                       | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
                       | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );
*/
    }

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
         | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED;
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

#undef LOCTEXT_NAMESPACE
