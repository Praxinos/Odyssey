// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/VectorSelectionTool/OdysseyPainterEditorVectorSelectionTool.h"
#include "Tools/VectorSelectionTool/OdysseyPainterEditorVectorSelectionToolHUD.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseToolHUD.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorSource.h"
#include "OdysseyPainterEditorViewportTab.h"
#include "OdysseyPainterEditorSource.h"
#include "OdysseyMediaVector.h"
#include "ISinglePropertyView.h"
#include "Widgets/Layout/SWrapBox.h"
#include "SOdysseySinglePropertyView.h"
// Vector engine
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyVectorCell.h"
#include "Undo/OdysseyVectorUndoSelectObject.h"
#include "Undo/OdysseyVectorUndoSelectVertex.h"


#include <chrono>

#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorSelectionTool::~UOdysseyPainterEditorVectorSelectionTool()
{
}

UOdysseyPainterEditorVectorSelectionTool::UOdysseyPainterEditorVectorSelectionTool()
    : UOdysseyPainterEditorVectorBaseTool( MakeShared<FOdysseyPainterEditorVectorSelectionToolHUD>( this ), false, true )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Lasso64");

    Shapes.AddShapeType( EOdysseyShapeType::kRectangle, nullptr );
    Shapes.AddShapeType( EOdysseyShapeType::kEllipse, nullptr );
    Shapes.AddShapeType( EOdysseyShapeType::kFreehand, nullptr );

    Shapes.SetActiveShapeType( EOdysseyShapeType::kFreehand );

    mPickHUD = static_cast<FOdysseyPainterEditorVectorSelectionToolHUD*>( mBaseHUD.Get() );
}

UOdysseyPainterEditorVectorSelectionTool::UOdysseyPainterEditorVectorSelectionTool( TSharedPtr<FOdysseyPainterEditorVectorBaseToolHUD> iHUD )
    : UOdysseyPainterEditorVectorBaseTool( iHUD, false, true )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Lasso64");

    Shapes.AddShapeType( EOdysseyShapeType::kRectangle, nullptr );
    Shapes.AddShapeType( EOdysseyShapeType::kEllipse, nullptr );
    Shapes.AddShapeType( EOdysseyShapeType::kFreehand, nullptr );

    Shapes.SetActiveShapeType( EOdysseyShapeType::kFreehand );

    mPickHUD = static_cast<FOdysseyPainterEditorVectorSelectionToolHUD*>( mBaseHUD.Get() );
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
    uint32 width = GetViewportWidth();
    uint32 height = GetViewportHeight();

    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // force redrawing when we switch tool
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    return 0;
}

uint64
UOdysseyPainterEditorVectorSelectionTool::UnloadVector( FOdysseyVectorGroupPaint* iScene )
{
    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    // Note: we use mworkingLayer because is not called in the same context as Load, so there could be
    // an orphan cell here in the case of a cell deletion for example.
    mWorkingLayer->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // force redrawing when we switch tool
    if( iScene->GetCell()->GetLayer() == mWorkingLayer )
    {
        mWorkingLayer->RequestRedraw( iScene->GetCell(), 0 );
    }

    return 0;
}

void
UOdysseyPainterEditorVectorSelectionTool::OnVectorLayerUpdate( const FOdysseyVectorObjectInvalidationFlags& iInvalidationFlags
                                                             , uint32 iUpdateFlags )
{
    if( ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::MATRIX] )
     || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::CHILD_MATRIX] )
     || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::TOPOLOGY] )
     || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::CHILD_TOPOLOGY] )
     || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::SHAPE] )
     || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::CHILD_SHAPE] )
     || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::TAG_INBETWEENER_SHAPE] )
     || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::CHILD_TAG_INBETWEENER_SHAPE] )
     || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::TAG_INBETWEENER_MATRIX] )
     || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::CHILD_TAG_INBETWEENER_MATRIX] ) )
    {
        if( mBaseHUD )
        {
            mBaseHUD->Reset();
        }
    }

    // will react to OBJECT_SELECTION and CHILD_OBJECT_SELECTION.
    // Will also Reset the HUD. the HUD in that case might be reset twice
    UOdysseyPainterEditorVectorBaseTool::OnVectorLayerUpdate( iInvalidationFlags, iUpdateFlags );
}

