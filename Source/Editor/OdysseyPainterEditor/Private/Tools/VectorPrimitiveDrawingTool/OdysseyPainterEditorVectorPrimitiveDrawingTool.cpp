// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPrimitiveDrawingTool/OdysseyPainterEditorVectorPrimitiveDrawingTool.h"
#include "Undo/OdysseyVectorUndoObjectAdd.h"
#include "OdysseyPaletteEntryColor.h"
#include "PainterEditor/OdysseyPainterEditorColorPaletteTab.h"
#include "PainterEditor/OdysseyPainterEditor.h"
#include "OdysseyMediaVector.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846L
#endif

#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPrimitiveDrawingTool::~UOdysseyPainterEditorVectorPrimitiveDrawingTool()
{
}

UOdysseyPainterEditorVectorPrimitiveDrawingTool::UOdysseyPainterEditorVectorPrimitiveDrawingTool()
    : UOdysseyPainterEditorVectorBaseTool( new FOdysseyPainterEditorVectorBaseToolHUD( this ) )
    , PrimitiveType ( EOdysseyVectorPrimitiveType::Ellipse )
    , StrokeWidth( 4.0f )
    , Uniform( false )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Circle64");
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

bool
UOdysseyPainterEditorVectorPrimitiveDrawingTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
}

uint64
UOdysseyPainterEditorVectorPrimitiveDrawingTool::UnloadVector( FOdysseyVectorGroupPaint* iScene )
{
    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorPrimitiveDrawingTool::LoadVector( FOdysseyVectorGroupPaint* iScene )
{
    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorPrimitiveDrawingTool::OnKeyDownVector( FOdysseyVectorGroupPaint* iScene
                                                                , const FKey& iKey )
{
    UniformAtKeyDown = Uniform;

    if ( FSlateApplication::Get().GetModifierKeys().IsShiftDown() )
    {
        Uniform = !Uniform; // flip the value
    }

    return 0;
}

uint64
UOdysseyPainterEditorVectorPrimitiveDrawingTool::OnKeyUpVector( FOdysseyVectorGroupPaint* iScene
                                                              , const FKey& iKey )
{
    Uniform = UniformAtKeyDown;

    return 0;
}

uint64
UOdysseyPainterEditorVectorPrimitiveDrawingTool::OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                                                  , const FOdysseyPoint& iPointInTexture
                                                                  , const FKey& iKey )
{
    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        BLPoint localCoords = iScene->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
        ::ULIS::FColor color = GetEditor()->PaintColor().GetValue();
        ::ULIS::FColor rgba8 = color.ToFormat( ::ULIS::eFormat::Format_RGBA8 );
        FColor ueColor = FColor( rgba8.R8(), rgba8.G8(), rgba8.B8(), rgba8.A8() );
        FOdysseyVectorPrimitive* primitive;
        UOdysseyPaletteEntry* entry = nullptr;
        BLPoint widthVector = iScene->GetInverseWorldMatrix().mapVector( 0.7071f * StrokeWidth, 0.7071f * StrokeWidth );
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

        iScene->GetEngine()->ClearObjectSelection();
        iScene->GetEngine()->SelectObject( primitive );

        //mSelectionChanged.Broadcast(iScene);

        iScene->Update( 0 ); // update invalidated objects
    }

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED
         | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;
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

uint64
UOdysseyPainterEditorVectorPrimitiveDrawingTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                                                  , const FOdysseyPoint& iPointInTexture )
{
    // Left mouse button clicked
    if( iPointInTexture.keysDown.Find( EKeys::LeftMouseButton ) != INDEX_NONE )
    {
        FOdysseyVectorPrimitive* primitive = static_cast<FOdysseyVectorPrimitive*>( iScene->GetEngine()->GetLastSelectedObject() );

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
    }

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED
         | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;
}

uint64
UOdysseyPainterEditorVectorPrimitiveDrawingTool::OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                                                , const FOdysseyPoint& iPointInTexture
                                                                , const FKey& iKey )
{
    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        FOdysseyVectorPrimitive* primitive = static_cast<FOdysseyVectorPrimitive*>( iScene->GetEngine()->GetLastSelectedObject() );

        if( primitive )
        {
            FOdysseyVectorPath* path = primitive->Convert();

            // Undo must be called before association with parent object
            // needed for valid GUndo pointer
            GEditor->BeginTransaction(LOCTEXT("vector-primitive-drawing-tool.transaction.draw-primitive","Vector Primitive Drawing Tool"));
            if( GUndo )
            {
                FOdysseyVectorUndo *undo = new FOdysseyVectorUndoObjectAdd( iScene, path );

                GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
            }
            GEditor->EndTransaction();

            iScene->GetEngine()->ClearObjectSelection();
            iScene->RemoveChild( primitive );

            delete primitive;

            iScene->AppendChild( path );
            //path->InvalidateAllSegments();
            path->UpdateMatrix();

            iScene->GetEngine()->SelectObject( path );
        }

        iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS ); // update invalidate objects
    }

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
         | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED
         | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED
         | FOdysseyVectorEngine::SIGNAL_OBJECT_TRANSFORMED;
}

#undef LOCTEXT_NAMESPACE
