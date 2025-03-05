// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPaintBucketTool/OdysseyPainterEditorVectorPaintBucketTool.h"
#include "Tools/VectorPaintBucketTool/OdysseyPainterEditorVectorPaintBucketToolHUD.h"

#include "OdysseyMediaVector.h"
#include "Palette/OdysseyPaletteEntryColor.h"
#include "OdysseyPainterEditorViewportTab.h"
#include "OdysseyPainterEditorVectorBucketView.h"
#include "OdysseyPainterEditor.h"
#include "ISinglePropertyView.h"
#include "OdysseyPainterEditorSource.h"
#include "OdysseyPalette.h"
#include "OdysseyPaletteEntry.h"

#include "OdysseyVector.h"
#include "OdysseyVectorCycle.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorBucket.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorLayer.h"
#include "Undo/OdysseyVectorUndoPointPosition.h"
#include "Undo/OdysseyVectorUndoBucketAdd.h"
#include "Undo/OdysseyVectorUndoBucketRemove.h"
#include "Undo/OdysseyVectorUndoBucketParam.h"

#include "SOdysseySinglePropertyView.h"
#include "Widgets/Input/SSegmentedControl.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPaintBucketTool::~UOdysseyPainterEditorVectorPaintBucketTool()
{
}

UOdysseyPainterEditorVectorPaintBucketTool::UOdysseyPainterEditorVectorPaintBucketTool()
    : UOdysseyPainterEditorVectorBaseTool( MakeShared<FOdysseyPainterEditorVectorPaintBucketToolHUD>( this ), false, true )
    , Propagate( true )
    , ColorMode ( EPaintBucketToolColorMode::Color )
    , Opacity( 1.0f )
    , Color1( 255, 255, 255, 255 )
    , Color2( 255, 255, 255, 255 )
    , PickingRadius( 20.0f )
    , mPickedBucket( nullptr )
    , mEditionMode( eVectorPaintBucketEditionMode::Default )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PaintBucket64");

    mBucketHUD = static_cast<FOdysseyPainterEditorVectorPaintBucketToolHUD*>( mBaseHUD.Get() );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

bool
UOdysseyPainterEditorVectorPaintBucketTool::IsActivable() const
{
    uint64 HUDFlags = GetEditor()->GetVectorHUDFlags();

    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>()
          && ( HUDFlags & FOdysseyVectorHUD::HUD_MODE_OBJECT
            || HUDFlags & FOdysseyVectorHUD::HUD_MODE_VERTEX );
}

uint64
UOdysseyPainterEditorVectorPaintBucketTool::UnloadVector( FOdysseyVectorGroupPaint* iScene )
{
    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // force redrawing when we switch tool
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    return 0;
}

uint64
UOdysseyPainterEditorVectorPaintBucketTool::LoadVector( FOdysseyVectorGroupPaint* iScene )
{
    TSharedPtr< SViewport > viewportWidget; // to force keyboard focus on mouse hover.
                                            // Prevents the user from having to click at least once in the viewport.
    // we need the focus on the viewport for keyboard
    TSharedPtr<FOdysseyPainterEditorViewportTab> viewportTab = GetEditor()->FindTab<FOdysseyPainterEditorViewportTab>();
    viewportWidget = viewportTab->GetViewport()->GetViewportWidget();

    // we need the focus on the viewport for keyboard
    FSlateApplication::Get().SetKeyboardFocus( viewportWidget );

    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // force redrawing when we switch tool
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    return 0;
}

