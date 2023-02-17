#include "OdysseyVectorGroup.h"

void
UOdysseyVectorGroup::Init( std::string iName )
{
    SetName( iName );
}

bool
UOdysseyVectorGroup::PickShape( ::ULIS::FRectD &iRoi, uint32 iSelectionFlags )
{
/*
    if ( ( iX > mBBox.x ) && ( iX < ( mBBox.x + mBBox.w ) )
      && ( iY > mBBox.y ) && ( iY < ( mBBox.y + mBBox.h ) ) )
    {
        return this;
    }
*/
    return false;
}

UOdysseyVectorObject*
UOdysseyVectorGroup::CopyShape()
{
    return NewObject<UOdysseyVectorGroup>();
}

void
UOdysseyVectorGroup::UpdateShape()
{
    ::ULIS::FRectD bbox;
    int init = 0;

    for( std::list<UOdysseyVectorObject*>::iterator it = mChildrenList.begin(); it != mChildrenList.end(); ++it )
    {
        UOdysseyVectorObject *child = (*it);
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
