// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "InbetweenerTag/InterpolatedObject.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVector.h"

FInterpolatedObject::~FInterpolatedObject()
{
}

FInterpolatedObject::FInterpolatedObject( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                        , FOdysseyVectorObject* iObject )
    : mOriginalObject( iObject )
{
    mRelativeMatrix = iInbetweenerTag->GetOwner()->GetInverseWorldMatrix();
    mRelativeMatrix.transform( iObject->GetWorldMatrix() );
}

FOdysseyVectorObject*
FInterpolatedObject::GetOriginalObject()
{
    return mOriginalObject;
}

BLMatrix2D&
FInterpolatedObject::GetRelativeMatrix()
{
    return mRelativeMatrix;
}

std::vector<FInterpolatedObject::PointGeometry>&
FInterpolatedObject::GetInterpolatedPointGeometryBuffer()
{
    return mInterpolatedPointGeometryBuffer;
}

std::vector<FInterpolatedPoint>&
FInterpolatedObject::GetInterpolatedPointBuffer()
{
    return mInterpolatedPointBuffer;
}