bool
UOdysseyPainterEditorVectorPaintBucketTool::OnKeyDownGlobalVector( FOdysseyVectorGroupPaint* iScene
                                                                 , const FKeyEvent& InKeyEvent
                                                                 , uint64& oSignalFlags )
{
    if( InKeyEvent.IsRepeat() == false )
    {
        FKey key = InKeyEvent.GetKey();

        // Note, we could FSlateApplication::Get().GetModifierKeys() as well, but for consistency
        // with the events processing in the OnKeyUpGlobalVector(), we do like that.
        if ( ( key == EKeys::LeftControl ) || ( key == EKeys::RightControl )
          || ( key == EKeys::LeftCommand ) || ( key == EKeys::RightCommand ) )
        {
            mEditionMode = eVectorPaintBucketEditionMode::Control;

            return true;
        }

        // Note, we could FSlateApplication::Get().GetModifierKeys() as well, but for consistency
        // with the events processing in the OnKeyUpGlobalVector(), we do like that.
        if ( ( key == EKeys::LeftAlt ) || ( key == EKeys::RightAlt ) )
        {
            mEditionMode = eVectorPaintBucketEditionMode::Remove;

            return true;
        }
    }

    return false;
}

bool
UOdysseyPainterEditorVectorPaintBucketTool::OnKeyUpGlobalVector( FOdysseyVectorGroupPaint* iScene
                                                               , const FKeyEvent& InKeyEvent
                                                               , uint64& oSignalFlags )
{
    mEditionMode = eVectorPaintBucketEditionMode::Default;

    return false;
}

bool
UOdysseyPainterEditorVectorPaintBucketTool::OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                                             , const FOdysseyPoint& iPointInTexture
                                                             , const FKey& iKey
                                                             , uint64& oSignalFlags )
{
    uint64 retFlags = 0;

    // valid for boith right and left clicks
    mPickedBucket = mBucketHUD->PickBucket( iPointInTexture.x, iPointInTexture.y );

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        mDownMouseX = mOldPointInTexture.x = iPointInTexture.x;
        mDownMouseY = mOldPointInTexture.y = iPointInTexture.y;

        mPickedArea = FOdysseyPainterEditorVectorPaintBucketToolHUD::PICK_NONE;

        if( mPickedBucket )
        {
            mPickedArea = mBucketHUD->PickBucketArea( mPickedBucket, iPointInTexture.x, iPointInTexture.y );
        }

        if( mPickedBucket )
        {
            FOdysseyVectorObject* bucketOwner = mPickedBucket->GetOwner();
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(bucketOwner);

            switch( mPickedArea )
            {
                // assuming bucket will be moved by the user
                case FOdysseyPainterEditorVectorPaintBucketToolHUD::PICK_BUCKET :
                    // needed for valid GUndo pointer
                    GEditor->BeginTransaction(LOCTEXT("vector-paint-bucket-tool.transaction.move-bucket","Paint Bucket"));
                    if( GUndo )
                    {
                        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoPointPosition( iScene
                                                                                      , mPickedBucket
                                                                                      , retFlags );

                        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

                        TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
                        if (source)
                            source->RecordCurrentFrameUndo();
                    }
                    GEditor->EndTransaction();

                    mPointPosition.x = mPickedBucket->GetX();
                    mPointPosition.y = mPickedBucket->GetY();
                break;

                case FOdysseyPainterEditorVectorPaintBucketToolHUD::PICK_LINEAR_HANDLE0 :
                case FOdysseyPainterEditorVectorPaintBucketToolHUD::PICK_LINEAR_HANDLE1 :
                case FOdysseyPainterEditorVectorPaintBucketToolHUD::PICK_RADIAL_AREA:
                case FOdysseyPainterEditorVectorPaintBucketToolHUD::PICK_RADIAL_HANDLE:
                {
                    // needed for valid GUndo pointer
                    GEditor->BeginTransaction(LOCTEXT("vector-paint-bucket-tool.transaction.alter-bucket","Paint Bucket"));
                    if( GUndo )
                    {
                        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoBucketParam( iScene
                                                                                    , mPickedBucket
                                                                                    , retFlags );

                        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

                        TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
                        if (source)
                            source->RecordCurrentFrameUndo();
                    }
                    GEditor->EndTransaction();
                }
                break;

                default :
                break;
            }
        }
    }

    return true;
}

