// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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
    mIconStyleSet = FName(TEXT("PainterEditor.ToolsTab.Grid64"));

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

bool
UOdysseyPainterEditorVectorGridTool::HasRadius() const
{
    return true;
}

void
UOdysseyPainterEditorVectorGridTool::SetRadius(float iRadius)
{
    PickingRadius = iRadius;
}

float
UOdysseyPainterEditorVectorGridTool::GetRadius() const
{
    return PickingRadius;
}

uint64
UOdysseyPainterEditorVectorGridTool::UnloadVector( FOdysseyVectorGroupPaint* iScene )
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
                                                          , const FKeyEvent& InKeyEvent )
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
                                                        , const FKeyEvent& InKeyEvent )
{
    mEditionMode = eVectorGridEditionMode::Single;

    return false;
}

bool
UOdysseyPainterEditorVectorGridTool::NodesAlreadySelected( std::vector<FGridNode *>& iNodeArray )
{
    for( FGridNode* node : iNodeArray )
    {
        if( node->IsSelected() == false )
        {
            return false;
        }
    }

    return true;
}

void
UOdysseyPainterEditorVectorGridTool::OnVectorLayerUpdate( const FOdysseyVectorObjectInvalidationFlags& iInvalidationFlags
                                                        , uint32 iUpdateFlags )
{
    if( ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::MATRIX] )
     || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::CHILD_MATRIX] )
     || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::TOPOLOGY] )
     || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::CHILD_TOPOLOGY] ) )
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
UOdysseyPainterEditorVectorGridTool::OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                                      , const FOdysseyPoint& iPointInTexture
                                                      , const FKey& iKey )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-grid-tool.transaction.edit-grid","Vector Grid Tool"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoPointPosition( iScene
                                                                          , mPointArray );

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
            std::vector<FGridNode *> nodeArray;

            mGridHUD->PickNodes( iPointInTexture.x
                               , iPointInTexture.y
                               , PickingRadius
                               , nodeArray );

            if( FSlateApplication::Get().GetModifierKeys().IsControlDown() == false )
            {
                if( NodesAlreadySelected( nodeArray ) == false )
                {
                    mGridHUD->ClearSelection();
                }
            }

            for( FGridNode* node : nodeArray )
            {
                node->SetSelected( true );
            }
        }
    }

    // Calling Update via Root will request a redraw even if root is not invalidated
    //iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_INTERACTIVE );

    return true;
}

void
UOdysseyPainterEditorVectorGridTool::OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                                       , const FOdysseyPoint& iPointInTexture )
{
    // TODO: highlight grid handles ?
}

void
UOdysseyPainterEditorVectorGridTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                                      , const FOdysseyPoint& iPointInTexture )
{
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
                BLPoint spaceDif = selectionBox.inverseWorldMatrix.map_vector( iPointInTexture.deltaPosition.X
                                                                            , iPointInTexture.deltaPosition.Y );
                std::vector<FGridNode *> nodeArray;

                mGridHUD->GetSelection( nodeArray );

                for( int i = 0; i < nodeArray.size(); i++ )
                {
                    nodeArray[i]->Set( nodeArray[i]->GetX() + spaceDif.x
                                     , nodeArray[i]->GetY() + spaceDif.y );
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
                                                    , const FKey& iKey )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        if( mEditionMode == eVectorGridEditionMode::Multi )
        {
            std::vector<FGridNode *> nodeArray;

            mGridHUD->EndSelectionRectangle( nodeArray );

            if( nodeArray.size() && ( FSlateApplication::Get().GetModifierKeys().IsControlDown() == false ) )
            {
                mGridHUD->ClearSelection();
            }

            for( FGridNode* node : nodeArray )
            {
                node->SetSelected( true );
            }
        }
    }

    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    return true;
}

bool
UOdysseyPainterEditorVectorGridTool::OnMouseClickVector( FOdysseyVectorGroupPaint* iScene
                                                       , const FOdysseyPoint& iPointInTexture
                                                       , const FKey& iKey )
{
    mGridHUD->ClearSelection();

    return false;
}

void
UOdysseyPainterEditorVectorGridTool::PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
                                                          , const FName& iPropertyName )
{
    iScene->GetLayer()->ResetHUD( iScene );

    // redraw
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    UOdysseyPainterEditorVectorBaseTool::PropertyChangedVector( iScene, iPropertyName );
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
    static FSlateColorBrush selected = FSlateColorBrush( FStyleColors::Select );

    return ( iMode == mEditionMode ) ? &selected : nullptr;
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
UOdysseyPainterEditorVectorGridTool::ExtendToolbar( UToolMenu* iToolMenu )
{
    Super::ExtendToolbar(iToolMenu);


    FToolMenuSection& section = iToolMenu->AddSection(NAME_None);
    section.AddEntry(
        FToolMenuEntry::InitWidget(
            NAME_None,
            CreateModifierSegmentControl(),
            FText()
        )
    );

    section.AddEntry(
        FToolMenuEntry::InitWidget(
            NAME_None,
            SNew(SBox)
            .Padding(10.f, 0.f, 10.f, 0.f)
            [
                SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorGridTool, DivisionsX ), FSinglePropertyParams())
                .InnerPadding(10.f)
                .ValueWidthOverride(100.f)
            ],
            FText()
        )
    );

    section.AddEntry(
        FToolMenuEntry::InitWidget(
            NAME_None,
            SNew(SBox)
            .Padding(10.f, 0.f, 10.f, 0.f)
            [
                SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorGridTool, DivisionsY ), FSinglePropertyParams())
                .InnerPadding(10.f)
                .ValueWidthOverride(100.f)
            ],
            FText()
        )
    );
}

FText
UOdysseyPainterEditorVectorGridTool::GetTooltip() const
{
    return LOCTEXT("vector-grid-tool.tooltip", "Grid Tool");
}

#undef LOCTEXT_NAMESPACE
