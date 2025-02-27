// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorGridTool/OdysseyPainterEditorVectorGridTool.h"
#include "Tools/VectorGridTool/OdysseyPainterEditorVectorGridToolHUD.h"
#include "OdysseyMediaVector.h"
#include "OdysseyPainterEditor.h"
#include "ISinglePropertyView.h"
#include "OdysseyPainterEditorSource.h"
#include "Undo/OdysseyVectorUndoPointPosition.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorGroupPaint.h"
#include "SOdysseySinglePropertyView.h"
#include "Widgets/Input/SSegmentedControl.h"

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
    : UOdysseyPainterEditorVectorBaseTool( MakeShared<FOdysseyPainterEditorVectorGridToolHUD>( this ), false, true )
    , mEditionMode( eVectorGridEditionMode::Single )
    , DivisionsX( 4 )
    , DivisionsY( 4 )
    , PickingRadius( 10.0f )
    , World ( false )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Grid64");

    mGridHUD = static_cast<FOdysseyPainterEditorVectorGridToolHUD*>( mBaseHUD.Get() );
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
UOdysseyPainterEditorVectorGridTool::OnKeyDownGlobalVector( FOdysseyVectorGroupPaint* iScene
                                                          , const FKeyEvent& InKeyEvent
                                                          , uint64& oSignalFlags )
{
    if( InKeyEvent.IsRepeat() == false )
    {
        FKey key = InKeyEvent.GetKey();

        // Note, we could FSlateApplication::Get().GetModifierKeys() as well, but for consistency
        // with the events processing in the OnKeyUpGlobalVector(), we do like that.
        if ( ( key == EKeys::LeftShift ) || ( key == EKeys::RightShift ) )
        {
            mEditionMode = eVectorGridEditionMode::Multi;

            return true;
        }
    }

    return false;
}

bool
UOdysseyPainterEditorVectorGridTool::OnKeyUpGlobalVector( FOdysseyVectorGroupPaint* iScene
                                                        , const FKeyEvent& InKeyEvent
                                                        , uint64& oSignalFlags )
{
    mEditionMode = eVectorGridEditionMode::Single;

    return false;
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
        if (  mEditionMode == eVectorGridEditionMode::Multi )
        {
            mGridHUD->StartSelectionRectangle( iPointInTexture.x, iPointInTexture.y );
        }

        // single selection mode
        if (  mEditionMode == eVectorGridEditionMode::Single )
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
        if( mEditionMode == eVectorGridEditionMode::Multi )
        {
            mGridHUD->DragSelectionRectangle( iPointInTexture.x, iPointInTexture.y );
        }

        if (  mEditionMode == eVectorGridEditionMode::Single )
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
        if( mEditionMode == eVectorGridEditionMode::Multi )
        {
            mGridHUD->EndSelectionRectangle( FSlateApplication::Get().GetModifierKeys().IsControlDown() ? false : true );
        }
    }

    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    return true;
}

uint64
UOdysseyPainterEditorVectorGridTool::PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
                                                          , const FName& iPropertyName )
{
    iScene->GetCell()->ResetHUD();

    // redraw
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    return UOdysseyPainterEditorVectorBaseTool::PropertyChangedVector( iScene, iPropertyName );
}

eVectorGridEditionMode
UOdysseyPainterEditorVectorGridTool::GetEditionMode()
{
    return mEditionMode;
}

void
UOdysseyPainterEditorVectorGridTool::SetEditionMode( eVectorGridEditionMode iMode )
{
    mEditionMode = iMode;
}

const FSlateBrush*
UOdysseyPainterEditorVectorGridTool::GetBackgroundColor( eVectorGridEditionMode iMode ) const
{
    static FSlateColorBrush orange = FSlateColorBrush( FLinearColor( 1.0f, 0.5f, 0.0f, 0.5f ) );

    return ( iMode == mEditionMode ) ? &orange : nullptr;
}

TSharedRef<SWidget>
UOdysseyPainterEditorVectorGridTool::CreateModifierSegmentControl()
{
    return SNew(SSegmentedControl<eVectorGridEditionMode>)
           .Value_Lambda( [this]{ return mEditionMode; } )
           .SupportsEmptySelection( false )
           .SupportsMultiSelection( false )
           .IsEnabled( false ) // currently not clickable - Info only
           .OnValueChanged( SSegmentedControl<eVectorGridEditionMode>::FOnValueChanged::CreateUObject( this, &UOdysseyPainterEditorVectorGridTool::SetEditionMode ) )
           // DEFAULT
           + SSegmentedControl<eVectorGridEditionMode>::Slot( eVectorGridEditionMode::Single )
           //.Icon( FOdysseyStyle::GetBrush( "PainterEditor.ToolsShortcuts.GridSingleSelection20") )
           .ToolTip( LOCTEXT("vector-grid-tool.edition-mode.default.name", "Single-selection") )
           [
               SNew(SBorder)
               .BorderImage_UObject( this, &UOdysseyPainterEditorVectorGridTool::GetBackgroundColor, eVectorGridEditionMode::Single  )
               [
                   SNew(SImage)
                   .Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsShortcuts.GridSingleSelection20") )
               ]
           ]
           // SHIFT
           + SSegmentedControl<eVectorGridEditionMode>::Slot( eVectorGridEditionMode::Multi )
           //.Icon( FOdysseyStyle::GetBrush( "PainterEditor.ToolsShortcuts.GridMultipleSelection20") )
           .ToolTip( LOCTEXT("vector-grid-tool.edition-mode.shift.name", "Multi-selection (SHIFT)") )
           [
               SNew(SBorder)
               .BorderImage_UObject( this, &UOdysseyPainterEditorVectorGridTool::GetBackgroundColor, eVectorGridEditionMode::Multi  )
               [
                   SNew(SImage)
                   .Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsShortcuts.GridMultipleSelection20") )
               ]
           ];
}

void
UOdysseyPainterEditorVectorGridTool::ExtendToolbar( FToolBarBuilder& iBuilder )
{
    Super::ExtendToolbar(iBuilder);

    iBuilder.BeginSection( NAME_None );

    iBuilder.AddWidget(
        CreateModifierSegmentControl()
    );

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
