#include "InbetweenerTag/InbetweenerBreakdown.h"
#include "InbetweenerTag/InbetweenerGrid.h"
#include "InbetweenerTag/InbetweenerDrawing.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVector.h"
#include "OdysseyVectorObject.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846L
#endif

FInbetweenerBreakdown::~FInbetweenerBreakdown()
{
    delete mGrid;
}

FInbetweenerBreakdown::FInbetweenerBreakdown( FOdysseyVectorTagInbetweener* iInbetweenerTag )
    : mInbetweenerTag( iInbetweenerTag )
    , mSourceDrawingIndex( 0 )
    , mTargetDrawingIndex( 1 )
    , mIndex( 0 )
    , mGrid( nullptr )
    , mPrevBreakdown( nullptr )
    , mNextBreakdown( nullptr )
    , mTargetTranslationX( 0.0f )
    , mTargetTranslationY( 0.0f )
    , mTargetScalingX    ( 1.0f )
    , mTargetScalingY    ( 1.0f )
    , mTargetRotation    ( 0.0f )
    , mChart( this )
{
    SetGrid( iInbetweenerTag->GetGridType() );
}

::ULIS::FRectD
FInbetweenerBreakdown::GetSourceBBox( bool iWorld )
{
    FInbetweenerDrawing* drawing = mInbetweenerTag->GetDrawing( mTargetDrawingIndex );
    ::ULIS::FRectD bbox = mGrid->GetSourceBBox();

    if ( iWorld == true )
    {
        BLMatrix2D worldMatrix = mInbetweenerTag->GetOwner()->GetWorldMatrix();

        worldMatrix.transform( drawing->localMatrix );

        {
            BLPoint p0 = worldMatrix.mapPoint( bbox.x         , bbox.y          );
            BLPoint p1 = worldMatrix.mapPoint( bbox.x + bbox.w, bbox.y          );
            BLPoint p2 = worldMatrix.mapPoint( bbox.x + bbox.w, bbox.y + bbox.h );
            BLPoint p3 = worldMatrix.mapPoint( bbox.x         , bbox.y + bbox.h );
            ::ULIS::FRectD worldBBox = ::ULIS::FRectD::FromMinMax( ::ULIS::FMath::Min4( p0.x, p1.x, p2.x, p3.x )
                                                                 , ::ULIS::FMath::Min4( p0.y, p1.y, p2.y, p3.y )
                                                                 , ::ULIS::FMath::Max4( p0.x, p1.x, p2.x, p3.x )
                                                                 , ::ULIS::FMath::Max4( p0.y, p1.y, p2.y, p3.y ) );

            return worldBBox;
        }
    }

    return bbox;
}

bool
FInbetweenerBreakdown::IsMaster()
{
    return ( this == mInbetweenerTag->GetMasterBreakdown() ) ? true : false;
}

::ULIS::FRectD
FInbetweenerBreakdown::GetTargetBBox( bool iWorld )
{
    FInbetweenerDrawing* drawing = mInbetweenerTag->GetDrawing( mTargetDrawingIndex );
    ::ULIS::FRectD bbox = mGrid->GetTargetBBox();

    if ( iWorld == true )
    {
        BLMatrix2D worldMatrix = mInbetweenerTag->GetOwner()->GetWorldMatrix();

        worldMatrix.transform( drawing->localMatrix );

        {

            BLPoint p0 = worldMatrix.mapPoint( bbox.x         , bbox.y          );
            BLPoint p1 = worldMatrix.mapPoint( bbox.x + bbox.w, bbox.y          );
            BLPoint p2 = worldMatrix.mapPoint( bbox.x + bbox.w, bbox.y + bbox.h );
            BLPoint p3 = worldMatrix.mapPoint( bbox.x         , bbox.y + bbox.h );
            ::ULIS::FRectD worldBBox = ::ULIS::FRectD::FromMinMax( ::ULIS::FMath::Min4( p0.x, p1.x, p2.x, p3.x )
                                                                 , ::ULIS::FMath::Min4( p0.y, p1.y, p2.y, p3.y )
                                                                 , ::ULIS::FMath::Max4( p0.x, p1.x, p2.x, p3.x )
                                                                 , ::ULIS::FMath::Max4( p0.y, p1.y, p2.y, p3.y ) );

            return worldBBox;
        }
    }

    return bbox;
}

