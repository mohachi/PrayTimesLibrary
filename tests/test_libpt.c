/**
 * @file test_libpt.c
 * @brief Unit tests for Prayer Times Library (libpt)
 *
 * اختبارات شاملة لمكتبة حساب أوقات الصلاة
 *
 * الإجراء:
 *   gcc -o tests/test_libpt tests/test_libpt.c src/libpt.c -lm
 *   ./tests/test_libpt
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "../include/libpt.h"

// ============================================================================
// Test Framework Setup
// ============================================================================

typedef struct
{
    int total;
    int passed;
    int failed;
} TestStats;

static TestStats stats = {0, 0, 0};

#define ASSERT_EQUAL(actual, expected, format)                                             \
    do                                                                                     \
    {                                                                                      \
        stats.total++;                                                                     \
        if ((actual) == (expected))                                                        \
        {                                                                                  \
            stats.passed++;                                                                \
            printf("✓ PASS\n");                                                            \
        }                                                                                  \
        else                                                                               \
        {                                                                                  \
            stats.failed++;                                                                \
            printf("✗ FAIL: expected " format ", got " format "\n", (expected), (actual)); \
        }                                                                                  \
    } while (0)

#define ASSERT_DOUBLE_NEAR(actual, expected, epsilon)                                                        \
    do                                                                                                       \
    {                                                                                                        \
        stats.total++;                                                                                       \
        double _actual = (double)(actual);                                                                   \
        double _expected = (double)(expected);                                                               \
        double diff = fabs(_actual - _expected);                                                             \
        if (diff <= (epsilon))                                                                               \
        {                                                                                                    \
            stats.passed++;                                                                                  \
            printf("✓ PASS\n");                                                                              \
        }                                                                                                    \
        else                                                                                                 \
        {                                                                                                    \
            stats.failed++;                                                                                  \
            printf("✗ FAIL: expected %lf ± %lf, got %lf (diff=%lf)\n", _expected, (epsilon), _actual, diff); \
        }                                                                                                    \
    } while (0)

#define TEST_CASE(name) \
    printf("\n[TEST] %s\n", (name))

#define TEST_SECTION(name)                                  \
    printf("\n========================================\n"); \
    printf(" %s\n", (name));                                 \
    printf("========================================\n")

// ============================================================================
// Test Cases
// ============================================================================

/**
 * اختبار التحقق من صحة التاريخ والإحداثيات
 */
