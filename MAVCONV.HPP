/*  File: MAVCONV.HPP      Updated: Tue Aug 15 15:55:02 1995
Copyright (c) Fabrizio Aversa
===========================================================*/
#ifndef MAVCONV_HPP
#define MAVCONV_HPP

void getCountryInfo(COUNTRYINFO * );
IDate _Export StringToDate(const IString &);
IString _Export DateToString(const IDate & , const Boolean , const Boolean = false);
double _Export StringToDouble(IString &);
IString _Export DoubleToString(const double, const Boolean);

#endif
