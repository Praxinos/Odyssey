// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPaintBucketTool/OdysseyPainterEditorVectorPaintBucketTool.h"
#include "Tools/VectorPaintBucketTool/OdysseyPainterEditorVectorPaintBucketToolHUD.h"
#include "Tools/VectorPaintBucketTool/OdysseyPainterEditorVectorPaintBucketToolContextMenu.h"

#include "PainterEditor/OdysseyPainterEditorColorPaletteTab.h"
#include "OdysseyMediaVector.h"
#include "Undo/OdysseyVectorUndoBucketAdd.h"
#include "Undo/OdysseyVectorUndoBucketRemove.h"
#include "Undo/OdysseyVectorUndoBucketParam.h"
#include "Palette/OdysseyPaletteEntryColor.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorPaintBucketTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPaintBucketTool::~UOdysseyPainterEditorVectorPaintBucketTool()
{
    delete mBucketHUD;
}

UOdysseyPainterEditorVectorPaintBucketTool::UOdysseyPainterEditorVectorPaintBucketTool()
    : RestrictToSelection( false )
    , Propagate( true )
    , ColorMode ( eBucketColorMode::SolidColor )
    , Color1( 255, 255, 255, 255 )
    , Color2( 255, 255, 255, 255 )
    , PickingRadius( 10.0f )
    , mShowControls( false )
    , mPickedBucket( nullptr )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PaintBucket64");

    mBucketHUD = new FOdysseyPainterEditorVectorPaintBucketToolHUD( this );
}

std::list<FOdysseyVectorObject*>&
UOdysseyPainterEditorVectorPaintBucketTool::GetFocusedObjectList( FOdysseyVectorScene* iScene )
{
    if( RestrictToSelection )
    {
        std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();

        if( selectedObjectList.size() )
        {
            return selectedObjectList;
        }
    }

    // return scene as list
    return iScene->GetEngine()->GetChildrenList();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

bool
UOdysseyPainterEditorVectorPaintBucketTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
}

void
UOdysseyPainterEditorVectorPaintBucketTool::Unload()
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        //Should be done in UnloadVector directly
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() > 0 )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

            UnloadVector( vectorEngine, vectorScene );
        }
    }
}

void
UOdysseyPainterEditorVectorPaintBucketTool::Load()
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        // It would be better if this is done in OnMouseDown()
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() > 0 )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

            LoadVector( vectorEngine, vectorScene );
        }
    }
}

void
UOdysseyPainterEditorVectorPaintBucketTool::UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->RemoveHUD( mBucketHUD );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorPaintBucketTool::LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    TSharedPtr< SViewport > viewportWidget; // to force keyboard focus on mouse hover.
                                            // Prevents the user from having to click at least once in the viewport.
    // we need the focus on the viewport for keyboard 
    TSharedPtr<FOdysseyPainterEditorViewportTab> viewportTab = GetEditor()->FindTab<FOdysseyPainterEditorViewportTab>();
    viewportWidget = viewportTab->GetViewport()->GetViewportWidget();

    // we need the focus on the viewport for keyboard 
    FSlateApplication::Get().SetKeyboardFocus( viewportWidget );

    iEngine->ClearHUD();
    iEngine->AddHUD( mBucketHUD );

    mBucketHUD->Reset( iScene );

    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}


bool
UOdysseyPainterEditorVectorPaintBucketTool::OnKeyDown( const FKey& iKey )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    bool ret = false;

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

            ret = OnKeyDownVector( vectorEngine, vectorScene, iKey );
        }
    }

    return ret;
}

bool
UOdysseyPainterEditorVectorPaintBucketTool::OnKeyUp( const FKey& iKey )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    bool ret = false;

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

            ret = OnKeyUpVector( vectorEngine, vectorScene, iKey );
        }
    }

    return ret;
}

bool
UOdysseyPainterEditorVectorPaintBucketTool::OnMouseDown( const FOdysseyPoint& iPointInTexture
                                                 , const FKey& iKey )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    bool ret = false;

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

            ret = OnMouseDownVector( vectorEngine, vectorScene, iPointInTexture, iKey );
        }
    }

    return ret;
}

void
UOdysseyPainterEditorVectorPaintBucketTool::OnMouseHover( const FOdysseyPoint& iPointInTexture )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

            OnMouseHoverVector( vectorEngine, vectorScene, iPointInTexture );
        }
    }
}

void
UOdysseyPainterEditorVectorPaintBucketTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

            OnMouseDragVector( vectorEngine, vectorScene, iPointInTexture );
        }
    }
}

