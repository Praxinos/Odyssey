// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "CoreMinimal.h"
#include "Containers/Ticker.h"

class SNotificationItem;

/**
 * Offers the conversion of the legacy assets of the project at editor startup, through a notification that opens the
 * conversion window, see Odyssey::TiledImageMigration::Run(). Game thread.
 */
class FOdysseyTiledImageMigrationStartupOffer
{
public:
    /**
     * Makes the offer once the asset registry has finished its initial scan, when the project content holds legacy packages,
     * unless the user chose not to be asked again or the editor cannot interact with the user.
     */
    void Start();

    /** Stops waiting and closes the notification. */
    void Stop();

private:
    /** Schedules the offer. */
    void OnFilesLoaded();

    /** Shows the notification when the project content holds legacy packages. Returns false, so the ticker calls it once. */
    bool Offer(float DeltaTime);

    /** Shows the notification offering the conversion of PackageCount packages. */
    void ShowNotification(int32 PackageCount);

    /** Closes the notification, then runs the conversion workflow. */
    void OnReview();

    /** Closes the notification. */
    void CloseNotification();

private:
    /** The subscription to the end of the initial scan of the asset registry. */
    FDelegateHandle FilesLoadedHandle;

    /** The ticker making the offer. */
    FTSTicker::FDelegateHandle OfferTickerHandle;

    /** The notification offering the conversion, while shown. */
    TWeakPtr<SNotificationItem> Notification;
};
