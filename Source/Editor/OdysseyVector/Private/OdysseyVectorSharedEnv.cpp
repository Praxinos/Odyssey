#include "OdysseyVectorSharedEnv.h"

FOdysseyVectorSharedEnv::~FOdysseyVectorSharedEnv()
{
}

FOdysseyVectorSharedEnv::FOdysseyVectorSharedEnv()
{
}

void
FOdysseyVectorSharedEnv::AddObject( FOdysseyVectorObject* iVectorObject )
{
    mObjectList.push_back( iVectorObject );
}

void
FOdysseyVectorSharedEnv::RemoveObject( FOdysseyVectorObject* iVectorObject )
{
    mObjectList.remove( iVectorObject );
}

void
FOdysseyVectorSharedEnv::AddTag( FOdysseyVectorTag* iVectorTag )
{
    mTagList.push_back( iVectorTag );
}

void
FOdysseyVectorSharedEnv::RemoveTag( FOdysseyVectorTag* iVectorTag )
{
    mTagList.remove( iVectorTag );
}

std::list<FOdysseyVectorTag*>&
FOdysseyVectorSharedEnv::GetTagList()
{
    return mTagList;
}

bool
FOdysseyVectorSharedEnv::HasTag( FOdysseyVectorTag* iTag )
{
    return ( std::find( mTagList.begin(), mTagList.end(), iTag ) == mTagList.end() ) ? false : true;
}
