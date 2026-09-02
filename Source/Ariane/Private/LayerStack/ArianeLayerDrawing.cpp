// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeLayerDrawing.h"
#include "ArianeImage.h"
#include "ArianeLayerStack.h"
#include "ArianePainting3DComponent.h"
#include "ArianePath.h"
#include "ArianeGroup.h"
#include "ArianeCycle.h"
#include "ArianeEllipse.h"
#include "ArianeRectangle.h"
#include "ArianeLine.h"
#include "ArianePolygon.h"

UArianeLayerDrawing::~UArianeLayerDrawing()
{
    delete InvalidationFlags;
}

UArianeLayerDrawing::UArianeLayerDrawing()
    : Image( nullptr )
    , DrawingOrigin ( EArianeLayerDrawingOrigin::Layer )
    , DrawingOrientation ( EArianeLayerDrawingOrientation::View )
{
    InvalidationFlags = new FArianeLayerInvalidationFlags();

    Image = CreateDefaultSubobject<UArianeImage>( "Default Image" );
    Image->SetupAttachment(this);

    //bWantsInitializeComponent = true;

/*
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
    PrimaryComponentTick.SetTickFunctionEnable(true);

    bAutoRegister = true;
    bWantsInitializeComponent = true;
    bAutoActivate = true;
    bTickInEditor = true;
*/
}

void
UArianeLayerDrawing::OnRegister()
{
    Super::OnRegister();

    if ( Image && !Image->IsRegistered() )
    {
        Image->RegisterComponent();
    }
}

void
UArianeLayerDrawing::Update( bool bInteractive )
{
    FArianeObject::EUpdateFlags ObjectUpdateFlags = FArianeObject::EUpdateFlags::None;

    if( bInteractive )
    {
        ObjectUpdateFlags = FArianeObject::EUpdateFlags::Interactive;
    }

    Image->Update( bInteractive );

    Super::Update( bInteractive );
}

void
UArianeLayerDrawing::OnRootObjectInvalidated()
{
    Invalidate( FArianeLayerInvalidationFlags() );
}

/*
FBoxSphereBounds
UArianeLayerDrawing::CalcBounds( const FTransform& LocalToWorld ) const
{
    return FBoxSphereBounds( Image->GetRootGroup()->GetBoundingBox().TransformBy( LocalToWorld ) );
}
*/

EArianeLayerDrawingOrigin
UArianeLayerDrawing::GetDrawingOrigin()
{
    return DrawingOrigin;
}

void
UArianeLayerDrawing::SetDrawingOrigin( EArianeLayerDrawingOrigin InDrawingOrigin )
{
    DrawingOrigin = InDrawingOrigin;
}

EArianeLayerDrawingOrientation
UArianeLayerDrawing::GetDrawingOrientation()
{
    return DrawingOrientation;
}

void
UArianeLayerDrawing::SetDrawingOrientation( EArianeLayerDrawingOrientation InDrawingOrientation )
{
    DrawingOrientation = InDrawingOrientation;
}

UArianeImage*
UArianeLayerDrawing::GetImage()
{
    return Image;
}
