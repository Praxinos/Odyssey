// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathDrawingTool/OdysseyPainterEditorVectorPathDrawingTool.h"
#include "Tools/VectorPathDrawingTool/OdysseyPainterEditorVectorPathDrawingToolHUD.h"
#include "Tools/VectorPathDrawingTool/SOdysseyPainterEditorVectorPathDrawingToolTopTab.h"
#include "Undo/OdysseyVectorUndoObjectAdd.h"
#include "Undo/OdysseyVectorUndoPathAlter.h"
#include "Palette/OdysseyPaletteEntryColor.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorPathDrawingTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathDrawingTool::~UOdysseyPainterEditorVectorPathDrawingTool()
{
    delete mPathDrawingHUD;
}

UOdysseyPainterEditorVectorPathDrawingTool::UOdysseyPainterEditorVectorPathDrawingTool()
    : Radius( 5.0f )
    , Opacity( 1.0f )
    , Absolute( true )
    , Stitch( false )
    , AverageStitchedRadius( true )
    , StitchingRadius( 10 )
    , mPathBuilder( nullptr )
    , mOldPointInTexture( 0, 0 )
    , mPreviousVertex( nullptr )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.VectoPen64");

    mPathDrawingHUD = new FOdysseyPainterEditorVectorPathDrawingToolHUD( this );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorPathDrawingTool::Load()
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    UOdysseyPainterEditorVectorPathDrawingTool::LoadVector( vectorEngine, vectorScene );
}

void
UOdysseyPainterEditorVectorPathDrawingTool::Unload()
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    UOdysseyPainterEditorVectorPathDrawingTool::UnloadVector( vectorEngine, vectorScene );
}

bool
UOdysseyPainterEditorVectorPathDrawingTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
}

void
UOdysseyPainterEditorVectorPathDrawingTool::UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->RemoveHUD( mPathDrawingHUD );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

TArray<TSharedPtr<SWidget>>
UOdysseyPainterEditorVectorPathDrawingTool::CreateTopTabWidgets()
{
    return {
        SNew(SOdysseyPainterEditorVectorPathDrawingToolTopTab, this)
    };
}

void
UOdysseyPainterEditorVectorPathDrawingTool::OnSizeChanged()
{

}

void
UOdysseyPainterEditorVectorPathDrawingTool::LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->ClearHUD();
    iEngine->AddHUD( mPathDrawingHUD );

    mPathDrawingHUD->Reset( iScene ); // creates the quadtree;

//    TSharedPtr<SOdysseyPaintModifiers> widget = GetEditor()->GetGUI()->GetTopTab().Get()->GetWidget();

//    widget.Get()->OnSizeChanged.AddRaw( this, &UOdysseyPainterEditorVectorPathDrawingTool::OnSizeChanged );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

static ::ULIS::FRectI
GetInvalidationAreaFromPointer( int iX, int iY, int iRadius ) 
{
    int diameter = iRadius * 2;

    return ::ULIS::FRectI( iX - iRadius - 1, iY - iRadius - 1, diameter + 2, diameter + 2 );
}

static ::ULIS::FRectI
RectangleDtoI( ::ULIS::FRectD iRect )
{
    return ::ULIS::FRectI( (int) iRect.x, (int) iRect.y, (int) iRect.w, (int) iRect.h );
}

FOdysseyVectorVertex*
UOdysseyPainterEditorVectorPathDrawingTool::PickVertex( FOdysseyVectorEngine* iVectorEngine
                                                      , FOdysseyVectorScene* iScene
                                                      , double iWorldX
                                                      , double iWorldY
                                                      , double iPickingRadius )
{
    if( Stitch || FSlateApplication::Get().GetModifierKeys().IsShiftDown() )
    {
        std::vector<FOdysseyVectorPoint*> pickedPointArray;
        FOdysseyVectorVertex* stitchCubicVertex = nullptr;

        pickedPointArray.reserve( 50 );

        iVectorEngine->PickPoints( iScene
                                 , false
                                 , iWorldX
                                 , iWorldY
                                 , iPickingRadius
                                 , pickedPointArray
                                 , FOdysseyVectorPath::PICK_POINT );

        if( pickedPointArray.size() )
        {
            stitchCubicVertex = static_cast<FOdysseyVectorVertex*>( pickedPointArray[0] );

            if( stitchCubicVertex->GetSegmentCount() == 1 )
            {
                return stitchCubicVertex;
            }
        }
    }

    return nullptr;
}

