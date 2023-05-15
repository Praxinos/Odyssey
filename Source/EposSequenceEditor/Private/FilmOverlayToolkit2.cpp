// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "FilmOverlayToolkit2.h"

TMap<FName, TSharedPtr<IFilmOverlay>> UFilmOverlayToolkit2::PrimaryFilmOverlays;
TMap<FName, TSharedPtr<IFilmOverlay>> UFilmOverlayToolkit2::ToggleableFilmOverlays;

void UFilmOverlayToolkit2::RegisterPrimaryFilmOverlay(const FName& FilmOverlayName, TSharedPtr<IFilmOverlay> FilmOverlay)
{
    PrimaryFilmOverlays.Add(FilmOverlayName, FilmOverlay);
}

void UFilmOverlayToolkit2::UnregisterPrimaryFilmOverlay(const FName& FilmOverlayName)
{
    PrimaryFilmOverlays.Remove(FilmOverlayName);
}

const TMap<FName, TSharedPtr<IFilmOverlay>>& UFilmOverlayToolkit2::GetPrimaryFilmOverlays()
{
    return PrimaryFilmOverlays;
}

void UFilmOverlayToolkit2::RegisterToggleableFilmOverlay(const FName& FilmOverlayName, TSharedPtr<IFilmOverlay> FilmOverlay)
{
    ToggleableFilmOverlays.Add(FilmOverlayName, FilmOverlay);
}

void UFilmOverlayToolkit2::UnregisterToggleableFilmOverlay(const FName& FilmOverlayName)
{
    ToggleableFilmOverlays.Remove(FilmOverlayName);
}

const TMap<FName, TSharedPtr<IFilmOverlay>>& UFilmOverlayToolkit2::GetToggleableFilmOverlays()
{
    return ToggleableFilmOverlays;
}
