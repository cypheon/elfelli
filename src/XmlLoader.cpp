/*
 * XmlLoader.cpp
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

#include <string>
#include <cstring>
#include <sstream>
#include <iostream>

#include "XmlLoader.h"

class TypeException{};
inline float str_to_float(const std::string& str)
{
  std::istringstream stream;
  float r;
  stream.str(str);
  stream >> r;

  if(stream.fail())
    throw TypeException();

  return r;
}

namespace Elfelli
{

const char *XmlLoader::version_string = "elfelli-xml-1";

XmlLoader::XmlLoader()
{
  parser = XML_ParserCreate(NULL);
}

XmlLoader::~XmlLoader()
{
  XML_ParserFree(parser);
}

int XmlLoader::load(const char *filename, Simulation *target)
{
  const int MAX_FILE_SIZE = 100*1024;

  XML_ParserReset(parser, NULL);
  XML_SetUserData(parser, this);
  sim = target;

  sim->reset();

  errors = 0;
  scene_started = false;

  void *buf = XML_GetBuffer(parser, MAX_FILE_SIZE);
  std::ifstream in(filename);
  if(!in)
    return 1;
  in.read(reinterpret_cast<char *>(buf), MAX_FILE_SIZE);
  in.close();

  XML_SetStartElementHandler(parser, XmlLoader::start_element);
  XML_ParseBuffer(parser, std::strlen(reinterpret_cast<char *>(buf)), 1);

  if(!scene_started)
    return 1;

  return errors;
}

void XmlLoader::start_element(void *data, const XML_Char *name, const XML_Char **attrs)
{
  XmlLoader *xml = static_cast<XmlLoader *>(data);
  
  if(xml->scene_started)
  {
    if(strcmp(name, "point") == 0)
    {
      bool have_x, have_y, have_charge;
      float x, y, charge;

      have_x=have_y=have_charge=false;

      for(int i=0; attrs[i]; i+=2)
      {
        const XML_Char *attr = attrs[i];

        try
        {
          if(!have_x && strcmp(attr, "x") == 0)
          {
            x = str_to_float(attrs[i+1]);
            have_x = true;
          }
          else if(!have_y && strcmp(attr, "y") == 0)
          {
            y = str_to_float(attrs[i+1]);
            have_y = true;
          }
          else if(!have_charge && strcmp(attr, "charge") == 0)
          {
            charge = str_to_float(attrs[i+1]);
            have_charge = true;
          }
          else
          {
            std::cerr << "warning: unexpected attribute: `" << attr << "'.\n";
          }
        }
        catch(TypeException)
        {
          std::cerr << "warning: no float value: " << attrs[i+1] << "\n";
        }
      }

      if(have_x && have_y && have_charge)
      {
        xml->sim->add_body(Vec2(x, y), charge);
#ifdef DEBUG
        std::cerr << "added body\n";
#endif // DEBUG
      }
    }
    else if(strcmp(name, "plate") == 0)
    {
      bool have_x1, have_y1, have_x2, have_y2, have_charge;
      float x1, y1, x2, y2, charge;

      have_x1=have_y1=have_x2=have_y2=have_charge=false;

      for(int i=0; attrs[i]; i+=2)
      {
        const XML_Char *attr = attrs[i];

        try
        {
          if(!have_x1 && strcmp(attr, "x1") == 0)
          {
            x1 = str_to_float(attrs[i+1]);
            have_x1 = true;
          }
          else if(!have_y1 && strcmp(attr, "y1") == 0)
          {
            y1 = str_to_float(attrs[i+1]);
            have_y1 = true;
          }
          else if(!have_x2 && strcmp(attr, "x2") == 0)
          {
            x2 = str_to_float(attrs[i+1]);
            have_x2 = true;
          }
          else if(!have_y2 && strcmp(attr, "y2") == 0)
          {
            y2 = str_to_float(attrs[i+1]);
            have_y2 = true;
          }
          else if(!have_charge && strcmp(attr, "charge") == 0)
          {
            charge = str_to_float(attrs[i+1]);
            have_charge = true;
          }
          else
          {
            std::cerr << "warning: unexpected attribute: `" << attr << "'.\n";
          }
        }
        catch(TypeException)
        {
          std::cerr << "warning: no float value: " << attrs[i+1] << "\n";
        }
      }

      if(have_x1 && have_y1 && have_x2 && have_y2 && have_charge)
      {
        xml->sim->add_plate(Vec2(x1, y1), Vec2(x2, y2), charge);
#ifdef DEBUG
        std::cerr << "added plate\n";
#endif // DEBUG
      }
    }
    else
    {
      std::cerr << "error: wrong element `" << name << "' expected `point' or `plate'.\n";
      xml->errors++;
    }
  }
  else
  {
    if(strcmp(name, "scene") == 0)
    {
      bool right_version = false;
      for(int i=0; attrs[i]; i+=2)
      {
        if(strcmp(attrs[i], "version") == 0)
        {
          if(strcmp(attrs[i+1], version_string) == 0)
          {
            right_version=true;
          }
        }
      }
      if(right_version)
      {
        xml->scene_started = true;
      }
      else
      {
        xml->errors++;
      }
    }
  }
}

}
