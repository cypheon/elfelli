// -*- C++ -*-
/*
 * XmlLoader.h
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

#ifndef _XML_LOADER_H_
#define _XML_LOADER_H_

#include <fstream>
#include <expat.h>

#include "Simulation.h"

namespace Elfelli
{

class XmlLoader
{
public:
  XmlLoader();
  ~XmlLoader();

  int load(const char *filename, Simulation *target);

  static const char* version_string;

private:
  static void start_element(void *data, const XML_Char *name, const XML_Char **attrs);

  XML_Parser parser;
  Simulation *sim;

  int errors;
  bool scene_started;

};

}

#endif // _XML_LOADER_H_
