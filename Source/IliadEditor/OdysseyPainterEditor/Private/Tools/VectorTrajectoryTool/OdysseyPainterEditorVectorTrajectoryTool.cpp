// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/VectorTrajectoryTool/OdysseyPainterEditorVectorTrajectoryTool.h"

#include "Brushes/SlateColorBrush.h"
#include "Editor.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "ISinglePropertyView.h"
#include "Misc/MessageDialog.h"
#include "Styling/StyleColors.h"
#include "ToolMenu.h"
#include "Widgets/Input/SSegmentedControl.h"
#include "Widgets/Layout/SBox.h"

#include "Tools/VectorTrajectoryTool/OdysseyPainterEditorVectorTrajectoryToolHUD.h"
#include "OdysseyMediaVector.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorSource.h"
#include "SOdysseySinglePropertyView.h"

// Vector engine
#include "OdysseyVector.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorTagInbetweener.h"
#include "InbetweenerTag/InbetweenerRoute.h"
#include "undo/OdysseyVectorUndoTagInbetweenerRouteAlter.h"
#include "undo/OdysseyVectorUndoTagInbetweenerTrajectoryAlter.h"
#include "undo/OdysseyVectorUndoTagInbetweenerTrajectoryShiftWaypoint.h"
#include "undo/OdysseyVectorUndoTagInbetweenerRouteAdd.h"
#include "undo/OdysseyVectorUndoTagInbetweenerRouteRemove.h"
#include "undo/OdysseyVectorUndoTagInbetweenerStepAlign.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

#define WARNING_LINEAR_ROUTE_FORBIDDEN "Trajectories are only allowed when interpolation type is set to ARAP"
#define WARNING_MODE_DEFAULT           "Use CTRL to alter the curve of the trajectory. Use SHIFT to alter spacing."
#define WARNING_MODE_CTRL              "Use CTRL to add a trajectory. Use SHIFT to alter spacing."
#define WARNING_MODE_SHIFT             "Use CTRL to alter the curve of the trajectory. Use SHIFT to add a trajectory."

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorTrajectoryTool::~UOdysseyPainterEditorVectorTrajectoryTool()
{
}

