#include "OdysseyVectorSegmentCubicGap.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorGroupPaint.h"

FOdysseyVectorSegmentCubicGap::~FOdysseyVectorSegmentCubicGap()
{

}

FOdysseyVectorSegmentCubicGap::FOdysseyVectorSegmentCubicGap( FOdysseyVectorGroupPaint* iPaintgroup
                                                            , FOdysseyVectorVertex* iPoint0
                                                            , FOdysseyVectorVertex* iPoint1 )
    : FOdysseyVectorSegmentCubic( nullptr, iPoint0, iPoint1, false )
    , mPaintgroup( iPaintgroup )
{
}

void
FOdysseyVectorSegmentCubicGap::Update()
{
    FOdysseyVectorPath* vertex0Path = static_cast<FOdysseyVectorVertex*>(mPoint[0])->GetPath();
    FOdysseyVectorPath* vertex1Path = static_cast<FOdysseyVectorVertex*>(mPoint[1])->GetPath();
    BLMatrix2D& vertex0PathWorldMatrix = vertex0Path->GetWorldMatrix();
    BLMatrix2D& vertex1PathWorldMatrix = vertex1Path->GetWorldMatrix();
    BLMatrix2D& paingroupInverseworldMatrix = mPaintgroup->GetInverseWorldMatrix();
    BLMatrix2D conversionMatrix;
    BLPoint pt; 

    // sets mBezier[4]
    FOdysseyVectorSegmentCubic::Update();

    FOdysseyVector::MatrixMultiply( paingroupInverseworldMatrix
                                  , vertex0PathWorldMatrix
                                  , conversionMatrix );

    pt = conversionMatrix.mapPoint( mBezier[0].x, mBezier[0].y );

    mBezier[0].x = pt.x;
    mBezier[0].y = pt.y;

    mBezier[1].x = pt.x;
    mBezier[1].y = pt.y;

    FOdysseyVector::MatrixMultiply( paingroupInverseworldMatrix
                                  , vertex1PathWorldMatrix
                                  , conversionMatrix );

    pt = conversionMatrix.mapPoint( mBezier[3].x, mBezier[3].y );

    mBezier[2].x = pt.x;
    mBezier[2].y = pt.y;

    mBezier[3].x = pt.x;
    mBezier[3].y = pt.y;
}
