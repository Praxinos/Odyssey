#include "InbetweenerTag/InbetweenerChart.h"
#include "OdysseyVectorTagInbetweener.h"

FInbetweenerDrawing::FInbetweenerDrawing( FInbetweenerChart* iChart )
    : mChart( iChart )
    , spacing( 0.0f )
{

}

uint32
FInbetweenerDrawing::GetIndex()
{
    return this - &mChart->GetDrawingBuffer()[0];
}

int32
FInbetweenerDrawing::GetAnimationCellIndex()
{
    uint32 tagCellIndex = mChart->GetInbetweenerTag()->GetAnimationCellIndex();

    return (int32)tagCellIndex + (int32)( GetIndex() * (int)mChart->GetInbetweenerTag()->GetInterpolationDirection());
}

FInbetweenerChart::~FInbetweenerChart()
{
}

FInbetweenerChart::FInbetweenerChart()
    : mInbetweenerTag( nullptr  )
{
    mDrawingBuffer.reserve( 16 );
}

FInbetweenerChart::FInbetweenerChart( FOdysseyVectorTagInbetweener* iInbetweenerTag )
    : mInbetweenerTag( iInbetweenerTag )
{
}

FOdysseyVectorTagInbetweener*
FInbetweenerChart::GetInbetweenerTag()
{
    return mInbetweenerTag;
}

std::vector<FInbetweenerDrawing>&
FInbetweenerChart::GetDrawingBuffer()
{
    return mDrawingBuffer;
}

FInbetweenerDrawing*
FInbetweenerChart::GetDrawing( uint32 iIndex )
{
    return &mDrawingBuffer[iIndex];
}

void
FInbetweenerChart::GetSpacing( std::vector<float>& oSpacingArray )
{
    oSpacingArray.clear();
    oSpacingArray.reserve( mDrawingBuffer.size() );

    for( FInbetweenerDrawing& drawing : mDrawingBuffer )
    {
        oSpacingArray.push_back( drawing.spacing );
    }
}

void
FInbetweenerChart::Reset()
{
    uint32 drawingCount = mInbetweenerTag->GetDrawingCount();
    float stepT = 1.0f / ( drawingCount - 1 );
    float nextT = 0.0f;

    for( uint32 i = 0; i < drawingCount - 1; i++ )
    {
        mDrawingBuffer[i].spacing = nextT;

        nextT += stepT;
    }

    mDrawingBuffer.back().spacing = 1.0f;

    mInbetweenerTag->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_SPACING
                               | FOdysseyVectorTagInbetweener::INVALIDATE_CELLS );
}

void
FInbetweenerChart::Resize()
{
    uint32 drawingCount = mInbetweenerTag->GetDrawingCount();
    FInbetweenerDrawing& lastInbewteen = *std::prev( mDrawingBuffer.end(), 2 );
    uint32 fromIndex = lastInbewteen.GetIndex();
    float fromT = lastInbewteen.spacing;
    float stepT = (  1.0f - fromT ) / ( drawingCount - fromIndex - 1 );
    float nextT = fromT;
    std::vector<float> spacingArray;

    // remember former spacing
    GetSpacing( spacingArray );

    mDrawingBuffer.resize( drawingCount, this );

    for( uint32 i = fromIndex; i < drawingCount - 1; i++ )
    {
        mDrawingBuffer[i].spacing = nextT;

        nextT += stepT;
    }

    mDrawingBuffer.back().spacing = 1.0f;

    mInbetweenerTag->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_SPACING
                               | FOdysseyVectorTagInbetweener::INVALIDATE_CELLS );
}
