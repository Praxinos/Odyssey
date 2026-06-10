// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeHandleSegment.h"
#include "ArianeSegment.h"
#include "ArianeSegmentCubic.h"

FArianeHandleSegment::~FArianeHandleSegment()
{
}

FArianeHandleSegment::FArianeHandleSegment( FArianeSegment* iOwnerSegment
                                           // A handle can be linked to multiple vertices, Quadratic bezier e.g
                                          , const TArray<uint32> InAttachedVertexIDs
                                          , double iX
                                          , double iY
                                          , double iZ )
    : FArianePoint( iX, iY, iZ )
    , OwnerSegment ( iOwnerSegment )
    , AttachedVertexIDs ( InAttachedVertexIDs )
{
}

FArianeHandleSegment::FArianeHandleSegment( FArianeSegment* iOwnerSegment
                                           // A handle can be linked to multiple vertices, Quadratic bezier e.g
                                          , const TArray<uint32> InAttachedVertexIDs
                                          , const FVector& iPosition )
    // Delegating constructor
    : FArianeHandleSegment ( iOwnerSegment, InAttachedVertexIDs, iPosition.X, iPosition.Y, iPosition.Z )
{
}

void
FArianeHandleSegment::SetPosition_Private( const FVector& iPosition )
{
    // invalidate the attached vertex in order to update the joint
    for( uint32 AttachedVertexID : AttachedVertexIDs )
    {
        FArianeVertex* AttachedVertex = OwnerSegment->GetVertex(AttachedVertexID);

        AttachedVertex->Invalidate();

        if( AttachedVertex->IsLocked() == false )
        {
            FArianePoint::SetPosition_Private( iPosition );

            OwnerSegment->Invalidate();

            if( AttachedVertex->IsHandleAligned() )
            {
                FVector HandleVector = Position - AttachedVertex->GetPosition();
                FArianeSegment* OtherSegment = AttachedVertex->GetOtherSegment( OwnerSegment );

                if( OtherSegment )
                {
                    if( OtherSegment->GetClass() == FArianeSegmentCubic::StaticClass() )
                    {
                        FArianeSegmentCubic* OtherCubicSegment = static_cast<FArianeSegmentCubic*>(OtherSegment);
                        FArianeHandleSegment* OtherHandle = OtherCubicSegment->GetHandle( AttachedVertex );
                        FVector OtherHandleVector = OtherHandle->GetPosition() - AttachedVertex->GetPosition();

                        if( HandleVector.SquaredLength() )
                        {
                            HandleVector.Normalize();

                            FVector AlignedCoords = AttachedVertex->GetPosition() - ( OtherHandleVector.Length() * HandleVector );

                            OtherHandle->FArianePoint::SetPosition_Private( AlignedCoords );

                            OtherSegment->Invalidate();
                        }
                    }
                }
            }
        }
    }
}

void
FArianeHandleSegment::SetOwnerSegment( FArianeSegment* InOwnerSegment )
{
    OwnerSegment = InOwnerSegment;
}


FArianeSegment*
FArianeHandleSegment::GetOwnerSegment()
{
    return OwnerSegment;
}
