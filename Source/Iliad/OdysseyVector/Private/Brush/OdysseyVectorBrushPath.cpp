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