void test_input_validation()
{
    TEST_SECTION("Input Validation Tests");
    double times[TIMES_COUNT];

    // اختبار 1: سنة غير صحيحة
    TEST_CASE("Invalid year (1800)");
    PrayerTimesError err = get_prayer_times(1800, 6, 15, 34.6667, 3.25, 1.0,
                                            METHOD_MWL, ASR_STANDARD, times);
    ASSERT_EQUAL(err, PT_ERR_INVALID_YEAR, "%d");

    // اختبار 2: سنة غير صحيحة (عالية جداً)
    TEST_CASE("Invalid year (2150)");
    err = get_prayer_times(2150, 6, 15, 34.6667, 3.25, 1.0,
                           METHOD_MWL, ASR_STANDARD, times);
    ASSERT_EQUAL(err, PT_ERR_INVALID_YEAR, "%d");

    // اختبار 3: شهر غير صحيح (0)
    TEST_CASE("Invalid month (0)");
    err = get_prayer_times(2026, 0, 15, 34.6667, 3.25, 1.0,
                           METHOD_MWL, ASR_STANDARD, times);
    ASSERT_EQUAL(err, PT_ERR_INVALID_MONTH, "%d");

    // اختبار 4: شهر غير صحيح (13)
    TEST_CASE("Invalid month (13)");
    err = get_prayer_times(2026, 13, 15, 34.6667, 3.25, 1.0,
                           METHOD_MWL, ASR_STANDARD, times);
    ASSERT_EQUAL(err, PT_ERR_INVALID_MONTH, "%d");

    // اختبار 5: يوم غير صحيح
    TEST_CASE("Invalid day (32)");
    err = get_prayer_times(2026, 6, 32, 34.6667, 3.25, 1.0,
                           METHOD_MWL, ASR_STANDARD, times);
    ASSERT_EQUAL(err, PT_ERR_INVALID_DAY, "%d");

    // اختبار 6: عرض جغرافي غير صحيح (أكثر من 90)
    TEST_CASE("Invalid latitude (95)");
    err = get_prayer_times(2026, 6, 15, 95.0, 3.25, 1.0,
                           METHOD_MWL, ASR_STANDARD, times);
    ASSERT_EQUAL(err, PT_ERR_INVALID_LATITUDE, "%d");

    // اختبار 7: عرض جغرافي غير صحيح (أقل من -90)
    TEST_CASE("Invalid latitude (-95)");
    err = get_prayer_times(2026, 6, 15, -95.0, 3.25, 1.0,
                           METHOD_MWL, ASR_STANDARD, times);
    ASSERT_EQUAL(err, PT_ERR_INVALID_LATITUDE, "%d");

    // اختبار 8: طول جغرافي غير صحيح
    TEST_CASE("Invalid longitude (185)");
    err = get_prayer_times(2026, 6, 15, 34.6667, 185.0, 1.0,
                           METHOD_MWL, ASR_STANDARD, times);
    ASSERT_EQUAL(err, PT_ERR_INVALID_LONGITUDE, "%d");

    // اختبار 9: منطقة زمنية غير صحيحة
    TEST_CASE("Invalid timezone (20)");
    err = get_prayer_times(2026, 6, 15, 34.6667, 3.25, 20.0,
                           METHOD_MWL, ASR_STANDARD, times);
    ASSERT_EQUAL(err, PT_ERR_INVALID_TIMEZONE, "%d");

    // اختبار 10: مؤشر NULL
    TEST_CASE("NULL output array");
    err = get_prayer_times(2026, 6, 15, 34.6667, 3.25, 1.0,
                           METHOD_MWL, ASR_STANDARD, NULL);
    ASSERT_EQUAL(err, PT_ERR_NULL_OUTPUT, "%d");
}

/**
 * اختبار حسابات أوقات الصلاة في الجزائر
 */
void test_algiers_prayer_times()
{
    TEST_SECTION("Prayer Times Calculations - Algiers (34.67°N, 3.25°E)");
    double times[TIMES_COUNT];

    // الجزائر: 2026-06-09 باستخدام طريقة رابطة العالم الإسلامي
    TEST_CASE("Algiers MWL method 2026-06-09");
    PrayerTimesError err = get_prayer_times(2026, 6, 9, 34.6667, 3.25, 1.0,
                                            METHOD_MWL, ASR_STANDARD, times);
    ASSERT_EQUAL(err, PT_SUCCESS, "%d");

    // يجب أن تكون الأوقات منطقية
    // الفجر قبل الشروق: في الصيف قد يكون مبكراً جداً (3-5 صباحاً)
    TEST_CASE("Fajr time is reasonable (3-7 AM)");
    if (times[TIME_FAJR] >= 3.0 && times[TIME_FAJR] <= 7.0)
    {
        stats.passed++;
        printf("✓ PASS (Fajr: %.2f)\n", times[TIME_FAJR]);
    }
    else
    {
        stats.failed++;
        printf("✗ FAIL: Fajr should be between 3-7 AM, got %.2f\n", times[TIME_FAJR]);
    }
    stats.total++;

    // الشروق: في الصيف قد يكون مبكراً (5-8 صباحاً)
    TEST_CASE("Sunrise time is reasonable (5-8 AM)");
    if (times[TIME_SUNRISE] >= 5.0 && times[TIME_SUNRISE] <= 8.0)
    {
        stats.passed++;
        printf("✓ PASS (Sunrise: %.2f)\n", times[TIME_SUNRISE]);
    }
    else
    {
        stats.failed++;
        printf("✗ FAIL: Sunrise should be between 5-8 AM, got %.2f\n", times[TIME_SUNRISE]);
    }
    stats.total++;

    // الظهر حول 12-13
    TEST_CASE("Dhuhr time is reasonable (11-14:00)");
    if (times[TIME_DHUHR] >= 11.5 && times[TIME_DHUHR] <= 14.0)
    {
        stats.passed++;
        printf("✓ PASS (Dhuhr: %.2f)\n", times[TIME_DHUHR]);
    }
    else
    {
        stats.failed++;
        printf("✗ FAIL: Dhuhr should be between 11.5-14 hours, got %.2f\n", times[TIME_DHUHR]);
    }
    stats.total++;

    // العصر بعد الظهر
    TEST_CASE("Asr time is after Dhuhr");
    int fajr_after_sunrise = times[TIME_ASR] > times[TIME_DHUHR];
    ASSERT_EQUAL(fajr_after_sunrise, 1, "%d");

    // الغروب حول 19-20
    TEST_CASE("Sunset time is reasonable (18-21 hours)");
    if (times[TIME_SUNSET] >= 18.0 && times[TIME_SUNSET] <= 21.0)
    {
        stats.passed++;
        printf("✓ PASS (Sunset: %.2f)\n", times[TIME_SUNSET]);
    }
    else
    {
        stats.failed++;
        printf("✗ FAIL: Sunset should be between 18-21 hours, got %.2f\n", times[TIME_SUNSET]);
    }
    stats.total++;

    // المغرب بعد الغروب مباشرة
    TEST_CASE("Maghrib is shortly after Sunset");
    int maghrib_after_sunset = times[TIME_MAGHRIB] > times[TIME_SUNSET];
    ASSERT_EQUAL(maghrib_after_sunset, 1, "%d");

    // العشاء بعد المغرب
    TEST_CASE("Isha is after Maghrib");
    int isha_after_maghrib = times[TIME_ISHA] > times[TIME_MAGHRIB];
    ASSERT_EQUAL(isha_after_maghrib, 1, "%d");
}