bool
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return false;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return false;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    return UOdysseyPainterEditorVectorPathDrawingTool::OnMouseDownVector( vectorEngine, vectorScene, iPointInTexture, iKey );
}

bool
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                                             , FOdysseyVectorScene* iScene
                                                             , const FOdysseyPoint& iPointInTexture
                                                             , const FKey& iKey )
{
    //TODO: change architecture to have a easier time getting the palette
    ::ULIS::FColor color = GetEditor()->PaintColor().GetValue();
    ::ULIS::FColor rgba8 = color.ToFormat( ::ULIS::eFormat::Format_RGBA8 );
    FColor ueColor = FColor( rgba8.R8(), rgba8.G8(), rgba8.B8(), rgba8.A8() );
    FOdysseyVectorVertex* vertex = PickVertex( iEngine, iScene, iPointInTexture.x, iPointInTexture.y, StitchingRadius );
    FOdysseyVectorPath* path = nullptr;
    UOdysseyPaletteEntry* entry = nullptr;
    BLPoint localCoords;

    if (GetEditor()->GetGUI()->GetColorPaletteTab()->PaletteWidget()->GetColorPalette()->GetPalette())
    {
        entry = GetEditor()->GetGUI()->GetColorPaletteTab()->PaletteWidget()->GetColorPalette()->GetPalette()->CurrentEntry.Get();
        if (entry && entry->IsA(UOdysseyPaletteEntryColor::StaticClass()))
        {
            FColor colorEntry = Cast< UOdysseyPaletteEntryColor >(entry)->GetUsedColor();
            ueColor = colorEntry;
        }
    }

 //UE_LOG(LogTemp, Warning, TEXT("radius:%f iPointInTexture.pressure:%f"), radius, iPointInTexture.pressure ); 

    // record for later undoing
    mVertexArray.clear();
    mSegmentArray.clear();

    // this is important to know what undo operation we are going to record: an ObjectAdd or a PathDrawing.
    mStitched = true;

    if( vertex )
    {
        path = vertex->GetPath();
    }
    else
    {
        if ( path == nullptr )
        {
            path = new FOdysseyVectorPath( FString("Path") );

            // This undo must be set before association with the new parent object
            // needed for valid GUndo pointer
            GEditor->BeginTransaction(LOCTEXT("VectorPathDrawingTool","Vector Path Drawing Tool"));
            if( GUndo )
            {
                FOdysseyVectorUndo* undo = static_cast<FOdysseyVectorUndo*>( new FOdysseyVectorUndoObjectAdd( iScene, path ) );

                GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
            }
            GEditor->EndTransaction();

            iScene->AppendChild( path );

            // this is important to know what undo operation we are going to record: an ObjectAdd or a PathDrawing.
            mStitched = false;
        }

        vertex = new FOdysseyVectorVertex( path, 0.0f, 0.0f, 0.0f );

        path->AddVertex( vertex );
        // record for undos
        mVertexArray.push_back( vertex );
    }

    mPreviousVertex = vertex;

    path->UpdateMatrix();

    mPathBuilder = new FOdysseyVectorPathBuilder();

    path->SetForegroundColor( ueColor );
    mPathBuilder->SetForegroundColor( ueColor );

    if (entry && entry->IsA(UOdysseyPaletteEntryColor::StaticClass()))
    {
        path->GetForegroundBucket().SetPaletteEntry( entry );
        mPathBuilder->GetForegroundBucket().SetPaletteEntry( entry );
    }

    localCoords = path->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
    //localRadius = path->GetInverseWorldMatrix().mapVector( 0.7071f * radius, 0.7071f * radius );

    vertex->Set( localCoords.x, localCoords.y );
    // vertex->SetRadius( ::ULIS::FVec2D( localRadius.x, localRadius.y ).Distance() );

    iScene->AppendChild( mPathBuilder );

    mPathBuilder->Attach( path );
    path->CopyTransformation( *path );
    mPathBuilder->UpdateMatrix();

    mPathBuilder->RecordStart( vertex );

    iScene->ClearSelection();
    iScene->Select( path );

    // update invalidated objects
    iScene->Update( 0 );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_TRANSFORMED );

    mPathDrawingHUD->Reset( iScene ); // re-creates the quadtree;

    return true;
}

void
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseHover( const FOdysseyPoint& iPointInTexture )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    UOdysseyPainterEditorVectorPathDrawingTool::OnMouseHoverVector( vectorEngine, vectorScene, iPointInTexture );
}

