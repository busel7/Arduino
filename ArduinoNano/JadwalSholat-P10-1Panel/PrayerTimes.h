

#ifndef Morse_h
#define Morse_h

#include "Arduino.h"
#include <math.h>
#include <string.h>
#include <WString.h>
#include <stdio.h>

       
	static const int NUM_ITERATIONS = 1;		// number of iterations needed to compute times
	
	/* ------------------------------------------------------------------------------------------------- */
	static const char* TimeName[] =
	{
		"Subuh",
		"Sunrise",
		"Dhuhur",
		"Ashar",
		"Sunset",
		"Maghrib",
		"Isya",
		"TimesCount"
	};
	// Calculation Methods
	enum CalculationMethod
	{
		Jafari, 	// Ithna Ashari
		Karachi,	// University of Islamic Sciences, Karachi
		ISNA,   	// Islamic Society of North America (ISNA)
		MWL,    	// Muslim World League (MWL)
		Makkah, 	// Umm al-Qura, Makkah
		Egypt,  	// Egyptian General Authority of Survey
		Custom, 	// Custom Setting

		CalculationMethodsCount
	};

	// Juristic Methods
	enum JuristicMethod
	{
		Shafii,    // Shafii (standard)
		Hanafi,    // Hanafi
	};

	// Adjusting Methods for Higher Latitudes
	enum AdjustingMethod
	{
		None,      	// No adjustment
		MidNight,  	// middle of night
		OneSeventh,	// 1/7th of night
		AngleBased,	// angle/60th of night
	};

	// Time IDs
	enum TimeID
	{
		Subuh,
		Sunrise,
		Dhuhur,
		Ashar,
		Sunset,
		Maghrib,
		Isya,
		TimesCount
	};
    
	struct MethodConfig
	{
		MethodConfig()
		{
		}

		MethodConfig(double fajr_angle,
				bool maghrib_is_minutes,
				double maghrib_value,
				bool isha_is_minutes,
				double isha_value)
		: fajr_angle(fajr_angle)
		, maghrib_is_minutes(maghrib_is_minutes)
		, maghrib_value(maghrib_value)
		, isha_is_minutes(isha_is_minutes)
		, isha_value(isha_value)
		{
		}

		double fajr_angle;
		bool   maghrib_is_minutes;
		double maghrib_value;		// angle or minutes
		bool   isha_is_minutes;
		double isha_value;		// angle or minutes
	};       
	
	struct DoublePair {
	  double  first;
	  double  second;
	};
        	

	void compute_times(double times[]);
	void adjust_times(double times[]);
	void day_portion(double times[]);
	void set_calc_method(CalculationMethod method_id);
	void adjust_high_lat_times(double times[]);
	void set_asr_method(JuristicMethod method_id);
	void set_high_lats_adjust_method(AdjustingMethod method_id);
	void set_calc_method(CalculationMethod method_id);
	void set_fajr_angle(double angle);
	void set_maghrib_angle(double angle);
	void set_isha_angle(double angle);
	
	double compute_time(double g, double t);    
	double sun_declination(double jd);
	double compute_mid_day(double _t);
	double equation_of_time(double jd);        
	double night_portion(double angle);
	
	static double fix_angle(double a);
	static double deg2rad(double d);
	static double rad2deg(double r);
	static String int_to_string(int num);
	
	void get_prayer_times(int year, int month, int day, double _latitude, double _longitude, double _timezone, double times[]);
	void get_float_time_parts(double time, int& hours, int& minutes);

#endif	