bool
UOdysseyPainterEditorVectorSelectionTool::OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                                           , const FOdysseyPoint& iPointInTexture
                                                           , const FKey& iKey )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        mPressedMouseCoords.x = iPointInTexture.x;
        mPressedMouseCoords.y = iPointInTexture.y;

        mPointArray.clear();

        mPointArray.push_back( ::ULIS::FVec2D( iPointInTexture.x, iPointInTexture.y ) );
    }

    //iScene->GetLayer()->RequestRedraw( 0 );

    return true;
}

void
UOdysseyPainterEditorVectorSelectionTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                                           , const FOdysseyPoint& iPointInTexture )
{
    //::ULIS::FRectI redrawRegion = { 0, 0, 0, 0 };

    if( iPointInTexture.keysDown.Find( EKeys::LeftMouseButton ) != INDEX_NONE )
    {
        ::ULIS::FVec2D point = { iPointInTexture.x, iPointInTexture.y };

        switch( Shapes.GetActiveShapeType() )
        {
            case EOdysseyShapeType::kRectangle:
            {
                mPointArray.clear();
                mPointArray.push_back( ::ULIS::FVec2D( mPressedMouseCoords.x, mPressedMouseCoords.y ) );
                mPointArray.push_back( ::ULIS::FVec2D( iPointInTexture.x    , mPressedMouseCoords.y ) );
                mPointArray.push_back( ::ULIS::FVec2D( iPointInTexture.x    , iPointInTexture.y     ) );
                mPointArray.push_back( ::ULIS::FVec2D( mPressedMouseCoords.x, iPointInTexture.y     ) );
            }
            break;

            case EOdysseyShapeType::kEllipse:
            {
                mPointArray.clear();
                mPointArray.push_back( ::ULIS::FVec2D( mPressedMouseCoords.x, mPressedMouseCoords.y ) );
                mPointArray.push_back( ::ULIS::FVec2D( iPointInTexture.x    , iPointInTexture.y     ) );
            }
            break;

            case EOdysseyShapeType::kFreehand :
                mPointArray.push_back( point );
            break;

            default:
            break;
        }
    }

    // force redraw
    //iScene->GetLayer()->RequestRedraw( iScene->GetCell(), FOdysseyVectorCell::REDRAW_INTERACTIVE );
}

void
UOdysseyPainterEditorVectorSelectionTool::OnMouseUpVectorObjectMode( FOdysseyVectorGroupPaint* iScene
                                                                   , const FOdysseyPoint& iPointInTexture
                                                                   , const FKey& iKey )
{
    std::vector<FOdysseyVectorObject*> pickedObjectArray;
    ::ULIS::FRectD roi;

    if( UOdysseyPainterEditorVectorBaseTool::DoubleClicked() == true )
    {
        FOdysseyVectorGroup* pickedGroup = nullptr;

        roi.x = iPointInTexture.x;
        roi.y = iPointInTexture.y;

        mPickHUD->SelectObject( iScene, pickedObjectArray );

        if( pickedObjectArray.size() )
        {
            FOdysseyVectorObject* pickedObject = pickedObjectArray.front();

            if( pickedObject->HasBaseClass( FOdysseyVectorGroup::StaticClass() ) )
            {
                pickedGroup = static_cast<FOdysseyVectorGroup*>(pickedObject);
            }
        }

        iScene->GetCell()->SetSelectionSpace( pickedGroup );
    }
    else
    {
        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-object-selection-tool.transaction.select-object","Vector Object Pick Tool"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoSelectObject( iScene->GetLayer()
                                                                         , iScene->GetCell() );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();

        // deselect all if control key is not pressed
        if( FSlateApplication::Get().GetModifierKeys().IsControlDown() == false )
        {
            iScene->GetCell()->ClearObjectSelection();
        }

        // dragging occured
        if ( mPointArray.size() > 1 )
        {
            mPickHUD->SelectObject( iScene, pickedObjectArray );

            // when dragging occured, we select all objects lying in the selection area.
            for ( int i = 0; i < pickedObjectArray.size(); i++ )
            {
                iScene->GetCell()->SelectObject( pickedObjectArray[i] );
            }
        }

        // no dragging occured
        if ( mPointArray.size() == 1 )
        {
            roi.x = mPointArray[0].x;
            roi.y = mPointArray[0].y;

            mPickHUD->SelectObject( iScene, pickedObjectArray );

            // if no dragging occured, we only select the object that is the most forward
            if( pickedObjectArray.size() )
            {
                iScene->GetCell()->SelectObject( pickedObjectArray.back() );
            }
        }
    }
}

