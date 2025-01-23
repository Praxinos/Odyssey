// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Tools/VectorTrajectoryTool/OdysseyPainterEditorVectorTrajectoryTool.h"
#include "Tools/VectorTrajectoryTool/OdysseyPainterEditorVectorTrajectoryToolHUD.h"
#include "OdysseyMediaVector.h"
#include "OdysseyPainterEditor.h"
#include "ISinglePropertyView.h"
#include "PainterEditor/OdysseyPainterEditorSource.h"
#include "Misc/MessageDialog.h"
#include "SOdysseySinglePropertyView.h"
// Vector engine
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorSharedEnv.h"
#include "OdysseyVectorRoot.h"
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
    : UOdysseyPainterEditorVectorBaseTool( new FOdysseyPainterEditorVectorTrajectoryToolHUD( this ), false )
    , mHoveredQuad( nullptr )
    , PickingRadius( 10.0f )
    , ShowInbetweens( true )
    , EditionMode( eTrajectoryEditionMode::Add )
    , GridDisplayMode( eTrajectoryGridDisplayMode::AsPoints )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Trajectory64");

    mTrajectoryHUD = static_cast<FOdysseyPainterEditorVectorTrajectoryToolHUD*>( mBaseHUD );
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

uint64
UOdysseyPainterEditorVectorTrajectoryTool::UnloadVector( FOdysseyVectorGroupPaint* iScene )
{
    // redraw
    iScene->GetSharedEnv()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    return 0;
}

uint64
UOdysseyPainterEditorVectorTrajectoryTool::LoadVector( FOdysseyVectorGroupPaint* iScene )
{
    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->GetSharedEnv()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    return 0;
}

bool
UOdysseyPainterEditorVectorTrajectoryTool::OnKeyDownGlobalVector( FOdysseyVectorGroupPaint* iScene
                                                                , const FKeyEvent& InKeyEvent
                                                                , uint64& oSignalFlags )
{
    if( InKeyEvent.IsRepeat() == false )
    {
        FKey key = InKeyEvent.GetKey();
        uint64 notificationFlags = 0;

        EditionModeAtKeyDown = EditionMode;

        //EditionMode = eTrajectoryEditionMode::Add;

        // Note, we could FSlateApplication::Get().GetModifierKeys() as well, but for consistency
        // with the events processing in the OnKeyUpGlobalVector(), we do like that.
        if ( ( key == EKeys::LeftControl ) || ( key == EKeys::RightControl )
          || ( key == EKeys::LeftCommand ) || ( key == EKeys::RightCommand ) )
        {
            ///EditionMode = eTrajectoryEditionMode::Curve;
            EditionMode = ( EditionMode == eTrajectoryEditionMode::Curve    ) ? eTrajectoryEditionMode::Add
                                                                              : eTrajectoryEditionMode::Curve;

        }

        // Note, we could FSlateApplication::Get().GetModifierKeys() as well, but for consistency
        // with the events processing in the OnKeyUpGlobalVector(), we do like that.
        if ( ( key == EKeys::LeftShift ) || ( key == EKeys::RightShift ) )
        {
            //EditionMode = eTrajectoryEditionMode::Spacing;
            EditionMode  = ( EditionMode == eTrajectoryEditionMode::Spacing ) ? eTrajectoryEditionMode::Add
                                                                              : eTrajectoryEditionMode::Spacing;
        }

        // force redraw
        iScene->GetEngine()->Invalidate( iScene, 0 );

        oSignalFlags = notificationFlags;
    }

    return false;
}