::ULIS::FRectI
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseHoverVector( FOdysseyVectorEngine* iEngine
                                                              , FOdysseyVectorScene* iScene
                                                              , const FOdysseyPoint& iPointInTexture )
{
    ::ULIS::FRectI redrawRegion = { 0, 0, 0, 0 };
    ::ULIS::FRectI imageRegion;
    ::ULIS::FRectI toolRegion = GetInvalidationAreaFromPointer( iPointInTexture.x
                                                              , iPointInTexture.y
                                                              , ::ULIS::FMath::Max( Radius, StitchingRadius ) )
                              // we use mOldPointInTexture instead of iPointInTexture.deltaPosition because the latter is not reliable
                              | GetInvalidationAreaFromPointer( mOldPointInTexture.x
                                                              , mOldPointInTexture.y
                                                              , ::ULIS::FMath::Max( Radius, StitchingRadius ) );
//UE_LOG(LogTemp, Warning, TEXT("Some warning message %f %f"), mOldPointInTexture.x, mOldPointInTexture.y );
    iEngine->GetColorImageSize( imageRegion );

    // commented out: redrawing will be performed by the caller function
    //iScene->Signal( FOdysseyVectorScene::SCENE_REDRAW );

    // crop toolRegion in case it leaves the screen
    FOdysseyVector::IntersectRegions( toolRegion, imageRegion, redrawRegion );

    mOldPointInTexture.x = iPointInTexture.x;
    mOldPointInTexture.y = iPointInTexture.y;

    if( mPathDrawingHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y ) == true )
    {
        redrawRegion = imageRegion;
    }

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );

    return redrawRegion;
}

void
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    UOdysseyPainterEditorVectorPathDrawingTool::OnMouseDragVector( vectorEngine, vectorScene, iPointInTexture );
}

::ULIS::FRectI
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                                             , FOdysseyVectorScene* iScene
                                                             , const FOdysseyPoint& iPointInTexture )
{
    ::ULIS::FRectI redrawRegion = { 0, 0, 0, 0 };
    ::ULIS::FRectI imageRegion;

    iEngine->GetColorImageSize( imageRegion );

    mPointRadius = iPointInTexture.pressure * Radius;

//UE_LOG(LogTemp, Warning, TEXT("Some warning message %f %f"), iPointInTexture.x, iPointInTexture.y );

    if( mPathBuilder )
    {
        FOdysseyVectorPath* path = mPathBuilder->GetPath();

        BLPoint localCoords;
        BLPoint localRadiusVec;
        double localRadius;
        //float roundedUpRadius = ceil (radius);
        FOdysseyVectorVertex* nextVertex;
        ::ULIS::FRectI toolRegion = GetInvalidationAreaFromPointer( iPointInTexture.x
                                                                  , iPointInTexture.y
                                                                  , ::ULIS::FMath::Max( Radius, StitchingRadius ) )
                                  // we use mOldPointInTexture instead of iPointInTexture.deltaPosition because the latter is not reliable
                                  | GetInvalidationAreaFromPointer( mOldPointInTexture.x
                                                                  , mOldPointInTexture.y
                                                                  , ::ULIS::FMath::Max( Radius, StitchingRadius ) );

        // crop toolRegion in case it leaves the screen, store to redrawRegion
        FOdysseyVector::IntersectRegions( toolRegion, imageRegion, redrawRegion );

        if( mPathDrawingHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y ) == true )
        {
            redrawRegion = imageRegion; // needs full redraw
        }

        localCoords = path->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
        localRadiusVec = path->GetInverseWorldMatrix().mapVector( 0.7071f * mPointRadius, 0.7071f * mPointRadius );
        localRadius = ::ULIS::FVec2D( localRadiusVec.x, localRadiusVec.y ).Distance();

        // small trick to set a radius to the first point, as pressure is only valid in drag events but first point is created on down events.
        if( mPathBuilder->GetPointCount() == 1 )
        {
            if( mStitched && AverageStitchedRadius )
            {
                mPreviousVertex->SetRadius( ( mPreviousVertex->GetRadius() + localRadius ) * 0.5f );
            }
            else
            {
                mPreviousVertex->SetRadius( localRadius );
            }
        }

        nextVertex = mPathBuilder->RecordIntermediate( localCoords.x
                                                     , localCoords.y
                                                     , localRadius
                                                     , mVertexArray
                                                     , mSegmentArray );

        if( nextVertex != mPreviousVertex )
        {
            redrawRegion = imageRegion; // needs full redraw
        }

        mPreviousVertex = nextVertex;

        iScene->Update( FOdysseyVectorObject::KEEPINVALIDATED );
    }

    mOldPointInTexture.x = iPointInTexture.x;
    mOldPointInTexture.y = iPointInTexture.y;

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );

    return redrawRegion;
}

