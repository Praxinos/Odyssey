// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Editor Headers
#include "Tools/PrimitiveDrawingTool/ArianeEditorPrimitiveDrawingTool.h"
#include "ArianeEditor.h"
#include "ArianeEditorStyle.h"
//Ariane Headers
#include "ArianeLayerStack.h"
#include "ArianeLayerDrawing.h"
#include "ArianeGroup.h"
#include "ArianeLine.h"
#include "ArianeEllipse.h"
#include "ArianeRectangle.h"
#include "ArianePolygon.h"
#include "ArianeCore.h"

#include "Editor.h"
#include "ToolMenu.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Layout/SBox.h"
#include "ISinglePropertyView.h"
#include "SOdysseySinglePropertyView.h"


#ifndef M_PI
#define M_PI 3.14159265358979323846L
#endif

#define LOCTEXT_NAMESPACE "ArianeEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UArianeEditorPrimitiveDrawingTool::~UArianeEditorPrimitiveDrawingTool()
{
}

UArianeEditorPrimitiveDrawingTool::UArianeEditorPrimitiveDrawingTool()
    : MaterialInterface ( nullptr )
    , StrokeWidth( 4.0f )
    , Uniform( false )
    , Primitive( nullptr )
    , RectangleNumber ( 0 )
    , LineNumber( 0 )
    , EllipseNumber( 0 )
    , PolygonNumber( 0 )
    , Cursor( EMouseCursor::Type::Crosshairs )
    , PrimitiveShapeType ( EArianePrimitiveToolShapeType::Line )
{
    Icon = FArianeEditorStyle::Get().GetBrush( "ArianeEditor.ToolsTab.PrimitiveDrawing64");

    //Shapes.AddShapeType( EOdysseyShapeType::kLine, nullptr );
    //Shapes.AddShapeType( EOdysseyShapeType::kRectangle, nullptr );
    //Shapes.AddShapeType( EOdysseyShapeType::kEllipse, nullptr );

    //Shapes.SetActiveShapeType( EOdysseyShapeType::kEllipse );
}

/*
void
UArianeEditorPrimitiveDrawingTool::BindDelegates()
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( Painting3DComponent )
    {
        Painting3DComponent->OnPostUpdateDelegate().AddUObject( this, &UArianeEditorPrimitiveDrawingTool::OnPostUpdate );
    }
}

void
UArianeEditorPrimitiveDrawingTool::UnbindDelegates()
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( Painting3DComponent )
    {
        Painting3DComponent->OnPostUpdateDelegate().RemoveAll( this );
    }
}
*/

void
UArianeEditorPrimitiveDrawingTool::Activate()
{
    FEditorViewportClient* ViewportClient = GetActiveViewportClient();

    Super::Activate();

    // ViewportClient can be nullptr when closing the editor
    if( ViewportClient )
    {
        // Hide outlining
        ViewportClient->EngineShowFlags.SetSelectionOutline(false);
        ViewportClient->Invalidate();

        GEditor->RedrawAllViewports();
    }

    //BindDelegates();
}

void
UArianeEditorPrimitiveDrawingTool::Inactivate()
{
    FEditorViewportClient* ViewportClient = GetActiveViewportClient();

    //UnbindDelegates();

    // ViewportClient can be nullptr when closing the editor
    if( ViewportClient )
    {
        ViewportClient->EngineShowFlags.SetSelectionOutline(true);
        ViewportClient->Invalidate();

        GEditor->RedrawAllViewports();
    }

    Super::Inactivate();
}

bool
UArianeEditorPrimitiveDrawingTool::OnKeyDownGlobal( const FKeyEvent& InKeyEvent )
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
UArianeEditorPrimitiveDrawingTool::OnKeyUpGlobal( const FKeyEvent& InKeyEvent )
{
    Uniform = UniformAtKeyDown;

    return false;
}

FArianeGroup*
UArianeEditorPrimitiveDrawingTool::GetParentGroup( UArianeLayerDrawing* DrawingLayer )
{
    TArray<FArianeObject*>& SelectedObjects = DrawingLayer->GetSelectedObjects();

    if( SelectedObjects.Num() == 1 )
    {
       if( SelectedObjects[0]->GetClass() == FArianeGroup::StaticClass() )
       {
           return static_cast<FArianeGroup*>(SelectedObjects[0]);
       }
    }

    return DrawingLayer->GetRootGroup();
}

