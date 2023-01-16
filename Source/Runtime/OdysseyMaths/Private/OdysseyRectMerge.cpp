// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyRectUtils.h"
#include "CoreMinimal.h"
#include <ULIS>

/**
 * In order
 * 
 * 1) Manage active segments update first
 * 2) Merge active segments into MergedSegments
 * 3) Close rects 
 * 4) Open rects 
 * 
 * When to close rects ?
 * 1) 0 Opened rects => should not happen, crash please
 * 2) 1+ opened rects =>
 *      - when an opened rect is not fully covered by all mergedSegments anymore
 * 
 * 
 * When to open rects ?
 * 
 * 1) 0 Opened rects => open a rect for each start segment
 * 2) 1+ opened rects 
 *      - when a mergedSegement or part of a mergedSegement is not covered by an opened rectangle
 *          - open a new rectangle for the part that is not covered 
 */

namespace OdysseyRectUtils {

struct FInputSegments
{
    TArray<int> mStartSegments;
    TArray<int> mEndSegments;
};

struct FSegment
{
    int y;
    int h;
};

TArray< ::ULIS::FRectI >
MergeRects(const TArray< ::ULIS::FRectI >& iInputRects)
{
    if ( iInputRects.Num() > 1000 )
        int a = 0;

    //store a map with key being the x coordinate and value being an array of 
    TMap<int, FInputSegments> segmentsForX;
    for(int i = 0; i < iInputRects.Num(); i++)
    {
        const ::ULIS::FRectI& inputRect = iInputRects[i];

        //register start of the inputRect
        FInputSegments& startSegments = segmentsForX.FindOrAdd(inputRect.x);
        startSegments.mStartSegments.Add(i);

        //register end of the inputRect
        FInputSegments& endSegments = segmentsForX.FindOrAdd(inputRect.x + inputRect.w);
        endSegments.mEndSegments.Add(i);
    }

    //Sort segments by X
    segmentsForX.KeySort(
        [](const int& A, const int& B)
        {
            return A < B;
        }
    );

    TArray<int> activeSegments;
    TArray<FSegment> mergedSegments;
    TArray<::ULIS::FRectI> outputRects;
    TArray<int> openedRects;

    for (auto& segmentElement : segmentsForX)
    {
        int& x = segmentElement.Key;
        FInputSegments& segments = segmentElement.Value;

        //Update active segments
        for (int& i : segments.mEndSegments)
        {
            activeSegments.Remove(i);
        }
        activeSegments.Append(segments.mStartSegments);

        //Sort active segments by Y
        activeSegments.Sort([&iInputRects](const int& A, const int& B)
        {
            const ::ULIS::FRectI& rectA = iInputRects[A];
            const ::ULIS::FRectI& rectB = iInputRects[B];
            return rectA.y < rectB.y;
        });

        //Merge active segments into mergedSegments
        mergedSegments.Empty();
        for (int& i : activeSegments)
        {
            const ::ULIS::FRectI& rect = iInputRects[i];
            if (mergedSegments.Num() <= 0)
            {
                mergedSegments.Add({rect.y, rect.h});
                continue;
            }

            FSegment& lastSegment = mergedSegments.Last();
            if (rect.y <= lastSegment.y + lastSegment.h) //x is y and y is h so it means lastSegment.y + lastSegment.h
            {
                lastSegment.h = rect.y + rect.h - lastSegment.y; //lastSegment.h = ...
                continue;
            }

            mergedSegments.Add({rect.y, rect.h});
        }

        //close rects
        for (int i = openedRects.Num() - 1; i >= 0; i--)
        {
            ::ULIS::FRectI& openedRect = outputRects[openedRects[i]];
            bool closeRect = true;
            for (const FSegment& mergedSegment : mergedSegments)
            {
                if (openedRect.y + openedRect.h > mergedSegment.y + mergedSegment.h) //x is y
                    continue; //continue until we met a potentially covering segment

                if (openedRect.y >= mergedSegment.y) //x is y and y is h
                {
                    closeRect = false; //fully contained, do not remove
                    break; 
                }

                //no need to check the next segments, as we already know it is not fully contained in a single segment
                //this works because each mergedSegment has no direct neighbour
                break;
            }

            if (closeRect)
            {
                //close rect
                openedRect.w = x - openedRect.x;
                openedRects.RemoveAt(i);
            }
        }
        
        //Sort opened rects by Y
        openedRects.Sort([&outputRects](const int& A, const int& B)
        {
            const ::ULIS::FRectI& rectA = outputRects[A];
            const ::ULIS::FRectI& rectB = outputRects[B];
            return rectA.y < rectB.y;
        });

        //Open rects
        for (const FSegment& mergedSegment : mergedSegments)
        {
            FSegment remainingSegment = mergedSegment;
            TArray<int> newlyOpenedRects;
            for (int i = 0; i < openedRects.Num(); i++)
            {
                ::ULIS::FRectI& openedRect = outputRects[openedRects[i]];

                //opened rects did not yet reached the segment 
                if (remainingSegment.y >= openedRect.y + openedRect.h ) //x is y
                    continue;

                //if remaining segment starts in the opened rect
                if (remainingSegment.y < openedRect.y ) //x is y
                {
                    int h = FMath::Min(remainingSegment.h, openedRect.y - remainingSegment.y);
                    outputRects.Add(::ULIS::FRectI::FromXYWH(x, remainingSegment.y, 0, h));
                    newlyOpenedRects.Add(outputRects.Num() - 1);
                }

                //reduce the rect
                remainingSegment.h = FMath::Max(0, (remainingSegment.y + remainingSegment.h) - (openedRect.y + openedRect.h));
                remainingSegment.y = openedRect.y + openedRect.h;

                if (remainingSegment.h <= 0) //if size is null, it has been fully covered, so break
                    break;
            }

            openedRects.Append(newlyOpenedRects);

            //Add last uncovered part of the segment
            if (remainingSegment.h > 0)
            {
                outputRects.Add(::ULIS::FRectI::FromXYWH(x, remainingSegment.y, 0, remainingSegment.h));
                openedRects.Add(outputRects.Num() - 1);
            }
        }
    }

    return outputRects;
}

}