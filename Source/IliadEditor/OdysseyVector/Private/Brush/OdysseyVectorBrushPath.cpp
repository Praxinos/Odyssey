// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include <Brush/OdysseyVectorBrushPath.h>
#include <OdysseyVectorPath.h>

FOdysseyVectorBrushPath::~FOdysseyVectorBrushPath()
{
}

FOdysseyVectorBrushPath::FOdysseyVectorBrushPath( FOdysseyVectorPath* iPath
                                                , const ::ULIS::FRectD& iBoundingBox )
{
    for( FOdysseyVectorSegment* segment : iPath->GetSegmentList() )
    {
        mBrushSegmentBuffer.emplace_back( segment, iBoundingBox );
    }
}

void
FOdysseyVectorBrushPath::Draw( BLContext* iBLContext
                             , const ::ULIS::FRectD& iInvalidationArea
                             , double iAncestorsOpacity
                             , FOdysseyVectorChain* iChain
                             , uint64 iDrawingFlags )
{
    for( FOdysseyVectorBrushSegment& brushSegment : mBrushSegmentBuffer )
    {
        brushSegment.Draw( iBLContext
                         , iInvalidationArea
                         , iAncestorsOpacity
                         , iChain
                         , iDrawingFlags );
    }
}
