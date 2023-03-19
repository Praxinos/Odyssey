#include "OdysseyVectorHandleBucket.h"

FOdysseyVectorHandleBucket::~FOdysseyVectorHandleBucket()
{
}

FOdysseyVectorHandleBucket::FOdysseyVectorHandleBucket( FOdysseyVectorBucket* iParentBucket )
    : FOdysseyVectorHandle()
{
    Init( iParentBucket );
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
    FOdysseyVectorHandleBucket* bucketHandle = new FOdysseyVectorHandleBucket( iParentBucket );

    return bucketHandle;
}

FOdysseyVectorBucket*
FOdysseyVectorHandleBucket::GetParent()
{
    return mParentBucket;
}