bool
UOdysseyPainterEditorVectorPaintBucketTool::OnKeyDownVector( FOdysseyVectorEngine* iEngine
                                                     , FOdysseyVectorScene* iScene
                                                     , const FKey& iKey )
{
    if( FSlateApplication::Get().GetModifierKeys().IsControlDown() )
    {
        mShowControls = true;
    }

    //UOdysseyPainterEditorDefaultTool::OnKeyDownVector( iEngine, iScene, iKey );
    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );

    return false;
}

bool
UOdysseyPainterEditorVectorPaintBucketTool::OnKeyUpVector( FOdysseyVectorEngine* iEngine
                                                   , FOdysseyVectorScene* iScene
                                                   , const FKey& iKey )
{
    mShowControls = false;

    //UOdysseyPainterEditorDefaultTool::OnKeyUpVector( iEngine, iScene, iKey );
    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );

    return false;
}

// static
bool
UOdysseyPainterEditorVectorPaintBucketTool::DoubleClicked()
{
    uint64 clickTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    static uint64 previousClickTime = 0;
    bool doubleClicked = ( ( clickTime - previousClickTime ) < 200 ) ? true : false;

    previousClickTime = clickTime;

    return doubleClicked;
}

void
UOdysseyPainterEditorVectorPaintBucketTool::OnMouseDownVectorRotateBucket( FOdysseyVectorScene* iScene
                                                                         , FOdysseyVectorBucket* iBucket )
{
    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("VectorPaintBucketTool","Paint Bucket"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoBucketParam( iScene, iBucket );

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();
}

bool
UOdysseyPainterEditorVectorPaintBucketTool::OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                                             , FOdysseyVectorScene* iScene
                                                             , const FOdysseyPoint& iPointInTexture
                                                             , const FKey& iKey )
{
    mDownMouseX = mOldPointInTexture.x = iPointInTexture.x;
    mDownMouseY = mOldPointInTexture.y = iPointInTexture.y;

    mPickedBucket = mBucketHUD->PickBucket( iScene, iPointInTexture.x, iPointInTexture.y );
    mPickedArea = FOdysseyPainterEditorVectorPaintBucketToolHUD::PICK_NONE;

    if( mPickedBucket )
    {
        mPickedArea = mBucketHUD->PickBucketArea( mPickedBucket, iPointInTexture.x, iPointInTexture.y );
    }

    // Left mouse-click
    if( iPointInTexture.keysDown.Find( EKeys::LeftMouseButton ) != INDEX_NONE )
    {
        if( mPickedBucket )
        {
            FOdysseyVectorObject* bucketOwner = mPickedBucket->GetOwner();
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(bucketOwner);

            switch( mPickedArea )
            {
                case FOdysseyPainterEditorVectorPaintBucketToolHUD::PICK_BUCKET :
                    // needed for valid GUndo pointer
                    GEditor->BeginTransaction(LOCTEXT("VectorPaintBucketTool","Paint Bucket"));
                    if( GUndo )
                    {
                        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoPointPosition( iScene, mPickedBucket );

                        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
                    }
                    GEditor->EndTransaction();

                    mPointPosition.x = mPickedBucket->GetX();
                    mPointPosition.y = mPickedBucket->GetY();
                break;

                case FOdysseyPainterEditorVectorPaintBucketToolHUD::PICK_HANDLE :
                    mPointRotation = mPickedBucket->GetRotation();
                    // Save undo
                    OnMouseDownVectorRotateBucket( iScene, mPickedBucket );
                break;

                default :
                break;
            }
        }
    }

    return true;
}

void
UOdysseyPainterEditorVectorPaintBucketTool::OnMouseHoverVector( FOdysseyVectorEngine* iEngine
                                                              , FOdysseyVectorScene* iScene
                                                              , const FOdysseyPoint& iPointInTexture )
{
    std::vector<FOdysseyVectorCycle*> pickedCycleArray;

    if( mShowControls == false )
    {
        ::ULIS::FRectD roi;

        roi.x = iPointInTexture.x;
        roi.y = iPointInTexture.y;

        mBucketHUD->PickCycles( iScene, iPointInTexture.x, iPointInTexture.y, pickedCycleArray );
        mBucketHUD->SetPickedCycles( pickedCycleArray );
    }

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_INTERACTIVE );

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
UOdysseyPainterEditorVectorPaintBucketTool::OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                                             , FOdysseyVectorScene* iScene
                                                             , const FOdysseyPoint& iPointInTexture )
{
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

                case FOdysseyPainterEditorVectorPaintBucketToolHUD::PICK_HANDLE:
                {
                    double deltaAngle = GetRotationAngle( mPickedBucket, iPointInTexture );

                    mPickedBucket->SetRotation( mPickedBucket->GetRotation() + deltaAngle );
                }
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

                default :
                break;
            }
        }
    }

    mOldPointInTexture = ::ULIS::FVec2D( iPointInTexture.x, iPointInTexture.y );

    iScene->Update( FOdysseyVectorObject::KEEPINVALIDATED ); // update vector scene

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_INTERACTIVE );
}