UOdysseyPainterEditorVectorTrajectoryTool::UOdysseyPainterEditorVectorTrajectoryTool()
    : UOdysseyPainterEditorVectorBaseTool(MakeShared<FOdysseyPainterEditorVectorTrajectoryToolHUD>( this ), false, true )
    , mHoveredQuad( nullptr )
    , mEditionMode( eVectorTrajectoryEditionMode::Add )
    , mUndo( nullptr )
    , ShowInbetweens( eShowInbetweens::All )
    , PickingRadius( 10.0f )
    , GridDisplayMode( eVectorTrajectoryGridDisplayMode::AsPoints )
{
    mIconStyleSet = FName(TEXT("PainterEditor.ToolsTab.Trajectory64"));
    mMouseCursor = EMouseCursor::Default;

    mTrajectoryHUD = static_cast<FOdysseyPainterEditorVectorTrajectoryToolHUD*>( mBaseHUD.Get() );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

bool
UOdysseyPainterEditorVectorTrajectoryTool::IsActivable() const
{
    uint64 HUDFlags = GetEditor()->GetVectorHUDFlags();

    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>()
          && ( HUDFlags & FOdysseyVectorHUD::HUD_MODE_INBETWEEN );
}

bool
UOdysseyPainterEditorVectorTrajectoryTool::HasRadius() const
{
    return true;
}

void
UOdysseyPainterEditorVectorTrajectoryTool::SetRadius(float iRadius)
{
    PickingRadius = iRadius;
}

float
UOdysseyPainterEditorVectorTrajectoryTool::GetRadius() const
{
    return PickingRadius;
}

uint64
UOdysseyPainterEditorVectorTrajectoryTool::UnloadVector( FOdysseyVectorGroupPaint* iScene )
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
UOdysseyPainterEditorVectorTrajectoryTool::LoadVector( FOdysseyVectorGroupPaint* iScene )
{
    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // force redrawing when we switch tool
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    return 0;
}

bool
UOdysseyPainterEditorVectorTrajectoryTool::OnKeyDownGlobalVector( FOdysseyVectorGroupPaint* iScene
                                                                , const FKeyEvent& InKeyEvent )
{
    if( InKeyEvent.IsRepeat() == false )
    {
        FKey key = InKeyEvent.GetKey();

        mEditionMode = eVectorTrajectoryEditionMode::Add;

        // Note, we could FSlateApplication::Get().GetModifierKeys() as well, but for consistency
        // with the events processing in the OnKeyUpGlobalVector(), we do like that.
        if ( ( key == EKeys::LeftControl ) || ( key == EKeys::RightControl )
          || ( key == EKeys::LeftCommand ) || ( key == EKeys::RightCommand ) )
        {
            mEditionMode = eVectorTrajectoryEditionMode::Curve;

        }

        // Note, we could FSlateApplication::Get().GetModifierKeys() as well, but for consistency
        // with the events processing in the OnKeyUpGlobalVector(), we do like that.
        if ( ( key == EKeys::LeftShift ) || ( key == EKeys::RightShift ) )
        {
            mEditionMode = eVectorTrajectoryEditionMode::Spacing;
        }
    }

    return false;
}

bool
UOdysseyPainterEditorVectorTrajectoryTool::OnKeyUpGlobalVector( FOdysseyVectorGroupPaint* iScene
                                                              , const FKeyEvent& InKeyEvent )
{
    FKey key = InKeyEvent.GetKey();

    // first reset display mode
    mEditionMode = eVectorTrajectoryEditionMode::Add;


    return false;
}

void UOdysseyPainterEditorVectorTrajectoryTool::GetMouseCursorImpl() const
{
    if( mTrajectoryHUD->GetSelectedInbetweenerTagList().size() )
    {
        FOdysseyVectorTagInbetweener* inbetweenerTag = mTrajectoryHUD->GetSelectedInbetweenerTagList().front();

        if( inbetweenerTag->GetInterpolationType() == eInbetweenerInterpolationType::Linear )
        {
            mMouseCursor = EMouseCursor::SlashedCircle;
        }
    }
}

bool
UOdysseyPainterEditorVectorTrajectoryTool::OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                                            , const FOdysseyPoint& iPointInTexture
                                                            , const FKey& iKey )
{
    //if (iKey != EKeys::LeftMouseButton)
    //    return false;
    FInbetweenerRoute* addedRoute = nullptr;

    mPickedStep = nullptr;
    mPickedHandle = nullptr;
    mHoveredQuad = nullptr;
    mPickedWaypoint = nullptr;
    mPickedRoute = nullptr;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::RightMouseButton )
    {
        mPickedRoute = mTrajectoryHUD->PickRoute( iPointInTexture.x
                                                , iPointInTexture.y
                                                , PickingRadius );
    }

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        if( mEditionMode == eVectorTrajectoryEditionMode::Add )
        {
            if( mTrajectoryHUD->GetSelectedInbetweenerTagList().size() )
            {
                FOdysseyVectorTagInbetweener* inbetweenerTag = mTrajectoryHUD->GetSelectedInbetweenerTagList().front();

                if( inbetweenerTag->GetInterpolationType() == eInbetweenerInterpolationType::ARAP )
                {
                    BLMatrix2D& ownerInverseWorldMatrix = inbetweenerTag->GetOwner()->GetInverseWorldMatrix();
                    BLPoint pt = ownerInverseWorldMatrix.map_point( iPointInTexture.x, iPointInTexture.y );

                    addedRoute = inbetweenerTag->AddRoute( ::ULIS::FVec2D( pt.x, pt.y ), true );

                    if( addedRoute )
                    {
                        mUndo = new FOdysseyVectorUndoTagInbetweenerRouteAdd( iScene
                                                                            , inbetweenerTag
                                                                            , addedRoute );

                        mUndo->Begin(); // snapshot before changes

                        // needed for valid GUndo pointer
                        GEditor->BeginTransaction(LOCTEXT("vector-trajectory-tool.transaction.add","Vector Trajectory Tool"));
                        if( GUndo )
                        {


                            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(mUndo) );

                            TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
                            if (source)
                                source->RecordCurrentFrameUndo();
                        }
                        GEditor->EndTransaction();
                    }
                }
                else
                {
                    FMessageDialog::Open( EAppMsgType::Ok
                                        , FText::FromString( TEXT ( WARNING_LINEAR_ROUTE_FORBIDDEN ) ) );
                }
            }
        }

        if( mEditionMode == eVectorTrajectoryEditionMode::Curve )
        {
            mPickedHandle = mTrajectoryHUD->PickHandle( iPointInTexture.x
                                                      , iPointInTexture.y
                                                      , PickingRadius );

            if( mPickedHandle )
            {
                mUndo = new FOdysseyVectorUndoTagInbetweenerTrajectoryAlter( iScene
                                                                           , mPickedHandle->GetTrajectory() );

                mUndo->Begin(); // snapshot before changes

                // needed for valid GUndo pointer
                GEditor->BeginTransaction(LOCTEXT("vector-trajectory-tool.transaction.alter","Vector Trajectory Tool"));
                if( GUndo )
                {
                    GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(mUndo) );

                    TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
                    if (source)
                        source->RecordCurrentFrameUndo();
                }
                GEditor->EndTransaction();
            }
            else
            {
                // pick a step (a trajectory's endpoint) to align or disalign handles
                mPickedStep = mTrajectoryHUD->PickStep( iPointInTexture.x
                                                      , iPointInTexture.y
                                                      , PickingRadius );
            }
        }

        if( mEditionMode == eVectorTrajectoryEditionMode::Spacing )
        {
            if( mTrajectoryHUD->GetSelectedInbetweenerTagList().size() )
            {
                FOdysseyVectorTagInbetweener* inbetweenerTag = mTrajectoryHUD->GetSelectedInbetweenerTagList().front();
                mPickedWaypoint = mTrajectoryHUD->PickWaypoint( inbetweenerTag
                                                              , iPointInTexture.x
                                                              , iPointInTexture.y
                                                              , PickingRadius );

                if( mPickedWaypoint )
                {
                    mUndo = new FOdysseyVectorUndoTagInbetweenerTrajectoryShiftWaypoint( iScene
                                                                                       , mPickedWaypoint->GetTrajectory() );

                    mUndo->Begin(); // snapshot before changes

                    // needed for valid GUndo pointer
                    GEditor->BeginTransaction(LOCTEXT("vector-trajectory-tool.transaction.shift","Vector Trajectory Tool"));
                    if( GUndo )
                    {
                        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(mUndo) );

                        TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
                        if (source)
                            source->RecordCurrentFrameUndo();
                    }
                    GEditor->EndTransaction();
                }
            }
        }
    }

    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    return true;
}

