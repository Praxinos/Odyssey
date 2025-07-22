// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "InbetweenerTag/InterpolatedGroupPaint.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVector.h"

FInterpolatedGroupPaint::~FInterpolatedGroupPaint()
{
}

void
FInterpolatedGroupPaint::Alloc()
{
    uint32 pointCount = GetOriginalGroupPaint()->GetBucketList().size();

    mInterpolatedPointBuffer.reserve( pointCount );
}

FInterpolatedGroupPaint::FInterpolatedGroupPaint( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                , FOdysseyVectorGroupPaint* iPaintgroup )
    : FInterpolatedObject( iInbetweenerTag, iPaintgroup )
{
    uint32 pointID = 0;
    uint32 segmentID = 0;

    Alloc();

    for( FOdysseyVectorBucket* bucket : GetOriginalGroupPaint()->GetBucketList() )
    {
            // Note: we add +1 for the target position
        mInterpolatedPointBuffer.emplace_back( bucket, 0.0f, mInterpolatedPointBuffer.size() );
    }
}

FOdysseyVectorGroupPaint*
FInterpolatedGroupPaint::GetOriginalGroupPaint()
{
    return static_cast<FOdysseyVectorGroupPaint*>(mOriginalObject);
}