void
UOdysseyPainterEditorVectorPaintBucketTool::SetBucketColor( FOdysseyVectorBucket* iBucket )
{
    if( ( ColorMode == eBucketColorMode::LinearGradient )
     || ( ColorMode == eBucketColorMode::RadialGradient ) )
    {
        iBucket->SetColorMode( ColorMode );
        iBucket->SetGradientColor0( Color1.R, Color1.G, Color1.B, Color1.A );
        iBucket->SetGradientColor1( Color2.R, Color2.G, Color2.B, Color2.A );
    }
    else
    {
        ::ULIS::FColor color = GetEditor()->PaintColor().GetValue();

        TSharedPtr<FOdysseyPainterEditorPaletteTab> colorPaletteTab = GetEditor()->FindTab<FOdysseyPainterEditorPaletteTab>();
        if ( colorPaletteTab->PaletteWidget()->GetColorPalette()->GetPalette())
        {
            UOdysseyPaletteEntry* entry = colorPaletteTab->PaletteWidget()->GetColorPalette()->GetPalette()->CurrentEntry.Get();

            if (entry && entry->IsA(UOdysseyPaletteEntryColor::StaticClass()))
            {
                FColor colorEntry = Cast< UOdysseyPaletteEntryColor >(entry)->GetUsedColor();
                color = ::ULIS::FColor::RGBAF(colorEntry.R, colorEntry.G, colorEntry.B, colorEntry.A);

                iBucket->SetPaletteEntry( entry );
            }

            iBucket->SetColorMode( eBucketColorMode::Palette );
        }
        else
        {
            iBucket->SetColorMode( eBucketColorMode::SolidColor );
        }

        ::ULIS::FColor rgba8 = color.ToFormat( ::ULIS::eFormat::Format_RGBA8 );
        uint8 R = rgba8.R8();
        uint8 G = rgba8.G8();
        uint8 B = rgba8.B8();
        uint8 A = rgba8.A8();

        iBucket->SetSolidColor( R, G, B, A );
    }

    iBucket->SetPropagated( Propagate );
}

