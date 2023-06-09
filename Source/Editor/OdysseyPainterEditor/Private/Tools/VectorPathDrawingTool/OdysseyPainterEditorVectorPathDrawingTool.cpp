// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathDrawingTool/OdysseyPainterEditorVectorPathDrawingTool.h"
#include "Tools/VectorPathDrawingTool/OdysseyPainterEditorVectorPathDrawingToolHUD.h"
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
    , Absolute( true )
    , Stitch( false )
    , AverageStitchedRadius( true )
    , StitchingRadius( 10 )
    , mPathBuilder( nullptr )
    , mPreviousVertex( nullptr )
    , mOldPointInTexture( 0, 0 )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.VectoPen64");

    mPathDrawingHUD = new FOdysseyPainterEditorVectorPathDrawingToolHUD( this );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorPathDrawingTool::UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->RemoveHUD( mPathDrawingHUD );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
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

//    TSharedPtr<SOdysseyPaintModifiers> widget = GetEditorAs<FOdysseyPainterEditor>()->GetGUI()->GetTopTab().Get()->GetWidget();

//    widget.Get()->OnSizeChanged.AddRaw( this, &UOdysseyPainterEditorVectorPathDrawingTool::OnSizeChanged );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
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
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                                             , FOdysseyVectorScene* iScene
                                                             , const FOdysseyPoint& iPointInTexture
                                                             , const FKey& iKey )
{
    //TODO: change architecture to have a easier time getting the palette
    ::ULIS::FColor color = GetEditorAs<FOdysseyPainterEditor>()->PaintColor().GetValue();
    UOdysseyPaletteEntry* entry = nullptr;
    if (GetEditorAs<FOdysseyPainterEditor>()->GetGUI()->GetColorPaletteTab()->PaletteWidget()->GetColorPalette()->GetPalette())
    {
        entry = GetEditorAs<FOdysseyPainterEditor>()->GetGUI()->GetColorPaletteTab()->PaletteWidget()->GetColorPalette()->GetPalette()->CurrentEntry.Get();
        if (entry && entry->IsA(UOdysseyPaletteEntryColor::StaticClass()))
        {
            UOdysseyPaletteEntryColor* colorEntry = Cast< UOdysseyPaletteEntryColor >(entry);
            color = ::ULIS::FColor::RGBA8(colorEntry->EntryColor.R, colorEntry->EntryColor.G, colorEntry->EntryColor.B, colorEntry->EntryColor.A);
        }
    }

    ::ULIS::FColor rgba8 = color.ToFormat( ::ULIS::eFormat::Format_RGBA8 );
    FOdysseyVectorVertex* cubicVertex = PickVertex( iEngine, iScene, iPointInTexture.x, iPointInTexture.y, StitchingRadius );
    FOdysseyVectorPathCubic* cubicPath = nullptr;
    BLPoint localCoords;

 //UE_LOG(LogTemp, Warning, TEXT("radius:%f iPointInTexture.pressure:%f"), radius, iPointInTexture.pressure ); 

    // record for later undoing
    mVertexArray.clear();
    mSegmentArray.clear();

    // this is important to know what undo operation we are going to record: an ObjectAdd or a PathDrawing.
    mStitched = true;

    if( cubicVertex )
    {
        cubicPath = static_cast<FOdysseyVectorPathCubic*>( cubicVertex->GetPath() );
    }
    else
    {
        if ( cubicPath == nullptr )
        {
            cubicPath = new FOdysseyVectorPathCubic( FString("CubicPath") );

            // This undo must be set before association with the new parent object
            // needed for valid GUndo pointer
            GEditor->BeginTransaction(LOCTEXT("VectorPathDrawingTool","Vector Path Drawing Tool"));
            if( GUndo )
            {
                FOdysseyVectorUndo* undo = static_cast<FOdysseyVectorUndo*>( new FOdysseyVectorUndoObjectAdd( iScene, cubicPath ) );

                GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
            }
            GEditor->EndTransaction();

            iScene->AppendChild( cubicPath );

            // this is important to know what undo operation we are going to record: an ObjectAdd or a PathDrawing.
            mStitched = false;
        }

        cubicVertex = new FOdysseyVectorVertex( cubicPath, 0.0f, 0.0f, 0.0f );

        cubicPath->AddVertex( cubicVertex );
        // record for undos
        mVertexArray.push_back( cubicVertex );
    }

    mPreviousVertex = cubicVertex;

    cubicPath->UpdateMatrix();
    cubicPath->SetForegroundColor( rgba8.R8(), rgba8.G8(), rgba8.B8(), rgba8.A8() );
    
    if (entry && entry->IsA(UOdysseyPaletteEntryColor::StaticClass()))
        cubicPath->SetPaletteEntry( entry );

    localCoords = cubicPath->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
    //localRadius = cubicPath->GetInverseWorldMatrix().mapVector( 0.7071f * radius, 0.7071f * radius );

    cubicVertex->Set( localCoords.x, localCoords.y );
    // cubicVertex->SetRadius( ::ULIS::FVec2D( localRadius.x, localRadius.y ).Distance() );

    mPathBuilder = new FOdysseyVectorPathBuilder();

    iScene->AppendChild( mPathBuilder );

    mPathBuilder->Attach( cubicPath );
    cubicPath->CopyTransformation( *cubicPath );
    mPathBuilder->UpdateMatrix();
    mPathBuilder->SetForegroundColor( cubicPath->GetForegroundColor() );
    mPathBuilder->RecordStart( cubicVertex );

    iScene->ClearSelection();
    iScene->Select( cubicPath );

    // update invalidated objects
    iScene->Update( 0 );
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW
                  | FOdysseyVectorScene::SIGNAL_OBJECT_SELECTED
                  | FOdysseyVectorScene::SIGNAL_OBJECT_MODIFIED
                  | FOdysseyVectorScene::SIGNAL_OBJECT_TRANSFORMED );

    mPathDrawingHUD->Reset( iScene ); // re-creates the quadtree;

    return true;
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

    return redrawRegion;
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
        FOdysseyVectorPathCubic* cubicPath = mPathBuilder->GetCubicPath();

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

        localCoords = cubicPath->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
        localRadiusVec = cubicPath->GetInverseWorldMatrix().mapVector( 0.7071f * mPointRadius, 0.7071f * mPointRadius );
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

        iScene->Update( FOdysseyVectorObject::FREQUENTUPDATES | FOdysseyVectorObject::KEEPINVALIDATED );
    }

    mOldPointInTexture.x = iPointInTexture.x;
    mOldPointInTexture.y = iPointInTexture.y;

    //iEngine->InvalidateRegion( redrawRegion );
    // commented out: redrawing will be performed by the caller function
    //iScene->Signal( FOdysseyVectorScene::SCENE_REDRAW );

    return redrawRegion;
}

