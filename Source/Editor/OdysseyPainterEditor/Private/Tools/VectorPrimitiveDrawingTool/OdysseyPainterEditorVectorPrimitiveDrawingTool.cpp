// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPrimitiveDrawingTool/OdysseyPainterEditorVectorPrimitiveDrawingTool.h"
#include "Undo/OdysseyVectorUndoObjectAdd.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPrimitiveDrawingTool::~UOdysseyPainterEditorVectorPrimitiveDrawingTool()
{
}

UOdysseyPainterEditorVectorPrimitiveDrawingTool::UOdysseyPainterEditorVectorPrimitiveDrawingTool()
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Circle64");
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorPrimitiveDrawingTool::Activate( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->ClearHUD();
}

bool
UOdysseyPainterEditorVectorPrimitiveDrawingTool::OnMouseDown( FOdysseyVectorEngine* iEngine
                                                            , FOdysseyVectorScene* iScene
                                                            , const FOdysseyPoint& iPointInTexture
                                                            , const FKey& iKey )
{
    BLPoint localCoords = iScene->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
    ::ULIS::FColor color = GetEditorAs<FOdysseyPainterEditor>()->PaintColor().GetValue();
    ::ULIS::FColor rgba8 = color.ToFormat( ::ULIS::eFormat::Format_RGBA8 );
    FOdysseyVectorEllipse* circle = FOdysseyVectorEllipse::New( "Circle", 0.0f, 0.0f );

    iScene->AppendChild( circle );

    circle->SetForegroundColor( rgba8.R8(), rgba8.G8(), rgba8.B8(), rgba8.A8() );
    circle->Translate( localCoords.x, localCoords.y );
    circle->UpdateMatrix();

    iScene->ClearSelection();
    iScene->Select( circle );

    mSelectionChanged.Broadcast(iScene);

    return true;
}

void
UOdysseyPainterEditorVectorPrimitiveDrawingTool::OnMouseDrag( FOdysseyVectorEngine* iEngine
                                                            , FOdysseyVectorScene* iScene
                                                            , const FOdysseyPoint& iPointInTexture )
{
    FOdysseyVectorEllipse* circle = static_cast<FOdysseyVectorEllipse*>( iScene->GetLastSelected() );

    if( circle )
    {
        /*BLPoint localCoords = circle->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );*/
        BLPoint bldif = circle->GetInverseWorldMatrix().mapVector( iPointInTexture.deltaPosition.X
                                                                 , iPointInTexture.deltaPosition.Y );
        ::ULIS::FVec2D dif = ::ULIS::FVec2D( bldif.x, bldif.y );

        circle->SetRadius( circle->GetRadiusX() + dif.x, circle->GetRadiusY() + dif.y /*difPosition.Distance()*/ );
        circle->Invalidate();

        iScene->Update( 0 );

        mSelectionChanged.Broadcast(iScene);
    }
}

bool
UOdysseyPainterEditorVectorPrimitiveDrawingTool::OnMouseUp( FOdysseyVectorEngine* iEngine
                                                          , FOdysseyVectorScene* iScene
                                                          , FOdysseyVectorUndo** iUndo
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , const FKey& iKey )
{
    FOdysseyVectorEllipse* ellipse = static_cast<FOdysseyVectorEllipse*>( iScene->GetLastSelected() );

    if( ellipse )
    {
        // BeginTransaction() must be called for GUndo to have a value. Please do it in the caller function.
        if( iUndo && GUndo )
        {
            (*iUndo) = new FOdysseyVectorUndoObjectAdd( iScene, ellipse );

            GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(*iUndo) );
        }
    }

    return false;
}

void
UOdysseyPainterEditorVectorPrimitiveDrawingTool::Commit()
{
}