void
UOdysseyPainterEditorVectorSelectionTool::OnMouseUpVectorVertexMode( FOdysseyVectorGroupPaint* iScene
                                                                   , const FOdysseyPoint& iPointInTexture
                                                                   , const FKey& iKey )
{
    std::list<FOdysseyVectorObject*> objectList;
    std::vector<FOdysseyVectorVertex*> pickedVertexArray;
    std::vector<FOdysseyVectorBucket*> pickedBucketArray;
    BLPath selectionPath;

    selectionPath.moveTo( mPointArray[0].x, mPointArray[0].y );
    for( uint32 i = 1; i < mPointArray.size(); i++ )
    {
        selectionPath.lineTo( mPointArray[i].x, mPointArray[i].y );
    }
    selectionPath.close();

    // run lambda on object tree
    FOdysseyVectorObject::Traverse
    ( iScene
    , 0
    , [ this
      , iScene
      , &selectionPath
      , &objectList
      , &pickedVertexArray
      , &pickedBucketArray ]( FOdysseyVectorObject* object, uint64 traversalFlags ) -> uint64
    {
        if( iScene->GetCell()->ObjectHasFocus( object, traversalFlags ) )
        {
            // all focused object are concerned, as their selection
            // might be cleared in case no vertex is selected.
            objectList.push_back( object );

            if( object->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
            {
                FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);

                path->PickVertex( pickedVertexArray, selectionPath );
            }

            if( object->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
            {
                FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(object);

                // note: bucket selection is invisible to the User. We nevertheless select bucket to be able to move
                // them with the vertices that are selected in the same area.
                paintGroup->PickBucket( pickedBucketArray, selectionPath );
            }

            return FOdysseyVectorObject::TRAVERSE_OBJECT_ACCEPTED; // keep traversing
        }

        return 0;
    } );

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("vector-object-selection-tool.transaction.select-vertex","Vector Vertex Pick Tool"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoSelectVertex( iScene
                                                                     , objectList );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
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
            vertex->GetOwnerAsPath()->SelectVertex( vertex );
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
}

bool
UOdysseyPainterEditorVectorSelectionTool::OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                                         , const FOdysseyPoint& iPointInTexture
                                                         , const FKey& iKey )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    FOdysseyVectorCell* vectorCell = iScene->GetCell();

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        mPickHUD->GenerateMask( mPointArray, Shapes.GetActiveShapeType() );

        if( ( mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_OBJECT    )
         || ( mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_INBETWEEN ) )
        {
            OnMouseUpVectorObjectMode( iScene, iPointInTexture, iKey );
        }

        if( mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_VERTEX )
        {
            OnMouseUpVectorVertexMode( iScene, iPointInTexture, iKey );
        }

        mPointArray.clear();
    }

    // force redraw
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->ResetHUD( iScene );

    //iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    return true;
}

std::vector<::ULIS::FVec2D>&
UOdysseyPainterEditorVectorSelectionTool::GetPointArray()
{
    return mPointArray;
}

void
UOdysseyPainterEditorVectorSelectionTool::ExtendToolbar( FToolBarBuilder& iBuilder )
{
    Super::ExtendToolbar(iBuilder);
/*
    iBuilder.BeginSection( NAME_None );

    iBuilder.AddWidget(
        SNew(SBox)
        .Padding(10.f, 0.f, 10.f, 0.f)
        [
            SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorSelectionTool, SelectionShape ), FSinglePropertyParams())
            .InnerPadding(10.f)
            .ValueWidthOverride(100.f)
        ]
    );

    iBuilder.EndSection();
*/
}

FText
UOdysseyPainterEditorVectorSelectionTool::GetTooltip() const
{
    return LOCTEXT("vector-selection-tool.tooltip", "Selection Tool");
}

#undef LOCTEXT_NAMESPACE
