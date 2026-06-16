#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "../include/libpt.h"

// استخدام قيمة دقيقة للـ PI
#define PI 3.14159265358979323846
#define EPSILON 0.0001

// Constants للتحقق من صحة المدخلات
#define MIN_YEAR 1900
#define MAX_YEAR 2100
#define MIN_LAT -90.0
#define MAX_LAT 90.0
#define MIN_LNG -180.0
#define MAX_LNG 180.0
#define MIN_TIMEZONE -12.0
#define MAX_TIMEZONE 14.0

/**
 * @brief تحويل درجات إلى راديان | Convert degrees to radians
 */
static double deg2rad(double d)
{
    return d * PI / 180.0;
}

/**
 * @brief تحويل راديان إلى درجات | Convert radians to degrees
 */
static double rad2deg(double r)
{
    return r * 180.0 / PI;
}

/**
 * @brief تطبيع الزاوية إلى النطاق [0, 360) | Normalize angle to [0, 360)
 */
static double fix_angle(double a)
{
    a = a - 360.0 * floor(a / 360.0);
    return a < 0 ? a + 360.0 : a;
}

/**
 * @brief تطبيع الساعة إلى النطاق [0, 24) | Normalize hour to [0, 24)
 */
static double fix_hour(double a)
{
    a = a - 24.0 * floor(a / 24.0);
    return a < 0 ? a + 24.0 : a;
}

/**
 * @brief التحقق من صحة التاريخ | Validate date values
 * @details تحقق من السنة والشهر واليوم | Checks year, month, and day validity
 */
static PrayerTimesError validate_date(int year, int month, int day)
{
    if (year < MIN_YEAR || year > MAX_YEAR)
        return PT_ERR_INVALID_YEAR;

    if (month < 1 || month > 12)
        return PT_ERR_INVALID_MONTH;

    // تحقق بسيط من صحة اليوم (لا يأخذ السنة الكبيسة في الاعتبار بالكامل)
    int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int max_day = days_in_month[month - 1];

    // السنة الكبيسة
    if (month == 2 && ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)))
        max_day = 29;

    if (day < 1 || day > max_day)
        return PT_ERR_INVALID_DAY;

    return PT_SUCCESS;
}

/**
 * @brief التحقق من صحة الإحداثيات الجغرافية | Validate geographic coordinates
 * @details تحقق من العرض والطول والمنطقة الزمنية | Validates latitude, longitude, and timezone
 */
static PrayerTimesError validate_coordinates(double lat, double lng, double timezone)
{
    if (lat < MIN_LAT || lat > MAX_LAT)
        return PT_ERR_INVALID_LATITUDE;

    if (lng < MIN_LNG || lng > MAX_LNG)
        return PT_ERR_INVALID_LONGITUDE;

    if (timezone < MIN_TIMEZONE || timezone > MAX_TIMEZONE)
        return PT_ERR_INVALID_TIMEZONE;

    return PT_SUCCESS;
}

static void sun_position(double jd, double *declination, double *eq_time)
{
    double d = jd - 2451545.0;
    double g = fix_angle(357.529 + 0.98560028 * d);
    double q = fix_angle(280.459 + 0.98564736 * d);
    double l = fix_angle(q + 1.915 * sin(deg2rad(g)) + 0.020 * sin(deg2rad(2 * g)));

    double e = 23.439 - 0.00000036 * d;
    *declination = rad2deg(asin(sin(deg2rad(e)) * sin(deg2rad(l))));

    double ra = rad2deg(atan2(cos(deg2rad(e)) * sin(deg2rad(l)), cos(deg2rad(l))));
    ra = fix_angle(ra);
    *eq_time = q / 15.0 - ra / 15.0;
}

static double equation_of_time(double jd)
{
    double dec, eq;
    sun_position(jd, &dec, &eq);
    return eq;
}

static double sun_declination(double jd)
{
    double dec, eq;
    sun_position(jd, &dec, &eq);
    return dec;
}

static double compute_mid_day(double t, double jd, double timezone, double lng)
{
    double eq = equation_of_time(jd + t);
    return fix_hour(12 - eq) - lng / 15.0 + timezone;
}