FInbetweenerQuad*
UOdysseyPainterEditorVectorTrajectoryTool::GetHoveredQuad()
{
    return mHoveredQuad;
}

void
UOdysseyPainterEditorVectorTrajectoryTool::ResetHoveredQuad()
{
    mHoveredQuad = nullptr;
}

void
UOdysseyPainterEditorVectorTrajectoryTool::OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                                             , const FOdysseyPoint& iPointInTexture )
{
    mHoveredQuad = nullptr;

    mTrajectoryHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );

    if( mEditionMode == eVectorTrajectoryEditionMode::Add )
    {
        // We use the breakdown list and not the inbetweener tag list because we only
        // want the trajectory to be displayed when the timeline cursor lies within the beginning
        // and the end of a breakdown
        if( mTrajectoryHUD->GetSelectedBreakdownList().size() )
        {
            FInbetweenerBreakdown* breakdown = mTrajectoryHUD->GetSelectedBreakdownList().front();
            FOdysseyVectorTagInbetweener* inbetweenerTag = breakdown->GetInbetweenerTag();

            if( inbetweenerTag->GetInterpolationType() == eInbetweenerInterpolationType::ARAP )
            {
                mHoveredQuad = mTrajectoryHUD->PickSourceQuad( inbetweenerTag->GetBreakdownList().front()->GetGrid()
                                                             , iPointInTexture.x
                                                             , iPointInTexture.y
                                                             , PickingRadius );
            }
        }
    }

    // force redraw HUD
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), FOdysseyVectorCell::REDRAW_INTERACTIVE );
}

