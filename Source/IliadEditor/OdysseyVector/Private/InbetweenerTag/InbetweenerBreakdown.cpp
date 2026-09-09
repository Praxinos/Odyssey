// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "InbetweenerTag/InbetweenerBreakdown.h"
#include "InbetweenerTag/InbetweenerGrid.h"
#include "InbetweenerTag/InbetweenerDrawing.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVector.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorLayer.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846L
#endif

FInbetweenerBreakdown::~FInbetweenerBreakdown()
{
    if( mGrid != nullptr )
    {
        delete mGrid;
        mGrid = nullptr;
    }
}

FInbetweenerBreakdown::FInbetweenerBreakdown( FOdysseyVectorTagInbetweener* iInbetweenerTag )
    : mTargetTranslationX( 0.0f )
    , mTargetTranslationY( 0.0f )
    , mTargetRotation    ( 0.0f )
    , mTargetScalingX    ( 1.0f )
    , mTargetScalingY    ( 1.0f )
    , mTargetSkewX       ( 0.0f )
    , mTargetSkewY       ( 0.0f )
    , mInbetweenerTag( iInbetweenerTag )
    , mPrevBreakdown( nullptr )
    , mNextBreakdown( nullptr )
    , mGrid( nullptr )
    , mTargetDrawingIndex( 1 )
    , mIndex( 0 )
    , mChart( this )
    , bTargetVisibility ( false )
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
            BLPoint p0 = worldMatrix.map_point( bbox.x         , bbox.y          );
            BLPoint p1 = worldMatrix.map_point( bbox.x + bbox.w, bbox.y          );
            BLPoint p2 = worldMatrix.map_point( bbox.x + bbox.w, bbox.y + bbox.h );
            BLPoint p3 = worldMatrix.map_point( bbox.x         , bbox.y + bbox.h );
            ::ULIS::FRectD worldBBox = ::ULIS::FRectD::FromMinMax( ::ULIS::FMath::Min4( p0.x, p1.x, p2.x, p3.x )
                                                                 , ::ULIS::FMath::Min4( p0.y, p1.y, p2.y, p3.y )
                                                                 , ::ULIS::FMath::Max4( p0.x, p1.x, p2.x, p3.x )
                                                                 , ::ULIS::FMath::Max4( p0.y, p1.y, p2.y, p3.y ) );

            return worldBBox;
        }
    }

    return bbox;
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

            BLPoint p0 = worldMatrix.map_point( bbox.x         , bbox.y          );
            BLPoint p1 = worldMatrix.map_point( bbox.x + bbox.w, bbox.y          );
            BLPoint p2 = worldMatrix.map_point( bbox.x + bbox.w, bbox.y + bbox.h );
            BLPoint p3 = worldMatrix.map_point( bbox.x         , bbox.y + bbox.h );
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

double
FInbetweenerBreakdown::GetTargetSkewX()
{
    return mTargetSkewX;
}

double
FInbetweenerBreakdown::GetTargetSkewY()
{
    return mTargetSkewY;
}

double
FInbetweenerBreakdown::GetSourceTranslationX()
{
    return mPrevBreakdown ? mPrevBreakdown->GetTargetTranslationX() : 0.0f;
}

double
FInbetweenerBreakdown::GetSourceTranslationY()
{
    return mPrevBreakdown ? mPrevBreakdown->GetTargetTranslationY() : 0.0f;
}

double
FInbetweenerBreakdown::GetSourceRotation()
{
    return mPrevBreakdown ? mPrevBreakdown->GetTargetRotation() : 0.0f;
}

double
FInbetweenerBreakdown::GetSourceScalingX()
{
    return mPrevBreakdown ? mPrevBreakdown->GetTargetScalingX() : 1.0f;
}

double
FInbetweenerBreakdown::GetSourceScalingY()
{
    return mPrevBreakdown ? mPrevBreakdown->GetTargetScalingY() : 1.0f;
}

double
FInbetweenerBreakdown::GetSourceSkewX()
{
    return mPrevBreakdown ? mPrevBreakdown->GetTargetSkewX() : 0.0f;
}

