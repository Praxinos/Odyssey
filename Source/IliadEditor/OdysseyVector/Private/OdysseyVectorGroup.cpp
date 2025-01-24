// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyVectorGroup.h"
#include "OdysseyVectorEngine.h"

FOdysseyVectorGroup::FOdysseyVectorGroup( const FString& iName )
    : FOdysseyVectorObject( iName )
{
}

void
FOdysseyVectorGroup::Init( FString& iName )
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
FOdysseyVectorGroup::PickShape( const ::ULIS::FRectD &iRoi, uint32 iSelectionFlags )
{
    if( iSelectionFlags & PICK_MATH_BASED )
    {
        BLPoint pt = mInverseWorldMatrix.mapPoint( iRoi.x, iRoi.y );

        if( mBBox.HitTest( ::ULIS::FVec2D( pt.x, pt.y ) ) )
        {
            return true;
        }
    }

    return false;
}

FOdysseyVectorObject*
FOdysseyVectorGroup::CopyShape( uint64 iCopyFlags )
{
    return new FOdysseyVectorGroup( mName );
}

void
FOdysseyVectorGroup::UpdateBBox()
{
    mBBox = ::ULIS::FRectD( 0, 0, 0, 0 );

/*
    FOdysseyVectorEngine::Traverse( this
                                  , 0
                                  , [ this ] ( FOdysseyVectorObject* object, uint64 iTraverseFlags )
    {
        ::ULIS::FRectD bbox = object->GetBBox( true, true );

        mBBox = mBBox.Area() ? ( mBBox | bbox ) : bbox;
                                                                                   // Traverse until depth 1 only
        return ( iTraverseFlags & FOdysseyVectorEngine::TRAVERSE_PARENT_HASFOCUS ) ? FOdysseyVectorEngine::TRAVERSE_OBJECT_IGNORE_CHILDREN
                                                                                   : FOdysseyVectorEngine::TRAVERSE_OBJECT_ACCEPTED;
    } );
*/
}

void
FOdysseyVectorGroup::UpdateShape( uint32 iUpdateFlags )
{

}