/**
 * @brief حساب الفرق الزمني لزاوية معينة (مثل الفجر أو العشاء) | Compute time for a given angle
 * @details يحسب الفرق الزمني بالساعات | Calculates time difference in hours
 * @note إذا كانت النقطة قطبية، قد تُرجع 0 بدون حساب صحيح | In polar regions, may return 0
 */
static double compute_time(double angle, double t, double jd, double lat)
{
    double dec = sun_declination(jd + t);
    double top = -sin(deg2rad(angle)) - sin(deg2rad(dec)) * sin(deg2rad(lat));
    double bottom = cos(deg2rad(dec)) * cos(deg2rad(lat));

    // تحقق من القسمة على صفر تقريباً
    if (fabs(bottom) < EPSILON)
        return 0.0;

    double v = top / bottom;

    // قد يحدث هذا في المناطق القطبية
    if (v < -1.0 || v > 1.0)
        return 0.0;

    return rad2deg(acos(v)) / 15.0;
}

static double compute_asr(int step, double t, double jd, double lat)
{
    double dec = sun_declination(jd + t);
    double angle = -rad2deg(atan(1.0 / (step + tan(deg2rad(fabs(lat - dec))))));
    return compute_time(angle, t, jd, lat);
}

static double get_julian_date(int year, int month, int day)
{
    if (month <= 2)
    {
        year -= 1;
        month += 12;
    }
    double a = floor(year / 100.0);
    double b = 2 - a + floor(a / 4.0);
    return floor(365.25 * (year + 4716)) + floor(30.6001 * (month + 1)) + day + b - 1524.5;
}

PrayerTimesError get_prayer_times(int year, int month, int day,
                                   double lat, double lng, double timezone,
                                   CalcMethod method, AsrJuristicMethod asr_method,
                                   double times_out[TIMES_COUNT])
{
    // التحقق من صحة المدخلات
    if (times_out == NULL)
        return PT_ERR_NULL_OUTPUT;

    PrayerTimesError err = validate_date(year, month, day);
    if (err != PT_SUCCESS)
        return err;

    err = validate_coordinates(lat, lng, timezone);
    if (err != PT_SUCCESS)
        return err;

    // تحقق من المناطق القطبية تقريباً
    if (fabs(lat) > 66.0)
    {
        // لا نعيد خطأ مباشرة، لكن قد تكون النتائج غير دقيقة
    }

    double fajr_angle = 18.0, isha_angle = 18.0;
    int isha_is_minutes = 0;
    double isha_minutes = 0.0;

    switch (method)
    {
    case METHOD_UMM_AL_QURA:
        fajr_angle = 18.5;
        isha_is_minutes = 1;
        isha_minutes = 90.0;
        break;
    case METHOD_MWL:
        fajr_angle = 18.0;
        isha_angle = 17.0;
        break;
    case METHOD_ISNA:
        fajr_angle = 15.0;
        isha_angle = 15.0;
        break;
    case METHOD_EGYPT:
        fajr_angle = 19.5;
        isha_angle = 17.5;
        break;
    case METHOD_KARACHI:
        fajr_angle = 18.0;
        isha_angle = 18.0;
        break;
    }

    double jd = get_julian_date(year, month, day);

    double t_fajr = 5.0 / 24.0, t_sunrise = 6.0 / 24.0, t_dhuhr = 12.0 / 24.0;
    double t_asr = 15.0 / 24.0, t_sunset = 18.0 / 24.0, t_isha = 19.0 / 24.0;

    double dhuhr = compute_mid_day(t_dhuhr, jd, timezone, lng);

    double fajr_diff = compute_time(fajr_angle, t_fajr, jd, lat);
    double sunrise_diff = compute_time(0.833, t_sunrise, jd, lat);
    double sunset_diff = compute_time(0.833, t_sunset, jd, lat);
    double isha_diff = isha_is_minutes ? 0 : compute_time(isha_angle, t_isha, jd, lat);

    int asr_factor = (asr_method == ASR_HANAFI) ? 2 : 1;
    double asr_diff = compute_asr(asr_factor, t_asr, jd, lat);

    times_out[TIME_DHUHR] = dhuhr;
    times_out[TIME_FAJR] = dhuhr - fajr_diff;
    times_out[TIME_SUNRISE] = dhuhr - sunrise_diff;
    times_out[TIME_SUNSET] = dhuhr + sunset_diff;
    times_out[TIME_MAGHRIB] = times_out[TIME_SUNSET] + 0.05;
    times_out[TIME_ASR] = dhuhr + asr_diff;

    if (isha_is_minutes)
    {
        times_out[TIME_ISHA] = times_out[TIME_MAGHRIB] + (isha_minutes / 60.0);
    }
    else
    {
        times_out[TIME_ISHA] = dhuhr + isha_diff;
    }

    return PT_SUCCESS;
}

