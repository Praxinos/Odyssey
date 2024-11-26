// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

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
    delete mGrid;
}

FInbetweenerBreakdown::FInbetweenerBreakdown( FOdysseyVectorTagInbetweener* iInbetweenerTag )
    : mInbetweenerTag( iInbetweenerTag )
    , mPrevBreakdown( nullptr )
    , mNextBreakdown( nullptr )
    , mTargetDrawingIndex( 1 )
    , mIndex( 0 )
    , mGrid( nullptr )
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
        FChartDivision* inbetween = &mChart.GetDivisionBuffer()[i];
        //double translationX, translationY, rotation, scalingX, scalingY;
        double t = inbetween->spacing;

        inbetween->drawing->translationX = sourceTranslationX + ( ( mTargetTranslationX - sourceTranslationX ) * t );
        inbetween->drawing->translationY = sourceTranslationY + ( ( mTargetTranslationY - sourceTranslationY ) * t );
        inbetween->drawing->rotation = sourceRotation + ( ( mTargetRotation - sourceRotation ) * t );
        inbetween->drawing->scalingX = sourceScalingX + ( ( mTargetScalingX - sourceScalingX ) * t );
        inbetween->drawing->scalingY = sourceScalingY + ( ( mTargetScalingY - sourceScalingY ) * t );

        inbetween->drawing->localMatrix.reset();
        inbetween->drawing->localMatrix.translate( inbetween->drawing->translationX
                                                 , inbetween->drawing->translationY );
        inbetween->drawing->localMatrix.rotate( inbetween->drawing->rotation * M_PI / 180.0f ); // convert to radians
        inbetween->drawing->localMatrix.scale( inbetween->drawing->scalingX
                                             , inbetween->drawing->scalingY );

        BLMatrix2D::invert( inbetween->drawing->inverseMatrix, inbetween->drawing->localMatrix );
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

void
FInbetweenerBreakdown::DrawTargetGrid( BLContext* iBLContext, bool iLock )
{
    DrawGrid( iBLContext
            , eInbetweenerPointPositionType::TargetPosition
            , mInbetweenerTag->GetGridColor()
            , iLock );
}

void
FInbetweenerBreakdown::DrawSourceGrid( BLContext* iBLContext, bool iLock )
{
    DrawGrid( iBLContext
            , eInbetweenerPointPositionType::SourcePosition
            , FColor( 127, 127, 127, 127 )
            , iLock );
}

void
FInbetweenerBreakdown::DrawGrid( BLContext* iBLContext
                               , eInbetweenerPointPositionType iPositionType
                               , const FColor& iColor
                               , bool iLock )
{
    BLMatrix2D worldMatrix = mInbetweenerTag->GetOwner()->GetWorldMatrix();
    BLRgba32 gridColor = BLRgba32( iColor.R
                                 , iColor.G
                                 , iColor.B
                                 , iColor.A );
    std::vector<FInbetweenerPoint>& pointBuffer = mGrid->GetPointBuffer();

    // lock because the proxy could call the draw function at anytime even though inbetweenerTag isn't up-to-date.
    // This mutex is then also locked in by Update function.
    if( iLock )
        mInbetweenerTag->mDrawingMutex.lock();

    iBLContext->save();
    iBLContext->resetMatrix();

    iBLContext->setFillStyle( gridColor );
    iBLContext->setStrokeStyle( gridColor );
    iBLContext->setStrokeWidth( 1.0f );

    if( iPositionType == eInbetweenerPointPositionType::TargetPosition )
    {
        worldMatrix.transform( GetTargetLocalMatrix() );
    }

    for( uint32 pointIndex : mInbetweenerTag->GetUsedPointIndexBuffer() )
    {
        FInbetweenerPoint& point = pointBuffer[pointIndex];
        ::ULIS::FVec2D position = point.GetPosition( iPositionType );
        BLPoint pt = worldMatrix.mapPoint( position.x, position.y );

        iBLContext->fillCircle( pt.x, pt.y, 2 );
    }

    iBLContext->restore();

    if( iLock )
        mInbetweenerTag->mDrawingMutex.unlock();
}

static double
GetEaseOutSpacing( double iT, double iFraction )
{
    double t = iT;
    double u = 1.0f - iT;

//UE_LOG(LogTemp, Warning, TEXT("%f %f"), t, u );

    ::ULIS::FVec2D p0 = ::ULIS::FVec2D( 0.0f, 0.0f );
    ::ULIS::FVec2D p2 = ::ULIS::FVec2D( 1.0f, 1.0f );
    ::ULIS::FVec2D p1 = ::ULIS::FVec2D( 0.0f + ( u * 0.5f ), 0.5f + ( t * 0.5f ) );

    return ::ULIS::QuadraticBezierPointAtParameter( p0, p1, p2, iFraction ).y;
}

static double
GetEaseInSpacing( double iT, double iFraction )
{
    double t = iT;
    double u = 1.0f - iT;

//UE_LOG(LogTemp, Warning, TEXT("%f %f"), t, u );

    ::ULIS::FVec2D p0 = ::ULIS::FVec2D( 0.0f, 0.0f );
    ::ULIS::FVec2D p2 = ::ULIS::FVec2D( 1.0f, 1.0f );
    ::ULIS::FVec2D p1 = ::ULIS::FVec2D( 0.5f + ( t * 0.5f ), 0.0f + ( u * 0.5f ) );

    return ::ULIS::QuadraticBezierPointAtParameter( p0, p1, p2, iFraction ).y;
}

