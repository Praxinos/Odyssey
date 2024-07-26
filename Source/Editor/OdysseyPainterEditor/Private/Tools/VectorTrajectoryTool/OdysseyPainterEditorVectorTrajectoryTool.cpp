// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorTrajectoryTool/OdysseyPainterEditorVectorTrajectoryTool.h"
#include "Tools/VectorTrajectoryTool/OdysseyPainterEditorVectorTrajectoryToolHUD.h"
#include "OdysseyMediaVector.h"
#include "OdysseyPainterEditor.h"
#include "ISinglePropertyView.h"
#include "PainterEditor/OdysseyPainterEditorSource.h"
// Vector engine
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorTagInbetweener.h"
#include "InbetweenerTag/InbetweenerRoute.h"
#include "undo/OdysseyVectorUndoTagInbetweenerTrajectoryAlter.h"
#include "undo/OdysseyVectorUndoTagInbetweenerTrajectoryShiftWaypoint.h"
#include "undo/OdysseyVectorUndoTagInbetweenerTrajectoryAdd.h"
#include "undo/OdysseyVectorUndoTagInbetweenerTrajectoryRemove.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

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
    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorTrajectoryTool::LoadVector( FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorTrajectoryTool::OnKeyDownGlobalVector( FOdysseyVectorGroupPaint* iScene
                                                                , const FKey& iKey )
{
    uint64 retFlags = 0;

    mPickingMode = eTrajectoryPickingMode::Add;

    // Note, we could FSlateApplication::Get().GetModifierKeys() as well, but for consistency
    // with the events processing in the OnKeyUpGlobalVector(), we do like that.
    if ( ( iKey == EKeys::LeftControl ) || ( iKey == EKeys::RightControl )
      || ( iKey == EKeys::LeftCommand ) || ( iKey == EKeys::RightCommand ) )
    {
        mPickingMode = eTrajectoryPickingMode::Alter;

        retFlags = FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
    }

    // Note, we could FSlateApplication::Get().GetModifierKeys() as well, but for consistency
    // with the events processing in the OnKeyUpGlobalVector(), we do like that.
    if ( ( iKey == EKeys::LeftShift ) || ( iKey == EKeys::RightShift ) )
    {
        mPickingMode  = eTrajectoryPickingMode::Shift;
        //mPickingFlags = FOdysseyVectorPath::PICK_HANDLE_VERTEX;

        retFlags = FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
    }

    // Note, we could FSlateApplication::Get().GetModifierKeys() as well, but for consistency
    // with the events processing in the OnKeyUpGlobalVector(), we do like that.
    if ( ( iKey == EKeys::LeftAlt ) || ( iKey == EKeys::RightAlt ) )
    {
        mPickingMode = eTrajectoryPickingMode::Remove;

        retFlags = FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
    }

    return UOdysseyPainterEditorVectorBaseTool::OnKeyDownGlobalVector( iScene, iKey )
         | retFlags;
}

uint64
UOdysseyPainterEditorVectorTrajectoryTool::OnKeyUpGlobalVector( FOdysseyVectorGroupPaint* iScene
                                                              , const FKey& iKey )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();
    uint64 retFlags = 0;

    // note, we cannot use FSlateApplication::Get().GetModifierKeys()
    // because the keys are already released. For consistency we do
    // the same in the KeyDown event even though we could use 
    // FSlateApplication::Get().GetModifierKeys()
    if ( ( iKey == EKeys::LeftControl ) || ( iKey == EKeys::RightControl )
      || ( iKey == EKeys::LeftCommand ) || ( iKey == EKeys::RightCommand )
      || ( iKey == EKeys::LeftShift   ) || ( iKey == EKeys::RightShift   )
      || ( iKey == EKeys::LeftAlt     ) || ( iKey == EKeys::RightAlt     ) )
    {
        retFlags = FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
    }

    // first reset display mode
    mPickingMode = eTrajectoryPickingMode::Add;

    return UOdysseyPainterEditorVectorBaseTool::OnKeyUpGlobalVector( iScene, iKey )
         | retFlags;
}



