/*
 * XmlWriter.cpp
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

#include <vector>
#include <fstream>

#include "XmlWriter.h"

namespace Elfelli
{

namespace XmlWriter
{

const char *version_string = "elfelli-xml-1";

bool write(const std::string& filename, const Simulation *sim)
{
  std::ofstream out(filename.c_str(), std::ios::out);

  const std::vector<Body>& bodies = sim->get_bodies();
  const std::vector<PlateBody>& plates = sim->get_plates();

  out << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
  out << "<scene version=\"" << version_string << "\">\n";

  std::vector<Body>::const_iterator b_iter;
  for(b_iter = bodies.begin(); b_iter != bodies.end(); b_iter++)
  {
    out << "  <point x=\"" << (*b_iter).pos.get_x() << "\" "
        << "y=\"" << (*b_iter).pos.get_y() << "\" "
        << "charge=\"" << (*b_iter).charge << "\" />\n";
  }

  std::vector<PlateBody>::const_iterator p_iter;
  for(p_iter = plates.begin(); p_iter != plates.end(); p_iter++)
  {
    out << "  <plate x1=\"" << (*p_iter).pos_a.get_x() << "\" "
        << "y1=\"" << (*p_iter).pos_a.get_y() << "\" "
        << "x2=\"" << (*p_iter).pos_b.get_x() << "\" "
        << "y2=\"" << (*p_iter).pos_b.get_y() << "\" "
        << "charge=\"" << (*p_iter).charge << "\" />\n";
  }

  out << "</scene>\n";
  out.close();
  return true;
}

}

}