void
UOdysseyPainterEditorVectorPaintBucketTool::OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                                              , const FOdysseyPoint& iPointInTexture
                                                              , uint64& oSignalFlags )
{
    mBucketHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );
}

double
UOdysseyPainterEditorVectorPaintBucketTool::GetRotationAngle( FOdysseyVectorBucket* iBucket
                                                            , const FOdysseyPoint& iPointInTexture )
{
    FOdysseyVectorObject* ownerObject = iBucket->GetOwner();
    BLMatrix2D& inverseMatrix = ownerObject->GetInverseWorldMatrix();
    double rotation = iBucket->GetRotation();
    ::ULIS::FVec2D bucketVector = ::ULIS::FVec2D( cos( rotation ), sin( rotation ) );
    BLPoint pt = inverseMatrix.mapPoint( iPointInTexture.x, iPointInTexture.y );
    ::ULIS::FVec2D& pivot = iBucket->GetCoords();
    ::ULIS::FVec2D ptVector;
    double angle = 0.0f;

    ptVector.x = pt.x - pivot.x;
    ptVector.y = pt.y - pivot.y;

    if( ptVector.DistanceSquared() )
    {
        ptVector.Normalize();

        angle = fabs( acos( bucketVector.DotProduct( ptVector ) ) );

        return FOdysseyVector::Cross2D( bucketVector, ptVector ) >= 0.0f ? angle : - angle;
    }

    return 0.0f;
}

void
UOdysseyPainterEditorVectorPaintBucketTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                                             , const FOdysseyPoint& iPointInTexture
                                                             , uint64& oSignalFlags )
{
    uint64 notificationFlags = 0;

    // Left mouse-click
    if( iPointInTexture.keysDown.Find( EKeys::LeftMouseButton ) != INDEX_NONE )
    {
        if( mPickedBucket )
        {
            FOdysseyVectorObject* bucketOwner = mPickedBucket->GetOwner();
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(bucketOwner);
            BLMatrix2D& inverseWorldMatrix = paintGroup->GetInverseWorldMatrix();
            BLPoint localPoint = inverseWorldMatrix.mapPoint( iPointInTexture.x, iPointInTexture.y );
            BLPoint localVector = inverseWorldMatrix.mapVector( iPointInTexture.x - mOldPointInTexture.x
                                                              , iPointInTexture.y - mOldPointInTexture.y );

            switch( mPickedArea )
            {
                case FOdysseyPainterEditorVectorPaintBucketToolHUD::PICK_BUCKET :
                    mPickedBucket->Set( mPickedBucket->GetX() + localVector.x
                                      , mPickedBucket->GetY() + localVector.y );
                break;

                case FOdysseyPainterEditorVectorPaintBucketToolHUD::PICK_RADIAL_AREA:
                {
                    ::ULIS::FVec2D offset = mPickedBucket->GetRadialOffset();

                    offset.x += localVector.x;
                    offset.y += localVector.y;

                    mPickedBucket->SetRadialOffset( offset );
                }
                break;

                case FOdysseyPainterEditorVectorPaintBucketToolHUD::PICK_RADIAL_HANDLE:
                {
                    ::ULIS::FVec2D radialCoords = mPickedBucket->GetCoords() + mPickedBucket->GetRadialOffset();
                    double distance = ::ULIS::FVec2D( localPoint.x - radialCoords.x
                                                    , localPoint.y - radialCoords.y ).Distance();

                    mPickedBucket->SetRadialRadius( distance );
                }
                break;

                case FOdysseyPainterEditorVectorPaintBucketToolHUD::PICK_LINEAR_HANDLE0:
                {
                    mPickedBucket->SetLinearP0( ::ULIS::FVec2D( localPoint.x, localPoint.y ) );
                }
                break;

                case FOdysseyPainterEditorVectorPaintBucketToolHUD::PICK_LINEAR_HANDLE1:
                {
                    mPickedBucket->SetLinearP1( ::ULIS::FVec2D( localPoint.x, localPoint.y ) );
                }
                break;

                default :
                break;
            }
        }
    }

    mOldPointInTexture = ::ULIS::FVec2D( iPointInTexture.x, iPointInTexture.y );

    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_INTERACTIVE );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), FOdysseyVectorCell::REDRAW_INTERACTIVE );
}