uint64
UOdysseyPainterEditorVectorTrajectoryTool::OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                                            , const FOdysseyPoint& iPointInTexture
                                                            , const FKey& iKey )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    mPickedHandleList.clear();
    mHoveredQuad = nullptr;
    mPickedWaypoint = nullptr;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        if( mPickingMode == eTrajectoryPickingMode::Add )
        {
            if( iEngine->GetSelectedObjectList().size() )
            {
                FOdysseyVectorObject* selectedObject = iEngine->GetSelectedObjectList().front();
                FOdysseyVectorTag* tag = selectedObject->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() );

                if( tag )
                {
                    FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);
                    BLMatrix2D& ownerInverseWorldMatrix = inbetweenerTag->GetOwner()->GetInverseWorldMatrix();
                    BLPoint pt = ownerInverseWorldMatrix.mapPoint( iPointInTexture.x, iPointInTexture.y );
                    FInbetweenerRoute* route = inbetweenerTag->AddRoute( ::ULIS::FVec2D( pt.x, pt.y ));

                    if( route )
                    {
/*--------------
                        // needed for valid GUndo pointer
                        GEditor->BeginTransaction(LOCTEXT("vector-trajectory-tool.transaction.add","Vector Trajectory Tool"));
                        if( GUndo )
                        {
                            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerTrajectoryAdd( iScene, inbetweenerTag, trajectory );

                            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

                            TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
                            if (source)
                                source->RecordCurrentFrameUndo();
                        }
                        GEditor->EndTransaction();
*/
                    }
                }
            }
        }

        if( mPickingMode == eTrajectoryPickingMode::Alter )
        {
            mTrajectoryHUD->PickHandle( iScene
                                      , iPointInTexture.x
                                      , iPointInTexture.y
                                      , PickingRadius
                                      , mPickedHandleList );

            if( mPickedHandleList.size() )
            {
                // needed for valid GUndo pointer
                GEditor->BeginTransaction(LOCTEXT("vector-trajectory-tool.transaction.alter","Vector Trajectory Tool"));
                if( GUndo )
                {
                    FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerTrajectoryAlter( iScene, mPickedHandleList );

                    GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

                    TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
                    if (source)
                        source->RecordCurrentFrameUndo();
                }
                GEditor->EndTransaction();
            }
        }

        if( mPickingMode == eTrajectoryPickingMode::Shift )
        {
            if( iEngine->GetSelectedObjectList().size() )
            {
                FOdysseyVectorObject* selectedObject = iEngine->GetSelectedObjectList().front();
                FOdysseyVectorTag* tag = selectedObject->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() );

                if( tag )
                {
                    FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);

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
                            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerTrajectoryShiftWaypoint( iScene, mPickedWaypoint->GetTrajectory() );

                            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

                            TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
                            if (source)
                                source->RecordCurrentFrameUndo();
                        }
                        GEditor->EndTransaction();
                    }
                }
            }
        }

        if( mPickingMode == eTrajectoryPickingMode::Remove )
        {
            std::list<FInbetweenerRoute*> pickedRouteList;
            mTrajectoryHUD->PickRoute( iScene
                                     , iPointInTexture.x
                                     , iPointInTexture.y
                                     , PickingRadius
                                     , pickedRouteList );

            for( FInbetweenerRoute* route : pickedRouteList )
            {
                route->GetInbetweenerTag()->RemoveRoute( route );
            }

            // needed for valid GUndo pointer
            GEditor->BeginTransaction(LOCTEXT("vector-trajectory-tool.transaction.remove","Vector Trajectory Tool"));
            if( GUndo )
            {
/*----------
                FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerTrajectoryRemove( iScene, pickedRouteList );

                GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

                TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
                if (source)
                    source->RecordCurrentFrameUndo();
*/
            }
            GEditor->EndTransaction();
        }
    }

    iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;
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

