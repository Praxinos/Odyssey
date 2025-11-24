// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorRasterLiquifyToolUndo.h"

FOdysseyPainterEditorRasterLiquifyToolUndo::~FOdysseyPainterEditorRasterLiquifyToolUndo()
{
/*
    if( mApplied )
    {
        // nothing to do
    }
    else
    {
        // nothing to do
    }
*/
}

FOdysseyPainterEditorRasterLiquifyToolUndo::FOdysseyPainterEditorRasterLiquifyToolUndo( const UOdysseyPainterEditorRasterLiquifyTool::FFlowMap& iFlowMapBackup
                                                                                      , const ::ULIS::FRectI& iRect
                                                                                      , UOdysseyPainterEditorRasterLiquifyTool::FFlowMap& iFlowMapOriginal )
    : mFlowMapOriginal( iFlowMapOriginal )
{
    mPartialFlowMap.SetSize( iRect.w, iRect.h );

    mPositionX = iRect.x;
    mPositionY = iRect.y;

    for( uint32 srcY = mPositionY, dstY = 0; dstY < mPartialFlowMap.height; srcY++, dstY++ )
    {
        uint32 srcOffset = ( srcY * iFlowMapBackup.width ) + mPositionX;
        uint32 dstOffset = ( dstY * mPartialFlowMap.width );

        memcpy( &mPartialFlowMap.currToPrevBuffer[dstOffset]
              , &iFlowMapBackup.currToPrevBuffer[srcOffset]
              , iRect.w );
    }
}

void
FOdysseyPainterEditorRasterLiquifyToolUndo::Apply( UObject* iIgnored )
{
    TArray<UOdysseyPainterEditorRasterLiquifyTool::FFlow> line;

    // for swapping values. We replace
    line.SetNum( mPartialFlowMap.width );

    for( uint32 srcY = 0, dstY = mPositionY; srcY < mPartialFlowMap.height; srcY++, dstY++ )
    {
        uint32 srcOffset = ( srcY * mPartialFlowMap.width );
        uint32 dstOffset = ( dstY * mFlowMapOriginal.width ) + mPositionX;

        memcpy( &line[0]
              , &mFlowMapOriginal.currToPrevBuffer[dstOffset]
              , mPartialFlowMap.width );

        memcpy( &mFlowMapOriginal.currToPrevBuffer[dstOffset]
              , &mPartialFlowMap.currToPrevBuffer[srcOffset]
              , mPartialFlowMap.width );

        memcpy( &mPartialFlowMap.currToPrevBuffer[srcOffset]
              , &line[0]
              , mPartialFlowMap.width );
    }
}

void
FOdysseyPainterEditorRasterLiquifyToolUndo::Revert( UObject* iIgnored )
{
    TArray<UOdysseyPainterEditorRasterLiquifyTool::FFlow> line;

    // for swapping values. We replace
    line.SetNum( mPartialFlowMap.width );

    for( uint32 srcY = 0, dstY = mPositionY; srcY < mPartialFlowMap.height; srcY++, dstY++ )
    {
        uint32 srcOffset = ( srcY * mPartialFlowMap.width );
        uint32 dstOffset = ( dstY * mFlowMapOriginal.width ) + mPositionX;

        memcpy( &line[0]
              , &mFlowMapOriginal.currToPrevBuffer[dstOffset]
              , mPartialFlowMap.width );

        memcpy( &mFlowMapOriginal.currToPrevBuffer[dstOffset]
              , &mPartialFlowMap.currToPrevBuffer[srcOffset]
              , mPartialFlowMap.width );

        memcpy( &mPartialFlowMap.currToPrevBuffer[srcOffset]
              , &line[0]
              , mPartialFlowMap.width );
    }
}

/** Describes this change (for debugging) */
FString
FOdysseyPainterEditorRasterLiquifyToolUndo::ToString() const
{
    return FString("FOdysseyPainterEditorRasterLiquifyToolUndo");
}
