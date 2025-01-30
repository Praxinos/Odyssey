// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorSelectionTool/OdysseyPainterEditorVectorSelectionTool.h"
#include "Tools/VectorSelectionTool/OdysseyPainterEditorVectorSelectionToolHUD.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseToolHUD.h"
#include "OdysseyPainterEditor.h"
#include "PainterEditor/OdysseyPainterEditorSource.h"
#include "PainterEditor/OdysseyPainterEditorViewportTab.h"
#include "PainterEditor/OdysseyPainterEditorSource.h"
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
    : UOdysseyPainterEditorVectorBaseTool( new FOdysseyPainterEditorVectorSelectionToolHUD( this ), false )
    , SelectionShape( EOdysseyVectorSelectionShape::Freehand )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Lasso64");

    mPickHUD = static_cast<FOdysseyPainterEditorVectorSelectionToolHUD*>( mBaseHUD );
}

UOdysseyPainterEditorVectorSelectionTool::UOdysseyPainterEditorVectorSelectionTool( FOdysseyPainterEditorVectorBaseToolHUD* iHUD )
    : UOdysseyPainterEditorVectorBaseTool( iHUD, false )
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
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // force redrawing when we switch tool
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    return 0;
}

uint64
UOdysseyPainterEditorVectorSelectionTool::UnloadVector( FOdysseyVectorGroupPaint* iScene )
{
    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // force redrawing when we switch tool
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    return 0;
}

bool
UOdysseyPainterEditorVectorSelectionTool::OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                                           , const FOdysseyPoint& iPointInTexture
                                                           , const FKey& iKey
                                                           , uint64& oSignalFlags )
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
                                                           , const FOdysseyPoint& iPointInTexture
                                                           , uint64& oSignalFlags )
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

    // force redraw
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), FOdysseyVectorCell::REDRAW_INTERACTIVE );
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

uint64
UOdysseyPainterEditorVectorSelectionTool::OnMouseUpVectorObjectMode( FOdysseyVectorGroupPaint* iScene
                                                                   , const FOdysseyPoint& iPointInTexture
                                                                   , const FKey& iKey )
{
    std::vector<FOdysseyVectorObject*> pickedObjectArray;
    ::ULIS::FRectD roi;
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyPainterEditor::UI_UPDATE_TIMELINE
                             | FOdysseyPainterEditor::UI_UPDATE_HUD;

    if( UOdysseyPainterEditorVectorBaseTool::DoubleClicked() == true )
    {
        FOdysseyVectorGroup* pickedGroup = nullptr;

        roi.x = iPointInTexture.x;
        roi.y = iPointInTexture.y;

        iScene->GetCell()->Pick( iScene, roi, pickedObjectArray, FOdysseyVectorObject::PICK_MATH_BASED );

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
                                                                         , iScene->GetCell()
                                                                         , notificationFlags );

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
            iScene->GetCell()->Pick( iScene, roi, pickedObjectArray, FOdysseyVectorObject::PICK_MASK_BASED );

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

            iScene->GetCell()->Pick( iScene, roi, pickedObjectArray, FOdysseyVectorObject::PICK_MATH_BASED );

            // if no dragging occured, we only select the object that is the most forward
            if( pickedObjectArray.size() )
            {
                iScene->GetCell()->SelectObject( pickedObjectArray.back() );
            }
        }
    }

    return notificationFlags;
}

uint64
UOdysseyPainterEditorVectorSelectionTool::OnMouseUpVectorVertexMode( FOdysseyVectorGroupPaint* iScene
                                                                   , const FOdysseyPoint& iPointInTexture
                                                                   , const FKey& iKey )
{
    std::list<FOdysseyVectorObject*> objectList;
    std::vector<FOdysseyVectorVertex*> pickedVertexArray;
    std::vector<FOdysseyVectorBucket*> pickedBucketArray;
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyPainterEditor::UI_UPDATE_TIMELINE
                             | FOdysseyPainterEditor::UI_UPDATE_HUD;

    // run lambda on object tree
    FOdysseyVectorObject::Traverse
    ( iScene
    , 0
    , [ this
      , iScene
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

                path->PickVertex( pickedVertexArray );
            }

            if( object->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
            {
                FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(object);

                paintGroup->PickBucket( pickedBucketArray );
            }

            return FOdysseyVectorObject::TRAVERSE_OBJECT_ACCEPTED; // keep traversing
        }

        return 0;
    } );

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("vector-object-selection-tool.transaction.select-vertex","Vector Vertex Pick Tool"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoSelectVertex( iScene, objectList, notificationFlags );

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

    return notificationFlags;
}

bool
UOdysseyPainterEditorVectorSelectionTool::OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                                         , const FOdysseyPoint& iPointInTexture
                                                         , const FKey& iKey
                                                         , uint64& oSignalFlags )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    FOdysseyVectorCell* vectorCell = iScene->GetCell();
    ::ULIS::FRectD roi;
    uint64 notificationFlags = 0;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        roi = GenerateMask();

        // TODO: pass the mask image as arg to Pick function
        vectorCell->SetBLMask( mPickHUD->GetMask() );
        if( ( mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_OBJECT    )
         || ( mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_INBETWEEN ) )
        {
            notificationFlags |= OnMouseUpVectorObjectMode( iScene, iPointInTexture, iKey );
        }

        if( mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_VERTEX )
        {
            notificationFlags |= OnMouseUpVectorVertexMode( iScene, iPointInTexture, iKey );
        }
        vectorCell->SetBLMask( nullptr );

        mPointArray.clear();
    }

    // force redraw
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetCell()->ResetHUD();
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    oSignalFlags = notificationFlags;

    return true;
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
UOdysseyPainterEditorVectorSelectionTool::ExtendToolbar( FToolBarBuilder& iBuilder )
{
    Super::ExtendToolbar(iBuilder);

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
}

FText
UOdysseyPainterEditorVectorSelectionTool::GetTooltip() const
{
    return LOCTEXT("vector-selection-tool.tooltip", "Selection Tool");
}

#undef LOCTEXT_NAMESPACE