bool
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return false;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return false;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    return UOdysseyPainterEditorVectorPathDrawingTool::OnMouseUpVector( vectorEngine, vectorScene, iPointInTexture, iKey );
}

bool
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                                           , FOdysseyVectorScene* iScene
                                                           , const FOdysseyPoint& iPointInTexture
                                                           , const FKey& iKey )
{
    if( mPathBuilder )
    {
        FOdysseyVectorVertex* vertex = PickVertex( iEngine, iScene, iPointInTexture.x, iPointInTexture.y, StitchingRadius );
        FOdysseyVectorPath* path = mPathBuilder->GetPath();
        FOdysseyVectorSegmentCubic* lastSegment = nullptr;

        // in some conditions (maximizing the window), you can get a Up event without a Down event. Check path exists.
        if( path )
        {
            BLPoint localCoords = path->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
            // in MouseUp events, pressure equals 0. We then use the last pressure value recorded in the Drag event.
            BLPoint localRadius = path->GetInverseWorldMatrix().mapVector( 0.7071f * mPointRadius, 0.7071f * mPointRadius );
            /*float roundedUpRadius = *//*ceil (radius)*//*mPreviousVertex->GetRadius();*/

            // the picked cubic vertex must belong to the path we are working with
            if( vertex )
            {
                if ( vertex->GetPath() != path )
                {
                    vertex = nullptr;
                }
            }

            if( vertex == nullptr )
            {
                vertex = new FOdysseyVectorVertex( path
                                                      , localCoords.x
                                                      , localCoords.y
                                                      , ::ULIS::FVec2D( localRadius.x, localRadius.y ).Distance() );
                // record for undos
                mVertexArray.push_back( vertex );

                path->AddVertex( vertex );
            }

            lastSegment = mPathBuilder->RecordEnd( vertex );

            if( lastSegment )
            {
                // record for undos
                mSegmentArray.push_back( lastSegment );
            }
        }

        iScene->RemoveChild( mPathBuilder );
        delete mPathBuilder;
        mPathBuilder = nullptr;

        if( mStitched == true )
        {
            // needed for valid GUndo pointer
            GEditor->BeginTransaction(LOCTEXT("VectorPathDrawingTool","Vector Path Drawing Tool"));
            if( GUndo )
            {
                std::vector<FOdysseyVectorPath*> addedPathArray; // empty on purpose.
                FOdysseyVectorUndo* undo = static_cast<FOdysseyVectorUndo*>( new FOdysseyVectorUndoPathAlter( iScene
                                                                                                            , addedPathArray
                                                                                                            , mVertexArray
                                                                                                            , mSegmentArray ) );

                GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
            }
            GEditor->EndTransaction();
        }
    }

    mPathDrawingHUD->Reset( iScene ); // refreshes the quadtree;

    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS ); // update invalidated objects

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY // important to remove the path builder from the hierarchy widget
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );

    return true;
}

void
UOdysseyPainterEditorVectorPathDrawingTool::Commit()
{

}

FOdysseyVectorPathBuilder*
UOdysseyPainterEditorVectorPathDrawingTool::GetPathBuilder()
{
    return mPathBuilder;
}

void
UOdysseyPainterEditorVectorPathDrawingTool::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;
    
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    PropertyChangedVector( vectorEngine, vectorScene, PropertyChangedEvent.GetPropertyName() );
}

void
UOdysseyPainterEditorVectorPathDrawingTool::PropertyChangedVector( FOdysseyVectorEngine* iEngine
                                                                 , FOdysseyVectorScene* iScene
                                                                 , const FName& iPropertyName )
{
/*
    if ( iPropertyName == "Radius" )
    {
    }

    if ( iPropertyName == "StitchingRadius" )
    {
    }

    if ( iPropertyName == "Stitch" )
    {
    }
*/
    mPathDrawingHUD->Reset( iScene ); // rebuilds quadtree if stitch mode changes

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

#undef LOCTEXT_NAMESPACE