bool
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                                           , FOdysseyVectorScene* iScene
                                                           , const FOdysseyPoint& iPointInTexture
                                                           , const FKey& iKey )
{
    if( mPathBuilder )
    {
        FOdysseyVectorVertex* cubicVertex = PickVertex( iEngine, iScene, iPointInTexture.x, iPointInTexture.y, StitchingRadius );
        FOdysseyVectorPathCubic* cubicPath = mPathBuilder->GetCubicPath();
        FOdysseyVectorSegmentCubic* lastSegment = nullptr;

        // in some conditions (maximizing the window), you can get a Up event without a Down event. Check cubicPath exists.
        if( cubicPath )
        {
            BLPoint localCoords = cubicPath->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
            // in MouseUp events, pressure equals 0. We then use the last pressure value recorded in the Drag event.
            BLPoint localRadius = cubicPath->GetInverseWorldMatrix().mapVector( 0.7071f * mPointRadius, 0.7071f * mPointRadius );
            /*float roundedUpRadius = *//*ceil (radius)*//*mPreviousVertex->GetRadius();*/

            // the picked cubic vertex must belong to the path we are working with
            if( cubicVertex )
            {
                if ( cubicVertex->GetPath() != cubicPath )
                {
                    cubicVertex = nullptr;
                }
            }

            if( cubicVertex == nullptr )
            {
                cubicVertex = new FOdysseyVectorVertex( cubicPath
                                                      , localCoords.x
                                                      , localCoords.y
                                                      , ::ULIS::FVec2D( localRadius.x, localRadius.y ).Distance() );
                // record for undos
                mVertexArray.push_back( cubicVertex );

                cubicPath->AddVertex( cubicVertex );
            }

            lastSegment = mPathBuilder->RecordEnd( cubicVertex );

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

    iScene->Update( 0 ); // update invalidated objects
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW | FOdysseyVectorScene::SIGNAL_OBJECT_MODIFIED );

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

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

#undef LOCTEXT_NAMESPACE
