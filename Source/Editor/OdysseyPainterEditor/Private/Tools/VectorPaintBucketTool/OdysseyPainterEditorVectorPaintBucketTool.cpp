// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPaintBucketTool/OdysseyPainterEditorVectorPaintBucketTool.h"
#include "Tools/VectorPaintBucketTool/OdysseyPainterEditorVectorPaintBucketToolHUD.h"

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
    : /*RestrictToSelection( false )
    ,*/ Propagate( true )
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

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

bool
UOdysseyPainterEditorVectorPaintBucketTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
}

uint64
UOdysseyPainterEditorVectorPaintBucketTool::UnloadVector( FOdysseyVectorScene* iScene )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    iEngine->RemoveHUD( mBucketHUD );

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorPaintBucketTool::LoadVector( FOdysseyVectorScene* iScene )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();
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

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorPaintBucketTool::OnKeyDownVector( FOdysseyVectorScene* iScene
                                                           , const FKey& iKey )
{
    if( FSlateApplication::Get().GetModifierKeys().IsControlDown() )
    {
        mShowControls = true;
    }

    return UOdysseyPainterEditorVectorBaseTool::OnKeyDownVector( iScene, iKey )
         | FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorPaintBucketTool::OnKeyUpVector( FOdysseyVectorScene* iScene
                                                         , const FKey& iKey )
{
    mShowControls = false;

    return UOdysseyPainterEditorVectorBaseTool::OnKeyUpVector( iScene, iKey )
         | FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
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

uint64
UOdysseyPainterEditorVectorPaintBucketTool::OnMouseDownVector( FOdysseyVectorScene* iScene
                                                             , const FOdysseyPoint& iPointInTexture
                                                             , const FKey& iKey )
{
    if( iPointInTexture.keysDown.Find( EKeys::LeftMouseButton ) != INDEX_NONE )
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
    }

    return 0;
}

uint64
UOdysseyPainterEditorVectorPaintBucketTool::OnMouseHoverVector( FOdysseyVectorScene* iScene
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

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;

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

uint64
UOdysseyPainterEditorVectorPaintBucketTool::OnMouseDragVector( FOdysseyVectorScene* iScene
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

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;
}

void
UOdysseyPainterEditorVectorPaintBucketTool::SetBucketColor( FOdysseyVectorBucket* iBucket )
{
    switch( ColorMode )
    {
        case eBucketColorMode::LinearGradient:
        case eBucketColorMode::RadialGradient:
            iBucket->SetGradientColor0( Color1.R, Color1.G, Color1.B, Color1.A );
            iBucket->SetGradientColor1( Color2.R, Color2.G, Color2.B, Color2.A );
        break;

        case eBucketColorMode::SolidColor:
        {
            ::ULIS::FColor color = GetEditor()->PaintColor().GetValue();
            ::ULIS::FColor rgba8 = color.ToFormat( ::ULIS::eFormat::Format_RGBA8 );
            uint8 R = rgba8.R8();
            uint8 G = rgba8.G8();
            uint8 B = rgba8.B8();
            uint8 A = rgba8.A8();

            iBucket->SetSolidColor( R, G, B, A );
        }
        break;

        case eBucketColorMode::Palette:
        {
            TSharedPtr<FOdysseyPainterEditorPaletteTab> colorPaletteTab = GetEditor()->FindTab<FOdysseyPainterEditorPaletteTab>();
            UOdysseyPalette* palette = colorPaletteTab->PaletteWidget()->GetColorPalette()->GetPalette();

            if ( palette )
            {
                UOdysseyPaletteEntry* paletteEntry = palette->CurrentEntry.Get();

                if ( paletteEntry && paletteEntry->IsA(UOdysseyPaletteEntryColor::StaticClass()))
                {
                    iBucket->SetPaletteEntry( paletteEntry );
                }
            }
        }
        break;

        default:
        break;
    }

    iBucket->SetColorMode( ColorMode );
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

uint64
UOdysseyPainterEditorVectorPaintBucketTool::OnMouseUpVector( FOdysseyVectorScene* iScene
                                                           , const FOdysseyPoint& iPointInTexture
                                                           , const FKey& iKey )
{
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

        mPickedBucket = nullptr;

        iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );
    }

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED;
}

bool
UOdysseyPainterEditorVectorPaintBucketTool::GetShowControls()
{
    return mShowControls;
}

void
UOdysseyPainterEditorVectorPaintBucketTool::ExtendContextMenu( FMenuBuilder& iMenu )
{
    if( mPickedBucket )
    {
        iMenu.BeginSection("Context");
        {
            iMenu.AddMenuEntry(
                LOCTEXT("DeleteBucket", "Delete Bucket")
              , LOCTEXT("DeleteBucket", "Delete Bucket")
              , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
              , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::DeleteBucket, mPickedBucket )));
            iMenu.AddMenuEntry(
                LOCTEXT("PropagateBucket", "Propagate Bucket")
              , LOCTEXT("PropagateBucket", "Propagate Bucket")
              , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
              , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::PropagateBucket, mPickedBucket )));
            iMenu.AddMenuEntry(
                LOCTEXT("UnpropagateBucket", "Unpropagate Bucket")
              , LOCTEXT("UnpropagateBucket", "Unpropagate Bucket")
              , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
              , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::UnpropagateBucket, mPickedBucket )));
            iMenu.AddMenuEntry(
                LOCTEXT("CopyBucketParam", "Copy Bucket Param")
              , LOCTEXT("CopyBucketParam", "Copy Bucket Param")
              , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
              , FUIAction(FExecuteAction::CreateStatic(&UOdysseyPainterEditorVectorPaintBucketTool::CopyBucketParam, mPickedBucket )));
            iMenu.AddMenuEntry(
              LOCTEXT("PasteBucketParam", "Paste Bucket Param")
              , LOCTEXT("PasteBucketParam", "Paste Bucket Param")
              , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
              , FUIAction(FExecuteAction::CreateStatic(&UOdysseyPainterEditorVectorPaintBucketTool::PasteBucketParam, mPickedBucket )));
            iMenu.AddMenuEntry(
                LOCTEXT("BucketProperties", "Bucket properties")
              , LOCTEXT("BucketProperties", "Bucket Properties")
              , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
              , FUIAction(FExecuteAction::CreateStatic(&UOdysseyPainterEditorVectorPaintBucketTool::BucketProperties, GetEditor(), mPickedBucket )));
        }
        iMenu.EndSection();
    }

    ExtendContextMenu( iMenu );
}

//static
void
UOdysseyPainterEditorVectorPaintBucketTool::BucketProperties( FOdysseyPainterEditor* iEditor, FOdysseyVectorBucket* iBucket )
{
    FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    TSharedPtr<IDetailsView> detailsView;
    FDetailsViewArgs DetailsViewArgs;
    UOdysseyPainterEditorVectorBucketView* bucketView = NewObject<UOdysseyPainterEditorVectorBucketView>();

    bucketView->Update( iBucket );

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

// static
void
UOdysseyPainterEditorVectorPaintBucketTool::PasteBucketParam( FOdysseyVectorBucket* iDestinationBucket )
{
    FOdysseyVectorBucket& sourceBucket = GetCopiedBucket();
    ::ULIS::FVec2D destinationBucketCoords = iDestinationBucket->GetCoords();

    sourceBucket.Copy( iDestinationBucket );

    // we only keep the coords
    iDestinationBucket->SetCoords( destinationBucketCoords.x, destinationBucketCoords.y, 0.0f );

    iDestinationBucket->Invalidate();
}

#undef LOCTEXT_NAMESPACE
