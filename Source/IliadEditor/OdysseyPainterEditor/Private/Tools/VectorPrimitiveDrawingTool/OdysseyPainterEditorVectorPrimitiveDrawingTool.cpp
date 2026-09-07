// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/VectorPrimitiveDrawingTool/OdysseyPainterEditorVectorPrimitiveDrawingTool.h"

#include "Editor.h"
#include "ToolMenu.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Layout/SBox.h"

#include "OdysseyPaletteEntryColor.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorSource.h"
#include "OdysseyMediaVector.h"
#include "ISinglePropertyView.h"
#include "Tools/VectorPrimitiveDrawingTool/OdysseyPainterEditorVectorPrimitiveDrawingToolHUD.h"
#include "OdysseyPainterEditorSource.h"
#include "SOdysseySinglePropertyView.h"

// Vector engine
#include "OdysseyVector.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorPrimitive.h"
#include "OdysseyVectorEllipse.h"
#include "OdysseyVectorLine.h"
#include "OdysseyVectorRectangle.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyVectorCell.h"
#include "Undo/OdysseyVectorUndoObjectAdd.h"

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
    : UOdysseyPainterEditorVectorBaseTool( MakeShared<FOdysseyPainterEditorVectorPrimitiveDrawingToolHUD>( this ), true, true )
    //, PrimitiveType ( EOdysseyVectorPrimitiveType::Ellipse )
    , Brush( nullptr )
    , StrokeWidth( 4.0f )
    , Uniform( false )
    , mPrimitive( nullptr )
    , mRectangleNumber ( 0 )
    , mLineNumber( 0 )
    , mEllipseNumber( 0 )
{
    mIconStyleSet = FName(TEXT("PainterEditor.ToolsTab.Shapes64"));

    Shapes.AddShapeType( EOdysseyShapeType::kLine, nullptr );
    Shapes.AddShapeType( EOdysseyShapeType::kRectangle, nullptr );
    Shapes.AddShapeType( EOdysseyShapeType::kEllipse, nullptr );

    Shapes.SetActiveShapeType( EOdysseyShapeType::kEllipse );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

bool
UOdysseyPainterEditorVectorPrimitiveDrawingTool::IsActivable() const
{
    uint64 HUDFlags = GetEditor()->GetVectorHUDFlags();

    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>()
          && ( HUDFlags & FOdysseyVectorHUD::HUD_MODE_OBJECT
            || HUDFlags & FOdysseyVectorHUD::HUD_MODE_VERTEX );
}

uint64
UOdysseyPainterEditorVectorPrimitiveDrawingTool::UnloadVector( FOdysseyVectorGroupPaint* iScene )
{
    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    // Note: we use mworkingLayer because is not called in the same context as Load, so there could be
    // an orphan cell here in the case of a cell deletion for example.
    mWorkingLayer->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // force redrawing when we switch tool
    if( iScene->GetCell()->GetLayer() == mWorkingLayer )
    {
        mWorkingLayer->RequestRedraw( iScene->GetCell(), 0 );
    }

    return 0;
}

uint64
UOdysseyPainterEditorVectorPrimitiveDrawingTool::LoadVector( FOdysseyVectorGroupPaint* iScene )
{
    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // force redrawing when we switch tool
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    return 0;
}

bool
UOdysseyPainterEditorVectorPrimitiveDrawingTool::OnKeyDownVector( FOdysseyVectorGroupPaint* iScene
                                                                , const FKey& iKey )
{
    UniformAtKeyDown = Uniform;

    if ( FSlateApplication::Get().GetModifierKeys().IsShiftDown() )
    {
        Uniform = !Uniform; // flip the value
        return true;
    }

    return false;
}

bool
UOdysseyPainterEditorVectorPrimitiveDrawingTool::OnKeyUpVector( FOdysseyVectorGroupPaint* iScene
                                                              , const FKey& iKey )
{
    Uniform = UniformAtKeyDown;

    return false;
}

FOdysseyVectorObject*
UOdysseyPainterEditorVectorPrimitiveDrawingTool::GetParentObject( FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorObject* parentObject = iScene;

    // Add the path to the current unique selected group
    if( iScene->GetCell()->GetSelectedObjectList().size() == 1 )
    {
        FOdysseyVectorObject* selectedObject = iScene->GetCell()->GetLastSelectedObject();

        if(  selectedObject->HasBaseClass( FOdysseyVectorGroup::StaticClass() ) )
        {
            parentObject = selectedObject;
        }
    }

    return parentObject;
}

bool
UOdysseyPainterEditorVectorPrimitiveDrawingTool::OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                                                  , const FOdysseyPoint& iPointInTexture
                                                                  , const FKey& iKey )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( ( iKey == EKeys::LeftMouseButton ) && GetWorkingGroup()->IsVisible( true ) )
    {
        FOdysseyVectorObject* parentObject = GetParentObject( iScene );
        BLPoint localCoords = parentObject->GetInverseWorldMatrix().map_point( iPointInTexture.x
                                                                            , iPointInTexture.y );
        UOdysseyPaletteEntry* entry = nullptr;
        BLPoint widthVector = parentObject->GetInverseWorldMatrix().map_vector( 0.7071f * StrokeWidth
                                                                             , 0.7071f * StrokeWidth );
        ::ULIS::FVec2D width = ::ULIS::FVec2D( widthVector.x, widthVector.y );

        mMouseDown.x = iPointInTexture.x;
        mMouseDown.y = iPointInTexture.y;

        mPrimitive = nullptr;

        switch( Shapes.GetActiveShapeType() )
        {
            case EOdysseyShapeType::kEllipse:
                mPrimitive = new FOdysseyVectorEllipse( FString("Ellipse_" + FString::FromInt( mEllipseNumber++ )), 0.0f, 0.0f, width.Distance() );
            break;

            case EOdysseyShapeType::kRectangle :
                mPrimitive = new FOdysseyVectorRectangle( FString("Rectangle_") + FString::FromInt( mRectangleNumber++ ), 0.0f, 0.0f, width.Distance() );
            break;

            case EOdysseyShapeType::kLine:
                mPrimitive = new FOdysseyVectorLine( FString("Line_") + FString::FromInt( mLineNumber++ ), 0.0f, 0.0f, width.Distance() );
            break;

            default:
            break;
        }

        parentObject->AppendChild( mPrimitive );

        SetPathColor( mPrimitive );
        mPrimitive->SetOpacity( 1.0f );
        mPrimitive->SetBrush( Brush );
        //mPrimitive->SetForegroundColor( ueColor );
        mPrimitive->Translate( localCoords.x, localCoords.y );
        mPrimitive->UpdateMatrix();

        //iScene->GetEngine()->ClearObjectSelection();
        //iScene->GetEngine()->SelectObject( mPrimitive );

        //mSelectionChanged.Broadcast(iScene);

        iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_INTERACTIVE
                                  | FOdysseyVectorObject::UPDATE_NOINBETWEENING ); // update invalidated objects
    }

    // redraw
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), FOdysseyVectorCell::REDRAW_INTERACTIVE );

    return true;
}

