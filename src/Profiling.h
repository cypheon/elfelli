// -*- C++ -*-
/*
 * Profiling.h
 * Copyright (C) 2006  Johann Rudloff
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _PROFILING_H_
#define _PROFILING_H_

#ifdef PROFILING
#include <iostream>
#include <iomanip>
#include <sys/time.h>
#include <time.h>

extern timeval start_times[1024];
extern int funclevel;
#endif // PROFILING

inline void profile_func_start(const char *fname)
{
#ifdef PROFILING
  funclevel++;

  std::cerr << std::setw(funclevel) << "" << "+" << fname << " started." << " [" << funclevel << "]" << std::endl;

  gettimeofday(&start_times[funclevel], NULL);
#endif // PROFILING
}

inline void profile_func_end(const char *fname)
{
#ifdef PROFILING
  timeval end;
  gettimeofday(&end, NULL);

  float dtime = (end.tv_usec-start_times[funclevel].tv_usec)/1000.0 + (end.tv_sec-start_times[funclevel].tv_sec)*1000.0;

  funclevel--;

  std::cerr << std::setw(funclevel) << "" << " -" << fname << "finished, time: " << dtime << "ms" << " [" << funclevel << "]" << std::endl;
  
  if(funclevel == 0)
    std::cerr << std::endl;

#endif // PROFILING
}

#endif // _PROFILING_H_
