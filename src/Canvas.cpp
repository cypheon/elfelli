/*
 * Canvas.cpp
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

#include "Canvas.h"
#include <iostream>

namespace Elfelli
{

Path::Path()
{
}

Path::Path(FluxLine l)
{
  for(unsigned int i=0; i < l.points.size(); ++i)
  {
    points.push_back(Gdk::Point(static_cast<int>(l.points[i].get_x()),
                                static_cast<int>(l.points[i].get_y())));
  }
}

Path::~Path()
{
}

void Path::add(const Gdk::Point& point)
{
  points.push_back(point);
}

void Path::add(int x, int y)
{
  add(Gdk::Point(x, y));
}

void Path::clear()
{
  points.clear();
}

const std::vector<Gdk::Point> Path::get_points()
{
  return points;
}



Canvas::Canvas()
{
  width = height = 0;
}

Canvas::~Canvas()
{
}

bool Canvas::on_expose_event(GdkEventExpose *event)
{
  int x, y, w, h;
  x = event->area.x;
  y = event->area.y;
  w = event->area.width;
  h = event->area.height;

  get_window()->draw_drawable(get_style()->get_fg_gc(get_state()),
                              pixmap, x, y, x, y, w, h);
  return true;
}

bool Canvas::on_configure_event(GdkEventConfigure *event)
{
  width = event->width;
  height = event->height;

  pixmap = Gdk::Pixmap::create(get_window(),
                               width, height);
  return false;
}

void Canvas::save(const std::string& filename, const std::string& format)
{
  Glib::RefPtr<Gdk::Pixbuf> pb = Gdk::Pixbuf::create(Glib::RefPtr<Gdk::Drawable>(get_pixmap()), get_pixmap()->get_colormap(),
                                                     0, 0, 0, 0, width, height);

  pb->save(filename, format);
}

}
