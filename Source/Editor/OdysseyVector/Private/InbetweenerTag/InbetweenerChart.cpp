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
FInbetweenerChart::Resize( bool iReset )
{
    uint32 drawingCount = mInbetweenerTag->GetDrawingCount();
    FInbetweenerDrawing& lastInbewteen = *std::prev( mDrawingBuffer.end(), 2 );
    uint32 fromIndex = lastInbewteen.GetIndex();
    float fromT = lastInbewteen.spacing;
    float stepT = (  1.0f - fromT ) / ( drawingCount - fromIndex );
    float nextT = fromT;
    std::vector<float> spacingArray;

    // remember former spacing
    GetSpacing( spacingArray );

    mDrawingBuffer.resize( drawingCount, this );

    for( uint32 i = lastInbewteen.GetIndex(); i < drawingCount - 1; i++ )
    {
        mDrawingBuffer[i].spacing = nextT;

        nextT += stepT;
    }

    mDrawingBuffer.back().spacing = 1.0f;

    if( iReset == false )
    {
        for( uint32 i = 1; ( i < ( spacingArray.size()   - 1 ) ) 
                        && ( i < ( mDrawingBuffer.size() - 1 ) ); i++ )
        {
            mDrawingBuffer[i].spacing = spacingArray[i];
        }
    }

    mInbetweenerTag->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_SPACING
                               | FOdysseyVectorTagInbetweener::INVALIDATE_CELLS );
}
