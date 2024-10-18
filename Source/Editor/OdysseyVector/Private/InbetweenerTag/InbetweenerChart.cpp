#include "InbetweenerTag/InbetweenerChart.h"
#include "InbetweenerTag/InbetweenerBreakdown.h"
#include "OdysseyVectorTagInbetweener.h"

#define DEFAULT_POSITION_P0_X 200.0f
#define DEFAULT_POSITION_P0_Y 40.0f
#define DEFAULT_POSITION_P1_X 399.0f
#define DEFAULT_POSITION_P1_Y 40.0f
#define DEFAULT_POSITION_P2_X 599.0f
#define DEFAULT_POSITION_P2_Y 40.0f

FChartDivision::FChartDivision( FInbetweenerChart* iChart )
    : chart( iChart )
    , spacing( 0.0f )
    , drawing ( nullptr )
{

}

uint32
FChartDivision::GetIndex()
{
    return this - &chart->GetDivisionBuffer()[0];
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
    uint32 tagCellIndex = inbetweenerTag->GetSourceAnimationCellIndex();
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
    mDivisionBuffer.reserve( 16 );
}
*/
FInbetweenerChart::FInbetweenerChart( FInbetweenerBreakdown* iBreakdown )
    : mBreakdown( iBreakdown )
    , mHUDBezier { ::ULIS::FVec2D( DEFAULT_POSITION_P0_X, DEFAULT_POSITION_P0_Y )
                 , ::ULIS::FVec2D( DEFAULT_POSITION_P1_X, DEFAULT_POSITION_P1_Y )
                 , ::ULIS::FVec2D( DEFAULT_POSITION_P2_X, DEFAULT_POSITION_P2_Y ) }
{
    mDivisionBuffer.reserve( 16 );

    mDivisionBuffer.emplace_back( this ).spacing = 0.0f;
    mDivisionBuffer.emplace_back( this ).spacing = 1.0f;
}

FInbetweenerBreakdown*
FInbetweenerChart::GetBreakdown()
{
    return mBreakdown;
}

std::vector<FChartDivision>&
FInbetweenerChart::GetDivisionBuffer()
{
    return mDivisionBuffer;
}

void
FInbetweenerChart::GetSpacing( std::vector<float>& oSpacingArray )
{
    oSpacingArray.clear();
    oSpacingArray.reserve( mDivisionBuffer.size() );

    for( FChartDivision& inbetween : mDivisionBuffer )
    {
        oSpacingArray.push_back( inbetween.spacing );
    }
}

void
FInbetweenerChart::Reset( bool iResetPositionning )
{
    uint32 drawingCount = mBreakdown->GetDrawingCount();
    float stepT = 1.0f / ( drawingCount - 1 );
    float nextT = 0.0f;

    for( uint32 i = 0; i < drawingCount - 1; i++ )
    {
        mDivisionBuffer[i].spacing = nextT;

        nextT += stepT;
    }
    // due to float imprecision, we get sure the last one is 1.0f
    mDivisionBuffer.back().spacing = 1.0f;

    if( iResetPositionning )
    {
        mHUDBezier[0].x = DEFAULT_POSITION_P0_X;
        mHUDBezier[0].y = DEFAULT_POSITION_P0_Y;
        mHUDBezier[1].x = DEFAULT_POSITION_P1_X;
        mHUDBezier[1].y = DEFAULT_POSITION_P1_Y;
        mHUDBezier[2].x = DEFAULT_POSITION_P2_X;
        mHUDBezier[2].y = DEFAULT_POSITION_P2_Y;
    }

    mBreakdown->GetInbetweenerTag()->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_SPACING
                                               | FOdysseyVectorTagInbetweener::INVALIDATE_CELLS );
}

void
FInbetweenerChart::Resize()
{
    uint32 drawingCount = mBreakdown->GetDrawingCount();
    FChartDivision& lastInbetween = *std::prev( mDivisionBuffer.end(), 2 );
    uint32 fromIndex = lastInbetween.GetIndex();
    float fromT = lastInbetween.spacing;
    float stepT = (  1.0f - fromT ) / ( drawingCount - fromIndex - 1 );
    float nextT = fromT;
    std::vector<float> spacingArray;

    // remember former spacing
    GetSpacing( spacingArray );

    mDivisionBuffer.resize( drawingCount, this );

    if( drawingCount )
    {
        for( uint32 i = fromIndex; i < drawingCount - 1; i++ )
        {
            mDivisionBuffer[i].spacing = nextT;

            nextT += stepT;
        }
    }
    else
    {
        mDivisionBuffer.front().spacing = 0.0f;
    }

    mDivisionBuffer.back().spacing = 1.0f;

    mBreakdown->GetInbetweenerTag()->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_SPACING
                                               | FOdysseyVectorTagInbetweener::INVALIDATE_CELLS );
}

::ULIS::FVec2D*
FInbetweenerChart::GetHUDBezier()
{
    return mHUDBezier;
}
