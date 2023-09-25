// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPrimitiveDrawingTool/OdysseyPainterEditorVectorPrimitiveDrawingTool.h"
#include "Undo/OdysseyVectorUndoObjectAdd.h"
#include "OdysseyPaletteEntryColor.h"
#include "PainterEditor/OdysseyPainterEditorColorPaletteTab.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846L
#endif

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorPrimitiveDrawingTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPrimitiveDrawingTool::~UOdysseyPainterEditorVectorPrimitiveDrawingTool()
{
}

UOdysseyPainterEditorVectorPrimitiveDrawingTool::UOdysseyPainterEditorVectorPrimitiveDrawingTool()
    : PrimitiveType ( EOdysseyVectorPrimitiveType::Ellipse )
    , StrokeWidth( 4.0f )
    , Uniform( false )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Circle64");
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorPrimitiveDrawingTool::Load()
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    UOdysseyPainterEditorVectorPrimitiveDrawingTool::LoadVector( vectorEngine, vectorScene );
}

void
UOdysseyPainterEditorVectorPrimitiveDrawingTool::Unload()
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    UOdysseyPainterEditorVectorPrimitiveDrawingTool::UnloadVector( vectorEngine, vectorScene );
}

bool
UOdysseyPainterEditorVectorPrimitiveDrawingTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
}

void
UOdysseyPainterEditorVectorPrimitiveDrawingTool::UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    //iEngine->RemoveHUD( &mDummyHUD );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorPrimitiveDrawingTool::LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    //iEngine->ClearHUD();
    //iEngine->AddHUD( &mDummyHUD );

    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorPrimitiveDrawingTool::OnKeyDown( const FKey& iKey )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return false;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return false;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();

    UniformAtKeyDown = Uniform;

    if ( FSlateApplication::Get().GetModifierKeys().IsShiftDown() )
    {
        Uniform = !Uniform; // flip the value
    }

    UOdysseyPainterEditorDefaultTool::OnKeyDownVector( vectorScene, iKey );
    //iScene->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );

    return false;
}

bool
UOdysseyPainterEditorVectorPrimitiveDrawingTool::OnKeyUp( const FKey& iKey )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return false;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return false;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();

    Uniform = UniformAtKeyDown;

    UOdysseyPainterEditorDefaultTool::OnKeyUpVector( vectorScene, iKey );
    //iScene->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );

    return false;
}

bool
UOdysseyPainterEditorVectorPrimitiveDrawingTool::OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return false;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return false;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    return UOdysseyPainterEditorVectorPrimitiveDrawingTool::OnMouseDownVector( vectorEngine, vectorScene, iPointInTexture,iKey  );
}


bool
UOdysseyPainterEditorVectorPrimitiveDrawingTool::OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                                                  , FOdysseyVectorScene* iScene
                                                                  , const FOdysseyPoint& iPointInTexture
                                                                  , const FKey& iKey )
{
    BLPoint localCoords = iScene->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
    ::ULIS::FColor color = GetEditor()->PaintColor().GetValue();
    ::ULIS::FColor rgba8 = color.ToFormat( ::ULIS::eFormat::Format_RGBA8 );
    FColor ueColor = FColor( rgba8.R8(), rgba8.G8(), rgba8.B8(), rgba8.A8() );
    FOdysseyVectorPrimitive* primitive;
    UOdysseyPaletteEntry* entry = nullptr;
    BLPoint widthVector = iScene->GetInverseWorldMatrix().mapVector( 0.7071f, 0.7071f );
    ::ULIS::FVec2D width = ::ULIS::FVec2D( widthVector.x, widthVector.y );

    TSharedPtr<FOdysseyPainterEditorPaletteTab> colorPaletteTab = GetEditor()->FindTab<FOdysseyPainterEditorPaletteTab>();
    if (colorPaletteTab->PaletteWidget()->GetColorPalette()->GetPalette())
    {
        entry = colorPaletteTab->PaletteWidget()->GetColorPalette()->GetPalette()->CurrentEntry.Get();
        if (entry && entry->IsA(UOdysseyPaletteEntryColor::StaticClass()))
        {
            FColor colorEntry = Cast< UOdysseyPaletteEntryColor >(entry)->GetUsedColor();
            ueColor = colorEntry;
        }
    }

    mMouseDown.x = iPointInTexture.x;
    mMouseDown.y = iPointInTexture.y;

    switch( PrimitiveType )
    {
        case EOdysseyVectorPrimitiveType::Rectangle:
            primitive = new FOdysseyVectorRectangle( FString("Rectangle"), 0.0f, 0.0f, width.Distance() );
        break;

        case EOdysseyVectorPrimitiveType::Line:
            primitive = new FOdysseyVectorLine( FString("Line"), 0.0f, 0.0f, width.Distance() );
        break;

        default:
            primitive = new FOdysseyVectorEllipse( FString("Circle"), 0.0f, 0.0f, width.Distance() );
        break;
    }

    if (entry && entry->IsA(UOdysseyPaletteEntryColor::StaticClass()))
        primitive->GetForegroundBucket().SetPaletteEntry(entry);

    iScene->AppendChild( primitive );

    primitive->SetForegroundColor( ueColor );
    primitive->Translate( localCoords.x, localCoords.y );
    primitive->UpdateMatrix();

    iScene->ClearSelection();
    iScene->Select( primitive );

    //mSelectionChanged.Broadcast(iScene);

    iScene->Update( 0 ); // update invalidated objects

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED
                   | FOdysseyVectorEngine::SIGNAL_INTERACTIVE );

    return true;
}

