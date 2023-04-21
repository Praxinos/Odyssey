#include "OdysseyVectorIntersection.h"

FOdysseyVectorIntersection::~FOdysseyVectorIntersection()
{
    delete mVertex[0];
    delete mVertex[1];
}

FOdysseyVectorIntersection::FOdysseyVectorIntersection( FOdysseyVectorVertexIntersection* iVertex0
                                                      , FOdysseyVectorVertexIntersection* iVertex1 )
    : mVertex { iVertex0, iVertex1 }
{
    iVertex0->SetIntersection( this );
    iVertex1->SetIntersection( this );
}

bool
FOdysseyVectorIntersection::HasVertex( FOdysseyVectorVertexIntersection* iVertex )
{
    return ( ( mVertex[0] == iVertex ) || ( mVertex[1] == iVertex ) );
}

FOdysseyVectorVertexIntersection*
FOdysseyVectorIntersection::GetOtherVertex( FOdysseyVectorVertexIntersection* iVertex )
{
    return ( iVertex == mVertex[0] ) ? mVertex[1] : mVertex[0];
}

FOdysseyVectorVertexIntersection*
FOdysseyVectorIntersection::GetVertex( int iNum )
{
    return mVertex[iNum];
}

void
FOdysseyVectorIntersection::BuildExplorationPairs()
{
    std::list<FOdysseyVectorSection*>& vertex0SectionList = mVertex[0]->GetSectionList();
    std::list<FOdysseyVectorSection*>& vertex1SectionList = mVertex[1]->GetSectionList();

    mExplorationPairs.reserve( vertex0SectionList.size() + vertex1SectionList.size() );

    for( std::list<FOdysseyVectorSection*>::iterator it = vertex0SectionList.begin(); it != vertex0SectionList.end(); ++it )
    {
        FOdysseyVectorSection* returnSection = static_cast<FOdysseyVectorSection*>(*it);
        ::ULIS::FVec2D returnSectionVector = -returnSection->GetVectorFromVertex( mVertex[0], false, false );
        FOdysseyVectorSection* departSection = mVertex[1]->GetCycleNextSection( returnSectionVector, 1.0f );

        mExplorationPairs.push_back( FExplorationPair( returnSection, mVertex[0], departSection ) );
    }

    for( std::list<FOdysseyVectorSection*>::iterator it = vertex1SectionList.begin(); it != vertex1SectionList.end(); ++it )
    {
        FOdysseyVectorSection* returnSection = static_cast<FOdysseyVectorSection*>(*it);
        ::ULIS::FVec2D returnSectionVector = -returnSection->GetVectorFromVertex( mVertex[1], false, false );
        FOdysseyVectorSection* departSection = mVertex[0]->GetCycleNextSection( returnSectionVector, 1.0f );

        mExplorationPairs.push_back( FExplorationPair( returnSection, mVertex[1], departSection ) );
    }
}

std::vector<FExplorationPair>&
FOdysseyVectorIntersection::GetExplorationPairs()
{
    return mExplorationPairs;
}