void
UOdysseyPainterEditorVectorTrajectoryTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                                            , const FOdysseyPoint& iPointInTexture )
{
    static FVector2D deltaPositionCumul = FVector2D( 0.0f, 0.0f );

    mTrajectoryHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );

    // because we ignore some events, we need to accumulate the delta
    deltaPositionCumul += iPointInTexture.deltaPosition;

    // For some reason we receive quite a lot of mouse events between 2 screen refresh, I don't know why
    // The issue is absent with the Ink driver. It is present with the Wintab and Native drivers. The simpliest
    // solution I've found is to discard events until the screen has been refreshed.
    if( iScene->GetCell()->PendingRedraw()  )
        return;

    if( iPointInTexture.keysDown.Find( EKeys::LeftMouseButton ) != INDEX_NONE )
    {
        if( mEditionMode == eVectorTrajectoryEditionMode::Add )
        {
        }

        if( mEditionMode == eVectorTrajectoryEditionMode::Spacing )
        {
            if( mPickedWaypoint )
            {
                FInbetweenerTrajectory* trajectory = mPickedWaypoint->GetTrajectory();
                FOdysseyVectorTagInbetweener* inbetweenerTag = trajectory->GetRoute()->GetInbetweenerTag();
                ::ULIS::FVec2D* cubicBezier = trajectory->GetCubicBezier();
                double cubicT = FOdysseyVector::CubicBezierHitTest( ::ULIS::FVec2D( iPointInTexture.x, iPointInTexture.y )
                                                                  , FOdysseyVector::MapPoint( inbetweenerTag->GetOwner()->GetWorldMatrix(), cubicBezier[0] )
                                                                  , FOdysseyVector::MapPoint( inbetweenerTag->GetOwner()->GetWorldMatrix(), cubicBezier[1] )
                                                                  , FOdysseyVector::MapPoint( inbetweenerTag->GetOwner()->GetWorldMatrix(), cubicBezier[2] )
                                                                  , FOdysseyVector::MapPoint( inbetweenerTag->GetOwner()->GetWorldMatrix(), cubicBezier[3] )
                                                                  , 16
                                                                  , DBL_MAX );

                if( cubicT >= 0.0f )
                {
                    // Convert to linear t, as t value for cubic bezier is not linear but spacings are.
                    double linearT = trajectory->GetLinearT( cubicT );

                    mPickedWaypoint->SetT( linearT );
                }
            }
        }

        if( mEditionMode == eVectorTrajectoryEditionMode::Curve )
        {
            if( mPickedHandle )
            {
                FInbetweenerTrajectory* trajectory = mPickedHandle->GetTrajectory();
                BLMatrix2D ownerInverseWorldMatrix = trajectory->GetRoute()->GetInbetweenerTag()->GetOwner()->GetInverseWorldMatrix();
                uint32 bezierEndpointIndex = ( mPickedHandle == trajectory->GetHandle(0) ) ? 0 : 3;
                uint32 bezierHandleIndex   = ( mPickedHandle == trajectory->GetHandle(0) ) ? 1 : 2;
                BLPoint diff = ownerInverseWorldMatrix.map_vector( deltaPositionCumul.X
                                                                , deltaPositionCumul.Y );
                ::ULIS::FVec2D* cubicBezier = trajectory->GetCubicBezier();
                ::ULIS::FVec2D controlPosition = cubicBezier[bezierEndpointIndex];
                ::ULIS::FVec2D handlePosition = cubicBezier[bezierHandleIndex];
                ::ULIS::FVec2D direction = ( ( handlePosition + ::ULIS::FVec2D( diff.x, diff.y ) ) - controlPosition );

                if( direction.DistanceSquared() )
                {
                    double bezierLength = ( cubicBezier[3] - cubicBezier[0] ).Distance();
                    double length = direction.Distance();
                    double lengthRatio = bezierLength ? length / bezierLength : 0.0f;

                    direction.Normalize();

                    uint32 handleIndex = ( mPickedHandle == trajectory->GetHandle( 0 ) ) ? 0
                                                                                                              : 1;
                    uint32 drawingIndex = ( handleIndex == 0 ) ? trajectory->GetBreakdown()->GetSourceDrawingIndex()
                                                               : trajectory->GetBreakdown()->GetTargetDrawingIndex();
                    FInbetweenerDrawing* drawing = trajectory->GetRoute()->GetInbetweenerTag()->GetDrawing( drawingIndex );

                    mPickedHandle->Set( FOdysseyVector::MapVector( drawing->inverseMatrix, direction ), lengthRatio );
                }
            }
        }
    }

    deltaPositionCumul = FVector2D( 0.0f, 0.0f );

    // mPickedHandle->Set will invalidate the handle owner's scene, which might not be this one. Plus, when we drag
    // we pass the UPDATE_INTERACTIVE flags that will prevent inbetweens to be refreshed. Hence, will invalidate only
    // this inbetween so that it gets refreshed.
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_INTERACTIVE );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), FOdysseyVectorCell::REDRAW_INTERACTIVE );
}