void
UOdysseyPainterEditorVectorPaintBucketTool::OnMouseUpVectorCreateBucket( FOdysseyVectorScene* iScene
                                                                       , const FOdysseyPoint& iPointInTexture
                                                                       , const FKey& iKey )
{
    std::vector<FOdysseyVectorBucket*> addedBucketArray;
    std::vector<FOdysseyVectorBucket*> paramBucketArray;
    std::vector<FOdysseyVectorCycle*> pickedCycleArray;

    mBucketHUD->PickCycles( iScene
                          , iPointInTexture.x
                          , iPointInTexture.y
                          , pickedCycleArray );

    if( pickedCycleArray.size() )
    {
        addedBucketArray.reserve( pickedCycleArray.size() );
        paramBucketArray.reserve( pickedCycleArray.size() );

        for( int i = 0; i < pickedCycleArray.size(); i++ )
        {
            FOdysseyVectorCycle* cycle = pickedCycleArray[i];
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
        std::list<FOdysseyVectorObject*>& focusedObjectList = GetFocusedObjectList( iScene );

        for( FOdysseyVectorObject* focusedObject : focusedObjectList )
        {
            if( focusedObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
            {
                FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(focusedObject);
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
    }

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("VectorPaintBucketTool","Paint Bucket"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoBucketParam( iScene
                                                                    , addedBucketArray
                                                                    , paramBucketArray );

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

    for( int i = 0; i < paramBucketArray.size(); i++ )
    {
        SetBucketColor( paramBucketArray[i] );
    }
}

void
UOdysseyPainterEditorVectorPaintBucketTool::OnMouseUpVectorRemoveBucket( FOdysseyVectorScene* iScene
                                                                       , FOdysseyVectorBucket* iBucket )
{
    FOdysseyVectorObject* ownerObject = iBucket->GetOwner();

    if( ownerObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
    {
        FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(ownerObject);

        paintGroup->RemoveBucket( iBucket );
    }

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("VectorPaintBucketTool","Paint Bucket"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoBucketRemove( iScene, iBucket );

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();
}

void
UOdysseyPainterEditorVectorPaintBucketTool::OnMouseUpVectorPropagateBucket( FOdysseyVectorScene* iScene
                                                                          , FOdysseyVectorBucket* iBucket
                                                                          , bool iPropagate )
{
    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("VectorPaintBucketTool","Paint Bucket"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoBucketParam( iScene, iBucket );

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

    iBucket->SetPropagated( iPropagate );
}

void
UOdysseyPainterEditorVectorPaintBucketTool::OnMouseUpVectorColorBucket( FOdysseyVectorScene* iScene
                                                                      , FOdysseyVectorBucket* iBucket )
{
    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("VectorPaintBucketTool","Paint Bucket"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoBucketParam( iScene, iBucket );

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

    SetBucketColor( iBucket );
}

void
UOdysseyPainterEditorVectorPaintBucketTool::OnMouseUpVectorClearBucket( FOdysseyVectorScene* iScene
                                                                , FOdysseyVectorBucket* iBucket )
{
    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("VectorPaintBucketTool","Paint Bucket"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoBucketParam( iScene, iBucket );

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

    iBucket->SetSolidColor( 0, 0, 0, 0 );
}

void
UOdysseyPainterEditorVectorPaintBucketTool::PopUpMenu( FOdysseyVectorBucket* iBucket )
{
    TSharedPtr<SWidget> contextMenu = FOdysseyPainterEditorVectorPaintBucketToolContextMenu::CreateWidget( GetEditor(), iBucket );

    TSharedPtr<FOdysseyPainterEditorViewportTab> viewportTab = GetEditor()->FindTab<FOdysseyPainterEditorViewportTab>();

    FSlateApplication::Get().PushMenu( viewportTab->Widget().ToSharedRef(),
                                       FWidgetPath(),
                                       contextMenu.ToSharedRef(),
                                       FSlateApplication::Get().GetCursorPos(),
                                       FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu) );
}

bool
UOdysseyPainterEditorVectorPaintBucketTool::OnMouseUp( const FOdysseyPoint& iPointInTexture
                                               , const FKey& iKey )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    bool ret = false;

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

            ret = OnMouseUpVector( vectorEngine, vectorScene, iPointInTexture, iKey );
        }
    }

    return ret;
}

bool
UOdysseyPainterEditorVectorPaintBucketTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                                     , FOdysseyVectorScene* iScene
                                                     , const FOdysseyPoint& iPointInTexture
                                                     , const FKey& iKey )
{
    // Right mouse-click
    if( iKey == EKeys::RightMouseButton )
    {
        if( mPickedBucket )
        {
            FOdysseyVectorObject* bucketOwner = mPickedBucket->GetOwner();
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(bucketOwner);

            switch( mPickedArea )
            {
                case FOdysseyPainterEditorVectorPaintBucketToolHUD::PICK_BUCKET:
                    PopUpMenu( mPickedBucket );
                break;

                default:
                break;
            }
        }
    }

    // Left mouse-click
    if( iKey == EKeys::LeftMouseButton )
    {
        if( mPickedBucket )
        {
            switch( mPickedArea )
            {
                case FOdysseyPainterEditorVectorPaintBucketToolHUD::PICK_BUCKET:
                    if ( FSlateApplication::Get().GetModifierKeys().IsAltDown() )
                    {
                        OnMouseUpVectorRemoveBucket( iScene, mPickedBucket );
                        //OnMouseUpVectorClearBucket( iScene, mPickedBucket );
                    }
                    /*else
                    {
                        OnMouseUpVectorColorBucket( iScene, mPickedBucket );
                    }*/
                break;

                default:
                break;
            }
        }
        else
        {
            OnMouseUpVectorCreateBucket( iScene, iPointInTexture, iKey );
        }
    }

    mPickedBucket = nullptr;

    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );

    return false;
}

void
UOdysseyPainterEditorVectorPaintBucketTool::Commit()
{
	mPaintEngine.Commit(FOdysseyBlendParameters());
}

void
UOdysseyPainterEditorVectorPaintBucketTool::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    // redraw
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if (hasVector)
    {
        //Should be done in OnKeyUpVector directly
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
        if( mediaVectors.Num() > 0 )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

            PropertyChangedVector( vectorEngine, vectorScene, PropertyChangedEvent.GetPropertyName() );
        }
    }
}

void
UOdysseyPainterEditorVectorPaintBucketTool::PropertyChangedVector( FOdysseyVectorEngine* iEngine
                                                           , FOdysseyVectorScene* iScene
                                                           , const FName& iPropertyName )
{
    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorPaintBucketTool::GetShowControls()
{
    return mShowControls;
}

#undef LOCTEXT_NAMESPACE
