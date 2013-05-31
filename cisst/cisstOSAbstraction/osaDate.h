
/*
  Author(s): Tae Soo Kim
  Created on: 2012-04-03

  (C) Copyright 2008-2011 Johns Hopkins University (JHU), All Rights Reserved.

*/

#ifndef osaDate_h
#define osaDate_h

#include <time.h>
#include <string>
#include "osaTimeData.h"

/*! \brief Implementation of a simple object to represent date
	\ingroup cisstOSAbstraction

	The osaDate provides an easy way to visualize and store date information
*/
class osaDate
{
public:
	/*! Default contructor*/
	osaDate();
	/*! Create an osaDate object from a given osaTimeData object 
		\param _otd osaTimeData object that will be converted to a osaDate object
	*/
	osaDate(const osaTimeData &t);
	/*!Default destructor*/
	~osaDate(){} 
	/*! Creates a std::string representation of this osaDate object.
	*/
	std::string ToString() const;

    int& GetYear(void);
    const int& GetYear(void) const;

    int& GetMonth(void);
    const int& GetMonth(void) const;
    
    int& GetDay(void);
    const int& GetDay(void) const;

    int& GetHour(void);
    const int& GetHour(void) const;
   
    int& GetMinute(void);
    const int& GetMinute(void) const;

    int& GetSecond(void);
    const int& GetSecond(void) const;
private:
protected:
	int YearMember;
	int MonthMember;
	int DayMember;
	int HourMember;
	int MinuteMember;
	int SecondMember;
};

#endif