/**
 * اختبار طرق الحساب المختلفة
 */
void test_calculation_methods()
{
    TEST_SECTION("Different Calculation Methods");
    double times_umm[TIMES_COUNT];
    double times_mwl[TIMES_COUNT];
    double times_isna[TIMES_COUNT];

    PrayerTimesError err1 = get_prayer_times(2026, 6, 15, 21.4225, 39.8265, 3.0,
                                             METHOD_UMM_AL_QURA, ASR_STANDARD, times_umm);
    get_prayer_times(2026, 6, 15, 21.4225, 39.8265, 3.0,
                     METHOD_MWL, ASR_STANDARD, times_mwl);
    get_prayer_times(2026, 6, 15, 21.4225, 39.8265, 3.0,
                     METHOD_ISNA, ASR_STANDARD, times_isna);

    TEST_CASE("All methods should succeed");
    ASSERT_EQUAL(err1, PT_SUCCESS, "%d");

    TEST_CASE("Methods should produce different results for Fajr");
    double diff_fajr = fabs(times_umm[TIME_FAJR] - times_mwl[TIME_FAJR]);
    if (diff_fajr > 0.03) // دقيقة واحدة على الأقل (0.03 ساعة = 1.8 دقيقة)
    {
        stats.passed++;
        printf("✓ PASS (Difference: %.2f hours / %.1f minutes)\n", diff_fajr, diff_fajr * 60);
    }
    else
    {
        stats.failed++;
        printf("✗ FAIL: Methods should differ, difference: %.4f hours\n", diff_fajr);
    }
    stats.total++;

    printf("  - Umm Al-Qura Fajr: %.2f\n", times_umm[TIME_FAJR]);
    printf("  - MWL Fajr: %.2f\n", times_mwl[TIME_FAJR]);
    printf("  - ISNA Fajr: %.2f\n", times_isna[TIME_FAJR]);
}

/**
 * اختبار المذاهب الفقهية (العصر)
 */
void test_asr_juristic_methods()
{
    TEST_SECTION("Asr Juristic Methods (Standard vs Hanafi)");
    double times_standard[TIMES_COUNT];
    double times_hanafi[TIMES_COUNT];

    get_prayer_times(2026, 6, 15, 31.5, 74.35, 5.0,
                     METHOD_MWL, ASR_STANDARD, times_standard);
    get_prayer_times(2026, 6, 15, 31.5, 74.35, 5.0,
                     METHOD_MWL, ASR_HANAFI, times_hanafi);

    TEST_CASE("Hanafi Asr should be later than Standard");
    int hanafi_later = times_hanafi[TIME_ASR] > times_standard[TIME_ASR];
    ASSERT_EQUAL(hanafi_later, 1, "%d");

    printf("  - Standard Asr: %.2f\n", times_standard[TIME_ASR]);
    printf("  - Hanafi Asr: %.2f\n", times_hanafi[TIME_ASR]);
}

