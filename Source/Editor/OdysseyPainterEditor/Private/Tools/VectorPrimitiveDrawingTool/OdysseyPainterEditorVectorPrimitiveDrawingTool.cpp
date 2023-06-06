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
    //iEngine->AddHUD( &mDummyHUD );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorPrimitiveDrawingTool::OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                                                  , FOdysseyVectorScene* iScene
                                                                  , const FOdysseyPoint& iPointInTexture
                                                                  , const FKey& iKey )
{
    BLPoint localCoords = iScene->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
    ::ULIS::FColor color = GetEditorAs<FOdysseyPainterEditor>()->PaintColor().GetValue();
    ::ULIS::FColor rgba8 = color.ToFormat( ::ULIS::eFormat::Format_RGBA8 );
    FOdysseyVectorPrimitive* primitive;

    switch( PrimitiveType )
    {
        case EOdysseyVectorPrimitiveType::Ellipse:
            primitive = new FOdysseyVectorEllipse( FString("Circle"), 0.0f, 0.0f, StrokeWidth );
        break;

        case EOdysseyVectorPrimitiveType::Rectangle:
            primitive = new FOdysseyVectorRectangle( FString("Rectangle"), 0.0f, 0.0f, StrokeWidth );
        break;

        default:
            primitive = new FOdysseyVectorEllipse( FString("Circle"), 0.0f, 0.0f, StrokeWidth );
        break;
    }

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
        switch( PrimitiveType )
        {
            case EOdysseyVectorPrimitiveType::Ellipse:
            {
                FOdysseyVectorEllipse* ellipse = static_cast<FOdysseyVectorEllipse*>(primitive);
                BLPoint bldif = ellipse->GetInverseWorldMatrix().mapVector( iPointInTexture.deltaPosition.X
                                                                          , iPointInTexture.deltaPosition.Y );
                ::ULIS::FVec2D dif = ::ULIS::FVec2D( bldif.x, bldif.y );

                ellipse->SetRadius( ellipse->GetRadiusX() + dif.x, ellipse->GetRadiusY() + dif.y /*difPosition.Distance()*/ );
                ellipse->Invalidate();
            }
            break;

            case EOdysseyVectorPrimitiveType::Rectangle:
            {
                FOdysseyVectorRectangle* rectangle = static_cast<FOdysseyVectorRectangle*>(primitive);
                BLPoint bldif = rectangle->GetInverseWorldMatrix().mapVector( iPointInTexture.deltaPosition.X
                                                                            , iPointInTexture.deltaPosition.Y );
                ::ULIS::FVec2D dif = ::ULIS::FVec2D( bldif.x, bldif.y );

                rectangle->SetSize( rectangle->GetWidth() + dif.x, rectangle->GetHeight() + dif.y /*difPosition.Distance()*/ );
                rectangle->Invalidate();
            }
            break;

            default:
            break;
        }
    }

    iScene->Update( FOdysseyVectorObject::FREQUENTUPDATES ); // update invalidated objects
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