bool
UOdysseyPainterEditorVectorTrajectoryTool::OnKeyUpGlobalVector( FOdysseyVectorGroupPaint* iScene
                                                              , const FKeyEvent& InKeyEvent
                                                              , uint64& oSignalFlags )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();
    FKey key = InKeyEvent.GetKey();
    uint64 notificationFlags = 0;

    // note, we cannot use FSlateApplication::Get().GetModifierKeys()
    // because the keys are already released. For consistency we do
    // the same in the KeyDown event even though we could use
    // FSlateApplication::Get().GetModifierKeys()
    if ( ( key == EKeys::LeftControl ) || ( key == EKeys::RightControl )
      || ( key == EKeys::LeftCommand ) || ( key == EKeys::RightCommand )
      || ( key == EKeys::LeftShift   ) || ( key == EKeys::RightShift   )
      || ( key == EKeys::LeftAlt     ) || ( key == EKeys::RightAlt     ) )
    {
        // redraw
        iScene->GetRoot()->Invalidate(0);
        iScene->GetSharedEnv()->Update( 0 );
    }

    // first reset display mode
    EditionMode = EditionModeAtKeyDown;

    oSignalFlags = notificationFlags;
    return false;
}

EMouseCursor::Type
UOdysseyPainterEditorVectorTrajectoryTool::GetMouseCursor() const
{
    if( mTrajectoryHUD->GetSelectedInbetweenerTagList().size() )
    {
        FOdysseyVectorTagInbetweener* inbetweenerTag = mTrajectoryHUD->GetSelectedInbetweenerTagList().front();

        if( inbetweenerTag->GetInterpolationType() == eInbetweenerInterpolationType::Linear )
        {
            return EMouseCursor::SlashedCircle;
        }
    }

    return EMouseCursor::Default;
}