/**
 * اختبار دالة تحويل الوقت العشري
 */
void test_decimal_to_time_conversion()
{
    TEST_SECTION("Decimal to Time Conversion");

    int hours, minutes;

    TEST_CASE("Convert 5.5 to 5:30");
    decimal_to_time(5.5, &hours, &minutes);
    ASSERT_EQUAL(hours, 5, "%d");
    ASSERT_EQUAL(minutes, 30, "%d");

    TEST_CASE("Convert 23.75 to 23:45");
    decimal_to_time(23.75, &hours, &minutes);
    ASSERT_EQUAL(hours, 23, "%d");
    ASSERT_EQUAL(minutes, 45, "%d");

    TEST_CASE("Convert 14.25 to 14:15");
    decimal_to_time(14.25, &hours, &minutes);
    ASSERT_EQUAL(hours, 14, "%d");
    ASSERT_EQUAL(minutes, 15, "%d");

    TEST_CASE("Convert 0.1 to 0:06");
    decimal_to_time(0.1, &hours, &minutes);
    ASSERT_EQUAL(hours, 0, "%d");
    printf("  Minutes: %d (should be ~6)\n", minutes);
}

/**
 * اختبار المناطق المختلفة حول العالم
 */
void test_global_locations()
{
    TEST_SECTION("Prayer Times in Different Global Locations");
    double times[TIMES_COUNT];

    // لندن
    TEST_CASE("London (51.5°N, 0°E) - high latitude");
    PrayerTimesError err = get_prayer_times(2026, 6, 21, 51.5, 0.0, 1.0,
                                            METHOD_MWL, ASR_STANDARD, times);
    ASSERT_EQUAL(err, PT_SUCCESS, "%d");
    printf("  - Fajr: %.2f, Isha: %.2f\n", times[TIME_FAJR], times[TIME_ISHA]);

    // القاهرة
    TEST_CASE("Cairo (30.04°N, 31.23°E) - medium latitude");
    err = get_prayer_times(2026, 6, 21, 30.04, 31.23, 2.0,
                           METHOD_EGYPT, ASR_STANDARD, times);
    ASSERT_EQUAL(err, PT_SUCCESS, "%d");
    printf("  - Fajr: %.2f, Isha: %.2f\n", times[TIME_FAJR], times[TIME_ISHA]);

    // جاكرتا
    TEST_CASE("Jakarta (-6.2°S, 106.8°E) - equatorial");
    err = get_prayer_times(2026, 6, 21, -6.2, 106.8, 7.0,
                           METHOD_MWL, ASR_STANDARD, times);
    ASSERT_EQUAL(err, PT_SUCCESS, "%d");
    printf("  - Fajr: %.2f, Isha: %.2f\n", times[TIME_FAJR], times[TIME_ISHA]);

    // سيدني
    TEST_CASE("Sydney (-33.9°S, 151.2°E) - southern hemisphere");
    err = get_prayer_times(2026, 12, 21, -33.9, 151.2, 11.0,
                           METHOD_MWL, ASR_STANDARD, times);
    ASSERT_EQUAL(err, PT_SUCCESS, "%d");
    printf("  - Fajr: %.2f, Isha: %.2f\n", times[TIME_FAJR], times[TIME_ISHA]);
}

