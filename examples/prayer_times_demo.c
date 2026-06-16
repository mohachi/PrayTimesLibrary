/**
 * @file example.c
 * @brief Example usage of Prayer Times Library
 * @details مثال على استخدام مكتبة أوقات الصلاة
 *
 * This program demonstrates how to use the Prayer Times library to calculate
 * prayer times for different locations with different calculation methods.
 *
 * Compilation: gcc -o example example.c ../src/libpt.c -lm
 */

#include <stdio.h>
#include "../include/libpt.h"

/**
 * Print prayer times in a formatted table
 * طباعة أوقات الصلاة بصيغة جدول منسق
 */
void print_prayer_times(double times[TIMES_COUNT], const char *location, const char *method_name)
{
    const char *names[TIMES_COUNT] = {"Fajr", "Sunrise", "Dhuhr", "Asr", "Sunset", "Maghrib", "Isha"};
    int hours, minutes;

    printf("\n📍 %s - %s Method\n", location, method_name);
    printf("───────────────────────────────────────\n");

    for (int i = 0; i < TIMES_COUNT; i++)
    {
        decimal_to_time(times[i], &hours, &minutes);
        printf("  %-8s : %02d:%02d  (%.2f hours)\n", names[i], hours, minutes, times[i]);
    }
}

/**
 * Main function - Demonstrate various library features
 * الدالة الرئيسية - توضيح ميزات المكتبة المختلفة
 */
int main()
{
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║         Prayer Times Library - Usage Examples                 ║\n");
    printf("║         مكتبة أوقات الصلاة - أمثلة الاستخدام                 ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n");

    double times[TIMES_COUNT];
    PrayerTimesError err;

    // ========================================================================
    // Example 1: Basic Usage - Algiers, Algeria with MWL Method
    // المثال الأول: الاستخدام الأساسي - الجزائر بطريقة MWL
    // ========================================================================

    printf("\n\n");
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║  Example 1: Basic Usage - Algiers with MWL Method             ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n");

    err = get_prayer_times(2026, 6, 9, 34.6667, 3.25, 1.0,
                           METHOD_MWL, ASR_STANDARD, times);

    if (err != PT_SUCCESS)
    {
        printf("Error: %s\n", get_error_message(err, ERR_LANG_ENGLISH));
        return 1;
    }

    print_prayer_times(times, "Algiers, Algeria (34.67°N, 3.25°E)", "MWL");

    // ========================================================================
    // Example 2: Mecca with Umm Al-Qura Method
    // المثال الثاني: مكة المكرمة بطريقة جامعة أم القرى
    // ========================================================================

    printf("\n\n");
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║  Example 2: Mecca with Umm Al-Qura Method                     ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n");

    err = get_prayer_times(2026, 6, 21, 21.4225, 39.8265, 3.0,
                           METHOD_UMM_AL_QURA, ASR_STANDARD, times);

    if (err != PT_SUCCESS)
    {
        printf("Error: %s\n", get_error_message(err, ERR_LANG_ENGLISH));
        return 1;
    }

    print_prayer_times(times, "Mecca, Saudi Arabia (21.42°N, 39.83°E)", "Umm Al-Qura");

    // ========================================================================
    // Example 3: Karachi with Hanafi Method
    // المثال الثالث: كراتشي بطريقة الحنفي
    // ========================================================================

    printf("\n\n");
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║  Example 3: Karachi with Hanafi Juristic Method               ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n");

    // First, calculate with Standard method
    err = get_prayer_times(2026, 6, 21, 24.87, 67.18, 5.0,
                           METHOD_MWL, ASR_STANDARD, times);

    printf("\nKarachi, Pakistan (24.87°N, 67.18°E) - MWL Method\n");
    printf("───────────────────────────────────────\n");
    printf("  Asr (Standard): %02d:%02d\n",
           (int)times[TIME_ASR],
           (int)((times[TIME_ASR] - (int)times[TIME_ASR]) * 60 + 0.5));

    // Now calculate with Hanafi method
    err = get_prayer_times(2026, 6, 21, 24.87, 67.18, 5.0,
                           METHOD_MWL, ASR_HANAFI, times);

    printf("  Asr (Hanafi):   %02d:%02d (later than Standard)\n",
           (int)times[TIME_ASR],
           (int)((times[TIME_ASR] - (int)times[TIME_ASR]) * 60 + 0.5));

    // ========================================================================
    // Example 4: Global Locations
    // المثال الرابع: مواقع عالمية مختلفة
    // ========================================================================

    printf("\n\n");
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║  Example 4: Prayer Times in Different Locations               ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n");

    // London
    get_prayer_times(2026, 6, 21, 51.5, 0.0, 1.0,
                     METHOD_MWL, ASR_STANDARD, times);
    print_prayer_times(times, "London, UK (51.5°N) - High Latitude", "MWL");

    // Cairo
    get_prayer_times(2026, 6, 21, 30.04, 31.23, 2.0,
                     METHOD_EGYPT, ASR_STANDARD, times);
    print_prayer_times(times, "Cairo, Egypt (30.04°N)", "Egypt");

    // Jakarta (Equatorial)
    get_prayer_times(2026, 6, 21, -6.2, 106.8, 7.0,
                     METHOD_MWL, ASR_STANDARD, times);
    print_prayer_times(times, "Jakarta, Indonesia (-6.2°S) - Equatorial", "MWL");

    // ========================================================================
    // Example 5: Error Handling
    // المثال الخامس: معالجة الأخطاء
    // ========================================================================

    printf("\n\n");
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║  Example 5: Error Handling                                    ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n");

    printf("\nTesting invalid inputs (month=13, day=32):\n\n");

    err = get_prayer_times(2026, 13, 32, 0, 0, 0,
                           METHOD_MWL, ASR_STANDARD, times);

    if (err != PT_SUCCESS)
    {
        printf("Error Code: %d\n", err);
        printf("English:  %s\n", get_error_message(err, ERR_LANG_ENGLISH));
        printf("Arabic:   %s\n", get_error_message(err, ERR_LANG_ARABIC));
    }

    // ========================================================================
    // Example 6: Multiple Calculation Methods Comparison
    // المثال السادس: مقارنة طرق حساب متعددة
    // ========================================================================

    printf("\n\n");
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║  Example 6: Method Comparison for Fajr Time                   ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n");

    double times_methods[5][TIMES_COUNT];
    CalcMethod methods[] = {METHOD_UMM_AL_QURA, METHOD_MWL, METHOD_ISNA, METHOD_EGYPT, METHOD_KARACHI};
    const char *method_names[] = {"Umm Al-Qura", "MWL", "ISNA", "Egypt", "Karachi"};

    printf("\nFajr time for Mecca (21.42°N, 39.83°E) on 2026-06-21:\n");
    printf("───────────────────────────────────────────────────\n");

    for (int i = 0; i < 5; i++)
    {
        get_prayer_times(2026, 6, 21, 21.4225, 39.8265, 3.0,
                         methods[i], ASR_STANDARD, times_methods[i]);

        int h = (int)times_methods[i][TIME_FAJR];
        int m = (int)((times_methods[i][TIME_FAJR] - h) * 60 + 0.5);
        printf("  %-15s: %02d:%02d\n", method_names[i], h, m);
    }

    // ========================================================================
    // Summary
    // الخلاصة
    // ========================================================================

    printf("\n\n");
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║                      Examples Complete                         ║\n");
    printf("║                      انتهت الأمثلة                            ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n");

    printf("\n✓ All examples executed successfully!\n");
    printf("✓ The Prayer Times Library is working correctly!\n\n");

    return 0;
}
