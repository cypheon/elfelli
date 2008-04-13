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

inline float sign(float f)
{
  if(f<0) return -1;
  return 1;
}

enum BodyState
  {
    BODY_STATE_NORMAL = 0,
    BODY_STATE_HIGHLIGHT,
    BODY_STATE_SELECTED,
    BODY_STATES_NUM
  };

enum DragState
  {
    DRAG_STATE_NONE = 0,
    DRAG_STATE_BODY,
    DRAG_STATE_PLATE,
    DRAG_STATE_PLATE_A,
    DRAG_STATE_PLATE_B
  };

class SimulationCanvas : public Simulation, public Canvas
{
public:
  SimulationCanvas();
  ~SimulationCanvas();

  void operator=(const Simulation& sim);

  bool has_selection();

  void refresh();
  void clear();
  bool delete_body(int n);
  bool delete_plate(int n);
  bool delete_selected();

  bool set_selected_charge(float value);
  float get_selected_charge();

  bool change_selected_charge(float delta);
  bool increase_selected_charge(bool small=false);
  bool decrease_selected_charge(bool small=false);

  sigc::signal<void> signal_selected_charge_changed();
  sigc::signal<void> signal_selection_changed();

  static const float MAX_CHARGE;
  static const float MIN_CHARGE;
  static const float CHARGE_STEP;
  static const float CHARGE_STEP_SMALL;

private:
  void draw_flux_lines();
  void draw_bodies(bool draw_selected=true);
  inline void draw_body(int n);
  void draw_plates(bool draw_selected=true);
  inline void draw_plate(int n);

  bool point_hits_body(Body& b, int x, int y);
  bool point_hits_plate_a(PlateBody& p, int x, int y);
  bool point_hits_plate_b(PlateBody& p, int x, int y);
  bool point_hits_plate(PlateBody& p, int x, int y);
  int object_at(int x, int y);

  static const char *color_names[];

  int body_radius, plate_radius;

  DragState drag_state; 
  int active;

  bool mouse_pressed;
  int mouse_over;
  Gdk::Point last_click, drag_offset;

  Glib::RefPtr<Gdk::GC> gc, gc_black, gc_white, gc_selection, gc_platebody;
  Gdk::Color colors[BODY_STATES_NUM * 2];
  Glib::RefPtr<Gdk::Pixmap> lines_pixmap;
  std::vector<Path> paths;

  sigc::signal<void> sig_selected_charge_changed;
  sigc::signal<void> sig_selection_changed;

protected:
  virtual void run();
  void plot();

  virtual void after_realize_event();
  virtual bool on_configure_event(GdkEventConfigure *event);

  virtual bool on_motion_notify_event(GdkEventMotion *event);
  virtual bool on_button_press_event(GdkEventButton *event);
  virtual bool on_button_release_event(GdkEventButton *event);
  virtual bool on_scroll_event(GdkEventScroll *event);

  virtual bool on_key_press_event(GdkEventKey *event);
};

}

#endif // _SIMULATIONCANVAS_H_
