#include "InbetweenerTag/InbetweenerChart.h"
#include "InbetweenerTag/InbetweenerBreakdown.h"
#include "OdysseyVectorTagInbetweener.h"

FChartInbetween::FChartInbetween( FInbetweenerChart* iChart )
    : chart( iChart )
    , spacing( 0.0f )
{

}

uint32
FChartInbetween::GetIndex()
{
    return this - &chart->GetInbetweenArray()[0];
}

uint32
FChartInbetween::GetAbsoluteIndex()
{
    return GetIndex() + chart->GetBreakdown()->GetSourceDrawingIndex();
}

int32
FChartInbetween::GetAnimationCellIndex()
{
    FOdysseyVectorTagInbetweener* inbetweenerTag = chart->GetBreakdown()->GetInbetweenerTag();
    uint32 tagCellIndex = inbetweenerTag->GetAnimationCellIndex();
    uint32 inbetweenIndex = chart->GetBreakdown()->GetSourceDrawingIndex() + GetIndex();

    return (int32)tagCellIndex + (int32)( inbetweenIndex * (int)inbetweenerTag->GetInterpolationDirection());
}

FInbetweenerChart::~FInbetweenerChart()
{
}

FInbetweenerChart::FInbetweenerChart()
    : mBreakdown( nullptr  )
{
    mInbetweenArray.reserve( 16 );
}

FInbetweenerChart::FInbetweenerChart( FInbetweenerBreakdown* iBreakdown )
    : mBreakdown( iBreakdown )
{
}

FInbetweenerBreakdown*
FInbetweenerChart::GetBreakdown()
{
    return mBreakdown;
}

std::vector<FChartInbetween>&
FInbetweenerChart::GetInbetweenArray()
{
    return mInbetweenArray;
}

void
FInbetweenerChart::GetSpacing( std::vector<float>& oSpacingArray )
{
    oSpacingArray.clear();
    oSpacingArray.reserve( mInbetweenArray.size() );

    for( FChartInbetween& inbetween : mInbetweenArray )
    {
        oSpacingArray.push_back( inbetween.spacing );
    }
}

void
FInbetweenerChart::Reset()
{
    uint32 drawingCount = mBreakdown->GetDrawingCount();
    float stepT = 1.0f / ( drawingCount - 1 );
    float nextT = 0.0f;

    for( uint32 i = 0; i < drawingCount - 1; i++ )
    {
        mInbetweenArray[i].spacing = nextT;

        nextT += stepT;
    }
    // due to float imprecision, we get sure the last one is 1.0f
    mInbetweenArray.back().spacing = 1.0f;

    mBreakdown->GetInbetweenerTag()->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_SPACING
                                               | FOdysseyVectorTagInbetweener::INVALIDATE_CELLS );
}

void
FInbetweenerChart::Resize()
{
    uint32 drawingCount = mBreakdown->GetDrawingCount();
    FChartInbetween& lastInbetween = *std::prev( mInbetweenArray.end(), 2 );
    uint32 fromIndex = lastInbetween.GetIndex();
    float fromT = lastInbetween.spacing;
    float stepT = (  1.0f - fromT ) / ( drawingCount - fromIndex - 1 );
    float nextT = fromT;
    std::vector<float> spacingArray;

    // remember former spacing
    GetSpacing( spacingArray );

    mInbetweenArray.resize( drawingCount, this );

    for( uint32 i = fromIndex; i < drawingCount - 1; i++ )
    {
        mInbetweenArray[i].spacing = nextT;

        nextT += stepT;
    }

    mInbetweenArray.back().spacing = 1.0f;

    mBreakdown->GetInbetweenerTag()->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_SPACING
                                               | FOdysseyVectorTagInbetweener::INVALIDATE_CELLS );
}