void
FInbetweenerBreakdown::EaseOut( float iEasing, uint32 iFrom, uint32 iTo )
{
    uint32 divisionCount = iTo - iFrom + 1;
    float fromSpacing = mChart.GetDivisionBuffer()[iFrom].spacing;
    float toSpacing = mChart.GetDivisionBuffer()[iTo].spacing;

    for( uint32 i = iFrom + 1, j = 1; j < iTo; i++, j++ )
    {
        double fraction = ( double ) j / ( divisionCount - 1 );
        double spacing = GetEaseOutSpacing( iEasing, fraction );
//UE_LOG(LogTemp, Warning, TEXT("Spacing:%f"), spacing );
        mChart.GetDivisionBuffer()[i].spacing = fromSpacing + ( spacing * ( toSpacing - fromSpacing ) );
    }

    mInbetweenerTag->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_CELLS
                               | FOdysseyVectorTagInbetweener::INVALIDATE_SPACING );
}

void
FInbetweenerBreakdown::EaseOut( float iEasing )
{
    EaseOut( fabs(iEasing)
           , mChart.GetDivisionBuffer().front().GetIndex()
           , mChart.GetDivisionBuffer().back().GetIndex() );
}

void
FInbetweenerBreakdown::EaseIn( float iEasing, uint32 iFrom, uint32 iTo )
{
    uint32 divisionCount = iTo - iFrom + 1;
    float fromSpacing = mChart.GetDivisionBuffer()[iFrom].spacing;
    float toSpacing = mChart.GetDivisionBuffer()[iTo].spacing;

    for( uint32 i = iFrom + 1, j = 1; i < iTo; i++, j++ )
    {
        double fraction = ( double ) j / ( divisionCount - 1 );
        double spacing = GetEaseInSpacing( iEasing, fraction );
//UE_LOG(LogTemp, Warning, TEXT("Spacing:%f"), spacing );
        mChart.GetDivisionBuffer()[i].spacing = fromSpacing + ( spacing * ( toSpacing - fromSpacing ) );
    }

    mInbetweenerTag->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_CELLS
                               | FOdysseyVectorTagInbetweener::INVALIDATE_SPACING );
}

void
FInbetweenerBreakdown::EaseIn( float iEasing )
{
    EaseIn( fabs(iEasing)
           , mChart.GetDivisionBuffer().front().GetIndex()
           , mChart.GetDivisionBuffer().back().GetIndex() );
}

void
FInbetweenerBreakdown::EaseInAndOut( float iEasing, FChartDivision* iInbetween )
{
    if( iEasing > 0.0f )
    {
        EaseIn ( fabs(iEasing), mChart.GetDivisionBuffer().front().GetIndex(), iInbetween->GetIndex() );
        EaseOut( fabs(iEasing), iInbetween->GetIndex(), mChart.GetDivisionBuffer().back().GetIndex() );
    }
    else
    {
        EaseOut( fabs(iEasing), mChart.GetDivisionBuffer().front().GetIndex(), iInbetween->GetIndex() );
        EaseIn ( fabs(iEasing), iInbetween->GetIndex(), mChart.GetDivisionBuffer().back().GetIndex() );
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

    drawing->translationX = mTargetTranslationX;
    drawing->translationY = mTargetTranslationY;
    drawing->rotation = mTargetRotation;
    drawing->scalingX = mTargetScalingX;
    drawing->scalingY = mTargetScalingY;

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

    mInbetweenerTag->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_SPACING
                               | FOdysseyVectorTagInbetweener::INVALIDATE_BUFFERS  );
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

IOdysseyVectorCell*
FInbetweenerBreakdown::GetSourceCell()
{
    uint32 sourceCellIndex = GetSourceCellIndex();

    return mInbetweenerTag->GetOwner()->GetEngine()->GetLayer()->GetCellByIndex( sourceCellIndex );
}

IOdysseyVectorCell*
FInbetweenerBreakdown::GetTargetCell()
{
    uint32 targetCellIndex = GetTargetCellIndex();

    return mInbetweenerTag->GetOwner()->GetEngine()->GetLayer()->GetCellByIndex( targetCellIndex );
}


int32
FInbetweenerBreakdown::GetTargetCellIndex()
{
    uint32 tagCellIndex = mInbetweenerTag->GetOwner()->GetEngine()->GetCell()->GetIndex();

    return (int32)tagCellIndex + (int32)( mTargetDrawingIndex * (int)mInbetweenerTag->GetInterpolationDirection());
}

int32
FInbetweenerBreakdown::GetSourceCellIndex()
{
    // we use GetSecene because the Scene is stored as a member varriable in order to be able to
    // redraw when the owner object is removed (its scene would be null then)
    uint32 tagCellIndex = mInbetweenerTag->GetScene()->GetEngine()->GetCell()->GetIndex();
    uint32 sourceDrawingIndex = GetSourceDrawingIndex();

    return (int32)tagCellIndex + (int32)( sourceDrawingIndex * (int)mInbetweenerTag->GetInterpolationDirection());
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