double
FInbetweenerBreakdown::GetSourceSkewY()
{
    return mPrevBreakdown ? mPrevBreakdown->GetTargetSkewY() : 0.0f;
}

void
FInbetweenerBreakdown::InterpolateTransform()
{
    FInbetweenerBreakdown* prevBreakdown = GetPrevBreakdown();
    double sourceTranslationX = prevBreakdown ? prevBreakdown->GetTargetTranslationX() : 0.0f
         , sourceTranslationY = prevBreakdown ? prevBreakdown->GetTargetTranslationY() : 0.0f
         , sourceRotation     = prevBreakdown ? prevBreakdown->GetTargetRotation()     : 0.0f
         , sourceScalingX     = prevBreakdown ? prevBreakdown->GetTargetScalingX()     : 1.0f
         , sourceScalingY     = prevBreakdown ? prevBreakdown->GetTargetScalingY()     : 1.0f
         , sourceSkewX        = prevBreakdown ? prevBreakdown->GetTargetSkewX()        : 0.0f
         , sourceSkewY        = prevBreakdown ? prevBreakdown->GetTargetSkewY()        : 0.0f;

    for( uint32 i = 1; i < GetDrawingCount() - 1; i++ )
    {
        FInbetweenerChart::Inbetween* inbetween = &mChart.GetInbetweenBuffer()[i];
        //double translationX, translationY, rotation, scalingX, scalingY;
        double t = inbetween->GetSpacing();

        inbetween->GetDrawing()->translationX = sourceTranslationX + ( ( mTargetTranslationX - sourceTranslationX ) * t );
        inbetween->GetDrawing()->translationY = sourceTranslationY + ( ( mTargetTranslationY - sourceTranslationY ) * t );
        inbetween->GetDrawing()->rotation = sourceRotation + ( ( mTargetRotation - sourceRotation ) * t );
        inbetween->GetDrawing()->scalingX = sourceScalingX + ( ( mTargetScalingX - sourceScalingX ) * t );
        inbetween->GetDrawing()->scalingY = sourceScalingY + ( ( mTargetScalingY - sourceScalingY ) * t );
        inbetween->GetDrawing()->skewX = sourceSkewX + ( ( mTargetSkewX - sourceSkewX ) * t );
        inbetween->GetDrawing()->skewY = sourceSkewY + ( ( mTargetSkewY - sourceSkewY ) * t );

        inbetween->GetDrawing()->localMatrix.reset();
        inbetween->GetDrawing()->localMatrix.translate( inbetween->GetDrawing()->translationX
                                                      , inbetween->GetDrawing()->translationY );
        inbetween->GetDrawing()->localMatrix.rotate( inbetween->GetDrawing()->rotation * M_PI / 180.0f ); // convert to radians
        inbetween->GetDrawing()->localMatrix.scale( inbetween->GetDrawing()->scalingX
                                                  , inbetween->GetDrawing()->scalingY );
        inbetween->GetDrawing()->localMatrix.skew( inbetween->GetDrawing()->skewX
                                                 , inbetween->GetDrawing()->skewY );

        BLMatrix2D::invert( inbetween->GetDrawing()->inverseMatrix, inbetween->GetDrawing()->localMatrix );
    }
}

