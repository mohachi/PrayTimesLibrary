/*
 * PrayTimes C Library
 * Based on the mathematical logic by Dr. Hamid Zarrabi-Zadeh.
 * License: MIT / Public Domain (Free for closed-source commercial use).
 *
 * كتبت بلغة C من قبل: [Your Name]
 * النسخة الأصلية: JavaScript
 */

#ifndef PRAYERTIMES_H
#define PRAYERTIMES_H

/**
 * @enum PrayerTimeIndex
 * @brief فهارس أوقات الصلاة
 * @details استخدام enum بدلاً من #define لفهارس آمنة من الناحية النوعية
 */
typedef enum
{
    TIME_FAJR,    // الفجر (0)
    TIME_SUNRISE, // الشروق (1)
    TIME_DHUHR,   // الظهر (2)
    TIME_ASR,     // العصر (3)
    TIME_SUNSET,  // الغروب (4)
    TIME_MAGHRIB, // المغرب (5)
    TIME_ISHA,    // العشاء (6)
    TIMES_COUNT   // عدد الأوقات (7) - يُستخدم كحجم للمصفوفة
} PrayerTimeIndex;

/**
 * @enum CalcMethod
 * @brief طرق الحساب الفلكية المختلفة
 */
typedef enum
{
    METHOD_UMM_AL_QURA, // جامعة أم القرى (مكة) - الأكثر استخداماً في السعودية
    METHOD_MWL,         // رابطة العالم الإسلامي - Muslim World League
    METHOD_ISNA,        // الجمعية الإسلامية لأمريكا الشمالية
    METHOD_EGYPT,       // الهيئة العامة المصرية للمساحة
    METHOD_KARACHI      // جامعة العلوم الإسلامية بكراتشي
} CalcMethod;

/**
 * @enum AsrJuristicMethod
 * @brief المذاهب الفقهية لحساب وقت العصر | Islamic juristic schools for Asr calculation
 */
typedef enum
{
    ASR_STANDARD, // الشافعي، المالكي، الحنبلي (طول الظل = 1) | Shafi'i, Maliki, Hanbali (shadow length = 1)
    ASR_HANAFI    // الحنفي (طول الظل = 2) | Hanafi (shadow length = 2)
} AsrJuristicMethod;

/**
 * @enum PrayerTimesError
 * @brief أكواد الأخطاء الممكنة | Error codes
 */
typedef enum
{
    PT_SUCCESS = 0,           // العملية نجحت | Operation succeeded
    PT_ERR_INVALID_YEAR,      // سنة غير صحيحة | Invalid year
    PT_ERR_INVALID_MONTH,     // شهر غير صحيح | Invalid month
    PT_ERR_INVALID_DAY,       // يوم غير صحيح | Invalid day
    PT_ERR_INVALID_LATITUDE,  // عرض جغرافي غير صحيح | Invalid latitude
    PT_ERR_INVALID_LONGITUDE, // طول جغرافي غير صحيح | Invalid longitude
    PT_ERR_INVALID_TIMEZONE,  // منطقة زمنية غير صحيحة | Invalid timezone
    PT_ERR_NULL_OUTPUT,       // مؤشر المخرجات فارغ | NULL output pointer
    PT_ERR_POLAR_REGION       // المنطقة قطبية - قد لا توجد أوقات صحيحة | Polar region - results may be inaccurate
} PrayerTimesError;

/**
 * @enum ErrorMessageLanguage
 * @brief لغة رسائل الأخطاء | Language for error messages
 */
typedef enum
{
    ERR_LANG_ARABIC,   // اللغة العربية | Arabic
    ERR_LANG_ENGLISH   // اللغة الإنجليزية | English
} ErrorMessageLanguage;

/**
 * @brief حساب أوقات الصلاة
 *
 * @param year   السنة الميلادية (يجب أن تكون > 1900)
 * @param month  الشهر (1-12)
 * @param day    اليوم (1-31)
 * @param lat    العرض الجغرافي (-90 إلى 90)
 * @param lng    الطول الجغرافي (-180 إلى 180)
 * @param timezone الفرق الزمني عن GMT (مثلاً: 3 للتوقيت العربي الصيفي)
 * @param method طريقة الحساب (من CalcMethod)
 * @param asr_method المذهب الفقهي لحساب العصر
 * @param times_out مصفوفة بحجم TIMES_COUNT لحفظ النتائج (ساعات عشرية)
 *
 * @return كود الخطأ (PrayerTimesError)
 *
 * @example
 *   double times[TIMES_COUNT];
 *   PrayerTimesError err = get_prayer_times(2026, 6, 9, 34.6667, 3.25, 1.0,
 *                                            METHOD_MWL, ASR_STANDARD, times);
 *   if (err != PT_SUCCESS) {
 *     printf("Error: %d\n", err);
 *   }
 */
PrayerTimesError get_prayer_times(int year, int month, int day,
                                   double lat, double lng, double timezone,
                                   CalcMethod method, AsrJuristicMethod asr_method,
                                   double times_out[TIMES_COUNT]);

/**
 * @brief تحويل الوقت بصيغة عشرية إلى ساعات ودقائق | Convert decimal time to hours and minutes
 *
 * @param decimal_time الوقت بصيغة عشرية (مثلاً: 5.5 = 5:30) | Decimal time (e.g., 5.5 = 5:30)
 * @param[out] hours   الساعات | Hours
 * @param[out] minutes الدقائق | Minutes
 */
void decimal_to_time(double decimal_time, int* hours, int* minutes);

/**
 * @brief الحصول على رسالة نصية للخطأ | Get error message
 *
 * @param error    كود الخطأ | Error code (PrayerTimesError)
 * @param language لغة الرسالة | Message language (ErrorMessageLanguage)
 * @return رسالة توضيحية للخطأ | Error message
 *
 * @example
 *   const char* msg_ar = get_error_message(PT_ERR_INVALID_YEAR, ERR_LANG_ARABIC);
 *   const char* msg_en = get_error_message(PT_ERR_INVALID_YEAR, ERR_LANG_ENGLISH);
 */
const char* get_error_message(PrayerTimesError error, ErrorMessageLanguage language);

#endif // PRAYERTIMES_H
