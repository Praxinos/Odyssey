// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "HUD/OdysseyVectorHUD.h"
#include "OdysseyVector.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorPoint.h"
#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyVectorCell.h"

// static
FColor&
FOdysseyVectorHUD::GetForegroundColor()
{
    //static FColor fg = FColor( 0x40, 0xE0, 0xD0, 0xFF ); // teal
    static FColor fg = FColor( 0, 169, 157, 255 ); // Odyssey's teal

    return fg;
}

// static
FColor&
FOdysseyVectorHUD::GetBackgroundColor()
{
    static FColor bg = FColor( 0x00, 0x00, 0x00, 0xFF ); // black

    return bg;
}

// static
FColor&
FOdysseyVectorHUD::GetHighlightColor()
{
    static FColor hc = FColor( 0xFF, 0x00, 0x00, 0xFF ); // red

    return hc;
}

// static
void
FOdysseyVectorHUD::DrawCenteredSquare( BLContext* iBLContext
                                     , double iWorldx
                                     , double iWorldY
                                     , double iRadius
                                     , const BLRgba32& fgColor
                                     , const BLRgba32& bgColor )
{
    double width = iRadius * 2;
    BLRect rect( iWorldx - iRadius, iWorldY - iRadius, width, width );

    // inner
    iBLContext->set_fill_style( fgColor );
    iBLContext->fill_rect( rect );
    iBLContext->set_stroke_width( 1.0f );
    iBLContext->set_stroke_style( bgColor );
    iBLContext->stroke_rect( rect );
}

/*
void
FOdysseyVectorHUD::DrawTargetGrid( BLContext* iBLContext
                                 , FInbetweenerGrid* iGrid )
{
    FOdysseyVectorTagInbetweener* inbetweenerTag = iGrid->GetBreakdown()->GetInbetweenerTag();

    DrawGrid( iBLContext
            , eInbetweenerPointPositionType::TargetPosition
            , inbetweenerTag->GetGridColor() );
}

void
FOdysseyVectorHUD::DrawSourceGrid( BLContext* iBLContext )
{
    DrawGrid( iBLContext
            , eInbetweenerPointPositionType::SourcePosition
            , FColor( 127, 127, 127, 127 ) );
}
*/

// static
::ULIS::FVec2D
FOdysseyVectorHUD::GetBucketPosition( FOdysseyVectorBucket* iBucket, bool iWorld )
{
    ::ULIS::FVec2D& bucketCoords = iBucket->GetCoords();

    if( iWorld )
    {
        FOdysseyVectorObject* ownerObject = iBucket->GetOwner();
        BLMatrix2D& worldMatrix = ownerObject->GetWorldMatrix();
        BLPoint worldPosition = worldMatrix.map_point( bucketCoords.x, bucketCoords.y );

        return ::ULIS::FVec2D( worldPosition.x, worldPosition.y );
    }

    return bucketCoords;
}

// static
::ULIS::FVec2D
FOdysseyVectorHUD::GetBucketRadialHandlePosition( FOdysseyVectorBucket* iBucket, bool iWorld )
{
    ::ULIS::FVec2D radialHandleCoords = iBucket->GetCoords() + iBucket->GetRadialOffset();

    radialHandleCoords.x += iBucket->GetRadialRadius();

    if( iWorld )
    {
        FOdysseyVectorObject* ownerObject = iBucket->GetOwner();
        BLMatrix2D& worldMatrix = ownerObject->GetWorldMatrix();
        BLPoint worldPosition = worldMatrix.map_point( radialHandleCoords.x, radialHandleCoords.y );

        return ::ULIS::FVec2D( worldPosition.x, worldPosition.y );
    }

    return radialHandleCoords;
}

// static
::ULIS::FVec2D
FOdysseyVectorHUD::GetBucketRadialPosition( FOdysseyVectorBucket* iBucket, bool iWorld )
{
    ::ULIS::FVec2D radialCoords = iBucket->GetCoords() + iBucket->GetRadialOffset();

    if( iWorld )
    {
        FOdysseyVectorObject* ownerObject = iBucket->GetOwner();
        BLMatrix2D& worldMatrix = ownerObject->GetWorldMatrix();
        BLPoint worldPosition = worldMatrix.map_point( radialCoords.x, radialCoords.y );

        return ::ULIS::FVec2D( worldPosition.x, worldPosition.y );
    }

    return radialCoords;
}

// static
::ULIS::FVec2D
FOdysseyVectorHUD::GetBucketHandleVector( FOdysseyVectorBucket* iBucket, bool iWorld )
{
    double a = iBucket->GetRotation();
    ::ULIS::FVec2D handleVector = ::ULIS::FVec2D( cos( a ), sin( a ) );

    if( iWorld )
    {
        FOdysseyVectorObject* ownerObject = iBucket->GetOwner();
        BLMatrix2D& worldMatrix = ownerObject->GetWorldMatrix();
        BLPoint worldVector = worldMatrix.map_vector( handleVector.x, handleVector.y );
        ::ULIS::FVec2D normalizedVector = ::ULIS::FVec2D( worldVector.x, worldVector.y );

        normalizedVector.Normalize();

        return normalizedVector;
    }

    return handleVector;
}

// static
bool
FOdysseyVectorHUD::IsPaintedPath( FOdysseyVectorObject* iObject, bool iHasParentSelected )
{
    if( iObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
    {
        FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(iObject);
        FOdysseyVectorObject* parent = path->GetParent();

        if( parent->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(parent);

            if( iHasParentSelected )
            {
                return paintGroup->IsSelected() ? true : false;
            }

            return true;
        }
    }

    return false;
}