double
FInbetweenerBreakdown::GetTargetTranslationX()
{
    return mTargetTranslationX;
}

double
FInbetweenerBreakdown::GetTargetTranslationY()
{
    return mTargetTranslationY;
}

double
FInbetweenerBreakdown::GetTargetRotation()
{
    return mTargetRotation;
}

double
FInbetweenerBreakdown::GetTargetScalingX()
{
    return mTargetScalingX;
}

double
FInbetweenerBreakdown::GetTargetScalingY()
{
    return mTargetScalingY;
}

void
FInbetweenerBreakdown::InterpolateTransform()
{
    FInbetweenerBreakdown* prevBreakdown = GetPrevBreakdown();
    double sourceTranslationX = prevBreakdown ? prevBreakdown->GetTargetTranslationX() : 0.0f
         , sourceTranslationY = prevBreakdown ? prevBreakdown->GetTargetTranslationY() : 0.0f
         , sourceRotation     = prevBreakdown ? prevBreakdown->GetTargetRotation()    : 0.0f
         , sourceScalingX     = prevBreakdown ? prevBreakdown->GetTargetScalingX()     : 1.0f
         , sourceScalingY     = prevBreakdown ? prevBreakdown->GetTargetScalingY()     : 1.0f;

    for( uint32 i = 1; i < GetDrawingCount() - 1; i++ )
    {
        FChartDivision* inbetween = &mChart.GetDivisionArray()[i];
        double translationX, translationY, rotation, scalingX, scalingY;
        double t = inbetween->spacing;

        translationX = sourceTranslationX + ( ( mTargetTranslationX - sourceTranslationX ) * t );
        translationY = sourceTranslationY + ( ( mTargetTranslationY - sourceTranslationY ) * t );
        rotation = sourceRotation + ( ( mTargetRotation - sourceRotation ) * t );
        scalingX = sourceScalingX + ( ( mTargetScalingX - sourceScalingX ) * t );
        scalingY = sourceScalingY + ( ( mTargetScalingY - sourceScalingY ) * t );

        inbetween->drawing->localMatrix.reset();
        inbetween->drawing->localMatrix.translate( translationX, translationY );
        inbetween->drawing->localMatrix.rotate( rotation * M_PI / 180.0f ); // convert to radians
        inbetween->drawing->localMatrix.scale( scalingX, scalingY );

        BLMatrix2D::invert( inbetween->drawing->inverseMatrix, inbetween->drawing->localMatrix );
    }
}

FInbetweenerBreakdown*
FInbetweenerBreakdown::GetMasterBreakdown()
{
    return mInbetweenerTag->GetMasterBreakdown();
}

void
FInbetweenerBreakdown::SetInbetweenerTag( FOdysseyVectorTagInbetweener* iInbetweenerTag )
{
    mInbetweenerTag = iInbetweenerTag;
}

void
FInbetweenerBreakdown::SetPrevBreakdown( FInbetweenerBreakdown* iPrevBreakdown )
{
    mPrevBreakdown = iPrevBreakdown;
}

void
FInbetweenerBreakdown::SetNextBreakdown( FInbetweenerBreakdown* iNextBreakdown )
{
    mNextBreakdown = iNextBreakdown;
}

FInbetweenerBreakdown*
FInbetweenerBreakdown::GetPrevBreakdown()
{
    return mPrevBreakdown;
}

FInbetweenerBreakdown*
FInbetweenerBreakdown::GetNextBreakdown()
{
    return mNextBreakdown;
}

FOdysseyVectorTagInbetweener*
FInbetweenerBreakdown::GetInbetweenerTag()
{
    return mInbetweenerTag;
}

void
FInbetweenerBreakdown::SetIndex( uint32 iIndex )
{
    mIndex = iIndex;
}

uint32
FInbetweenerBreakdown::GetIndex()
{
    return mIndex;
}