uint64
UOdysseyPainterEditorVectorTrajectoryTool::OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                                       , const FOdysseyPoint& iPointInTexture )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    mHoveredQuad = nullptr;

    mTrajectoryHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );

    if( mPickingMode == eTrajectoryPickingMode::Add )
    {
        if( iEngine->GetSelectedObjectList().size() )
        {
            FOdysseyVectorObject* selectedObject = iEngine->GetSelectedObjectList().front();
            FOdysseyVectorTag* tag = selectedObject->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() );

            if( tag )
            {
                FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);

                mHoveredQuad = mTrajectoryHUD->PickSourceQuad( inbetweenerTag->GetBreakdownList().front()->GetGrid()
                                                             , iPointInTexture.x
                                                             , iPointInTexture.y
                                                             , PickingRadius );

            }
        }
    }

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorTrajectoryTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                                          , const FOdysseyPoint& iPointInTexture )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    mTrajectoryHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );

    if( iPointInTexture.keysDown.Find( EKeys::LeftMouseButton ) != INDEX_NONE )
    {
        if( mPickingMode == eTrajectoryPickingMode::Add )
        {
        }

        if( mPickingMode == eTrajectoryPickingMode::Shift )
        {
            if( mPickedWaypoint )
            {
/*--------------
                FInbetweenerTrajectory* trajectory = mPickedWaypoint->GetTrajectory();
                FOdysseyVectorTagInbetweener* inbetweenerTag = trajectory->GetInbetweenerTag();

                ::ULIS::FVec2D* cubicBezier = trajectory->GetCubicBezier();

                double newT = FOdysseyVector::BezierHitTest( ::ULIS::FVec2D( iPointInTexture.x, iPointInTexture.y )
                                                           , FOdysseyVector::MapPoint( inbetweenerTag->GetOwner()->GetWorldMatrix(), cubicBezier[0] )
                                                           , FOdysseyVector::MapPoint( inbetweenerTag->GetOwner()->GetWorldMatrix(), cubicBezier[1] )
                                                           , FOdysseyVector::MapPoint( inbetweenerTag->GetOwner()->GetWorldMatrix(), cubicBezier[2] )
                                                           , FOdysseyVector::MapPoint( inbetweenerTag->GetOwner()->GetWorldMatrix(), cubicBezier[3] )
                                                           , 16 );

                //if( ( newT > prevT ) && ( newT < nextT ) )
                {
                    mPickedWaypoint->SetT( newT );
                }
*/
            }
        }

        if( mPickingMode == eTrajectoryPickingMode::Alter )
        {
            for( FInbetweenerHandleTrajectory* trajectoryHandle : mPickedHandleList )
            {
                FInbetweenerTrajectory* trajectory = trajectoryHandle->GetTrajectory();
                BLMatrix2D ownerInverseWorldMatrix = trajectory->GetRoute()->GetInbetweenerTag()->GetOwner()->GetInverseWorldMatrix();
                uint32 endpointIndex = ( trajectoryHandle == trajectory->GetHandle(0) ) ? 0 : 3;
                uint32 handleIndex   = ( trajectoryHandle == trajectory->GetHandle(0) ) ? 1 : 2;
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

                    trajectoryHandle->Set( direction, lengthRatio );
                }
            }
        }
    }

    iScene->Update( FOdysseyVectorObject::UPDATE_INTERACTIVE );

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;
}

uint64
UOdysseyPainterEditorVectorTrajectoryTool::OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , const FKey& iKey )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        if( iEngine->GetSelectedObjectList().size() )
        {
            FOdysseyVectorObject* selectedObject = iEngine->GetSelectedObjectList().front();
            FOdysseyVectorTag* tag = selectedObject->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() );
            FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);

            if( inbetweenerTag )
            {

            }
        }
    }

    iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorTrajectoryTool::PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
                                                              , const FName& iPropertyName )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    iEngine->ResetHUD();

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
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