bool
UArianeEditorPrimitiveDrawingTool::OnMouseDown( FEditorViewportClient* ViewportClient
                                              , FSceneView* View
                                              , const FKey& Key
                                              , const FArianePointerState& PointerState
                                              , bool bRepeat )
{
    MouseDown.X = PointerState.ViewportX;
    MouseDown.Y = PointerState.ViewportY;
    Primitive = nullptr;

    if( Key == EKeys::LeftMouseButton )
    {
        UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

        if( Painting3DComponent )
        {
            //painting3DComponent->PrintPointers();
            UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();
            UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>(LayerStack->GetCurrentLayer());

            if( DrawingLayer )
            {
                FArianeGroup* ParentGroup = GetParentGroup( DrawingLayer );
                FVector4 DrawingPlane = GetDrawingPlane( ViewportClient, DrawingLayer );
                FVector RayOrigin, RayDirection;
                FVector IntersectAt;
                FVector2D ViewportPosition = FVector2D( PointerState.ViewportX
                                                      , PointerState.ViewportY );

                View->DeprojectFVector2D( ViewportPosition
                                        , RayOrigin
                                        , RayDirection );

                if( FArianeCore::IntersectPlane( DrawingPlane, RayOrigin, RayDirection, IntersectAt  ) > 0.0f )
                {
                    const FTransform& ParentGroupTransform = ParentGroup->GetTransform();

                    LocalCoordsAtDown = ParentGroupTransform.InverseTransformPosition( IntersectAt );

                    switch( PrimitiveShapeType/*Shapes.GetActiveShapeType()*/ )
                    {
                        //case EOdysseyShapeType::kEllipse:
                        case EArianePrimitiveToolShapeType::Ellipse:
                            Primitive = DrawingLayer->AllocEllipse( *(FString("Ellipse_" + FString::FromInt( EllipseNumber++ )))
                                                                  , 0.0f
                                                                  , 0.0f
                                                                  , StrokeWidth//width.Distance()
                                                                  , EArianeAllocationModel::InstancedStruct );
                        break;

                        //case EOdysseyShapeType::kRectangle :
                        case EArianePrimitiveToolShapeType::Rectangle:
                            Primitive = DrawingLayer->AllocRectangle( *(FString("Rectangle_") + FString::FromInt( RectangleNumber++ ))
                                                                    , 0.0f
                                                                    , 0.0f
                                                                    , StrokeWidth//width.Distance()
                                                                    , EArianeAllocationModel::InstancedStruct );
                        break;

                        //case EOdysseyShapeType::kLine:
                        case EArianePrimitiveToolShapeType::Line:
                            Primitive = DrawingLayer->AllocLine( *(FString("Line_") + FString::FromInt( LineNumber++ ))
                                                               , FVector::Zero()
                                                               , FVector::Zero()
                                                               , StrokeWidth//width.Distance()
                                                               , EArianeAllocationModel::InstancedStruct );
                        break;

                        //case EOdysseyShapeType::kPolygon:
                        case EArianePrimitiveToolShapeType::Polygon:
                            Primitive = DrawingLayer->AllocPolygon( *(FString("Polygon_") + FString::FromInt( PolygonNumber++ ))
                                                                  , 0.0f
                                                                  , 0.0f
                                                                  , StrokeWidth//width.Distance()
                                                                  , EArianeAllocationModel::InstancedStruct );
                        break;

                        default:
                        break;
                    }

                    ParentGroup->AppendChild( Primitive );

                    //SetPathColor( Primitive );

                    //Primitive->SetOpacity( 1.0f );
                    if( MaterialInterface )
                    {
                        Primitive->SetMaterial( MaterialInterface );
                    }

                    //Primitive->Translate( LocalCoords.X, LocalCoords.Y, LocalCoords.Z );
                    //Primitive->UpdateMatrix();
                }
            }

            Painting3DComponent->Update( true );
        }
    }

    return true;
}

void
UArianeEditorPrimitiveDrawingTool::OnMouseHover( FEditorViewportClient* ViewportClient
                                               , FSceneView* View
                                               , const FArianePointerState& State )
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( Painting3DComponent )
    {
        UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>(Painting3DComponent->GetLayerStack()->GetCurrentLayer());

        if( DrawingLayer )
        {
            FVector4 DrawingPlane = GetDrawingPlane( ViewportClient, DrawingLayer );
            FVector RayOrigin, RayDirection;
            FVector IntersectAt;
            FVector2D ViewportPosition = FVector2D( ViewportClient->Viewport->GetMouseX()
                                                  , ViewportClient->Viewport->GetMouseY() );

            View->DeprojectFVector2D( ViewportPosition
                                    , RayOrigin
                                    , RayDirection );

            Cursor = ( FArianeCore::IntersectPlane( DrawingPlane
                                                  , RayOrigin
                                                  , RayDirection
                                                  , IntersectAt ) > 0.0f ) ? EMouseCursor::Type::Crosshairs
                                                                           : EMouseCursor::Type::SlashedCircle;
        }
    }
}

bool
UArianeEditorPrimitiveDrawingTool::GetCursor( EMouseCursor::Type& OutCursor )
{
    OutCursor = Cursor;

    return true;
}