void
UOdysseyPainterEditorVectorPaintBucketTool::SetBucketColor( FOdysseyVectorBucket* iBucket )
{
    eBucketColorMode bucketColorMode = eBucketColorMode::SolidColor;
    switch( ColorMode )
    {
        case EPaintBucketToolColorMode::LinearGradient:
        {
            iBucket->SetGradientColor0( Color1.R, Color1.G, Color1.B, Color1.A );
            iBucket->SetGradientColor1( Color2.R, Color2.G, Color2.B, Color2.A );
            bucketColorMode = eBucketColorMode::LinearGradient;
        }

        break;

        case EPaintBucketToolColorMode::RadialGradient:
        {
            iBucket->SetGradientColor0( Color1.R, Color1.G, Color1.B, Color1.A );
            iBucket->SetGradientColor1( Color2.R, Color2.G, Color2.B, Color2.A );
            bucketColorMode = eBucketColorMode::RadialGradient;
        }
        break;

        case EPaintBucketToolColorMode::Color:
        {
            switch( GetEditor()->GetColorType() )
            {
                case EOdysseyPainterEditorColorType::Raw:
                {
                    ::ULIS::FColor color = GetEditor()->PaintColor().GetValue();
                    ::ULIS::FColor rgba8 = color.ToFormat( ::ULIS::eFormat::Format_RGBA8 );
                    uint8 R = rgba8.R8();
                    uint8 G = rgba8.G8();
                    uint8 B = rgba8.B8();
                    uint8 A = Opacity * 255/*rgba8.A8()*/;

                    iBucket->SetSolidColor( R, G, B, A );
                    bucketColorMode = eBucketColorMode::SolidColor;
                }
                break;

                case EOdysseyPainterEditorColorType::Indexed:
                {
                    iBucket->SetPaletteEntry( GetEditor()->GetCurrentPaletteColorEntry() );
                    iBucket->SetPaletteSet( GetEditor()->GetCurrentPaletteSet() );
                    bucketColorMode = eBucketColorMode::Palette;
                }
                break;

                default:
                break;
            }
        }
        break;

        default:
        break;
    }

    iBucket->SetColorMode( bucketColorMode );
    iBucket->SetPropagated( Propagate );
}