bool
UOdysseyPainterEditorVectorTrajectoryTool::OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                                            , const FOdysseyPoint& iPointInTexture
                                                            , const FKey& iKey
                                                            , uint64& oSignalFlags )
{
    //if (iKey != EKeys::LeftMouseButton)
    //    return false;
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();
    uint64 notificationFlags = 0;
    FInbetweenerRoute* addedRoute = nullptr;

    mPickedStep = nullptr;
    mPickedHandle = nullptr;
    mHoveredQuad = nullptr;
    mPickedWaypoint = nullptr;
    mPickedRoute = nullptr;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::RightMouseButton )
    {
        mPickedRoute = mTrajectoryHUD->PickRoute( iScene
                                                , iPointInTexture.x
                                                , iPointInTexture.y
                                                , PickingRadius );
    }

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        if( EditionMode == eTrajectoryEditionMode::Add )
        {
            if( mTrajectoryHUD->GetSelectedInbetweenerTagList().size() )
            {
                FOdysseyVectorTagInbetweener* inbetweenerTag = mTrajectoryHUD->GetSelectedInbetweenerTagList().front();

                if( inbetweenerTag->GetInterpolationType() == eInbetweenerInterpolationType::ARAP )
                {
                    BLMatrix2D& ownerInverseWorldMatrix = inbetweenerTag->GetOwner()->GetInverseWorldMatrix();
                    BLPoint pt = ownerInverseWorldMatrix.mapPoint( iPointInTexture.x, iPointInTexture.y );

                    addedRoute = inbetweenerTag->AddRoute( ::ULIS::FVec2D( pt.x, pt.y ), true );

                    if( addedRoute )
                    {
                        // needed for valid GUndo pointer
                        GEditor->BeginTransaction(LOCTEXT("vector-trajectory-tool.transaction.add","Vector Trajectory Tool"));
                        if( GUndo )
                        {
                            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerRouteAdd( iScene
                                                                                                    , inbetweenerTag
                                                                                                    , addedRoute
                                                                                                    , notificationFlags );

                            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

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

        if( EditionMode == eTrajectoryEditionMode::Curve )
        {
            mPickedHandle = mTrajectoryHUD->PickHandle( iScene
                                                      , iPointInTexture.x
                                                      , iPointInTexture.y
                                                      , PickingRadius );

            if( mPickedHandle )
            {
                // needed for valid GUndo pointer
                GEditor->BeginTransaction(LOCTEXT("vector-trajectory-tool.transaction.alter","Vector Trajectory Tool"));
                if( GUndo )
                {
                    FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerTrajectoryAlter( iScene
                                                                                                  , mPickedHandle->GetTrajectory()
                                                                                                  , notificationFlags );

                    GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

                    TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
                    if (source)
                        source->RecordCurrentFrameUndo();
                }
                GEditor->EndTransaction();
            }
            else
            {
                // pick a step (a trajectory's endpoint) to align or disalign handles
                mPickedStep = mTrajectoryHUD->PickStep( iScene
                                                      , iPointInTexture.x
                                                      , iPointInTexture.y
                                                      , PickingRadius );
            }
        }

        if( EditionMode == eTrajectoryEditionMode::Spacing )
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
                    // needed for valid GUndo pointer
                    GEditor->BeginTransaction(LOCTEXT("vector-trajectory-tool.transaction.shift","Vector Trajectory Tool"));
                    if( GUndo )
                    {
                        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerTrajectoryShiftWaypoint( iScene
                                                                                                              , mPickedWaypoint->GetTrajectory()
                                                                                                              , notificationFlags );

                        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

                        TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
                        if (source)
                            source->RecordCurrentFrameUndo();
                    }
                    GEditor->EndTransaction();
                }
            }
        }

        if( ( EditionMode == eTrajectoryEditionMode::Add     ) && ( FSlateApplication::Get().GetModifierKeys().AnyModifiersDown() == false ) )
        {
            if( addedRoute == nullptr )
            {
                FMessageDialog::Open( EAppMsgType::Ok
                                    , FText::FromString( TEXT ( WARNING_MODE_DEFAULT ) ) );
            }
        }

        if( ( EditionMode == eTrajectoryEditionMode::Curve   ) && ( FSlateApplication::Get().GetModifierKeys().AnyModifiersDown() == false ) )
        {
            if((  mPickedHandle == nullptr ) && ( mPickedStep == nullptr ) )
            {
                FMessageDialog::Open( EAppMsgType::Ok
                                    , FText::FromString( TEXT ( WARNING_MODE_CTRL ) ) );
            }
        }

        if( ( EditionMode == eTrajectoryEditionMode::Spacing ) && ( FSlateApplication::Get().GetModifierKeys().AnyModifiersDown() == false ) )
        {
           if( mPickedWaypoint == nullptr )
           {
               FMessageDialog::Open( EAppMsgType::Ok
                                   , FText::FromString( TEXT ( WARNING_MODE_SHIFT ) ) );
           }
        }

/*
        if( EditionMode == eTrajectoryEditionMode::Remove )
        {
            FInbetweenerRoute* pickedRoute = mTrajectoryHUD->PickRoute( iScene
                                                                      , iPointInTexture.x
                                                                      , iPointInTexture.y
                                                                      , PickingRadius );

            if( pickedRoute )
            {
                // needed for valid GUndo pointer
                GEditor->BeginTransaction(LOCTEXT("vector-trajectory-tool.transaction.remove","Vector Trajectory Tool"));
                if( GUndo )
                {
                    FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerRouteRemove( iScene
                                                                                              , pickedRoute->GetInbetweenerTag()
                                                                                              , pickedRoute
                                                                                              , notificationFlags );

                    GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

                    TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
                    if (source)
                        source->RecordCurrentFrameUndo();
                }
                GEditor->EndTransaction();

                pickedRoute->GetInbetweenerTag()->RemoveRoute( pickedRoute );
            }
        }
*/
    }

    // Updating via the Shared env allow multiple cells to be updated which is paramount
    // here because we may be on a cell different from the tag's starting cell
    iScene->GetSharedEnv()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    oSignalFlags = notificationFlags;

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
                                                               , const FOdysseyPoint& iPointInTexture
                                                            , uint64& oSignalFlags )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    mHoveredQuad = nullptr;

    mTrajectoryHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );

    if( EditionMode == eTrajectoryEditionMode::Add )
    {
        if( mTrajectoryHUD->GetSelectedInbetweenerTagList().size() )
        {
            FOdysseyVectorTagInbetweener* inbetweenerTag = mTrajectoryHUD->GetSelectedInbetweenerTagList().front();

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
    iScene->GetRoot()->Invalidate( 0 );
    iScene->GetSharedEnv()->Update( 0 );
}

void
UOdysseyPainterEditorVectorTrajectoryTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                                            , const FOdysseyPoint& iPointInTexture
                                                            , uint64& oSignalFlags )
{
    FOdysseyVectorEngine* engine = iScene->GetEngine();
    static FVector2D deltaPositionCumul = FVector2D( 0.0f, 0.0f );

    mTrajectoryHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );

    // because we ignore some events, we need to accumulate the delta
    deltaPositionCumul += iPointInTexture.deltaPosition;

    // For some reason we receive quite a lot of mouse events between 2 screen refresh, I don't know why
    // The issue is absent with the Ink driver. It is present with the Wintab and Native drivers. The simpliest
    // solution I've found is to discard events until the screen has been refreshed.
    if( engine->GetInvalidationFlags()  )
        return;

    if( iPointInTexture.keysDown.Find( EKeys::LeftMouseButton ) != INDEX_NONE )
    {
        if( EditionMode == eTrajectoryEditionMode::Add )
        {
        }

        if( EditionMode == eTrajectoryEditionMode::Spacing )
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

        if( EditionMode == eTrajectoryEditionMode::Curve )
        {
            if( mPickedHandle )
            {
                FInbetweenerTrajectory* trajectory = mPickedHandle->GetTrajectory();
                BLMatrix2D ownerInverseWorldMatrix = trajectory->GetRoute()->GetInbetweenerTag()->GetOwner()->GetInverseWorldMatrix();
                uint32 endpointIndex = ( mPickedHandle == trajectory->GetHandle(0) ) ? 0 : 3;
                uint32 handleIndex   = ( mPickedHandle == trajectory->GetHandle(0) ) ? 1 : 2;
                BLPoint diff = ownerInverseWorldMatrix.mapVector( deltaPositionCumul.X
                                                                , deltaPositionCumul.Y );
                ::ULIS::FVec2D* cubicBezier = trajectory->GetCubicBezier();
                ::ULIS::FVec2D controlPosition = cubicBezier[endpointIndex];
                ::ULIS::FVec2D handlePosition = cubicBezier[handleIndex];
                ::ULIS::FVec2D direction = ( ( handlePosition + ::ULIS::FVec2D( diff.x, diff.y ) ) - controlPosition );

                if( direction.DistanceSquared() )
                {
                    double bezierLength = ( cubicBezier[3] - cubicBezier[0] ).Distance();
                    double length = direction.Distance();
                    double lengthRatio = bezierLength ? length / bezierLength : 0.0f;

                    direction.Normalize();

                    mPickedHandle->Set( direction, lengthRatio );
                }
            }
        }
    }

    deltaPositionCumul = FVector2D( 0.0f, 0.0f );

    // mPickedHandle->Set will invalidate the handle owner's scene, which might not be this one. Plus, when we drag
    // we pass the UPDATE_INTERACTIVE flags that will prevent inbetweens to be refreshed. Hence, will invalidate only
    // this inbetween so that it gets refreshed.
    iScene->Invalidate( 0 );

    // Updating via the Shared env allow multiple cells to be updated which is paramount
    // here because we may be on a cell different from the tag's starting cell
    // Note: will also redraw the image
    iScene->GetSharedEnv()->Update( FOdysseyVectorObject::UPDATE_INTERACTIVE );
}

