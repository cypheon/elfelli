// -*- C++ -*-
/*
 * SimulationCanvas.h
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

#ifndef _SIMULATIONCANVAS_H_
#define _SIMULATIONCANVAS_H_

#include <vector>

#include "Simulation.h"
#include "Canvas.h"

namespace Elfelli
{

enum BodyState
  {
    BODY_STATE_NORMAL = 0,
    BODY_STATE_HIGHLIGHT,
    BODY_STATE_SELECTED,
    BODY_STATES_NUM
  };

class SimulationCanvas : public Simulation, public Canvas
{
public:
  SimulationCanvas();
  ~SimulationCanvas();

  void refresh();
  void clear();
  bool delete_body(int n);
  bool delete_selected();

private:
  void draw_flux_lines();
  void draw_bodies(bool draw_selected=true);
  inline void draw_body(int n);

  static char *color_names[];

  int body_radius;

  bool mouse_pressed, dragging;
  int mouse_over, selected;
  Gdk::Point last_click, drag_offset;

  Glib::RefPtr<Gdk::GC> gc, gc_black, gc_white, gc_selection;
  Gdk::Color colors[BODY_STATES_NUM * 2];
  Glib::RefPtr<Gdk::Pixmap> lines_pixmap;
  std::vector<Path> paths;

protected:
  virtual void run();
  void plot();

  virtual void after_realize_event();
  virtual bool on_configure_event(GdkEventConfigure *event);

  virtual bool on_motion_notify_event(GdkEventMotion *event);
  virtual bool on_button_press_event(GdkEventButton *event);
  virtual bool on_button_release_event(GdkEventButton *event);

  virtual bool on_key_press_event(GdkEventKey *event);
};

}

#endif // _SIMULATIONCANVAS_H_