uint64
UOdysseyPainterEditorVectorPaintBucketTool::OnMouseUpVectorCreateBucket( FOdysseyVectorGroupPaint* iScene
                                                                       , const FOdysseyPoint& iPointInTexture
                                                                       , const FKey& iKey )
{
    std::vector<FOdysseyVectorBucket*> addedBucketArray;
    std::vector<FOdysseyVectorBucket*> paramBucketArray;
    std::vector<FOdysseyVectorCycle*> pickedCycleArray;
    uint64 notificationFlags = 0;

    mBucketHUD->PickCycles( iPointInTexture.x
                          , iPointInTexture.y
                          , pickedCycleArray );

    if( pickedCycleArray.size() )
    {
        addedBucketArray.reserve( pickedCycleArray.size() );
        paramBucketArray.reserve( pickedCycleArray.size() );

        for(FOdysseyVectorCycle* cycle : pickedCycleArray )
        {
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(cycle->GetOwner());
            FOdysseyVectorBucket* bucket = cycle->GetBucket();

            if( bucket == nullptr )
            {
                BLMatrix2D& inverseWorldMatrix = paintGroup->GetInverseWorldMatrix();
                BLPoint localCoords = inverseWorldMatrix.mapPoint( iPointInTexture.x, iPointInTexture.y );

                bucket = new FOdysseyVectorBucket( paintGroup
                                                 , localCoords.x
                                                 , localCoords.y
                                                 , Propagate );

                paintGroup->AddBucket( bucket );

                addedBucketArray.push_back( bucket );
            }

            paramBucketArray.push_back( bucket );
        }
    }
    // No cycles picked, we create an orphan bucket
    else
    {
        for( FOdysseyVectorGroupPaint* paintGroup : mBucketHUD->GetWorkingPaintgroupList() )
        {
            BLMatrix2D& inverseWorldMatrix = paintGroup->GetInverseWorldMatrix();
            BLPoint localCoords = inverseWorldMatrix.mapPoint( iPointInTexture.x, iPointInTexture.y );
            FOdysseyVectorBucket* bucket = new FOdysseyVectorBucket( paintGroup
                                                                   , localCoords.x
                                                                   , localCoords.y
                                                                   , Propagate );

            paintGroup->AddBucket( bucket );

            addedBucketArray.push_back( bucket );
            paramBucketArray.push_back( bucket );
        }
    }

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("vector-paint-bucket-tool.transaction.create-bucket","Paint Bucket"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoBucketParam( iScene
                                                                    , addedBucketArray
                                                                    , paramBucketArray
                                                                    , notificationFlags );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    for( int i = 0; i < paramBucketArray.size(); i++ )
    {
        SetBucketColor( paramBucketArray[i] );
    }

    return notificationFlags;
}

uint64
UOdysseyPainterEditorVectorPaintBucketTool::OnMouseUpVectorRemoveBucket( FOdysseyVectorGroupPaint* iScene
                                                                       , FOdysseyVectorBucket* iBucket )
{
    FOdysseyVectorObject* ownerObject = iBucket->GetOwner();
    uint64 notificationFlags = 0;

    if( ownerObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
    {
        FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(ownerObject);

        paintGroup->RemoveBucket( iBucket );
    }

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("vector-paint-bucket-tool.transaction.remove-bucket","Paint Bucket"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoBucketRemove( iScene, iBucket, notificationFlags );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    return notificationFlags;
}

uint64
UOdysseyPainterEditorVectorPaintBucketTool::OnMouseUpVectorPropagateBucket( FOdysseyVectorGroupPaint* iScene
                                                                          , FOdysseyVectorBucket* iBucket
                                                                          , bool iPropagate )
{
    uint64 notificationFlags = 0;

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("vector-paint-bucket-tool.transaction.propagate-bucket","Paint Bucket"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoBucketParam( iScene, iBucket, notificationFlags );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    iBucket->SetPropagated( iPropagate );

    return notificationFlags;
}

uint64
UOdysseyPainterEditorVectorPaintBucketTool::OnMouseUpVectorColorBucket( FOdysseyVectorGroupPaint* iScene
                                                                      , FOdysseyVectorBucket* iBucket )
{
    uint64 notificationFlags = 0;

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("vector-paint-bucket-tool.transaction.color-bucket","Paint Bucket"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoBucketParam( iScene, iBucket, notificationFlags );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    SetBucketColor( iBucket );

    return notificationFlags;
}

uint64
UOdysseyPainterEditorVectorPaintBucketTool::OnMouseUpVectorClearBucket( FOdysseyVectorGroupPaint* iScene
                                                                      , FOdysseyVectorBucket* iBucket )
{
    uint64 notificationFlags = 0;

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("vector-paint-bucket-tool.transaction.clear-bucket","Paint Bucket"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoBucketParam( iScene, iBucket, notificationFlags );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    iBucket->SetSolidColor( 0, 0, 0, 0 );

    return notificationFlags;
}

bool
UOdysseyPainterEditorVectorPaintBucketTool::OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                                           , const FOdysseyPoint& iPointInTexture
                                                           , const FKey& iKey
                                                           , uint64& oSignalFlags )
{
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_HUD;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        if( mPickedBucket )
        {
            switch( mPickedArea )
            {
                case FOdysseyPainterEditorVectorPaintBucketToolHUD::PICK_BUCKET:
                    if ( mEditionMode == eVectorPaintBucketEditionMode::Remove )
                    {
                        notificationFlags |= OnMouseUpVectorRemoveBucket( iScene, mPickedBucket );
                        //OnMouseUpVectorClearBucket( iScene, mPickedBucket );
                    }
                    /*else
                    {
                        OnMouseUpVectorColorBucket( iScene, mPickedBucket );
                    }*/
                break;

                case FOdysseyPainterEditorVectorPaintBucketToolHUD::PICK_PROPAGATE:
                    notificationFlags |= OnMouseUpVectorPropagateBucket( iScene
                                                                       , mPickedBucket
                                                                       , mPickedBucket->IsPropagated() ? false : true );
                break;

                default:
                break;
            }
        }
        else
        {
            if ( mEditionMode == eVectorPaintBucketEditionMode::Default )
            {
                notificationFlags |= OnMouseUpVectorCreateBucket( iScene, iPointInTexture, iKey );
            }
        }

        mPickedBucket = nullptr;

        iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
        iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );
    }

    oSignalFlags = notificationFlags;

    return true;
}

void
UOdysseyPainterEditorVectorPaintBucketTool::ExtendContextMenu( FMenuBuilder& iMenu )
{
    if( mPickedBucket )
    {
        iMenu.AddMenuEntry(
              LOCTEXT("vector-paint-bucket-tool.context-menu.delete-bucket.name", "Delete Bucket")
            , LOCTEXT("vector-paint-bucket-tool.context-menu.delete-bucket.tooltip", "Delete Bucket")
            , FSlateIcon()
            , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::DeleteBucket, GetEditor(), mPickedBucket )));
        iMenu.AddMenuEntry(
              LOCTEXT("vector-paint-bucket-tool.context-menu.propagate-bucket.name", "Propagate Bucket")
            , LOCTEXT("vector-paint-bucket-tool.context-menu.propagate-bucket.tooltip", "Propagate Bucket")
            , FSlateIcon()
            , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::PropagateBucket, GetEditor(), mPickedBucket )));
        iMenu.AddMenuEntry(
              LOCTEXT("vector-paint-bucket-tool.context-menu.unpropagate-bucket.name", "Unpropagate Bucket")
            , LOCTEXT("vector-paint-bucket-tool.context-menu.unpropagate-bucket.tooltip", "Unpropagate Bucket")
            , FSlateIcon()
            , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::UnpropagateBucket, GetEditor(), mPickedBucket )));
        iMenu.AddMenuEntry(
              LOCTEXT("vector-paint-bucket-tool.context-menu.copy-bucket-param.name", "Copy Bucket Param")
            , LOCTEXT("vector-paint-bucket-tool.context-menu.copy-bucket-param.tooltip", "Copy Bucket Param")
            , FSlateIcon()
            , FUIAction(FExecuteAction::CreateStatic(&UOdysseyPainterEditorVectorPaintBucketTool::CopyBucketParam, mPickedBucket )));
        iMenu.AddMenuEntry(
              LOCTEXT("vector-paint-bucket-tool.context-menu.paste-bucket-param.name", "Paste Bucket Param")
            , LOCTEXT("vector-paint-bucket-tool.context-menu.paste-bucket-param.tooltip", "Paste Bucket Param")
            , FSlateIcon()
            , FUIAction(FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorVectorPaintBucketTool::PasteBucketParam, mPickedBucket )));
        iMenu.AddMenuEntry(
              LOCTEXT("vector-paint-bucket-tool.context-menu.bucket-properties.name", "Bucket properties")
            , LOCTEXT("vector-paint-bucket-tool.context-menu.bucket-properties.tooltip", "Bucket Properties")
            , FSlateIcon()
            , FUIAction(FExecuteAction::CreateStatic(&UOdysseyPainterEditorVectorPaintBucketTool::BucketProperties, GetEditor(), mPickedBucket )));

        mPickedBucket = nullptr;
    }
    else
    {
        UOdysseyPainterEditorVectorBaseTool::ExtendContextMenu( iMenu );
    }
}

