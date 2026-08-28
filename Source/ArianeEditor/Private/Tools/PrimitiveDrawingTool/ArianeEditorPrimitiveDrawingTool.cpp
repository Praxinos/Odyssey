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
//Unreal Headers
#include "InteractiveToolManager.h"
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
    : PrimitiveShapeType ( EArianePrimitiveToolShapeType::Polygon )
    , StrokeWidth( 30.0f )
    , LineType ( EArianePathLineType::Flat )
    , bShowGrid ( true )
    , MaterialInterface ( nullptr )
    , DivisionCount( 3 )
    , Uniform( false )
    , Primitive( nullptr )
    , RectangleNumber ( 0 )
    , LineNumber( 0 )
    , EllipseNumber( 0 )
    , PolygonNumber( 0 )
    , Cursor( EMouseCursor::Type::Crosshairs )
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
    if( InKeyEvent.IsRepeat() == false )
    {
        UniformAtKeyDown = Uniform;

        if ( FSlateApplication::Get().GetModifierKeys().IsShiftDown() )
        {
            Uniform = !Uniform; // flip the value
            return true;
        }
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
        ::ULIS::FColor color = Editor->GetPaintColor();
        ::ULIS::FColor rgba8 = color.ToFormat( ::ULIS::eFormat::Format_RGBA8 );
        FColor ueColor = FColor( rgba8.R8(), rgba8.G8(), rgba8.B8(), rgba8.A8() );

        if( Painting3DComponent )
        {
            //painting3DComponent->PrintPointers();
            UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();
            UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>(LayerStack->GetCurrentLayer());

            if( DrawingLayer )
            {
                FArianeGroup* ParentGroup = GetParentGroup( DrawingLayer );
                //const FTransform& ParentGroupTransform = ParentGroup->GetTransform();
                FPlane DrawingPlane = GetDrawingPlane( ViewportClient, DrawingLayer );
                FVector RayOrigin, RayDirection;
                FVector2D ViewportPosition = FVector2D( PointerState.ViewportX
                                                      , PointerState.ViewportY );

                View->DeprojectFVector2D( ViewportPosition
                                        , RayOrigin
                                        , RayDirection );

                if( FArianeCore::IntersectPlane( DrawingPlane, RayOrigin, RayDirection, IntersectAtDown  ) > 0.0f )
                {
                    const FTransform& ParentGroupTransform = ParentGroup->GetTransform();
                    FVector LocalOrientation;

                    PrimitiveCoordsAtDown = ParentGroupTransform.InverseTransformPosition( IntersectAtDown );

                    switch( Editor->GetLayerDrawingOrientation( DrawingLayer ) )
                    {
                        case EArianeLayerDrawingOrientation::View :
                        {
                            FVector CamForward = View->GetViewDirection();
                            FVector CamUp = View->GetViewUp();
                            FVector CamRight = View->GetViewRight();
                            FMatrix ViewAlignedRot = FMatrix( CamUp, CamRight, -CamForward, FVector::ZeroVector );
                            FQuat LocalOrientationQuat = ParentGroupTransform.InverseTransformRotation( ViewAlignedRot.ToQuat() );

                            LocalOrientation = LocalOrientationQuat.Euler();
                        }
                        break;

                        default :
                        {
                            FQuat WorldRotation = FQuat::FindBetweenNormals( FVector::UpVector, DrawingPlane.GetSafeNormal() );
                            FTransform WorldTransform = FTransform( WorldRotation );
                            FTransform LocalTransform = WorldTransform.GetRelativeTransform( ParentGroup->GetTransform() );
                            FQuat LocalOrientationQuat = LocalTransform.GetRotation();

                            LocalOrientation = LocalOrientationQuat.Euler();
                        }
                        break;
                    };

                    GetToolManager()->BeginUndoTransaction(LOCTEXT("ariane-primitive-tool.create","Create Primitive"));

                    DrawingLayer->Modify();

                    switch( PrimitiveShapeType/*Shapes.GetActiveShapeType()*/ )
                    {
                        //case EOdysseyShapeType::kEllipse:
                        case EArianePrimitiveToolShapeType::Ellipse:
                            Primitive = DrawingLayer->AllocEllipse( MaterialInterface
                                                                  , *(FString("Ellipse_" + FString::FromInt( EllipseNumber++ )))
                                                                  , 0.0f
                                                                  , 0.0f
                                                                  , StrokeWidth//width.Distance()
                                                                  , EArianeAllocationModel::InstancedStruct );
                        break;

                        //case EOdysseyShapeType::kRectangle :
                        case EArianePrimitiveToolShapeType::Rectangle:
                            Primitive = DrawingLayer->AllocRectangle( MaterialInterface
                                                                    , *(FString("Rectangle_") + FString::FromInt( RectangleNumber++ ))
                                                                    , 0.0f
                                                                    , 0.0f
                                                                    , StrokeWidth//width.Distance()
                                                                    , EArianeAllocationModel::InstancedStruct );
                        break;

                        //case EOdysseyShapeType::kLine:
                        case EArianePrimitiveToolShapeType::Line:
                            Primitive = DrawingLayer->AllocLine( MaterialInterface
                                                               , *(FString("Line_") + FString::FromInt( LineNumber++ ))
                                                               , FVector::Zero()
                                                               , FVector::Zero()
                                                               , StrokeWidth//width.Distance()
                                                               , EArianeAllocationModel::InstancedStruct );
                        break;

                        //case EOdysseyShapeType::kPolygon:
                        case EArianePrimitiveToolShapeType::Polygon:
                            Primitive = DrawingLayer->AllocPolygon( MaterialInterface
                                                                  , *(FString("Polygon_") + FString::FromInt( PolygonNumber++ ))
                                                                  , DivisionCount
                                                                  , 0.0f
                                                                  , StrokeWidth//width.Distance()
                                                                  , EArianeAllocationModel::InstancedStruct );
                        break;

                        default:
                        break;
                    }

                    ParentGroup->AppendChild( Primitive );
                    Primitive->SetColor( ueColor );
                    Primitive->SetLineType( LineType );
                    Primitive->SetTransform( PrimitiveCoordsAtDown, LocalOrientation, FVector::One(), FVector::Zero() );
                    Primitive->UpdateTransform();
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
            FPlane DrawingPlane = GetDrawingPlane( ViewportClient, DrawingLayer );
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
                FPlane DrawingPlane = GetDrawingPlane( ViewportClient, DrawingLayer );
                FVector RayOrigin, RayDirection;
                FVector IntersectAtDrag;
                FVector2D ViewportPosition = FVector2D( PointerState.ViewportX
                                                      , PointerState.ViewportY );

                View->DeprojectFVector2D( ViewportPosition
                                        , RayOrigin
                                        , RayDirection );

                if( FArianeCore::IntersectPlane( DrawingPlane, RayOrigin, RayDirection, IntersectAtDrag  ) > 0.0f )
                {
                    const FTransform& PrimitiveTransform = Primitive->GetTransform();
                    //FVector2D PlaneCoordsAtDown = WorldToPlane( IntersectAtDown, DrawingPlane );
                    //FVector2D PlaneCoordsAtDrag = WorldToPlane( IntersectAtDrag, DrawingPlane );


                    FVector LocalCoordsAtDown = PrimitiveTransform.InverseTransformPosition( IntersectAtDown );
                    FVector LocalCoordsAtDrag = PrimitiveTransform.InverseTransformPosition( IntersectAtDrag );

                    switch( PrimitiveShapeType/*Shapes.GetActiveShapeType()*/ )
                    {
                        //case EOdysseyShapeType::kEllipse:
                        case EArianePrimitiveToolShapeType::Ellipse:
                        {
                            FArianeEllipse* Ellipse = static_cast<FArianeEllipse*>(Primitive);
                            FVector Diff = ( LocalCoordsAtDrag - FVector::Zero() );

                            if( Uniform )
                            {
                                Diff.Y = Diff.X;
                            }

                            Ellipse->SetRadius( Diff.X, Diff.Y );
                        }
                        break;

                        //case EOdysseyShapeType::kRectangle:
                        case EArianePrimitiveToolShapeType::Rectangle:
                        {
                            FArianeRectangle* Rectangle = static_cast<FArianeRectangle*>(Primitive);
                            const FTransform& ParentGroupTransform = ParentGroup->GetTransform();
                            //double XMin = FMath::Min( LocalCoordsAtDown.X, LocalCoordsAtDrag.X );
                            //double YMin = FMath::Min( LocalCoordsAtDown.Y, LocalCoordsAtDrag.Y );
                            //double XMax = FMath::Max( LocalCoordsAtDown.X, LocalCoordsAtDrag.X );
                            //double YMax = FMath::Max( LocalCoordsAtDown.Y, LocalCoordsAtDrag.Y );
                            FVector Diff = LocalCoordsAtDrag - LocalCoordsAtDown;

                            if( Uniform )
                            {
                                 Diff.Y = ( Diff.Y > 0.0f ) ? fabs(Diff.X) : -fabs(Diff.X);
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
                            Line->SetEndPoint( FVector( LocalCoordsAtDrag.X
                                                      , LocalCoordsAtDrag.Y
                                                      , LocalCoordsAtDrag.Z ) );
                        }
                        break;

                        case EArianePrimitiveToolShapeType::Polygon:
                        {
                            FArianePolygon* Polygon = static_cast<FArianePolygon*>(Primitive);
                            FVector Diff = ( LocalCoordsAtDrag - FVector::Zero() );

                            Polygon->SetRadius( Diff.Length() );
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
            //painting3DComponent->PrintPointers();
            UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();
            UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>(LayerStack->GetCurrentLayer());

            if( DrawingLayer )
            {
                GetToolManager()->EndUndoTransaction();
            }

            Primitive = nullptr;
            Painting3DComponent->Update( false );
        }
    }

    return true;
}

void
//UArianeEditorTool::OnTick(float DeltaTime)
UArianeEditorPrimitiveDrawingTool::Render(IToolsContextRenderAPI* RenderAPI)
{
    if  ( 1 )
    {
        UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

        //Super::OnTick( DeltaTime );

        if( Painting3DComponent )
        {
            UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();
            UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>( LayerStack->GetCurrentLayer() );

            if( DrawingLayer )
            {
                DrawLayerOrientationGrid( RenderAPI, DrawingLayer );
            }
        }
    }
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
