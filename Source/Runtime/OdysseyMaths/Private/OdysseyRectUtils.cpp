// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyRectUtils.h"

namespace OdysseyRectUtils {

TArray< ::ULIS::FRectI >
ToNonOverlappingRects(const TArray< ::ULIS::FRectI >& iRects)
{
    if( iRects.Num() == 1 )
        return iRects;

    TArray< ::ULIS::FRectI > finalRects;
    ::ULIS::TArray<::ULIS::FRectI> rectsToExclude;
    if( iRects.Num() != 0 )
    {
        for (int i = 0; i < iRects.Num()-1; i++)
        {
            rectsToExclude.PushBack(iRects[i]);

            ::ULIS::TArray<::ULIS::FRectI> exclusionRectsTemp;
            for (int k = 0; k < rectsToExclude.Size(); k++)
            {
                rectsToExclude[k].Exclusion(iRects[i+1], &exclusionRectsTemp);

                if ( exclusionRectsTemp[0] != rectsToExclude[k] )
                {
                    rectsToExclude.Erase(k);
                    int sizeOfRectsNotEmpty = 0;

                    for (int l = 0; l < exclusionRectsTemp.Size(); l++)
                    {
                        if( exclusionRectsTemp[l].Area() > 0 )
                        {
                            rectsToExclude.Insert(k + sizeOfRectsNotEmpty, exclusionRectsTemp[l]);
                            sizeOfRectsNotEmpty++;
                        }
                    }
                    k = k + sizeOfRectsNotEmpty - 1;
                }
            }
        }
        rectsToExclude.PushBack(iRects.Last());
    }

        
    for (int j = 0; j < rectsToExclude.Size(); j++)
    {
        if ( rectsToExclude[j].Area() > 0 )
        {
            finalRects.Add(rectsToExclude[j]);
        }
    }

    return finalRects;
}

}