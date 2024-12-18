// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyVectorSharedEnv.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorObject.h"

FOdysseyVectorSharedEnv::~FOdysseyVectorSharedEnv()
{
}

FOdysseyVectorSharedEnv::FOdysseyVectorSharedEnv()
    : FOdysseyVectorObject("SharedEnv")
{
    bIsSystem = true;
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
    // Because the Proxy can run this function at anytime, we must alos protect the access
    // to the list of shared tags
    mSharedTagMutex.lock();

    mSharedTagList.push_back( iVectorTag );

    mSharedTagMutex.unlock();
}

void
FOdysseyVectorSharedEnv::RemoveSharedTag( FOdysseyVectorTag* iVectorTag )
{
    // Because the Proxy can run this function at anytime, we must alos protect the access
    // to the list of shared tags
    mSharedTagMutex.lock();

    mSharedTagList.remove( iVectorTag );

    mSharedTagMutex.unlock();
}

std::list<FOdysseyVectorTag*>&
FOdysseyVectorSharedEnv::GetSharedTagList()
{
    return mSharedTagList;
}

const std::list<FOdysseyVectorTag*>&
FOdysseyVectorSharedEnv::GetSharedTagList() const
{
    return mSharedTagList;
}

std::mutex&
FOdysseyVectorSharedEnv::GetSharedTagMutex()
{
    return mSharedTagMutex;
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

void
FOdysseyVectorSharedEnv::GetSelectedTagByClassType( uint32 iClassType
                                                  , std::list<FOdysseyVectorTag*>& oSelectedTagList )
{
    for( FOdysseyVectorTag* tag : mSharedTagList )
    {
        if( tag->GetClass() == iClassType )
        {
            if( tag->GetOwner()->IsSelected() )
            {
                oSelectedTagList.push_back(tag);
            }
        }
    }
}

bool
FOdysseyVectorSharedEnv::HasSharedTag( FOdysseyVectorTag* iTag )
{
    return ( std::find( mSharedTagList.begin(), mSharedTagList.end(), iTag ) == mSharedTagList.end() ) ? false : true;
}