bool
UOdysseyPainterEditorVectorTrajectoryTool::OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                                              , const FOdysseyPoint& iPointInTexture
                                                              , const FKey& iKey
                                                            , uint64& oSignalFlags )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    FOdysseyVectorEngine* iEngine = iScene->GetEngine();
    uint64 notificationFlags = 0;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        if( mPickedStep )
        {
            FInbetweenerRoute* route = mPickedStep->GetRoute();

            // needed for valid GUndo pointer
            GEditor->BeginTransaction(LOCTEXT("vector-trajectory-tool.transaction.align","Vector Trajectory Tool"));
            if( GUndo )
            {
                FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerStepAlign( iScene
                                                                                        , route->GetInbetweenerTag()
                                                                                        , route
                                                                                        , notificationFlags );

                GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

                TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
                if (source)
                    source->RecordCurrentFrameUndo();
            }
            GEditor->EndTransaction();

            mPickedStep->SetAligned( mPickedStep->IsAligned() ? false : true );
        }
    }

    // Updating via the Shared env allow multiple cells to be updated which is paramount
    // here because we may be on a cell different from the tag's starting cell
    iScene->GetSharedEnv()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    oSignalFlags = notificationFlags;

    return true;
}

uint64
UOdysseyPainterEditorVectorTrajectoryTool::PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
                                                                , const FName& iPropertyName )
{
    iScene->GetRoot()->ResetHUD();

    // redraw
    iScene->GetRoot()->Invalidate(0);
    iScene->GetSharedEnv()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    return 0;
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
              LOCTEXT("vector-paint-bucket-tool.context-menu.delete-bucket.name", "Delete Trajectory")
            , LOCTEXT("vector-paint-bucket-tool.context-menu.delete-bucket.tooltip", "Delete Trajectory")
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
    uint64 notificationFlags = 0;

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("vector-trajectory-tool.transaction.remove","Vector Trajectory Tool"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerRouteAlter( scene
                                                                                 , mPickedRoute->GetInbetweenerTag()
                                                                                 , mPickedRoute
                                                                                 , notificationFlags );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    mPickedRoute->Reset();

    // Updating via the Shared env allow multiple cells to be updated which is paramount
    // here because we may be on a cell different from the tag's starting cell
    scene->GetSharedEnv()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
}

void
UOdysseyPainterEditorVectorTrajectoryTool::DeleteRoute()
{
    FOdysseyVectorGroupPaint* scene = mPickedRoute->GetInbetweenerTag()->GetOwner()->GetScene();
    uint64 notificationFlags = 0;

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("vector-trajectory-tool.transaction.remove","Vector Trajectory Tool"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerRouteRemove( scene
                                                                                  , mPickedRoute->GetInbetweenerTag()
                                                                                  , mPickedRoute
                                                                                  , notificationFlags );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    mPickedRoute->GetInbetweenerTag()->RemoveRoute( mPickedRoute );

    // Updating via the Shared env allow multiple cells to be updated which is paramount
    // here because we may be on a cell different from the tag's starting cell
    scene->GetSharedEnv()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    // redraw
    //scene->GetRoot()->GetCellEngine()->Invalidate( 0 );
}

void
UOdysseyPainterEditorVectorTrajectoryTool::ExtendToolbar( FToolBarBuilder& iBuilder )
{
    Super::ExtendToolbar(iBuilder);

    iBuilder.BeginSection( NAME_None );

    iBuilder.AddWidget(
        SNew(SBox)
        .Padding(10.f, 0.f, 10.f, 0.f)
        [
            SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorTrajectoryTool, ShowInbetweens ), FSinglePropertyParams())
            .InnerPadding(10.f)
        ]
    );

    iBuilder.AddWidget(
        SNew(SBox)
        .Padding(10.f, 0.f, 10.f, 0.f)
        [
            SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorTrajectoryTool, EditionMode ), FSinglePropertyParams())
            .InnerPadding(10.f)
            .ValueWidthOverride(100.f)
        ]
    );

    iBuilder.EndSection();
}

FText
UOdysseyPainterEditorVectorTrajectoryTool::GetTooltip() const
{
    return LOCTEXT("vector-matching-tool.tooltip", "Trajectory Tool");
}

#undef LOCTEXT_NAMESPACE
