#include "OdysseyVectorGroup.h"

FOdysseyVectorGroup::~FOdysseyVectorGroup()
{
}

FOdysseyVectorGroup::FOdysseyVectorGroup()
    : FOdysseyVectorObject()
{
}

FOdysseyVectorObject*
FOdysseyVectorGroup::PickShape( double iX, double iY, double iRadius )
{
    if ( ( iX > mBBox.x ) && ( iX < ( mBBox.x + mBBox.w ) )
      && ( iY > mBBox.y ) && ( iY < ( mBBox.y + mBBox.h ) ) )
    {
        return this;
    }

    return nullptr;
}

FOdysseyVectorObject*
FOdysseyVectorGroup::CopyShape()
{
    return new FOdysseyVectorGroup();
}

void
FOdysseyVectorGroup::UpdateShape()
{
    ::ULIS::FRectD bbox;
    int init = 0;

    for( std::list<FOdysseyVectorObject*>::iterator it = mChildrenList.begin(); it != mChildrenList.end(); ++it )
    {
        FOdysseyVectorObject *child = (*it);
        ::ULIS::FRectD childBBox = child->GetBBox( true );

        bbox = ( init == 0 ) ? childBBox : bbox | childBBox;

        init = 1;
    }

    BLPoint localOrigin = mInverseWorldMatrix.mapPoint( bbox.x, bbox.y );
    BLPoint LocalSize = mInverseWorldMatrix.mapVector( bbox.w, bbox.h );

    mBBox.x = localOrigin.x;
    mBBox.y = localOrigin.y;
    mBBox.w = LocalSize.x;
    mBBox.h = LocalSize.y;
}