bool
UOdysseyPainterEditorVectorTrajectoryTool::OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                                              , const FOdysseyPoint& iPointInTexture
                                                              , const FKey& iKey )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        if( mPickedStep )
        {
            FInbetweenerRoute* route = mPickedStep->GetRoute();
            mUndo = new FOdysseyVectorUndoTagInbetweenerStepAlign( iScene
                                                                 , route->GetInbetweenerTag()
                                                                 , route );

            mUndo->Begin(); // snapshot before changes

            // needed for valid GUndo pointer
            GEditor->BeginTransaction(LOCTEXT("vector-trajectory-tool.transaction.align","Vector Trajectory Tool"));
            if( GUndo )
            {
                GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(mUndo) );

                TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
                if (source)
                    source->RecordCurrentFrameUndo();
            }
            GEditor->EndTransaction();

            mPickedStep->SetAligned( mPickedStep->IsAligned() ? false : true );
        }

        iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
        iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

        if( mUndo )
        {
            mUndo->End(); // snapshot after changes

            mUndo = nullptr;
        }
    }

    return true;
}

void
UOdysseyPainterEditorVectorTrajectoryTool::PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
                                                                , const FName& iPropertyName )
{
    iScene->GetLayer()->ResetHUD( iScene );

    // redraw
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );
}

