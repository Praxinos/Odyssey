#include "OdysseyVectorGroup.h"

void
FOdysseyVectorGroup::Init( std::string iName )
{
    SetName( iName );
}

bool
FOdysseyVectorGroup::HasBaseClass( uint32 iBaseClassID )
{
    if( mStaticClass == iBaseClassID )
    {
        return true;
    }

    return FOdysseyVectorObject::HasBaseClass( iBaseClassID );
}

bool
FOdysseyVectorGroup::PickShape( ::ULIS::FRectD &iRoi, uint32 iSelectionFlags )
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

FOdysseyVectorObject*
FOdysseyVectorGroup::CopyShape()
{
    return new FOdysseyVectorGroup();
}

void
FOdysseyVectorGroup::UpdateShape( uint32 iUpdateFlags )
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

    BLPoint localP0 = mInverseWorldMatrix.mapPoint( bbox.x, bbox.y );
    BLPoint localP1 = mInverseWorldMatrix.mapVector( bbox.x + bbox.w, bbox.y + bbox.h );

    mBBox = ::ULIS::FRectD::FromMinMax( ::ULIS::FMath::Min( localP0.x, localP1.x )
                                      , ::ULIS::FMath::Min( localP0.y, localP1.y )
                                      , ::ULIS::FMath::Max( localP0.x, localP1.x )
                                      , ::ULIS::FMath::Max( localP0.y, localP1.y ) );
}
