// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPrimitiveDrawingTool/OdysseyPainterEditorVectorPrimitiveDrawingTool.h"
#include "Undo/OdysseyVectorUndoObjectAdd.h"
#include "OdysseyPaletteEntryColor.h"

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
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

    UniformAtKeyDown = Uniform;

    if ( FSlateApplication::Get().GetModifierKeys().IsShiftDown() )
    {
        Uniform = !Uniform; // flip the value
    }

    UOdysseyPainterEditorDefaultTool::OnKeyDownVector( vectorEngine, vectorScene, iKey );
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
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

    Uniform = UniformAtKeyDown;

    UOdysseyPainterEditorDefaultTool::OnKeyUpVector( vectorEngine, vectorScene, iKey );
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

    if (GetEditor()->GetGUI()->GetColorPaletteTab()->PaletteWidget()->GetColorPalette()->GetPalette())
    {
        entry = GetEditor()->GetGUI()->GetColorPaletteTab()->PaletteWidget()->GetColorPalette()->GetPalette()->CurrentEntry.Get();
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
        case EOdysseyVectorPrimitiveType::Ellipse:
            primitive = new FOdysseyVectorEllipse( FString("Circle"), 0.0f, 0.0f, StrokeWidth );
        break;

        case EOdysseyVectorPrimitiveType::Rectangle:
            primitive = new FOdysseyVectorRectangle( FString("Rectangle"), 0.0f, 0.0f, StrokeWidth );
        break;

        case EOdysseyVectorPrimitiveType::Line:
            primitive = new FOdysseyVectorLine( FString("Rectangle"), 0.0f, 0.0f, StrokeWidth );
        break;

        default:
            primitive = new FOdysseyVectorEllipse( FString("Circle"), 0.0f, 0.0f, StrokeWidth );
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
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );

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

        if( Uniform || FSlateApplication::Get().GetModifierKeys().IsControlDown() )
        {
            size.x = ::ULIS::FVec2D( bldif.x, bldif.y ).Distance() * 0.7071f;
            size.y = size.x;
        }

        switch( PrimitiveType )
        {
            case EOdysseyVectorPrimitiveType::Ellipse:
            {
                FOdysseyVectorEllipse* ellipse = static_cast<FOdysseyVectorEllipse*>(primitive);

                ellipse->SetRadius( size.x, size.y );
            }
            break;

            case EOdysseyVectorPrimitiveType::Rectangle:
            {
                FOdysseyVectorRectangle* rectangle = static_cast<FOdysseyVectorRectangle*>(primitive);

                rectangle->SetSize( size.x, size.y );
            }
            break;

            case EOdysseyVectorPrimitiveType::Line:
            {
                FOdysseyVectorLine* line = static_cast<FOdysseyVectorLine*>(primitive);

                line->SetSize( size.x, size.y );
            }
            break;

            default:
            break;
        }
    }

    iScene->Update( /*FOdysseyVectorObject::FREQUENTUPDATES*/0 ); // update invalidated objects

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
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
        path->UpdateMatrix();
        iScene->Select( path );
    }

    iScene->Update( 0 ); // update invalidate objects

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
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