/**
 * @brief تحويل الوقت من صيغة عشرية إلى ساعات ودقائق | Convert decimal time to hours and minutes
 * @details مثال: 5.5 يصبح 5:30 | Example: 5.5 becomes 5:30
 * @param decimal_time الوقت بصيغة عشرية | Decimal time
 * @param hours[out] المؤشر إلى متغير الساعات | Hours output
 * @param minutes[out] المؤشر إلى متغير الدقائق | Minutes output
 */
void decimal_to_time(double decimal_time, int* hours, int* minutes)
{
    if (hours == NULL || minutes == NULL)
        return;

    // تطبيع الوقت إذا كان خارج النطاق
    decimal_time = fix_hour(decimal_time);

    *hours = (int)decimal_time;
    *minutes = (int)((decimal_time - *hours) * 60 + 0.5);

    if (*minutes >= 60)
    {
        *hours += 1;
        *minutes -= 60;
        if (*hours >= 24)
            *hours -= 24;
    }
}

/**
 * @brief الحصول على رسالة نصية توضيحية للخطأ | Get error message
 *
 * @param error كود الخطأ | Error code
 * @param language لغة الرسالة (عربي أو إنجليزي) | Message language (Arabic or English)
 * @return رسالة الخطأ | Error message
 */
const char* get_error_message(PrayerTimesError error, ErrorMessageLanguage language)
{
    if (language == ERR_LANG_ARABIC)
    {
        switch (error)
        {
        case PT_SUCCESS:
            return "نجحت العملية";
        case PT_ERR_INVALID_YEAR:
            return "سنة غير صحيحة (يجب أن تكون بين 1900-2100)";
        case PT_ERR_INVALID_MONTH:
            return "شهر غير صحيح (يجب أن يكون 1-12)";
        case PT_ERR_INVALID_DAY:
            return "يوم غير صحيح للشهر المعطى";
        case PT_ERR_INVALID_LATITUDE:
            return "عرض جغرافي غير صحيح (يجب أن يكون -90 إلى 90)";
        case PT_ERR_INVALID_LONGITUDE:
            return "طول جغرافي غير صحيح (يجب أن يكون -180 إلى 180)";
        case PT_ERR_INVALID_TIMEZONE:
            return "منطقة زمنية غير صحيحة (يجب أن تكون -12 إلى 14)";
        case PT_ERR_NULL_OUTPUT:
            return "مؤشر مصفوفة المخرجات فارغ (NULL)";
        case PT_ERR_POLAR_REGION:
            return "النتائج قد تكون غير دقيقة في المناطق القطبية";
        default:
            return "خطأ غير معروف";
        }
    }
    else // ERR_LANG_ENGLISH
    {
        switch (error)
        {
        case PT_SUCCESS:
            return "Operation succeeded";
        case PT_ERR_INVALID_YEAR:
            return "Invalid year (must be between 1900-2100)";
        case PT_ERR_INVALID_MONTH:
            return "Invalid month (must be 1-12)";
        case PT_ERR_INVALID_DAY:
            return "Invalid day for the given month";
        case PT_ERR_INVALID_LATITUDE:
            return "Invalid latitude (must be -90 to 90)";
        case PT_ERR_INVALID_LONGITUDE:
            return "Invalid longitude (must be -180 to 180)";
        case PT_ERR_INVALID_TIMEZONE:
            return "Invalid timezone (must be -12 to 14)";
        case PT_ERR_NULL_OUTPUT:
            return "Output array pointer is NULL";
        case PT_ERR_POLAR_REGION:
            return "Results may be inaccurate in polar regions";
        default:
            return "Unknown error";
        }
    }
}