/*
double
UArianeEditorPrimitiveDrawingTool::GetLineRotationAngle( FArianeLine* iLine
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
*/

bool
UArianeEditorPrimitiveDrawingTool::OnMouseDrag( FEditorViewportClient* ViewportClient
                                              , FSceneView* View
                                              , const FKey& Key
                                              , const FArianePointerState& PointerState )
{
    if( Key == EKeys::LeftMouseButton )
    {
        UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

        if( Painting3DComponent && Primitive )
        {
            UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();
            UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>(LayerStack->GetCurrentLayer());

            if( DrawingLayer )
            {
                FArianeGroup* ParentGroup = GetParentGroup( DrawingLayer );
                FVector4 DrawingPlane = GetDrawingPlane( ViewportClient, DrawingLayer );
                FVector RayOrigin, RayDirection;
                FVector IntersectAt;
                FVector2D ViewportPosition = FVector2D( PointerState.ViewportX
                                                      , PointerState.ViewportY );

                View->DeprojectFVector2D( ViewportPosition
                                        , RayOrigin
                                        , RayDirection );

                if( FArianeCore::IntersectPlane( DrawingPlane, RayOrigin, RayDirection, IntersectAt  ) > 0.0f )
                {
                    const FTransform& PrimitiveTransform = Primitive->GetTransform();
                    FVector LocalCoordsAtDrag = PrimitiveTransform.InverseTransformPosition( IntersectAt );
                    FVector Diff = ( LocalCoordsAtDrag - LocalCoordsAtDown );

                    switch( PrimitiveShapeType/*Shapes.GetActiveShapeType()*/ )
                    {
                        //case EOdysseyShapeType::kEllipse:
                        case EArianePrimitiveToolShapeType::Ellipse:
                        {
                            FArianeEllipse* Ellipse = static_cast<FArianeEllipse*>(Primitive);

                            if( Uniform )
                            {
                                Diff.X = Diff.Y = Diff.Length();
                            }

                            Ellipse->SetRadius( Diff.X, Diff.Y );
                        }
                        break;

                        //case EOdysseyShapeType::kRectangle:
                        case EArianePrimitiveToolShapeType::Rectangle:
                        {
                            FArianeRectangle* Rectangle = static_cast<FArianeRectangle*>(Primitive);

                            if( Uniform )
                            {
                                Diff.X = Diff.Y = Diff.Length();
                            }

                            Rectangle->SetSize( Diff.X, Diff.Y );
                        }
                        break;

                        //case EOdysseyShapeType::kLine:
                        case EArianePrimitiveToolShapeType::Line:
                        {
                            FArianeLine* Line = static_cast<FArianeLine*>(Primitive);
                            FVector StartPoint = Line->GetStartPoint();
/*
                            if( Uniform )
                            {
                                const FTransform& LineTransform = Line->GetTransform();
                                double Distance = Diff.Length();
                                int Rotation = GetLineRotationAngle( Line, iPointInTexture ) / M_PI * 180.0f;

                                    // to force the tip of the line in the middle of the 45 degrees steps
                                Rotation += 22;
                                // 45 deg by 45 deg
                                double Angle = ( ( Rotation / 45 ) * 45 ) * M_PI / 180.0f;

                                Diff.X = cos( Angle ) * Distance;
                                Diff.Y = sin( Angle ) * Distance;
                            }
*/
                            Line->SetEndPoint( FVector( StartPoint.X + Diff.X
                                                      , StartPoint.Y + Diff.Y
                                                      , 0.0f ) );
                        }
                        break;

                        default:
                        break;
                    }
                }
            }

            Painting3DComponent->Update( true );
        }
    }

    return false;
}

bool
UArianeEditorPrimitiveDrawingTool::OnMouseUp( FEditorViewportClient* ViewportClient
                                            , FSceneView* View
                                            , const FKey& Key
                                            , const FArianePointerState& PointerState )
{
    if( Key == EKeys::LeftMouseButton )
    {
        UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

        if( Painting3DComponent && Primitive )
        {
            Painting3DComponent->Update( false );
        }
    }

    return true;
}

void
UArianeEditorPrimitiveDrawingTool::ExtendToolbar( UToolMenu* ToolMenu )
{
    Super::ExtendToolbar( ToolMenu );

    FToolMenuSection& section = ToolMenu->AddSection(NAME_None);
    section.AddEntry(
        FToolMenuEntry::InitWidget(
            NAME_None,
            SNew(SBox)
            .Padding(10.f, 0.f, 10.f, 0.f)
            [
                SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UArianeEditorPrimitiveDrawingTool, StrokeWidth ), FSinglePropertyParams())
                .InnerPadding(10.f)
                .ValueWidthOverride(100.f)
            ],
            FText()
        )
    );
}

#undef LOCTEXT_NAMESPACE