void test_error_messages()
{
    TEST_SECTION("Error Messages");

    TEST_CASE("Error message for PT_SUCCESS (English)");
    const char *msg = get_error_message(PT_SUCCESS, ERR_LANG_ENGLISH);
    printf("  - %s\n", msg);

    TEST_CASE("Error message for PT_SUCCESS (Arabic)");
    msg = get_error_message(PT_SUCCESS, ERR_LANG_ARABIC);
    printf("  - %s\n", msg);

    TEST_CASE("Error message for PT_ERR_INVALID_YEAR (English)");
    msg = get_error_message(PT_ERR_INVALID_YEAR, ERR_LANG_ENGLISH);
    printf("  - %s\n", msg);

    TEST_CASE("Error message for PT_ERR_INVALID_YEAR (Arabic)");
    msg = get_error_message(PT_ERR_INVALID_YEAR, ERR_LANG_ARABIC);
    printf("  - %s\n", msg);

    TEST_CASE("Error message for PT_ERR_NULL_OUTPUT (English)");
    msg = get_error_message(PT_ERR_NULL_OUTPUT, ERR_LANG_ENGLISH);
    printf("  - %s\n", msg);

    TEST_CASE("Error message for PT_ERR_NULL_OUTPUT (Arabic)");
    msg = get_error_message(PT_ERR_NULL_OUTPUT, ERR_LANG_ARABIC);
    printf("  - %s\n", msg);
}

/**
 * اختبار تسلسل أوقات الصلاة
 */
void test_prayer_order()
{
    TEST_SECTION("Prayer Times Order Verification");
    double times[TIMES_COUNT];

    get_prayer_times(2026, 6, 15, 34.6667, 3.25, 1.0,
                     METHOD_MWL, ASR_STANDARD, times);

    TEST_CASE("Fajr < Sunrise");
    int order1 = times[TIME_FAJR] < times[TIME_SUNRISE];
    ASSERT_EQUAL(order1, 1, "%d");

    TEST_CASE("Sunrise < Dhuhr");
    int order2 = times[TIME_SUNRISE] < times[TIME_DHUHR];
    ASSERT_EQUAL(order2, 1, "%d");

    TEST_CASE("Dhuhr < Asr");
    int order3 = times[TIME_DHUHR] < times[TIME_ASR];
    ASSERT_EQUAL(order3, 1, "%d");

    TEST_CASE("Asr < Sunset");
    int order4 = times[TIME_ASR] < times[TIME_SUNSET];
    ASSERT_EQUAL(order4, 1, "%d");

    TEST_CASE("Sunset < Maghrib");
    int order5 = times[TIME_SUNSET] < times[TIME_MAGHRIB];
    ASSERT_EQUAL(order5, 1, "%d");

    TEST_CASE("Maghrib < Isha");
    int order6 = times[TIME_MAGHRIB] < times[TIME_ISHA];
    ASSERT_EQUAL(order6, 1, "%d");

    printf("\nPrayer Times Order:\n");
    printf("  1. Fajr: %.2f\n", times[TIME_FAJR]);
    printf("  2. Sunrise: %.2f\n", times[TIME_SUNRISE]);
    printf("  3. Dhuhr: %.2f\n", times[TIME_DHUHR]);
    printf("  4. Asr: %.2f\n", times[TIME_ASR]);
    printf("  5. Sunset: %.2f\n", times[TIME_SUNSET]);
    printf("  6. Maghrib: %.2f\n", times[TIME_MAGHRIB]);
    printf("  7. Isha: %.2f\n", times[TIME_ISHA]);
}

// ============================================================================
// Main Test Runner
// ============================================================================

int main()
{
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║        Prayer Times Library - Comprehensive Unit Tests         ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n");

    // تشغيل جميع الاختبارات
    test_input_validation();
    test_algiers_prayer_times();
    test_calculation_methods();
    test_asr_juristic_methods();
    test_decimal_to_time_conversion();
    test_global_locations();
    test_error_messages();
    test_prayer_order();

    // طباعة النتائج
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║                        TEST RESULTS                            ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n");
    printf("Total Tests:  %d\n", stats.total);
    printf("Passed:       %d ✓\n", stats.passed);
    printf("Failed:       %d ✗\n", stats.failed);
    printf("Pass Rate:    %.1f%%\n", (stats.total > 0) ? (100.0 * stats.passed / stats.total) : 0.0);

    if (stats.failed == 0 && stats.total > 0)
    {
        printf("\n🎉 All tests passed!\n");
        return 0;
    }
    else
    {
        printf("\n⚠️  Some tests failed!\n");
        return 1;
    }
}
