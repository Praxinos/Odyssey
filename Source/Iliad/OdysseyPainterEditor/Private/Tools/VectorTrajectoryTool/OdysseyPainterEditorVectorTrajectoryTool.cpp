// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Tools/VectorTrajectoryTool/OdysseyPainterEditorVectorTrajectoryTool.h"
#include "Tools/VectorTrajectoryTool/OdysseyPainterEditorVectorTrajectoryToolHUD.h"
#include "OdysseyMediaVector.h"
#include "OdysseyPainterEditor.h"
#include "ISinglePropertyView.h"
#include "PainterEditor/OdysseyPainterEditorSource.h"
// Vector engine
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorSharedEnv.h"
#include "OdysseyVectorTagInbetweener.h"
#include "InbetweenerTag/InbetweenerRoute.h"
#include "undo/OdysseyVectorUndoTagInbetweenerTrajectoryAlter.h"
#include "undo/OdysseyVectorUndoTagInbetweenerTrajectoryShiftWaypoint.h"
#include "undo/OdysseyVectorUndoTagInbetweenerRouteAdd.h"
#include "undo/OdysseyVectorUndoTagInbetweenerRouteRemove.h"
#include "undo/OdysseyVectorUndoTagInbetweenerStepAlign.h"

#define LOCTEXT_NAMESPACE "PainterEditor"
#define WARNING_LINEAR_ROUTE_FORBIDDEN "Trajectories are only allowed when interpolation type is set to ARAP"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorTrajectoryTool::~UOdysseyPainterEditorVectorTrajectoryTool()
{
}

UOdysseyPainterEditorVectorTrajectoryTool::UOdysseyPainterEditorVectorTrajectoryTool()
    : UOdysseyPainterEditorVectorBaseTool( new FOdysseyPainterEditorVectorTrajectoryToolHUD( this ), false )
    , PickingRadius( 10.0f )
    , mPickingMode( eTrajectoryPickingMode::Add )
    , mHoveredQuad( nullptr )
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
    iScene->GetEngine()->Invalidate( 0 );

    return 0;
}

uint64
UOdysseyPainterEditorVectorTrajectoryTool::LoadVector( FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    // force redraw
    iScene->GetEngine()->Invalidate( 0 );

    return 0;
}

bool
UOdysseyPainterEditorVectorTrajectoryTool::OnKeyDownGlobalVector( FOdysseyVectorGroupPaint* iScene
                                                                , const FKey& iKey
                                                                , uint64& oSignalFlags )
{
    uint64 notificationFlags = 0;

    mPickingMode = eTrajectoryPickingMode::Add;

    // Note, we could FSlateApplication::Get().GetModifierKeys() as well, but for consistency
    // with the events processing in the OnKeyUpGlobalVector(), we do like that.
    if ( ( iKey == EKeys::LeftControl ) || ( iKey == EKeys::RightControl )
      || ( iKey == EKeys::LeftCommand ) || ( iKey == EKeys::RightCommand ) )
    {
        mPickingMode = eTrajectoryPickingMode::Alter;
    }

    // Note, we could FSlateApplication::Get().GetModifierKeys() as well, but for consistency
    // with the events processing in the OnKeyUpGlobalVector(), we do like that.
    if ( ( iKey == EKeys::LeftShift ) || ( iKey == EKeys::RightShift ) )
    {
        mPickingMode  = eTrajectoryPickingMode::Shift;
        //mPickingFlags = FOdysseyVectorPath::PICK_HANDLE_VERTEX;
    }

    // Note, we could FSlateApplication::Get().GetModifierKeys() as well, but for consistency
    // with the events processing in the OnKeyUpGlobalVector(), we do like that.
    if ( ( iKey == EKeys::LeftAlt ) || ( iKey == EKeys::RightAlt ) )
    {
        mPickingMode = eTrajectoryPickingMode::Remove;
    }

    // redraw
    iScene->GetEngine()->Invalidate( 0 );

    oSignalFlags = notificationFlags;
    return false;
}