void
FInbetweenerBreakdown::DrawPathsAtTarget( BLContext* iBLContext )
{
    BLMatrix2D worldMatrix = mInbetweenerTag->GetOwner()->GetWorldMatrix();

    iBLContext->save();
    iBLContext->resetMatrix();

    worldMatrix.transform( GetTargetLocalMatrix() );

    for( FInterpolatedPath& interpolatedPath : mInbetweenerTag->GetInterpolatedPathBuffer() )
    {
        uint32 pointCount = interpolatedPath.GetInterpolatedPointBuffer().size();
        ::ULIS::FVec2D* pointPositionBuffer = &interpolatedPath.GetInterpolatedPointPositionBuffer()[pointCount * mTargetDrawingIndex];

        mInbetweenerTag->DrawPathAt( &interpolatedPath
                                   , pointPositionBuffer
                                   , worldMatrix
                                   , iBLContext );
    }

    iBLContext->restore();
}

void
FInbetweenerBreakdown::DrawPathsAtSource( BLContext* iBLContext )
{
    BLMatrix2D worldMatrix = mInbetweenerTag->GetOwner()->GetWorldMatrix();

    iBLContext->save();
    iBLContext->resetMatrix();

    for( FInterpolatedPath& interpolatedPath : mInbetweenerTag->GetInterpolatedPathBuffer() )
    {
        uint32 pointCount = interpolatedPath.GetInterpolatedPointBuffer().size();
        ::ULIS::FVec2D* pointPositionBuffer = &interpolatedPath.GetInterpolatedPointPositionBuffer()[pointCount * mSourceDrawingIndex];

        mInbetweenerTag->DrawPathAt( &interpolatedPath
                                   , pointPositionBuffer
                                   , worldMatrix
                                   , iBLContext );
    }

    iBLContext->restore();
}

void
FInbetweenerBreakdown::GetTargetTransform( double& oTranslationX
                                         , double& oTranslationY
                                         , double& oRotation
                                         , double& oScalingX
                                         , double& oScalingY )
{
    oTranslationX = mTargetTranslationX;
    oTranslationY = mTargetTranslationY;
    oRotation = mTargetRotation;
    oScalingX = mTargetScalingX;
    oScalingY = mTargetScalingY;
}

void
FInbetweenerBreakdown::SetTargetTransform( double iTranslationX
                                         , double iTranslationY
                                         , double iRotation
                                         , double iScalingX
                                         , double iScalingY )
{
    mTargetTranslationX = iTranslationX;
    mTargetTranslationY = iTranslationY;
    mTargetRotation = iRotation;
    mTargetScalingX = iScalingX;
    mTargetScalingY = iScalingY;

    //UpdateMatrix( );
}

void
FInbetweenerBreakdown::UpdateMatrix()
{
    FInbetweenerDrawing* drawing = mInbetweenerTag->GetDrawing( mTargetDrawingIndex );

    drawing->localMatrix.reset();
    drawing->localMatrix.translate( mTargetTranslationX, mTargetTranslationY );
    drawing->localMatrix.rotate( mTargetRotation * M_PI / 180.0f );
    drawing->localMatrix.scale( mTargetScalingX, mTargetScalingY );

    drawing->worldMatrix = mInbetweenerTag->GetOwner()->GetWorldMatrix();
    drawing->worldMatrix.transform( drawing->localMatrix );

    BLMatrix2D::invert( drawing->inverseWorldMatrix, drawing->worldMatrix );

/*
    mTargetWorldMatrix = mOwner->GetWorldMatrix();
    mTargetWorldMatrix.transform( mTargetLocalMatrix );

    BLMatrix2D::invert( mTargetInverseWorldMatrix, mTargetWorldMatrix );

    // first drawing does not need interpolation. It is actually the source position
    for( uint32 drawingIndex = 1; drawingIndex < GetDrawingCount(); drawingIndex++ )
    {
        InterpolateTransform( drawingIndex );
    }
*/
    mInbetweenerTag->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_CELLS
                               | FOdysseyVectorTagInbetweener::INVALIDATE_ROUTES
                               | FOdysseyVectorTagInbetweener::INVALIDATE_SPACING );
}

void
FInbetweenerBreakdown::Translate( double iX, double iY )
{
    mTargetTranslationX = iX;
    mTargetTranslationY = iY;
}

void
FInbetweenerBreakdown::Rotate( double iAngle )
{
    mTargetRotation = iAngle;
}

void
FInbetweenerBreakdown::Scale( double iX, double iY )
{
    mTargetScalingX = iX;
    mTargetScalingY = iY;
}