void
UOdysseyPainterEditorVectorTrajectoryTool::ExtendContextMenuInbetween( FOdysseyVectorGroupPaint* iScene
                                                                     , FMenuBuilder& iMenu
                                                                     , uint64 iInbetweenMenuFlags )
{
    if( mPickedRoute )
    {
        iMenu.AddMenuEntry(
              LOCTEXT("vector-trajectory-tool.context-menu.reset-trajectory.name", "Reset Trajectory")
            , LOCTEXT("vector-trajectory-tool.context-menu.reset-trajectory.tooltip", "Reset Trajectory")
            , FSlateIcon()
            , FUIAction(FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorVectorTrajectoryTool::ResetRoute )));
        iMenu.AddMenuEntry(
              LOCTEXT("vector-paint-bucket-tool.context-menu.delete-trajectory.name", "Delete Trajectory")
            , LOCTEXT("vector-paint-bucket-tool.context-menu.delete-trajectory.tooltip", "Delete Trajectory")
            , FSlateIcon()
            , FUIAction(FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorVectorTrajectoryTool::DeleteRoute )));
    }
    else
    {
        UOdysseyPainterEditorVectorBaseTool::ExtendContextMenuInbetween( iScene, iMenu, iInbetweenMenuFlags );
    }
}

void
UOdysseyPainterEditorVectorTrajectoryTool::ResetRoute()
{
    FOdysseyVectorGroupPaint* scene = mPickedRoute->GetInbetweenerTag()->GetOwner()->GetScene();
    FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerRouteAlter( scene
                                                                             , mPickedRoute->GetInbetweenerTag()
                                                                             , mPickedRoute );

    undo->Begin(); // snapshot before change

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("vector-trajectory-tool.transaction.remove","Vector Trajectory Tool"));
    if( GUndo )
    {
        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    mPickedRoute->Reset();

    scene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    scene->GetLayer()->RequestRedraw( scene->GetCell(), 0 );

    undo->End(); // snapshot after change
}

void
UOdysseyPainterEditorVectorTrajectoryTool::DeleteRoute()
{
    FOdysseyVectorGroupPaint* scene = mPickedRoute->GetInbetweenerTag()->GetOwner()->GetScene();

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("vector-trajectory-tool.transaction.remove","Vector Trajectory Tool"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerRouteRemove( scene
                                                                                  , mPickedRoute->GetInbetweenerTag()
                                                                                  , mPickedRoute );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    mPickedRoute->GetInbetweenerTag()->RemoveRoute( mPickedRoute );


    scene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    scene->GetLayer()->RequestRedraw( scene->GetCell(), 0 );
}

eVectorTrajectoryEditionMode
UOdysseyPainterEditorVectorTrajectoryTool::GetEditionMode()
{
    return mEditionMode;
}

void
UOdysseyPainterEditorVectorTrajectoryTool::SetEditionMode( eVectorTrajectoryEditionMode iMode )
{
    mEditionMode = iMode;
}

const FSlateBrush*
UOdysseyPainterEditorVectorTrajectoryTool::GetBackgroundColor( eVectorTrajectoryEditionMode iMode ) const
{
    static FSlateColorBrush selected = FSlateColorBrush( FStyleColors::Select );

    return ( iMode == mEditionMode ) ? &selected : nullptr;
}

void
UOdysseyPainterEditorVectorTrajectoryTool::OnVectorLayerUpdate( const FOdysseyVectorObjectInvalidationFlags& iInvalidationFlags
                                                              , uint32 iUpdateFlags )
{
    if( ( iUpdateFlags & FOdysseyVectorObject::UPDATE_INTERACTIVE ) == 0 )
    {
        // we need to reset when a tag is removed
        if( ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::TAG_LIST] )
         || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::CHILD_TAG_LIST] )
         || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::TAG_INBETWEENER_BREAKDOWN_LIST] )
         || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::CHILD_TAG_INBETWEENER_BREAKDOWN_LIST] ) )
        {
            if( mBaseHUD )
            {
                mBaseHUD->Reset();
            }
        }
    }

    // will react to OBJECT_SELECTION and CHILD_OBJECT_SELECTION.
    // Will also Reset the HUD. the HUD in that case might be reset twice
    UOdysseyPainterEditorVectorBaseTool::OnVectorLayerUpdate( iInvalidationFlags, iUpdateFlags );
}