double
UOdysseyPainterEditorVectorPrimitiveDrawingTool::GetLineRotationAngle( FOdysseyVectorLine* iLine
                                                                     , const FOdysseyPoint& iPointInTexture )
{
    BLPoint pt = iLine->GetInverseWorldMatrix().map_point( iPointInTexture.x, iPointInTexture.y );
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
UOdysseyPainterEditorVectorPrimitiveDrawingTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                                                  , const FOdysseyPoint& iPointInTexture )
{
    // Left mouse button clicked
    if( iPointInTexture.keysDown.Find( EKeys::LeftMouseButton ) != INDEX_NONE )
    {
        if( mPrimitive )
        {
            BLPoint bldif = mPrimitive->GetInverseWorldMatrix().map_vector( iPointInTexture.x - mMouseDown.x
                                                                         , iPointInTexture.y - mMouseDown.y );
            ::ULIS::FVec2D size = ::ULIS::FVec2D( bldif.x, bldif.y );

            switch( Shapes.GetActiveShapeType() )
            {
                case EOdysseyShapeType::kEllipse:
                {
                    FOdysseyVectorEllipse* ellipse = static_cast<FOdysseyVectorEllipse*>(mPrimitive);

                    if( Uniform )
                    {
                        size.x = ::ULIS::FVec2D( bldif.x, bldif.y ).Distance() * 0.7071f;
                        size.y = size.x;
                    }

                    ellipse->SetRadius( size.x, size.y );
                }
                break;

                case EOdysseyShapeType::kRectangle:
                {
                    FOdysseyVectorRectangle* rectangle = static_cast<FOdysseyVectorRectangle*>(mPrimitive);

                    if( Uniform )
                    {
                        size.x = ::ULIS::FVec2D( bldif.x, bldif.y ).Distance() * 0.7071f;
                        size.y = size.x;
                    }

                    rectangle->SetSize( size.x, size.y );
                }
                break;

                case EOdysseyShapeType::kLine:
                {
                    FOdysseyVectorLine* line = static_cast<FOdysseyVectorLine*>(mPrimitive);

                    if( Uniform )
                    {
                        BLMatrix2D& inverseMatrix = line->GetInverseWorldMatrix();
                        BLPoint pt = inverseMatrix.map_point( iPointInTexture.x, iPointInTexture.y );
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

        iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_INTERACTIVE
                                  | FOdysseyVectorObject::UPDATE_NOINBETWEENING ); // update invalidated objects
    }

    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), FOdysseyVectorCell::REDRAW_INTERACTIVE );
}

bool
UOdysseyPainterEditorVectorPrimitiveDrawingTool::OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                                                , const FOdysseyPoint& iPointInTexture
                                                                , const FKey& iKey )
{
    if( iKey != EKeys::LeftMouseButton )
        return false;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( ( iKey == EKeys::LeftMouseButton ) && GetWorkingGroup()->IsVisible( true ) )
    {
        if( mPrimitive )
        {
            FOdysseyVectorPath* path = mPrimitive->Convert();
            FOdysseyVectorObject* parentObject = mPrimitive->GetParent();

            //iScene->GetEngine()->ClearObjectSelection();
            parentObject->RemoveChild( mPrimitive );

            delete mPrimitive;
            mPrimitive = nullptr;

            parentObject->AppendChild( path );
            //path->Invalidate();
            path->UpdateMatrix();

            //iScene->GetEngine()->SelectObject( path );

            // needed for valid GUndo pointer
            GEditor->BeginTransaction(LOCTEXT("vector-primitive-drawing-tool.transaction.draw-primitive","Vector Primitive Drawing Tool"));
            if( GUndo )
            {
                FOdysseyVectorUndo *undo = new FOdysseyVectorUndoObjectAdd( iScene
                                                                          , path );

                GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

                TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
                if (source)
                    source->RecordCurrentFrameUndo();
            }
            GEditor->EndTransaction();
        }

        iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS ); // update invalidate objects
    }

    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    return true;
}

void
UOdysseyPainterEditorVectorPrimitiveDrawingTool::ExtendToolbar( UToolMenu* iToolMenu )
{
    Super::ExtendToolbar(iToolMenu);

    FToolMenuSection& section = iToolMenu->AddSection(NAME_None);
    section.AddEntry(
        FToolMenuEntry::InitWidget(
            NAME_None,
            SNew(SBox)
            .Padding(10.f, 0.f, 10.f, 0.f)
            [
                SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorPrimitiveDrawingTool, StrokeWidth ), FSinglePropertyParams())
                .InnerPadding(10.f)
                .ValueWidthOverride(100.f)
            ],
            FText()
        )
    );
}

FText
UOdysseyPainterEditorVectorPrimitiveDrawingTool::GetTooltip() const
{
    return LOCTEXT("vector-primitive-drawing-tool.tooltip", "Primitive Drawing Tool");
}

#undef LOCTEXT_NAMESPACE
