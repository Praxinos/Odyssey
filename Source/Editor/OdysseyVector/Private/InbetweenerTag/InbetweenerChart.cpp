#include "InbetweenerTag/InbetweenerChart.h"
#include "InbetweenerTag/InbetweenerBreakdown.h"
#include "OdysseyVectorTagInbetweener.h"

FChartDivision::FChartDivision( FInbetweenerChart* iChart )
    : chart( iChart )
    , spacing( 0.0f )
    , drawing ( nullptr )
{

}

uint32
FChartDivision::GetIndex()
{
    return this - &chart->GetDivisionArray()[0];
}

uint32
FChartDivision::GetAbsoluteIndex()
{
    return GetIndex() + chart->GetBreakdown()->GetSourceDrawingIndex();
}

int32
FChartDivision::GetAnimationCellIndex()
{
    FOdysseyVectorTagInbetweener* inbetweenerTag = chart->GetBreakdown()->GetInbetweenerTag();
    uint32 tagCellIndex = inbetweenerTag->GetAnimationCellIndex();
    uint32 inbetweenIndex = chart->GetBreakdown()->GetSourceDrawingIndex() + GetIndex();

    return (int32)tagCellIndex + (int32)( inbetweenIndex * (int)inbetweenerTag->GetInterpolationDirection());
}

void
FChartDivision::SetSpacing( float iSpacing )
{
    spacing = iSpacing;

    chart->GetBreakdown()->GetInbetweenerTag()->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_SPACING );
}

FInbetweenerChart::~FInbetweenerChart()
{
}

/*
FInbetweenerChart::FInbetweenerChart()
    : mBreakdown( nullptr  )
{
    mDivisionArray.reserve( 16 );
}
*/
FInbetweenerChart::FInbetweenerChart( FInbetweenerBreakdown* iBreakdown )
    : mBreakdown( iBreakdown )
    , mHUDBezier { ::ULIS::FVec2D( 200.0f, 40.0f )
                 , ::ULIS::FVec2D( 399.0f, 40.0f )
                 , ::ULIS::FVec2D( 599.0f, 40.0f )  }
{
    mDivisionArray.reserve( 16 );

    mDivisionArray.emplace_back( this ).spacing = 0.0f;
    mDivisionArray.emplace_back( this ).spacing = 1.0f;
}

FInbetweenerBreakdown*
FInbetweenerChart::GetBreakdown()
{
    return mBreakdown;
}

std::vector<FChartDivision>&
FInbetweenerChart::GetDivisionArray()
{
    return mDivisionArray;
}

void
FInbetweenerChart::GetSpacing( std::vector<float>& oSpacingArray )
{
    oSpacingArray.clear();
    oSpacingArray.reserve( mDivisionArray.size() );

    for( FChartDivision& inbetween : mDivisionArray )
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
        mDivisionArray[i].spacing = nextT;

        nextT += stepT;
    }
    // due to float imprecision, we get sure the last one is 1.0f
    mDivisionArray.back().spacing = 1.0f;

    mBreakdown->GetInbetweenerTag()->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_SPACING
                                               | FOdysseyVectorTagInbetweener::INVALIDATE_CELLS );
}

void
FInbetweenerChart::Resize()
{
    uint32 drawingCount = mBreakdown->GetDrawingCount();
    FChartDivision& lastInbetween = *std::prev( mDivisionArray.end(), 2 );
    uint32 fromIndex = lastInbetween.GetIndex();
    float fromT = lastInbetween.spacing;
    float stepT = (  1.0f - fromT ) / ( drawingCount - fromIndex - 1 );
    float nextT = fromT;
    std::vector<float> spacingArray;

    // remember former spacing
    GetSpacing( spacingArray );

    mDivisionArray.resize( drawingCount, this );

    for( uint32 i = fromIndex; i < drawingCount - 1; i++ )
    {
        mDivisionArray[i].spacing = nextT;

        nextT += stepT;
    }

    mDivisionArray.back().spacing = 1.0f;

    mBreakdown->GetInbetweenerTag()->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_SPACING
                                               | FOdysseyVectorTagInbetweener::INVALIDATE_CELLS );
}

::ULIS::FVec2D*
FInbetweenerChart::GetHUDBezier()
{
    return mHUDBezier;
}