bool
UOdysseyPainterEditorVectorTrajectoryTool::OnKeyUpGlobalVector( FOdysseyVectorGroupPaint* iScene
                                                                  , const FKey& iKey
                                                                , uint64& oSignalFlags )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();
    uint64 notificationFlags = 0;

    // note, we cannot use FSlateApplication::Get().GetModifierKeys()
    // because the keys are already released. For consistency we do
    // the same in the KeyDown event even though we could use
    // FSlateApplication::Get().GetModifierKeys()
    if ( ( iKey == EKeys::LeftControl ) || ( iKey == EKeys::RightControl )
      || ( iKey == EKeys::LeftCommand ) || ( iKey == EKeys::RightCommand )
      || ( iKey == EKeys::LeftShift   ) || ( iKey == EKeys::RightShift   )
      || ( iKey == EKeys::LeftAlt     ) || ( iKey == EKeys::RightAlt     ) )
    {
        // redraw
        iScene->GetEngine()->Invalidate( 0 );
    }

    // first reset display mode
    mPickingMode = eTrajectoryPickingMode::Add;

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
    if (iKey != EKeys::LeftMouseButton)
        return false;
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();
    uint64 notificationFlags = 0;

    mPickedStep = nullptr;
    mPickedHandle = nullptr;
    mHoveredQuad = nullptr;
    mPickedWaypoint = nullptr;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        if( mPickingMode == eTrajectoryPickingMode::Add )
        {
            if( mTrajectoryHUD->GetSelectedInbetweenerTagList().size() )
            {
                FOdysseyVectorTagInbetweener* inbetweenerTag = mTrajectoryHUD->GetSelectedInbetweenerTagList().front();

                if( inbetweenerTag->GetInterpolationType() == eInbetweenerInterpolationType::ARAP )
                {
                    BLMatrix2D& ownerInverseWorldMatrix = inbetweenerTag->GetOwner()->GetInverseWorldMatrix();
                    BLPoint pt = ownerInverseWorldMatrix.mapPoint( iPointInTexture.x, iPointInTexture.y );
                    FInbetweenerRoute* route = inbetweenerTag->AddRoute( ::ULIS::FVec2D( pt.x, pt.y ));

                    if( route )
                    {
                        // needed for valid GUndo pointer
                        GEditor->BeginTransaction(LOCTEXT("vector-trajectory-tool.transaction.add","Vector Trajectory Tool"));
                        if( GUndo )
                        {
                            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerRouteAdd( iScene
                                                                                                    , inbetweenerTag
                                                                                                    , route
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

        if( mPickingMode == eTrajectoryPickingMode::Alter )
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

        if( mPickingMode == eTrajectoryPickingMode::Shift )
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

        if( mPickingMode == eTrajectoryPickingMode::Remove )
        {
            std::list<FInbetweenerRoute*> pickedRouteList;
            FInbetweenerRoute* pickedRoute = mTrajectoryHUD->PickRoute( iScene
                                                                      , iPointInTexture.x
                                                                      , iPointInTexture.y
                                                                      , PickingRadius );

            if( pickedRoute )
            {
                pickedRoute->GetInbetweenerTag()->RemoveRoute( pickedRoute );

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
            }
        }
    }

    // Updating via the Shared env allow multiple cells to be updated which is paramount
    // here because we may be on a cell different from the tag's starting cell
    iScene->GetSharedEnv()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    // redraw
    iScene->GetEngine()->Invalidate( FOdysseyVectorEngine::INVALIDATE_INTERACTIVE );

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

eTrajectoryPickingMode
UOdysseyPainterEditorVectorTrajectoryTool::GetPickingMode()
{
    return mPickingMode;
}

void
UOdysseyPainterEditorVectorTrajectoryTool::OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                                               , const FOdysseyPoint& iPointInTexture
                                                            , uint64& oSignalFlags )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    mHoveredQuad = nullptr;

    mTrajectoryHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );

    if( mPickingMode == eTrajectoryPickingMode::Add )
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

    // redraw
    iScene->GetEngine()->Invalidate( 0 );
}

void
UOdysseyPainterEditorVectorTrajectoryTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                                              , const FOdysseyPoint& iPointInTexture
                                                            , uint64& oSignalFlags )
{
    FOdysseyVectorEngine* engine = iScene->GetEngine();

    mTrajectoryHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );

    // For some reason we receive quite a lot of mouse events between 2 screen refresh, I don't know why
    // The issue is absent with the Ink driver. It is present with the Wintab and Native drivers. The simpliest
    // solution I've found is to discard events until the screen has been refreshed.
    if( engine->GetInvalidationFlags()  )
        return;

    if( iPointInTexture.keysDown.Find( EKeys::LeftMouseButton ) != INDEX_NONE )
    {
        if( mPickingMode == eTrajectoryPickingMode::Add )
        {
        }

        if( mPickingMode == eTrajectoryPickingMode::Shift )
        {
            if( mPickedWaypoint )
            {
                FInbetweenerTrajectory* trajectory = mPickedWaypoint->GetTrajectory();
                FOdysseyVectorTagInbetweener* inbetweenerTag = trajectory->GetRoute()->GetInbetweenerTag();
                ::ULIS::FVec2D* cubicBezier = trajectory->GetCubicBezier();
                double newT = FOdysseyVector::CubicBezierHitTest( ::ULIS::FVec2D( iPointInTexture.x, iPointInTexture.y )
                                                                  , FOdysseyVector::MapPoint( inbetweenerTag->GetOwner()->GetWorldMatrix(), cubicBezier[0] )
                                                                  , FOdysseyVector::MapPoint( inbetweenerTag->GetOwner()->GetWorldMatrix(), cubicBezier[1] )
                                                                  , FOdysseyVector::MapPoint( inbetweenerTag->GetOwner()->GetWorldMatrix(), cubicBezier[2] )
                                                                  , FOdysseyVector::MapPoint( inbetweenerTag->GetOwner()->GetWorldMatrix(), cubicBezier[3] )
                                                                  , 16 );

                //if( ( newT > prevT ) && ( newT < nextT ) )
                {
                    mPickedWaypoint->SetT( newT );
                }
            }
        }

        if( mPickingMode == eTrajectoryPickingMode::Alter )
        {
            if( mPickedHandle )
            {
                FInbetweenerTrajectory* trajectory = mPickedHandle->GetTrajectory();
                BLMatrix2D ownerInverseWorldMatrix = trajectory->GetRoute()->GetInbetweenerTag()->GetOwner()->GetInverseWorldMatrix();
                uint32 endpointIndex = ( mPickedHandle == trajectory->GetHandle(0) ) ? 0 : 3;
                uint32 handleIndex   = ( mPickedHandle == trajectory->GetHandle(0) ) ? 1 : 2;
                BLPoint diff = ownerInverseWorldMatrix.mapVector( iPointInTexture.deltaPosition.X
                                                                , iPointInTexture.deltaPosition.Y );
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

    // Updating via the Shared env allow multiple cells to be updated which is paramount
    // here because we may be on a cell different from the tag's starting cell
    iScene->GetSharedEnv()->Update( /*FOdysseyVectorObject::UPDATE_INTERACTIVE*/0 );

    // redraw
    iScene->GetEngine()->Invalidate( FOdysseyVectorEngine::INVALIDATE_INTERACTIVE );
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

    // redraw
    iScene->GetEngine()->Invalidate( 0 );

    oSignalFlags = notificationFlags;
    return true;
}

uint64
UOdysseyPainterEditorVectorTrajectoryTool::PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
                                                              , const FName& iPropertyName )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    iEngine->ResetHUD();

    // redraw
    iScene->GetEngine()->Invalidate( 0 );

    return 0;
}

TSharedRef<SWidget>
UOdysseyPainterEditorVectorTrajectoryTool::CreateTopTabWidget()
{
    FPropertyEditorModule& propertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FSinglePropertyParams defaultPropertyParams;
/*
    const TSharedPtr<ISinglePropertyView> XDivPropertyView = propertyEditorModule.CreateSingleProperty(this, "DivisionsX", defaultPropertyParams);
    const TSharedPtr<ISinglePropertyView> YDivPropertyView = propertyEditorModule.CreateSingleProperty(this, "DivisionsY", defaultPropertyParams);
    TSharedPtr<class IPropertyHandle> XDivHandle = XDivPropertyView->GetPropertyHandle();
    TSharedPtr<class IPropertyHandle> YDivHandle = YDivPropertyView->GetPropertyHandle();
*/
    return SNew(SUniformWrapPanel)
        .SlotPadding(FVector2D(3.f, 0.f))
        .EvenRowDistribution(true)
        .HAlign(HAlign_Left)
        + SUniformWrapPanel::Slot()
        [
            SNew( SOdysseyPainterEditorVectorEditionMode, GetEditor() )
        ]
/*
        + SUniformWrapPanel::Slot()
        [
            CreatePropertyWidget(XDivHandle, XDivPropertyView).ToSharedRef()
        ]
        + SUniformWrapPanel::Slot()
        [
            CreatePropertyWidget(YDivHandle, YDivPropertyView).ToSharedRef()
        ]
*/;
}

FText
UOdysseyPainterEditorVectorTrajectoryTool::GetTooltip() const
{
    return LOCTEXT("vector-matching-tool.tooltip", "Trajectory Tool");
}

#undef LOCTEXT_NAMESPACE