//static
void
UOdysseyPainterEditorVectorPaintBucketTool::BucketProperties( FOdysseyPainterEditor* iEditor, FOdysseyVectorBucket* iBucket )
{
    FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    TSharedPtr<IDetailsView> detailsView;
    FDetailsViewArgs DetailsViewArgs;
    UOdysseyPainterEditorVectorBucketView* bucketView = NewObject<UOdysseyPainterEditorVectorBucketView>();

    bucketView->Update( iEditor, iBucket );

    DetailsViewArgs.bUpdatesFromSelection = false;
    DetailsViewArgs.bLockable = false;
    DetailsViewArgs.bAllowSearch = false;
    DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;

    detailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
    detailsView->SetObject(bucketView);

    TSharedRef<SWindow> BucketWindow = SNew(SWindow)
    .Title(FText::FromString(TEXT("Bucket Properties")))
    //.ClientSize(FVector2D(800, 400))
    .SizingRule(ESizingRule::Autosized)
    .SupportsMaximize(false)
    .SupportsMinimize(false)
    [
        detailsView.ToSharedRef()
      /*SNew(SVerticalBox)
      +SVerticalBox::Slot()
      .HAlign(HAlign_Center)
      .VAlign(VAlign_Center)
      [
        SNew(STextBlock)
        .Text(FText::FromString(TEXT("Hello from Slate")))
      ]*/
    ];

    TSharedPtr<FOdysseyPainterEditorViewportTab> viewportTab = iEditor->FindTab<FOdysseyPainterEditorViewportTab>();

    FSlateApplication::Get().AddModalWindow
    (
        BucketWindow,
        viewportTab->Widget(),
        false
    );

    bucketView->ConditionalBeginDestroy();
}