void
UOdysseyPainterEditorVectorPrimitiveDrawingTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    UOdysseyPainterEditorVectorPrimitiveDrawingTool::OnMouseDragVector( vectorEngine, vectorScene, iPointInTexture );
}

double
UOdysseyPainterEditorVectorPrimitiveDrawingTool::GetLineRotationAngle( FOdysseyVectorLine* iLine
                                                                     , const FOdysseyPoint& iPointInTexture )
{
    BLPoint pt = iLine->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
    ::ULIS::FVec2D hzVector = ::ULIS::FVec2D( 1.0f, 0.0f );
    ::ULIS::FVec2D ptVector = ::ULIS::FVec2D( pt.x, pt.y );
    double angle = 0.0f;

    if( ptVector.DistanceSquared() )
    {
        ptVector.Normalize();

        angle = fabs( acos( hzVector.DotProduct( ptVector ) ) );

        return FOdysseyVector::Cross2D( hzVector, ptVector ) >= 0.0f ? angle : ( M_PI * 2.0f ) - angle;
    }

    return 0.0f;
}

void
UOdysseyPainterEditorVectorPrimitiveDrawingTool::OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                                                  , FOdysseyVectorScene* iScene
                                                                  , const FOdysseyPoint& iPointInTexture )
{
    FOdysseyVectorPrimitive* primitive = static_cast<FOdysseyVectorPrimitive*>( iScene->GetLastSelected() );

    if( primitive )
    {
        BLPoint bldif = primitive->GetInverseWorldMatrix().mapVector( iPointInTexture.x - mMouseDown.x
                                                                    , iPointInTexture.y - mMouseDown.y );
        ::ULIS::FVec2D size = ::ULIS::FVec2D( bldif.x, bldif.y );

        switch( PrimitiveType )
        {
            case EOdysseyVectorPrimitiveType::Ellipse:
            {
                FOdysseyVectorEllipse* ellipse = static_cast<FOdysseyVectorEllipse*>(primitive);

                if( Uniform )
                {
                    size.x = ::ULIS::FVec2D( bldif.x, bldif.y ).Distance() * 0.7071f;
                    size.y = size.x;
                }

                ellipse->SetRadius( size.x, size.y );
            }
            break;

            case EOdysseyVectorPrimitiveType::Rectangle:
            {
                FOdysseyVectorRectangle* rectangle = static_cast<FOdysseyVectorRectangle*>(primitive);

                if( Uniform )
                {
                    size.x = ::ULIS::FVec2D( bldif.x, bldif.y ).Distance() * 0.7071f;
                    size.y = size.x;
                }

                rectangle->SetSize( size.x, size.y );
            }
            break;

            case EOdysseyVectorPrimitiveType::Line:
            {
                FOdysseyVectorLine* line = static_cast<FOdysseyVectorLine*>(primitive);

                if( Uniform )
                {
                    BLMatrix2D& inverseMatrix = line->GetInverseWorldMatrix();
                    BLPoint pt = inverseMatrix.mapPoint( iPointInTexture.x, iPointInTexture.y );
                    double distance = ::ULIS::FVec2D( pt.x, pt.y ).Distance();
                    int rotation = GetLineRotationAngle( line, iPointInTexture ) / M_PI * 180.0f;

                     // to force the tip of the line in the middle of the 45 degrees steps
                    rotation += 22;
                    // 45 deg by 45 deg
                    double angle = ( ( rotation / 45 ) * 45 ) * M_PI / 180.0f;

                    line->SetSize( cos( angle ) * distance, sin( angle ) * distance );
                }
                else
                {
                    line->SetSize( size.x, size.y );
                }
            }
            break;

            default:
            break;
        }
    }

    iScene->Update( /*FOdysseyVectorObject::FREQUENTUPDATES*/0 ); // update invalidated objects

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED
                   | FOdysseyVectorEngine::SIGNAL_INTERACTIVE );
}

bool
UOdysseyPainterEditorVectorPrimitiveDrawingTool::OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return false;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return false;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    return UOdysseyPainterEditorVectorPrimitiveDrawingTool::OnMouseUpVector( vectorEngine, vectorScene, iPointInTexture, iKey );
}

bool
UOdysseyPainterEditorVectorPrimitiveDrawingTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                                                , FOdysseyVectorScene* iScene
                                                                , const FOdysseyPoint& iPointInTexture
                                                                , const FKey& iKey )
{
    FOdysseyVectorPrimitive* primitive = static_cast<FOdysseyVectorPrimitive*>( iScene->GetLastSelected() );

    if( primitive )
    {
        FOdysseyVectorPath* path = primitive->Convert();

        // Undo must be called before association with parent object
        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("VectorPrimitiveDrawingTool","Vector Primitive Drawing Tool"));
        if( GUndo )
        {
            FOdysseyVectorUndo *undo = new FOdysseyVectorUndoObjectAdd( iScene, path );

            GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
        }
        GEditor->EndTransaction();

        iScene->ClearSelection();
        iScene->RemoveChild( primitive );

        delete primitive;

        iScene->AppendChild( path );
        //path->InvalidateAllSegments();
        path->UpdateMatrix();
        iScene->Select( path );
    }

    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS ); // update invalidate objects

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_TRANSFORMED );

    return false;
}

void
UOdysseyPainterEditorVectorPrimitiveDrawingTool::Commit()
{
}

#undef LOCTEXT_NAMESPACE
