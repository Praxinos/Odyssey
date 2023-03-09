#include "OdysseyVectorHandleBucket.h"

FOdysseyVectorHandleBucket::~FOdysseyVectorHandleBucket()
{
}

FOdysseyVectorHandleBucket::FOdysseyVectorHandleBucket()
    : FOdysseyVectorHandle()
    , mParentBucket ( nullptr )
{

}

void
FOdysseyVectorHandleBucket::Init( FOdysseyVectorBucket* iParentBucket )
{
    mParentBucket = iParentBucket;
}

//static
FOdysseyVectorHandleBucket*
FOdysseyVectorHandleBucket::New( FOdysseyVectorBucket* iParentBucket )
{
    FOdysseyVectorHandleBucket* bucketHandle = new FOdysseyVectorHandleBucket();

    bucketHandle->Init( iParentBucket );

    return bucketHandle;
}

FOdysseyVectorBucket*
FOdysseyVectorHandleBucket::GetParent()
{
    return mParentBucket;
}
