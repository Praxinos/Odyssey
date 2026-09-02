// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "ArianePrimitive.h"
#include "ArianeLayerDrawing.h"

bool
FArianePrimitiveInvalidationFlags::HasBaseClass( uint32 BaseClass ) const
{
    if( StaticClass() == BaseClass )
    {
        return true;
    }

    return Super::HasBaseClass( BaseClass );
}

//-------------------------------------------------------

FArianePrimitive::~FArianePrimitive()
{
}

FArianePrimitive::FArianePrimitive()
    : FArianePrimitive( nullptr
                      , FName( "Ariane Primitive" )
                      , 0.0f
                      , EArianeAllocationModel::InstancedStruct
                      , new FArianePrimitiveInvalidationFlags() )
{
}

FArianePrimitive::FArianePrimitive( UArianeImage* InImage
                                  , const FName& InName
                                  , double InStrokeWidth
                                  , EArianeAllocationModel InAllocationModel
                                  , FArianePrimitiveInvalidationFlags* InInvalidationflags )
    : FArianePath( InImage
                 , InName
                 , InAllocationModel
                 , InInvalidationflags ? InInvalidationflags
                                       : new FArianePrimitiveInvalidationFlags() )
    , StrokeWidth ( InStrokeWidth )
{
}

bool
FArianePrimitive::HasBaseClass( uint32 BaseClassID )
{
    if( StaticClass() == BaseClassID )
    {
        return true;
    }

    return Super::HasBaseClass( BaseClassID );
}

FArianePath*
FArianePrimitive::Convert( EConversionFlags ConversionFlags )
{
    FCopyArgs CopyArgs = FCopyArgs();

    CopyArgs.Flags = ECopyFlags::PrimitiveAsPath
                   | ( EnumHasAllFlags( ConversionFlags, EConversionFlags::Bezier ) ? ECopyFlags::AsBezier
                                                                                    : ECopyFlags::AsPolyline );
    CopyArgs.AllocationModel = AllocationModel;
    CopyArgs.Image = Image;

    FArianePath* Path = static_cast<FArianePath*>(this->Copy( CopyArgs ) );


    return Path;
}

void
FArianePrimitive::PostEditUndo()
{
    Super::PostEditUndo();

    ResetGeometry();
    ReshapeGeometry();
}

void
FArianePrimitive::PostLoad()
{
    Super::PostLoad();

    ResetGeometry();
    ReshapeGeometry();
}

void
FArianePrimitive::SetStrokeWidth( double InStrokeWidth )
{
    StrokeWidth = InStrokeWidth;

    ReshapeGeometry();
}

double
FArianePrimitive::GetStrokeWidth()
{
    return StrokeWidth;
}
