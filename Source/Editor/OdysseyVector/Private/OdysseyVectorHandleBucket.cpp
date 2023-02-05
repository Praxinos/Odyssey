#include "OdysseyVectorHandleBucket.h"

UOdysseyVectorHandleBucket::~UOdysseyVectorHandleBucket()
{
}

UOdysseyVectorHandleBucket::UOdysseyVectorHandleBucket()
    : UOdysseyVectorHandle()
    , mParentBucket ( nullptr )
{

}

void
UOdysseyVectorHandleBucket::Init( FOdysseyVectorBucket* iParentBucket )
{
    mParentBucket = iParentBucket;
}

//static
UOdysseyVectorHandleBucket*
UOdysseyVectorHandleBucket::New( FOdysseyVectorBucket* iParentBucket )
{
    UOdysseyVectorHandleBucket* bucketHandle = NewObject<UOdysseyVectorHandleBucket>();

    bucketHandle->Init( iParentBucket );

    return bucketHandle;
}

FOdysseyVectorBucket*
UOdysseyVectorHandleBucket::GetParent()
{
    return mParentBucket;
}
