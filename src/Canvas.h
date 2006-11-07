// -*- C++ -*-
/*
 * Canvas.h
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

#ifndef _CANVAS_H_
#define _CANVAS_H_

#include <gtkmm/drawingarea.h>

#include "Simulation.h"

namespace Elfelli
{

class Path
{
public:
  Path();
  Path(FluxLine l);
  ~Path();

  void add(const Gdk::Point& point);
  void add(int x, int y);
  void clear();
  const std::vector<Gdk::Point> get_points();

private:
  std::vector<Gdk::Point> points;

};

struct CanvasBody
{
  int num;
  float charge;
  float size;

  Gdk::Point pos;
};

class Canvas : public Gtk::DrawingArea
{
public:
  Canvas();
  ~Canvas();

  Glib::RefPtr<Gdk::Pixmap> get_pixmap(){return pixmap;};

  void save(const std::string& filename, const std::string& format);

  int get_width() const { return width; };
  int get_height() const { return height; };

private:
  int width, height;

protected:
  Glib::RefPtr<Gdk::Pixmap> pixmap;
  virtual bool on_expose_event(GdkEventExpose *event);
  virtual bool on_configure_event(GdkEventConfigure *event);

};

}

#endif // _CANVAS_H_
