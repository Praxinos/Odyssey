// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include <ULIS>

namespace FArianeCore
{
    double ARIANE_API DistanceToSegment( const ::ULIS::FVec2D& iPt
                                       , const ::ULIS::FVec2D& iSegmentP0
                                       , const ::ULIS::FVec2D& iSegmentP1
                                       , double& oDistance );

    // This is a constrained version of a segment-to-point proximity test.
    // It means that if the projection of the point on the segment is beyond limits,
    // it will remain within limits (0.0f) or (1.0f). On the figure below, x would be at t=0.0
    //
    //                    ° (point)
    //     segment        |
    // 1______________0   x (proj. point will also be at t = 0.0,
    //                       even though it is outside the segment)
    //
    double ARIANE_API DistanceToSegmentConstrained( const ::ULIS::FVec2D& iPt
                                                  , const ::ULIS::FVec2D& iSegmentP0
                                                  , const ::ULIS::FVec2D& iSegmentP1
                                                  , double&         oDistance );

}