BLMatrix2D&
FInbetweenerBreakdown::GetSourceLocalMatrix()
{
    static BLMatrix2D identityMatrix = BLMatrix2D::makeIdentity();

    if( mSourceDrawingIndex == 0 )
    {
        return identityMatrix;
    }

    return mInbetweenerTag->GetDrawing( mSourceDrawingIndex )->localMatrix;
}

BLMatrix2D&
FInbetweenerBreakdown::GetTargetLocalMatrix()
{
    return mInbetweenerTag->GetDrawing( mTargetDrawingIndex )->localMatrix;
}

BLMatrix2D&
FInbetweenerBreakdown::GetTargetWorldMatrix()
{
    return mInbetweenerTag->GetDrawing( mTargetDrawingIndex )->worldMatrix;
}

BLMatrix2D&
FInbetweenerBreakdown::GetTargetInverseWorldMatrix()
{
    return mInbetweenerTag->GetDrawing( mTargetDrawingIndex )->inverseWorldMatrix;
}

void
FInbetweenerBreakdown::SetSourceDrawingIndex( uint32 iSourceDrawingIndex )
{
    mSourceDrawingIndex = iSourceDrawingIndex;

    if( mPrevBreakdown )
    {
        mPrevBreakdown->mTargetDrawingIndex = iSourceDrawingIndex;

        mPrevBreakdown->mChart.Resize();
    }

    mChart.Resize();

    mInbetweenerTag->ResizeDrawings();

    mInbetweenerTag->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_SPACING 
                               // force deformation of interpolated paths at target
                               | FOdysseyVectorTagInbetweener::INVALIDATE_RANGE  );
}

FInbetweenerChart*
FInbetweenerBreakdown::GetChart()
{
    return &mChart;
}

void
FInbetweenerBreakdown::SetTargetDrawingIndex( uint32 iTargetDrawingIndex )
{
    // Invalidate current cells
    mInbetweenerTag->RedrawAnimationCells();

    mTargetDrawingIndex = iTargetDrawingIndex;

    if( mNextBreakdown )
    {
        mNextBreakdown->mSourceDrawingIndex = iTargetDrawingIndex;

        mNextBreakdown->mChart.Resize();
    }

    mChart.Resize();

    mInbetweenerTag->ResizeDrawings();

    // TODO: put this somewhere else. I put it here so it can geenrate matrices based on
    // the t value fromthe chart, but I don't think it is the best place. 
    mInbetweenerTag->UpdateMatrix();

    mInbetweenerTag->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_SPACING 
                               // force deformation of interpolated paths at target
                               | FOdysseyVectorTagInbetweener::INVALIDATE_RANGE );
}

uint32
FInbetweenerBreakdown::GetSourceDrawingIndex()
{
    return mSourceDrawingIndex;
}

uint32
FInbetweenerBreakdown::GetTargetDrawingIndex()
{
    return mTargetDrawingIndex;
}

uint32
FInbetweenerBreakdown::GetDrawingCount()
{
    return mTargetDrawingIndex - mSourceDrawingIndex + 1;
}

int32
FInbetweenerBreakdown::GetTargetAnimationCellIndex()
{
    uint32 tagCellIndex = mInbetweenerTag->GetAnimationCellIndex();

    return (int32)tagCellIndex + (int32)( mTargetDrawingIndex * (int)mInbetweenerTag->GetInterpolationDirection());
}

int32
FInbetweenerBreakdown::GetSourceAnimationCellIndex()
{
    uint32 tagCellIndex = mInbetweenerTag->GetAnimationCellIndex();

    return (int32)tagCellIndex + (int32)( mSourceDrawingIndex * (int)mInbetweenerTag->GetInterpolationDirection());
}

void
FInbetweenerBreakdown::SetGrid( eInbetweenerGridType iGridType )
{
    if( mGrid )
    {
        delete mGrid;

        mGrid = nullptr;
    }

    switch( iGridType )
    {
        case eInbetweenerGridType::ARAP :
            mGrid = new FInbetweenerGridARAP( this );
        break;

        default:
            mGrid = new FInbetweenerGridFFD( this );
        break;
    }

    mGrid->Make();
}

FInbetweenerGrid*
FInbetweenerBreakdown::GetGrid()
{
    return mGrid;
}