void
FInbetweenerBreakdown::SetInbetweenerTag( FOdysseyVectorTagInbetweener* iInbetweenerTag )
{
    mInbetweenerTag = iInbetweenerTag;
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

static double
GetEaseOutSpacing( double iStrength, double iT )
{
    double t = iStrength;
    double u = 1.0f - iStrength;

//UE_LOG(LogTemp, Warning, TEXT("%f %f"), t, u );

    ::ULIS::FVec2D p0 = ::ULIS::FVec2D( 0.0f, 0.0f );
    ::ULIS::FVec2D p2 = ::ULIS::FVec2D( 1.0f, 1.0f );
    ::ULIS::FVec2D p1 = ::ULIS::FVec2D( 0.0f + ( u * 0.5f ), 0.5f + ( t * 0.5f ) );

    return ::ULIS::QuadraticBezierPointAtParameter( p0, p1, p2, iT ).y;
}

static double
GetEaseInSpacing( double iStrength, double iT )
{
    double t = iStrength;
    double u = 1.0f - iStrength;

//UE_LOG(LogTemp, Warning, TEXT("%f %f"), t, u );

    ::ULIS::FVec2D p0 = ::ULIS::FVec2D( 0.0f, 0.0f );
    ::ULIS::FVec2D p2 = ::ULIS::FVec2D( 1.0f, 1.0f );
    ::ULIS::FVec2D p1 = ::ULIS::FVec2D( 0.5f + ( t * 0.5f ), 0.0f + ( u * 0.5f ) );

    return ::ULIS::QuadraticBezierPointAtParameter( p0, p1, p2, iT ).y;
}

void
FInbetweenerBreakdown::EaseOut( float iStrength, uint32 iFactor, uint32 iFrom, uint32 iTo )
{
    uint32 divisionCount = iTo - iFrom + 1;
    float fromSpacing = mChart.GetInbetweenBuffer()[iFrom].GetSpacing();
    float toSpacing = mChart.GetInbetweenBuffer()[iTo].GetSpacing();

    for( uint32 i = iFrom + 1, j = 1; i < iTo; i++, j++ )
    {
        double t = ( double ) j / ( divisionCount - 1 );
        double spacing = t;

        for( uint32 k = 0; k < iFactor; k++ )
        {
            spacing = GetEaseOutSpacing( iStrength, spacing );
        }

//UE_LOG(LogTemp, Warning, TEXT("Spacing:%f"), spacing );
        mChart.GetInbetweenBuffer()[i].SetSpacing( fromSpacing + ( spacing * ( toSpacing - fromSpacing ) ) );
    }

    mInbetweenerTag->GetOwner()->Invalidate( FOdysseyVectorObjectInvalidationFlags().Set( FOdysseyVectorObjectInvalidationFlags::TAG_INBETWEENER_SPACING )
                                                                                    .Set( FOdysseyVectorObjectInvalidationFlags::TAG_INBETWEENER_CELLS ) );
}

void
FInbetweenerBreakdown::EaseOut( float iStrength, uint32 iFactor )
{
    EaseOut( fabs(iStrength)
           , iFactor
           , mChart.GetInbetweenBuffer().front().GetIndex()
           , mChart.GetInbetweenBuffer().back().GetIndex() );
}

void
FInbetweenerBreakdown::EaseIn( float iStrength, uint32 iFactor, uint32 iFrom, uint32 iTo )
{
    uint32 divisionCount = iTo - iFrom + 1;
    float fromSpacing = mChart.GetInbetweenBuffer()[iFrom].GetSpacing();
    float toSpacing = mChart.GetInbetweenBuffer()[iTo].GetSpacing();

    for( uint32 i = iFrom + 1, j = 1; i < iTo; i++, j++ )
    {
        double t = ( double ) j / ( divisionCount - 1 );
        double spacing = t;

        for( uint32 k = 0; k < iFactor; k++ )
        {
            spacing = GetEaseInSpacing( iStrength, spacing );
        }

//UE_LOG(LogTemp, Warning, TEXT("Spacing:%f"), spacing );
        mChart.GetInbetweenBuffer()[i].SetSpacing ( fromSpacing + ( spacing * ( toSpacing - fromSpacing ) ) );
    }

    mInbetweenerTag->GetOwner()->Invalidate( FOdysseyVectorObjectInvalidationFlags().Set(FOdysseyVectorObjectInvalidationFlags::TAG_INBETWEENER_SPACING)
                                                                                    .Set(FOdysseyVectorObjectInvalidationFlags::TAG_INBETWEENER_CELLS) );
}

void
FInbetweenerBreakdown::EaseIn( float iStrength, uint32 iFactor )
{
    EaseIn( fabs(iStrength)
           , iFactor
           , mChart.GetInbetweenBuffer().front().GetIndex()
           , mChart.GetInbetweenBuffer().back().GetIndex() );
}

void
FInbetweenerBreakdown::EaseInAndOut( float iStrength, uint32 iFactor, FInbetweenerChart::Inbetween* iInbetween )
{
    if( iStrength > 0.0f )
    {
        EaseIn ( fabs(iStrength), iFactor, mChart.GetInbetweenBuffer().front().GetIndex(), iInbetween->GetIndex() );
        EaseOut( fabs(iStrength), iFactor, iInbetween->GetIndex(), mChart.GetInbetweenBuffer().back().GetIndex() );
    }
    else
    {
        EaseOut( fabs(iStrength), iFactor, mChart.GetInbetweenBuffer().front().GetIndex(), iInbetween->GetIndex() );
        EaseIn ( fabs(iStrength), iFactor, iInbetween->GetIndex(), mChart.GetInbetweenBuffer().back().GetIndex() );
    }
}

/*
void
FInbetweenerBreakdown::DrawPathsAtTarget( FOdysseyVectorGroupPaint* iDisplayedScene
                                        , BLContext* iBLContext
                                        , bool iLock )
{
    BLMatrix2D worldMatrix = mInbetweenerTag->GetOwner()->GetWorldMatrix();

    // lock because the proxy could call the draw function at anytime even though inbetweenerTag isn't up-to-date.
    // This mutex is then also locked in by Update function.
    if( iLock )
        mInbetweenerTag->mDrawingMutex.lock();

    iBLContext->save();
    iBLContext->resetMatrix();

    worldMatrix.transform( GetTargetLocalMatrix() );

    for( FInterpolatedPath& interpolatedPath : mInbetweenerTag->GetInterpolatedPathBuffer() )
    {
        uint32 pointCount = interpolatedPath.GetInterpolatedPointBuffer().size();
        ::ULIS::FVec2D* pointPositionBuffer = &interpolatedPath.GetInterpolatedPointPositionBuffer()[pointCount * mTargetDrawingIndex];
        float scaling = mInbetweenerTag->HasConstantWidth() ? 1.0f / ( GetTargetScalingX()
                                                                     * GetTargetScalingY() ) : 1.0f;

        mInbetweenerTag->DrawPathAt( iDisplayedScene
                                   , &interpolatedPath
                                   , pointPositionBuffer
                                   , worldMatrix
                                   , iBLContext
                                   , false
                                   // note: a surface grows or shrink at the square of the scaling factor.
                                   // That's why we use sqrt to get the actual scaling factor from the surface ratio.
                                   , sqrt(scaling) );
    }

    iBLContext->restore();

    if( iLock )
        mInbetweenerTag->mDrawingMutex.unlock();
}
*/
/*
void
FInbetweenerBreakdown::DrawPathsAtSource( FOdysseyVectorGroupPaint* iDisplayedScene
                                        , BLContext* iBLContext
                                        , bool iLock )
{
    BLMatrix2D worldMatrix = mInbetweenerTag->GetOwner()->GetWorldMatrix();
    uint32 sourceDrawingIndex = GetSourceDrawingIndex();

    // lock because the proxy could call the draw function at anytime even though inbetweenerTag isn't up-to-date.
    // This mutex is then also locked in by Update function.
    if( iLock )
        mInbetweenerTag->mDrawingMutex.lock();

    iBLContext->save();
    iBLContext->resetMatrix();

    worldMatrix.transform( GetSourceLocalMatrix() );

    for( FInterpolatedPath& interpolatedPath : mInbetweenerTag->GetInterpolatedPathBuffer() )
    {
        uint32 pointCount = interpolatedPath.GetInterpolatedPointBuffer().size();
        ::ULIS::FVec2D* pointPositionBuffer = &interpolatedPath.GetInterpolatedPointPositionBuffer()[pointCount * sourceDrawingIndex];
        float scaling = mInbetweenerTag->HasConstantWidth() ? 1.0f / ( GetSourceScalingX()
                                                                     * GetSourceScalingY() ) : 1.0f;

        mInbetweenerTag->DrawPathAt( iDisplayedScene
                                   , &interpolatedPath
                                   , pointPositionBuffer
                                   , worldMatrix
                                   , iBLContext
                                   , false
                                   // note: a surface grows or shrink at the square of the scaling factor.
                                   // That's why we use sqrt to get the actual scaling factor from the surface ratio.
                                   , sqrt(scaling) );
    }

    iBLContext->restore();

    if( iLock )
        mInbetweenerTag->mDrawingMutex.unlock();
}
*/

void
FInbetweenerBreakdown::GetTargetTransform( double& oTranslationX
                                         , double& oTranslationY
                                         , double& oRotation
                                         , double& oScalingX
                                         , double& oScalingY
                                         , double& oSkewX
                                         , double& oSkewY )
{
    oTranslationX = mTargetTranslationX;
    oTranslationY = mTargetTranslationY;
    oRotation = mTargetRotation;
    oScalingX = mTargetScalingX;
    oScalingY = mTargetScalingY;
    oSkewX = mTargetSkewX;
    oSkewY = mTargetSkewY;
}

void
FInbetweenerBreakdown::SetTargetTransform( double iTranslationX
                                         , double iTranslationY
                                         , double iRotation
                                         , double iScalingX
                                         , double iScalingY
                                         , double iSkewX
                                         , double iSkewY )
{
    mTargetTranslationX = iTranslationX;
    mTargetTranslationY = iTranslationY;
    mTargetRotation = iRotation;
    mTargetScalingX = iScalingX;
    mTargetScalingY = iScalingY;
    mTargetSkewX    = iSkewX;
    mTargetSkewY    = iSkewY;

    //UpdateMatrix( );
}

void
FInbetweenerBreakdown::UpdateMatrix()
{
    FInbetweenerDrawing* drawing = mInbetweenerTag->GetDrawing( mTargetDrawingIndex );

    drawing->translationX = mTargetTranslationX;
    drawing->translationY = mTargetTranslationY;
    drawing->rotation = mTargetRotation;
    drawing->scalingX = mTargetScalingX;
    drawing->scalingY = mTargetScalingY;
    drawing->skewX    = mTargetSkewX;
    drawing->skewY    = mTargetSkewY;

    drawing->localMatrix.reset();
    drawing->localMatrix.translate( mTargetTranslationX, mTargetTranslationY );
    drawing->localMatrix.rotate( mTargetRotation * M_PI / 180.0f );
    drawing->localMatrix.scale( mTargetScalingX, mTargetScalingY );
    drawing->localMatrix.skew( mTargetSkewX, mTargetSkewY );

    BLMatrix2D::invert( drawing->inverseMatrix, drawing->localMatrix );

    drawing->worldMatrix = mInbetweenerTag->GetOwner()->GetWorldMatrix();
    drawing->worldMatrix.transform( drawing->localMatrix );

    BLMatrix2D::invert( drawing->inverseWorldMatrix, drawing->worldMatrix );

    mInbetweenerTag->GetOwner()->Invalidate( FOdysseyVectorObjectInvalidationFlags().Set(FOdysseyVectorObjectInvalidationFlags::TAG_INBETWEENER_MATRIX)
                                                                                    .Set(FOdysseyVectorObjectInvalidationFlags::TAG_INBETWEENER_CELLS)
                                                                                    .Set(FOdysseyVectorObjectInvalidationFlags::TAG_INBETWEENER_ROUTES)
                                                                                    .Set(FOdysseyVectorObjectInvalidationFlags::TAG_INBETWEENER_SPACING) );
}

void
FInbetweenerBreakdown::Translate( double iX, double iY )
{
    mTargetTranslationX = iX;
    mTargetTranslationY = iY;
}

void
FInbetweenerBreakdown::Skew( double iX, double iY )
{
    mTargetSkewX = iX;
    mTargetSkewY = iY;
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
    static BLMatrix2D identityMatrix = BLMatrix2D::make_identity();
    uint32 sourceDrawingIndex = GetSourceDrawingIndex();

    if( sourceDrawingIndex == 0 )
    {
        return identityMatrix;
    }

    return mInbetweenerTag->GetDrawing( sourceDrawingIndex )->localMatrix;
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
FInbetweenerBreakdown::SetTargetDrawingIndex( uint32 iTargetDrawingIndex )
{
    FInbetweenerBreakdown* nextBreakdown = GetNextBreakdown();

    mTargetDrawingIndex = iTargetDrawingIndex;

    if( nextBreakdown )
    {
        nextBreakdown->mChart.Resize();
    }

    mChart.Resize();

    mInbetweenerTag->ResizeDrawings();

    // TODO: put this somewhere else. I put it here so it can geenrate matrices based on
    // the t value fromthe chart, but I don't think it is the best place.
    mInbetweenerTag->UpdateMatrix();

    mInbetweenerTag->GetOwner()->Invalidate( FOdysseyVectorObjectInvalidationFlags().Set( FOdysseyVectorObjectInvalidationFlags::TAG_INBETWEENER_SPACING )
                                                                                    .Set( FOdysseyVectorObjectInvalidationFlags::TAG_INBETWEENER_BUFFERS ) );
}

FInbetweenerChart*
FInbetweenerBreakdown::GetChart()
{
    return &mChart;
}

uint32
FInbetweenerBreakdown::GetSourceDrawingIndex()
{
    FInbetweenerBreakdown* prevBreakdown = GetPrevBreakdown();

    return prevBreakdown ? prevBreakdown->mTargetDrawingIndex : 0;
}

uint32
FInbetweenerBreakdown::GetTargetDrawingIndex()
{
    return mTargetDrawingIndex;
}

uint32
FInbetweenerBreakdown::GetDrawingCount()
{
    uint32 sourceDrawingIndex = GetSourceDrawingIndex();

    return mTargetDrawingIndex - sourceDrawingIndex + 1;
}

FOdysseyVectorCell*
FInbetweenerBreakdown::GetSourceCell()
{
    FOdysseyVectorLayer* layer = mInbetweenerTag->GetOwner()->GetLayer();

    // layer can be null when the cell is removed from the layer
    if( layer )
    {
        uint32 sourceCellIndex = GetSourceCellIndex();

        return layer->GetCellByIndex( sourceCellIndex );
    }

    return nullptr;
}

FOdysseyVectorCell*
FInbetweenerBreakdown::GetTargetCell()
{
    FOdysseyVectorLayer* layer = mInbetweenerTag->GetOwner()->GetLayer();

    // layer can be null when the cell is removed from the layer
    if( layer )
    {
        uint32 targetCellIndex = GetTargetCellIndex();

        return layer->GetCellByIndex( targetCellIndex );
    }

    return nullptr;
}


int32
FInbetweenerBreakdown::GetTargetCellIndex()
{
    uint32 tagCellIndex = mInbetweenerTag->GetOwner()->GetCell()->GetIndex();

    return (int32)tagCellIndex + (int32)( mTargetDrawingIndex * (int)mInbetweenerTag->GetInterpolationDirection());
}

int32
FInbetweenerBreakdown::GetSourceCellIndex()
{
    // we use GetSecene because the Scene is stored as a member varriable in order to be able to
    // redraw when the owner object is removed (its scene would be null then)
    uint32 tagCellIndex = mInbetweenerTag->GetScene()->GetCell()->GetIndex();
    uint32 sourceDrawingIndex = GetSourceDrawingIndex();

    return (int32)tagCellIndex + (int32)( sourceDrawingIndex * (int)mInbetweenerTag->GetInterpolationDirection());
}

bool
FInbetweenerBreakdown::IsTargetVisible()
{
    return bTargetVisibility;
}

void
FInbetweenerBreakdown::SetTargetVisibility( bool iTargetVisibility )
{
    bTargetVisibility = iTargetVisibility;

    mInbetweenerTag->GetOwner()->Invalidate( FOdysseyVectorObjectInvalidationFlags().Set( FOdysseyVectorObjectInvalidationFlags::TAG_INBETWEENER_CELLS ) );
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

    mGrid->Make( true );
}

FInbetweenerGrid*
FInbetweenerBreakdown::GetGrid()
{
    return mGrid;
}