// static
FOdysseyVectorBucket&
UOdysseyPainterEditorVectorPaintBucketTool::GetCopiedBucket()
{
    static FOdysseyVectorBucket copiedBucket( nullptr, 0, 0, false );

    return copiedBucket;
}

// static
void
UOdysseyPainterEditorVectorPaintBucketTool::CopyBucketParam( FOdysseyVectorBucket* iSourceBucket )
{
    FOdysseyVectorBucket& destinationBucket = GetCopiedBucket();

    iSourceBucket->Copy( &destinationBucket );
}

void
UOdysseyPainterEditorVectorPaintBucketTool::PasteBucketParam( FOdysseyVectorBucket* iDestinationBucket )
{
    FOdysseyVectorGroupPaint* scene = iDestinationBucket->GetOwner()->GetScene();
    FOdysseyVectorBucket& sourceBucket = GetCopiedBucket();
    ::ULIS::FVec2D destinationBucketCoords = iDestinationBucket->GetCoords();
    uint64 notificationFlags = 0;

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("vector-paint-bucket-tool.transaction.create-bucket","Paint Bucket"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoBucketParam( scene, iDestinationBucket, notificationFlags );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    sourceBucket.Copy( iDestinationBucket );

    // we only keep the coords
    // Note: this will invalidate the engine as well.
    iDestinationBucket->SetCoords( destinationBucketCoords.x, destinationBucketCoords.y );

    scene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    scene->GetLayer()->RequestRedraw( scene->GetCell(), 0 );
}

eVectorPaintBucketEditionMode
UOdysseyPainterEditorVectorPaintBucketTool::GetEditionMode()
{
    return mEditionMode;
}

void
UOdysseyPainterEditorVectorPaintBucketTool::SetEditionMode( eVectorPaintBucketEditionMode iMode )
{
    mEditionMode = iMode;
}

const FSlateBrush*
UOdysseyPainterEditorVectorPaintBucketTool::GetBackgroundColor( eVectorPaintBucketEditionMode iMode ) const
{
    static FSlateColorBrush selected = FSlateColorBrush( FStyleColors::Select );

    return ( iMode == mEditionMode ) ? &selected : nullptr;
}

TSharedRef<SWidget>
UOdysseyPainterEditorVectorPaintBucketTool::CreateModifierSegmentControl()
{
    return SNew(SSegmentedControl<eVectorPaintBucketEditionMode>)
           .Value_Lambda( [this]{ return mEditionMode; } )
           .SupportsEmptySelection( false )
           .SupportsMultiSelection( false )
           .UniformPadding( FMargin( 2, 0, 2, 0 ) )
           .IsEnabled( false ) // currently not clickable - Info only
           .OnValueChanged( SSegmentedControl<eVectorPaintBucketEditionMode>::FOnValueChanged::CreateUObject( this, &UOdysseyPainterEditorVectorPaintBucketTool::SetEditionMode ) )
           // DEFAULT
           + SSegmentedControl<eVectorPaintBucketEditionMode>::Slot( eVectorPaintBucketEditionMode::Default )
           //.Icon( FOdysseyStyle::GetBrush( "PainterEditor.ToolsShortcuts.PaintBucketAddMove20") )
           .ToolTip( LOCTEXT("vector-paint-bucket-tool.edition-mode.default.name", "Add bucket") )
           [
               SNew(SBorder)
               .BorderImage_UObject( this, &UOdysseyPainterEditorVectorPaintBucketTool::GetBackgroundColor, eVectorPaintBucketEditionMode::Default  )
               [
                   SNew(SImage)
                   .Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsShortcuts.PaintBucketAddMove20") )
               ]
           ]
           // CTRL
           + SSegmentedControl<eVectorPaintBucketEditionMode>::Slot( eVectorPaintBucketEditionMode::Control )
           //.Icon( FOdysseyStyle::GetBrush( "PainterEditor.ToolsShortcuts.PaintBucketRadialLinear20") )
#if PLATFORM_WINDOWS
           .ToolTip( LOCTEXT("vector-paint-bucket-tool.edition-mode.ctrl.name", "Control radial/linear gradient settings (CTRL)") )
#endif
#if PLATFORM_MAC
           .ToolTip( LOCTEXT("vector-paint-bucket-tool.edition-mode.ctrl.name", "Erase to intersection (CMD)") )
#endif
           [
               SNew(SBorder)
               .BorderImage_UObject( this, &UOdysseyPainterEditorVectorPaintBucketTool::GetBackgroundColor, eVectorPaintBucketEditionMode::Control )
               [
                   SNew(SImage)
                   .Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsShortcuts.PaintBucketRadialLinear20") )
               ]
           ]
           // ALT
           + SSegmentedControl<eVectorPaintBucketEditionMode>::Slot( eVectorPaintBucketEditionMode::Remove )
           //.Icon( FOdysseyStyle::GetBrush( "PainterEditor.ToolsShortcuts.PaintBucketRemove20") )
           .ToolTip( LOCTEXT("vector-paint-bucket-tool.edition-mode.remove.name", "Remove bucket (ALT)") )
           [
               SNew(SBorder)
               .BorderImage_UObject( this, &UOdysseyPainterEditorVectorPaintBucketTool::GetBackgroundColor, eVectorPaintBucketEditionMode::Remove  )
               [
                   SNew(SImage)
                   .Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsShortcuts.PaintBucketRemove20") )
               ]
           ];
}

void
UOdysseyPainterEditorVectorPaintBucketTool::ExtendToolbar( FToolBarBuilder& iBuilder )
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
            SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorPaintBucketTool, Opacity ), FSinglePropertyParams())
            .InnerPadding(10.f)
            .ValueWidthOverride(100.f)
        ]
    );

    iBuilder.EndSection();
}

FText
UOdysseyPainterEditorVectorPaintBucketTool::GetTooltip() const
{
    return LOCTEXT("vector-paint-bucket-tool.tooltip", "Paint Bucket Tool");
}

#undef LOCTEXT_NAMESPACE
