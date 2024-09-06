#include "OdysseyVectorSharedEnv.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorObject.h"

FOdysseyVectorSharedEnv::~FOdysseyVectorSharedEnv()
{
}

FOdysseyVectorSharedEnv::FOdysseyVectorSharedEnv()
    : FOdysseyVectorObject("SharedEnv")
{
}

bool
FOdysseyVectorSharedEnv::HasBaseClass( uint32 iBaseClassID )
{
    if( mStaticClass == iBaseClassID )
    {
        return true;
    }

    return FOdysseyVectorObject::HasBaseClass( iBaseClassID );
}

void
FOdysseyVectorSharedEnv::AddSharedObject( FOdysseyVectorObject* iVectorObject )
{
    mSharedObjectList.push_back( iVectorObject );
}

void
FOdysseyVectorSharedEnv::RemoveSharedObject( FOdysseyVectorObject* iVectorObject )
{
    mSharedObjectList.remove( iVectorObject );
}

void
FOdysseyVectorSharedEnv::AddSharedTag( FOdysseyVectorTag* iVectorTag )
{
    mSharedTagList.push_back( iVectorTag );
}

void
FOdysseyVectorSharedEnv::RemoveSharedTag( FOdysseyVectorTag* iVectorTag )
{
    mSharedTagList.remove( iVectorTag );
}

std::list<FOdysseyVectorTag*>&
FOdysseyVectorSharedEnv::GetSharedTagList()
{
    return mSharedTagList;
}

FOdysseyVectorTag*
FOdysseyVectorSharedEnv::GetSelectedTagByClassType( uint32 iClassType )
{
    for( FOdysseyVectorTag* tag : mSharedTagList )
    {
        if( tag->GetClass() == iClassType )
        {
            if( tag->GetOwner()->IsSelected() )
            {
                return tag;
            }
        }
    }

    return nullptr;
}

bool
FOdysseyVectorSharedEnv::HasSharedTag( FOdysseyVectorTag* iTag )
{
    return ( std::find( mSharedTagList.begin(), mSharedTagList.end(), iTag ) == mSharedTagList.end() ) ? false : true;
}