TSharedRef<SWidget>
UOdysseyPainterEditorVectorTrajectoryTool::CreateModifierSegmentControl()
{
    return SNew(SSegmentedControl<eVectorTrajectoryEditionMode>)
           .Value_Lambda( [this]{ return mEditionMode; } )
           .SupportsEmptySelection( false )
           .SupportsMultiSelection( false )
           .IsEnabled( false ) // currently not clickable - Info only
           .UniformPadding( FMargin( 2, 0, 2, 0 ) )
           .OnValueChanged( SSegmentedControl<eVectorTrajectoryEditionMode>::FOnValueChanged::CreateUObject( this, &UOdysseyPainterEditorVectorTrajectoryTool::SetEditionMode ) )
           // DEFAULT
           + SSegmentedControl<eVectorTrajectoryEditionMode>::Slot( eVectorTrajectoryEditionMode::Add )
           //.Icon( FOdysseyStyle::GetBrush( "PainterEditor.ToolsShortcuts.TrajectoryAdd20") )
           .ToolTip( LOCTEXT("vector-trajectory-tool.edition-mode.default.name", "Default") )
           [
               SNew(SBorder)
               .BorderImage_UObject( this, &UOdysseyPainterEditorVectorTrajectoryTool::GetBackgroundColor, eVectorTrajectoryEditionMode::Add  )
               [
                   SNew(SImage)
                   .Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsShortcuts.TrajectoryAdd20") )
               ]
           ]
           // CTRL
           + SSegmentedControl<eVectorTrajectoryEditionMode>::Slot( eVectorTrajectoryEditionMode::Curve )
           //.Icon( FOdysseyStyle::GetBrush( "PainterEditor.ToolsShortcuts.TrajectoryEditCurve20") )
#if PLATFORM_WINDOWS
           .ToolTip( LOCTEXT("vector-trajectory-tool.edition-mode.ctrl.name", "Curve (CTRL)") )
#endif
#if PLATFORM_MAC
           .ToolTip( LOCTEXT("vector-trajectory-tool.edition-mode.cmd.name", "Adjust curve (CMD)") )
#endif
           [
               SNew(SBorder)
               .BorderImage_UObject( this, &UOdysseyPainterEditorVectorTrajectoryTool::GetBackgroundColor, eVectorTrajectoryEditionMode::Curve  )
               [
                   SNew(SImage)
                   .Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsShortcuts.TrajectoryEditCurve20") )
               ]
           ]
           // SHIFT
           + SSegmentedControl<eVectorTrajectoryEditionMode>::Slot( eVectorTrajectoryEditionMode::Spacing )
           //.Icon( FOdysseyStyle::GetBrush( "PainterEditor.ToolsShortcuts.TrajectoryEditSpacing20") )
           .ToolTip( LOCTEXT("vector-trajectory-tool.edition-mode.shift.name", "Spacing (SHIFT)") )
           [
               SNew(SBorder)
               .BorderImage_UObject( this, &UOdysseyPainterEditorVectorTrajectoryTool::GetBackgroundColor, eVectorTrajectoryEditionMode::Spacing  )
               [
                   SNew(SImage)
                   .Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsShortcuts.TrajectoryEditSpacing20") )
               ]
           ];
}

void
UOdysseyPainterEditorVectorTrajectoryTool::ExtendToolbar( UToolMenu* iToolMenu )
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
                SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorTrajectoryTool, ShowInbetweens ), FSinglePropertyParams())
                .InnerPadding(10.f)
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
                SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorTrajectoryTool, PickingRadius ), FSinglePropertyParams())
                .InnerPadding(10.f)
                .ValueWidthOverride(100.f)
            ],
            FText()
        )
    );
}

FText
UOdysseyPainterEditorVectorTrajectoryTool::GetTooltip() const
{
    return LOCTEXT("vector-trajectory-tool.tooltip", "Trajectory Tool");
}

#undef LOCTEXT_NAMESPACE
