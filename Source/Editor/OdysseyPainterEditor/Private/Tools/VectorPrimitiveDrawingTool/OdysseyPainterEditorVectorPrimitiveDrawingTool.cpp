// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPrimitiveDrawingTool/OdysseyPainterEditorVectorPrimitiveDrawingTool.h"
#include "Undo/OdysseyVectorUndoObjectAdd.h"

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
UOdysseyPainterEditorVectorPrimitiveDrawingTool::UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    //iEngine->RemoveHUD( &mDummyHUD );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorPrimitiveDrawingTool::LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    //iEngine->ClearHUD();
    //iEngine->AddHUD( &mDummyHUD );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorPrimitiveDrawingTool::OnKeyDownVector( FOdysseyVectorEngine* iEngine
                                                                , FOdysseyVectorScene* iScene
                                                                , const FKey& iKey )
{
    UniformAtKeyDown = Uniform;

    if ( FSlateApplication::Get().GetModifierKeys().IsShiftDown() )
    {
        Uniform = true;
    }

    UOdysseyPainterEditorDefaultTool::OnKeyDownVector( iEngine, iScene, iKey );
    //iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );

    return false;
}

bool
UOdysseyPainterEditorVectorPrimitiveDrawingTool::OnKeyUpVector( FOdysseyVectorEngine* iEngine
                                                              , FOdysseyVectorScene* iScene
                                                              , const FKey& iKey )
{
    Uniform = UniformAtKeyDown;

    UOdysseyPainterEditorDefaultTool::OnKeyUpVector( iEngine, iScene, iKey );
    //iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );

    return false;
}


bool
UOdysseyPainterEditorVectorPrimitiveDrawingTool::OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                                                  , FOdysseyVectorScene* iScene
                                                                  , const FOdysseyPoint& iPointInTexture
                                                                  , const FKey& iKey )
{
    BLPoint localCoords = iScene->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );

    ::ULIS::FColor color = GetEditorAs<FOdysseyPainterEditor>()->PaintColor().GetValue();
    UOdysseyPaletteEntry* entry = nullptr;
    if (GetEditorAs<FOdysseyPainterEditor>()->GetGUI()->GetColorPaletteTab()->PaletteWidget()->GetColorPalette()->GetPalette())
    {
        entry = GetEditorAs<FOdysseyPainterEditor>()->GetGUI()->GetColorPaletteTab()->PaletteWidget()->GetColorPalette()->GetPalette()->CurrentEntry.Get();
        if (entry && entry->IsA(UOdysseyPaletteEntryColor::StaticClass()))
        {
            FColor colorEntry = Cast< UOdysseyPaletteEntryColor >(entry)->GetUsedColor();
            color = ::ULIS::FColor::RGBA8(colorEntry.R, colorEntry.G, colorEntry.B, colorEntry.A);
        }
    }

    ::ULIS::FColor rgba8 = color.ToFormat( ::ULIS::eFormat::Format_RGBA8 );

    FOdysseyVectorPrimitive* primitive;

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

    primitive->SetForegroundColor( rgba8.R8(), rgba8.G8(), rgba8.B8(), rgba8.A8() );
    primitive->Translate( localCoords.x, localCoords.y );
    primitive->UpdateMatrix();

    iScene->ClearSelection();
    iScene->Select( primitive );

    //mSelectionChanged.Broadcast(iScene);

    iScene->Update( 0 ); // update invalidated objects
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW | FOdysseyVectorScene::SIGNAL_OBJECT_SELECTED );

    return true;
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
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW | FOdysseyVectorScene::SIGNAL_OBJECT_MODIFIED );
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
        FOdysseyVectorPathCubic* cubicPath = primitive->Convert();

        // Undo must be called before association with parent object
        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("VectorPrimitiveDrawingTool","Vector Primitive Drawing Tool"));
        if( GUndo )
        {
            FOdysseyVectorUndo *undo = new FOdysseyVectorUndoObjectAdd( iScene, cubicPath );

            GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
        }
        GEditor->EndTransaction();

        iScene->ClearSelection();
        iScene->RemoveChild( primitive );

        delete primitive;

        iScene->AppendChild( cubicPath );
        cubicPath->UpdateMatrix();
        iScene->Select( cubicPath );
    }

    iScene->Update( 0 ); // update invalidate objects
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW
                  | FOdysseyVectorScene::SIGNAL_OBJECT_SELECTED
                  | FOdysseyVectorScene::SIGNAL_OBJECT_MODIFIED
                  | FOdysseyVectorScene::SIGNAL_OBJECT_TRANSFORMED );

    return false;
}

void
UOdysseyPainterEditorVectorPrimitiveDrawingTool::Commit()
{
}

#undef LOCTEXT_NAMESPACE
