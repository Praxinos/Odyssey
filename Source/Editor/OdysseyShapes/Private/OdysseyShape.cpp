// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyShape.h"
#include "OdysseyHUDElement.h"


UOdysseyShape::~UOdysseyShape()
{
}

bool
UOdysseyShape::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
	return false;
}

bool
UOdysseyShape::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
	return false;
}

void
UOdysseyShape::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{

}

void
UOdysseyShape::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{

}

bool
UOdysseyShape::OnKeyDown(const FKey& iKey)
{
	return false;
}

bool
UOdysseyShape::OnKeyUp(const FKey& iKey)
{
	return false;
}

void
UOdysseyShape::Tick(float iDeltaTime)
{

}

void
UOdysseyShape::ApplyOverrides(const TMap< TObjectPtr<UClass>, TObjectPtr<UObject>>& iOverrides)
{

}

void 
UOdysseyShape::SetHUD(TSharedPtr<FOdysseyHUDElement> iHUD)
{
	mHUD = iHUD;
}

bool
UOdysseyShape::IsProgressive() const
{
	return mIsProgressive;
}

void
UOdysseyShape::Abort()
{
}

TArray<FOdysseyPoint>
UOdysseyShape::GeneratePointsFromFunction(TFunction<FVector2D(float)> iFunction) const
{
    float first = 0.f;
    float last = 1.0f;

    FVector2D firstPos = iFunction(first);
    FVector2D lastPos = iFunction(last);

    TArray<FOdysseyPoint> points = { FOdysseyPoint(firstPos.X, firstPos.Y) };

    struct FCandidate
    {
        float mMin;
        float mMid;
        float mMax;
        bool mCheckMin = true;
    };

    TArray<FCandidate> candidates;
    candidates.Add(
        {
            first,
            first + (last - first) * 0.5f,
            last
        }
    );

    while(!candidates.IsEmpty())
    {
        FCandidate& candidate = candidates.Last();
        FVector2D midPos = iFunction(candidate.mMid);

        if ( candidate.mCheckMin )
        {
            candidate.mCheckMin = false;

            FVector2D minPos = iFunction(candidate.mMin);
            float dist = FVector2D::DistSquared(minPos, midPos);
            if ( dist >= 1 )
            {
                candidates.Add(
                    {
                        candidate.mMin,
                        candidate.mMin + (candidate.mMid - candidate.mMin) * 0.5f,
                        candidate.mMid
                    }
                );
                continue;
            };
        }

        points.Add(FOdysseyPoint(midPos.X, midPos.Y));
        FVector2D maxPos = iFunction(candidate.mMax);
        float dist = FVector2D::DistSquared(midPos, maxPos);
        candidates.Pop(false); //false does not allow shrinking the array, so we keep memory available

        if (dist >= 1)
        {
            candidates.Add(
                {
                    candidate.mMid,
                    candidate.mMid + (candidate.mMax - candidate.mMid) * 0.5f,
                    candidate.mMax
                }
            );
        }
    }

    points.Add(FOdysseyPoint(lastPos.X, lastPos.Y));

    return points;
}