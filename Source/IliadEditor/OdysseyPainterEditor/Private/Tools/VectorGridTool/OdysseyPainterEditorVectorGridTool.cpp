// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorGridTool/OdysseyPainterEditorVectorGridTool.h"
#include "Tools/VectorGridTool/OdysseyPainterEditorVectorGridToolHUD.h"
#include "OdysseyMediaVector.h"
#include "OdysseyPainterEditor.h"
#include "ISinglePropertyView.h"
#include "PainterEditor/OdysseyPainterEditorSource.h"
#include "Undo/OdysseyVectorUndoPointPosition.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorGroupPaint.h"
#include "SOdysseySinglePropertyView.h"

// testing
#include "Import/svg/OdysseyVectorImportSVG.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

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
    uint64 HUDFlags = GetEditor()->GetVectorHUDFlags();

    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>()
          && ( HUDFlags & FOdysseyVectorHUD::HUD_MODE_OBJECT
            || HUDFlags & FOdysseyVectorHUD::HUD_MODE_VERTEX );
}

uint64
UOdysseyPainterEditorVectorGridTool::UnloadVector( FOdysseyVectorGroupPaint* iScene )
{
    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // force redrawing when we switch tool
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    return 0;
}

uint64
UOdysseyPainterEditorVectorGridTool::LoadVector( FOdysseyVectorGroupPaint* iScene )
{
    mGridHUD->Export( mPointArray );

    //MakeTest( iScene );
    //#ifdef _DEBUG
    //FOdysseyVectorImportSVG svgReader( iScene, TEXT("C:\\CODE\\tiger.svg") );
    //#endif

    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // force redrawing when we switch tool
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    return 0;
}

bool
UOdysseyPainterEditorVectorGridTool::OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                                      , const FOdysseyPoint& iPointInTexture
                                                      , const FKey& iKey
                                                      , uint64& oSignalFlags )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    uint64 notificationFlags = 0;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-grid-tool.transaction.edit-grid","Vector Grid Tool"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoPointPosition( iScene, mPointArray, notificationFlags );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
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

    // Calling Update via Root will request a redraw even if root is not invalidated
    //iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_INTERACTIVE );

    return true;
}

void
UOdysseyPainterEditorVectorGridTool::OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                                       , const FOdysseyPoint& iPointInTexture
                                                       , uint64& oSignalFlags )
{
    // TODO: highlight grid handles ?
}

void
UOdysseyPainterEditorVectorGridTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                                      , const FOdysseyPoint& iPointInTexture
                                                      , uint64& oSignalFlags )
{
    uint64 notificationFlags = 0;

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
                // Note: will rezquest a redraw as well
                iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_INTERACTIVE
                                          | FOdysseyVectorObject::UPDATE_NOINBETWEENING );
            }
        }
    }

    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), FOdysseyVectorCell::REDRAW_INTERACTIVE );
}

bool
UOdysseyPainterEditorVectorGridTool::OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                                    , const FOdysseyPoint& iPointInTexture
                                                    , const FKey& iKey
                                                    , uint64& oSignalFlags )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    uint64 notificationFlags = 0;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        if( mMultipleSelectionMode == true )
        {
            mGridHUD->EndSelectionRectangle( FSlateApplication::Get().GetModifierKeys().IsControlDown() ? false : true );
        }

        mMultipleSelectionMode = false;
    }

    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( 0 );

    return true;
}

uint64
UOdysseyPainterEditorVectorGridTool::PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
                                                          , const FName& iPropertyName )
{
    iScene->GetCell()->ResetHUD();

    // redraw
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( 0 );

    return UOdysseyPainterEditorVectorSelectionTool::PropertyChangedVector( iScene, iPropertyName );
}

void
UOdysseyPainterEditorVectorGridTool::ExtendToolbar( FToolBarBuilder& iBuilder )
{
    Super::ExtendToolbar(iBuilder);

    iBuilder.BeginSection( NAME_None );

    iBuilder.AddWidget(
        SNew(SBox)
        .Padding(10.f, 0.f, 10.f, 0.f)
        [
            SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorGridTool, DivisionsX ), FSinglePropertyParams())
            .InnerPadding(10.f)
            .ValueWidthOverride(100.f)
        ]
    );

    iBuilder.AddWidget(
        SNew(SBox)
        .Padding(10.f, 0.f, 10.f, 0.f)
        [
            SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorGridTool, DivisionsY ), FSinglePropertyParams())
            .InnerPadding(10.f)
            .ValueWidthOverride(100.f)
        ]
    );

    iBuilder.EndSection();
}

FText
UOdysseyPainterEditorVectorGridTool::GetTooltip() const
{
    return LOCTEXT("vector-grid-tool.tooltip", "Grid Tool");
}

#undef LOCTEXT_NAMESPACE
