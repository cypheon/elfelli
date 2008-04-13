/*
 * SimulationCanvas.cpp
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

#include "SimulationCanvas.h"
#include "Profiling.h"

#include <iostream>

#include <gdk/gdkkeysyms.h>

namespace Elfelli
{

const char *SimulationCanvas::color_names[] = {
  "#0000ff",
  "#ccccff",
  "#000066",
  "#ff0000",
  "#ffcccc",
  "#660000"};

const float SimulationCanvas::MAX_CHARGE(10.0);
const float SimulationCanvas::MIN_CHARGE(1.0);
const float SimulationCanvas::CHARGE_STEP(1.0);
const float SimulationCanvas::CHARGE_STEP_SMALL(0.1);

SimulationCanvas::SimulationCanvas():
  mouse_pressed(false), body_radius(10), plate_radius(5),
  drag_state(DRAG_STATE_NONE), mouse_over(-1), active(-1)
{
  signal_realize().connect(sigc::mem_fun(*this, &SimulationCanvas::after_realize_event));
}

SimulationCanvas::~SimulationCanvas()
{
}

void SimulationCanvas::operator=(const Simulation& sim)
{
  bodies = sim.get_bodies();
  plates = sim.get_plates();

  drag_state = DRAG_STATE_NONE;
  mouse_pressed = false;
  mouse_over = -1;
  active = -1;

  sig_selection_changed.emit();
}

void SimulationCanvas::refresh()
{
  run();
  plot();
}

void SimulationCanvas::clear()
{
  bodies.clear();
  plates.clear();
  paths.clear();

  mouse_over = active = -1;
  drag_state = DRAG_STATE_NONE;

  sig_selection_changed.emit();

  refresh();
}

bool SimulationCanvas::has_selection()
{
  if(active < 0)
    return false;

  if(active < 1024)
  {
    if(active < bodies.size())
      return true;
    else
      return false;
  }
  else
  {
    if((active-1024) < plates.size())
      return true;
    else
      return false;
  }
}

bool SimulationCanvas::delete_body(int n)
{
  if(!((n < bodies.size())
       && (n >= 0)))
    {
      return false;
    }

  bodies.erase(bodies.begin() + n);

  int num = bodies.size();

  if(active == n)
    active = -1;
  if(mouse_over == n)
    mouse_over = -1;

  drag_state = DRAG_STATE_NONE;

  refresh();
}

bool SimulationCanvas::delete_plate(int n)
{
  if(!((n < plates.size())
       && (n >= 0)))
    {
      return false;
    }

  plates.erase(plates.begin() + n);

  int num = plates.size();

  if(active == (n + 1024))
    active = -1;
  if(mouse_over == (n + 1024))
    mouse_over = -1;

  drag_state = DRAG_STATE_NONE;

  refresh();
}

bool SimulationCanvas::delete_selected()
{
  bool r;
  if(active < 0)
    return false;

  if(active < 1024)
  {
    r = delete_body(active);
  }
  else
  {
    r = delete_plate(active - 1024);
  }

  active = -1;
  sig_selection_changed.emit();
  return r;
}

float SimulationCanvas::get_selected_charge()
{
  int n;

  if(active < 0)
    return 0;

  if(active < 1024)
  {
    n = active;
    if(n < bodies.size())
    {
      return fabs(bodies[n].charge);
    }
  }
  else
  {
    n = active-1024;
    if(n < plates.size())
    {
      return fabs(plates[n].charge);
    }
  }

  refresh();

  return 0;
}

bool SimulationCanvas::set_selected_charge(float value)
{
  int n;
  float delta = 0;

  if(active < 0)
    return false;

  if(active < 1024)
  {
    n = active;
    if(n < bodies.size())
    {
      delta = fabs(fabs(bodies[n].charge) - value);
      bodies[n].charge = sign(bodies[n].charge)*value;
    }
  }
  else
  {
    n = active-1024;
    if(n < plates.size())
    {
      delta = fabs(fabs(plates[n].charge) - value);
      plates[n].charge = sign(plates[n].charge)*value;
    }
  }

  if(delta > 0.01)
  {
    refresh();
    sig_selected_charge_changed.emit();
  }

  return true;
}

bool SimulationCanvas::change_selected_charge(float delta)
{
  float new_charge;

  float charge = get_selected_charge();
  new_charge = charge + sign(charge)*delta;
  if((fabs(new_charge) >= (MIN_CHARGE - 0.01))
     && (fabs(new_charge) <= MAX_CHARGE)
     && (sign(new_charge) == sign(charge)))
  {
    set_selected_charge(new_charge);
  }

  return true;
}

bool SimulationCanvas::increase_selected_charge(bool small)
{
  return change_selected_charge(small ? CHARGE_STEP_SMALL : CHARGE_STEP);
}

bool SimulationCanvas::decrease_selected_charge(bool small)
{
  return change_selected_charge(small ? -CHARGE_STEP_SMALL : -CHARGE_STEP);
}

sigc::signal<void> SimulationCanvas::signal_selected_charge_changed()
{
  return sig_selected_charge_changed;
}

sigc::signal<void> SimulationCanvas::signal_selection_changed()
{
  return sig_selection_changed;
}

void SimulationCanvas::run()
{
  profile_func_start(__PRETTY_FUNCTION__);

  Simulation::run();

  paths.clear();
  for(int i=0; i<result.size(); i++)
    {
      paths.push_back(result[i]);
    }

  draw_flux_lines();

  profile_func_end(__PRETTY_FUNCTION__);
}

void SimulationCanvas::plot()
{
  Glib::RefPtr<Gdk::Drawable> pixmap = get_pixmap();

  pixmap->draw_drawable(gc, lines_pixmap, 0, 0, 0, 0, get_width(), get_height());

  draw_plates();
  draw_bodies();

  get_window()->invalidate_rect(Gdk::Rectangle(0, 0, get_width(), get_height()), false);
}

void SimulationCanvas::draw_flux_lines()
{
  profile_func_start(__PRETTY_FUNCTION__);

  lines_pixmap->draw_rectangle(gc_white, true, 0, 0, get_width(), get_height());

  for(int i=0; i<paths.size(); i++)
    {
      const std::vector<Gdk::Point>& points = paths[i].get_points();
      lines_pixmap->draw_lines(gc_black, points);
    }

  profile_func_end(__PRETTY_FUNCTION__);
}

inline void SimulationCanvas::draw_body(int n)
{
  const Body& body = bodies[n];

  int offset = 0;
  if(body.charge > 0)
    offset = 3;

  Gdk::Color color;
  if(mouse_over == n)
    color = colors[offset + BODY_STATE_HIGHLIGHT];
  else
    color = colors[offset + BODY_STATE_NORMAL];

  gc->set_foreground(color);

  pixmap->draw_arc(gc, true,
                   static_cast<int>(body.pos.get_x() - body_radius),
                   static_cast<int>(body.pos.get_y() - body_radius),
                   body_radius*2, body_radius*2, 0, (360*64));

  pixmap->draw_arc(gc_black, false,
                   static_cast<int>(body.pos.get_x() - body_radius),
                   static_cast<int>(body.pos.get_y() - body_radius),
                   body_radius*2, body_radius*2, 0, (360*64));

  Gdk::Rectangle rect;
      
  rect.set_x(static_cast<int>(body.pos.get_x() - body_radius*2)-5);
  rect.set_y(static_cast<int>(body.pos.get_y() - body_radius*2)-5);
  rect.set_width(body_radius*4+10);
  rect.set_height(body_radius*4+10);

  get_window()->invalidate_rect(rect, false);
}

void SimulationCanvas::draw_bodies(bool draw_selected)
{
  for(int i=0; i<bodies.size(); i++)
    {
      if(active == i)
        {/*
          pixmap->draw_arc(gc_selection, false,
                           static_cast<int>(body.pos.get_x() - body_radius*2),
                           static_cast<int>(body.pos.get_y() - body_radius*2),
                           body_radius*4, body_radius*4, 0, (360*64));
          rect.set_x(static_cast<int>(body.pos.get_x() - body_radius*2)-2);
          rect.set_y(static_cast<int>(body.pos.get_y() - body_radius*2)-2);
          rect.set_width(body_radius*4+4);
          rect.set_height(body_radius*4+4);*/
      //get_window()->invalidate_rect(rect, false);
        }
      else
        {
          draw_body(i);
        }
    }
  if((active >= 0 && active < 1024) && draw_selected)
    {
      const Body& body = bodies[active];
      draw_body(active);
      pixmap->draw_arc(gc_selection, false,
                       static_cast<int>(body.pos.get_x() - body_radius*2),
                       static_cast<int>(body.pos.get_y() - body_radius*2),
                       body_radius*4, body_radius*4, 0, (360*64));

      Gdk::Rectangle rect;
      rect.set_x(static_cast<int>(body.pos.get_x() - body_radius*2)-5);
      rect.set_y(static_cast<int>(body.pos.get_y() - body_radius*2)-5);
      rect.set_width(body_radius*4+10);
      rect.set_height(body_radius*4+10);
      get_window()->invalidate_rect(rect, false);
    }
}

inline void SimulationCanvas::draw_plate(int n)
{
  const PlateBody& plate = plates[n];

  int offset = 0;
  if(plate.charge > 0)
    offset = 3;

  Gdk::Color color;
  if(mouse_over == (n+1024))
    color = colors[offset + BODY_STATE_HIGHLIGHT];
  else
    color = colors[offset + BODY_STATE_NORMAL];

  gc_platebody->set_foreground(color);

  pixmap->draw_line(gc_platebody,
                    static_cast<int>(plate.pos_a.get_x()),
                    static_cast<int>(plate.pos_a.get_y()),
                    static_cast<int>(plate.pos_b.get_x()),
                    static_cast<int>(plate.pos_b.get_y()));

  Gdk::Rectangle rect;

  if(plate.pos_a.get_x() < plate.pos_b.get_x())
  {
    rect.set_x(static_cast<int>(plate.pos_a.get_x()) - plate_radius);
    rect.set_width(static_cast<int>(plate.pos_b.get_x()) - static_cast<int>(plate.pos_a.get_x()) + 2*plate_radius);
  }
  else
  {
    rect.set_x(static_cast<int>(plate.pos_b.get_x()) - plate_radius);
    rect.set_width(static_cast<int>(plate.pos_a.get_x()) - static_cast<int>(plate.pos_b.get_x()) + 2*plate_radius);
  }
     
  if(plate.pos_a.get_y() < plate.pos_b.get_y())
  {
    rect.set_y(static_cast<int>(plate.pos_a.get_y()) - plate_radius);
    rect.set_height(static_cast<int>(plate.pos_b.get_y()) - static_cast<int>(plate.pos_a.get_y()) + 2*plate_radius);
  }
  else
  {
    rect.set_y(static_cast<int>(plate.pos_b.get_y()) - plate_radius);
    rect.set_height(static_cast<int>(plate.pos_a.get_y()) - static_cast<int>(plate.pos_b.get_y()) + 2*plate_radius);
  }

  get_window()->invalidate_rect(rect, false);
}

void SimulationCanvas::draw_plates(bool draw_selected)
{
  for(int i=0; i<plates.size(); i++)
    {
      if(active == (i+1024))
        {
        }
      else
        {
          draw_plate(i);
        }
    }

  if(active >= 1024 && draw_selected)
    {
      const PlateBody& plate = plates[active-1024];
      draw_plate(active-1024);
      Gdk::Rectangle rect;

      pixmap->draw_arc(gc_selection, true,
                       static_cast<int>(plate.pos_a.get_x() - plate_radius),
                       static_cast<int>(plate.pos_a.get_y() - plate_radius),
                       plate_radius*2, plate_radius*2, 0, (360*64));

      
      rect.set_x(static_cast<int>(plate.pos_a.get_x() - plate_radius)-5);
      rect.set_y(static_cast<int>(plate.pos_a.get_y() - plate_radius)-5);
      rect.set_width(plate_radius*2+10);
      rect.set_height(plate_radius*2+10);
      get_window()->invalidate_rect(rect, false);

      pixmap->draw_arc(gc_selection, true,
                       static_cast<int>(plate.pos_b.get_x() - plate_radius),
                       static_cast<int>(plate.pos_b.get_y() - plate_radius),
                       plate_radius*2, plate_radius*2, 0, (360*64));

      
      rect.set_x(static_cast<int>(plate.pos_b.get_x() - plate_radius)-5);
      rect.set_y(static_cast<int>(plate.pos_b.get_y() - plate_radius)-5);
      rect.set_width(plate_radius*2+10);
      rect.set_height(plate_radius*2+10);
      get_window()->invalidate_rect(rect, false);
    }
}

void SimulationCanvas::after_realize_event()
{
  add_events(Gdk::POINTER_MOTION_MASK
             |Gdk::BUTTON_PRESS_MASK
             |Gdk::BUTTON_RELEASE_MASK
             |Gdk::KEY_PRESS_MASK);

  gc = Gdk::GC::create(get_pixmap());

  gc_black = Gdk::GC::create(get_pixmap());
  gc_black->set_rgb_fg_color(Gdk::Color("black"));
  gc_black->set_rgb_bg_color(Gdk::Color("black"));

  gc_white = Gdk::GC::create(get_pixmap());
  gc_white->set_rgb_fg_color(Gdk::Color("white"));
  gc_white->set_rgb_bg_color(Gdk::Color("white"));

  gc_selection = Gdk::GC::create(get_pixmap());
  gc_selection->set_rgb_fg_color(Gdk::Color("green"));
  gc_selection->set_rgb_bg_color(Gdk::Color("black"));
  gc_selection->set_line_attributes(4, Gdk::LINE_SOLID,
                                    Gdk::CAP_ROUND, Gdk::JOIN_ROUND);

  gc_platebody = Gdk::GC::create(get_pixmap());
  gc_platebody->set_rgb_bg_color(Gdk::Color("white"));
  gc_platebody->set_line_attributes(4, Gdk::LINE_SOLID,
                                    Gdk::CAP_ROUND, Gdk::JOIN_ROUND);

  Glib::RefPtr<Gdk::Colormap> cmap = get_pixmap()->get_colormap();
  for(int i=0; i < BODY_STATES_NUM*2; i++)
    {
      colors[i] = Gdk::Color(color_names[i]);
      cmap->alloc_color(colors[i]);
    }

  get_pixmap()->draw_rectangle(gc_white, true, 0, 0, get_width(), get_height());

  set_flags(get_flags() | Gtk::CAN_FOCUS);
}

bool SimulationCanvas::on_configure_event(GdkEventConfigure *event)
{
  Canvas::on_configure_event(event);

  lines_pixmap = Gdk::Pixmap::create(get_window(),
                                     get_width(),
                                     get_height());

  if(gc_white)
    {
      draw_flux_lines();
      plot();
    }
}

bool SimulationCanvas::on_motion_notify_event(GdkEventMotion *event)
{
  int old = mouse_over;

  switch(drag_state)
  {
  case DRAG_STATE_BODY:
    {
      int x, y;
      x = static_cast<int>(bodies[active].pos.get_x()) - 2*body_radius;
      y = static_cast<int>(bodies[active].pos.get_y()) - 2*body_radius;
      
      bodies[active].pos = Vec2(event->x+drag_offset.get_x(), event->y+drag_offset.get_y());

      pixmap->draw_drawable(gc, lines_pixmap, x-5, y-5, x-5, y-5,
                            4*body_radius+10, 4*body_radius+10);

      get_window()->invalidate_rect(Gdk::Rectangle(x-5, y-5, 4*body_radius+10, 4*body_radius+10), false);
      
      draw_plates();
      draw_bodies();
      break;
    }
  case DRAG_STATE_PLATE:
  case DRAG_STATE_PLATE_A:
  case DRAG_STATE_PLATE_B:
    {
      Gdk::Rectangle rect;
      PlateBody& plate = plates[active-1024];

      if(plate.pos_a.get_x() < plate.pos_b.get_x())
      {
        rect.set_x(static_cast<int>(plate.pos_a.get_x()) - plate_radius);
        rect.set_width(static_cast<int>(plate.pos_b.get_x()) - static_cast<int>(plate.pos_a.get_x()) + 2*plate_radius);
      }
      else
      {
        rect.set_x(static_cast<int>(plate.pos_b.get_x()) - plate_radius);
        rect.set_width(static_cast<int>(plate.pos_a.get_x()) - static_cast<int>(plate.pos_b.get_x()) + 2*plate_radius);
      }
     
      if(plate.pos_a.get_y() < plate.pos_b.get_y())
      {
        rect.set_y(static_cast<int>(plate.pos_a.get_y()) - plate_radius);
        rect.set_height(static_cast<int>(plate.pos_b.get_y()) - static_cast<int>(plate.pos_a.get_y()) + 2*plate_radius);
      }
      else
      {
        rect.set_y(static_cast<int>(plate.pos_b.get_y()) - plate_radius);
        rect.set_height(static_cast<int>(plate.pos_a.get_y()) - static_cast<int>(plate.pos_b.get_y()) + 2*plate_radius);
      }

      if(drag_state == DRAG_STATE_PLATE_A)
      {
        plate.pos_a = Vec2(event->x+drag_offset.get_x(), event->y+drag_offset.get_y());
      }
      else if(drag_state == DRAG_STATE_PLATE_B)
      {
        plate.pos_b = Vec2(event->x+drag_offset.get_x(), event->y+drag_offset.get_y());
      }
      else
      {
        float sx = plate.pos_b.get_x() - plate.pos_a.get_x();
        float sy = plate.pos_b.get_y() - plate.pos_a.get_y();
        plate.pos_a = Vec2(event->x+drag_offset.get_x(), event->y+drag_offset.get_y());
        plate.pos_b = Vec2(event->x+drag_offset.get_x()+sx, event->y+drag_offset.get_y()+sy);
      }

      pixmap->draw_drawable(gc, lines_pixmap, rect.get_x(), rect.get_y(), rect.get_x(), rect.get_y(),
                            rect.get_width(), rect.get_height());
      get_window()->invalidate_rect(rect, false);
      
      draw_plates();
      draw_bodies();

      break;
    }
  default:
    {
      if(!mouse_pressed)
        {
          mouse_over = -1;

          mouse_over = object_at(static_cast<int>(event->x), static_cast<int>(event->y));

          if(old != mouse_over)
            {
              draw_plates();
              draw_bodies();
            }
        }
      else
        {
          if((abs(static_cast<int>(event->x)-last_click.get_x()) + abs(static_cast<int>(event->y)-last_click.get_y())) > 5)
            {
              int n = object_at(last_click.get_x(), last_click.get_y());
              if(n >= 0)
              {
                if(n < 1024)
                {
                  drag_state = DRAG_STATE_BODY;
                }
                else
                {
                  drag_state = DRAG_STATE_PLATE;
                  if(point_hits_plate_a(plates[n-1024], last_click.get_x(), last_click.get_y()))
                  {
                    drag_state = DRAG_STATE_PLATE_A;
                  }
                  else if(point_hits_plate_b(plates[n-1024], last_click.get_x(), last_click.get_y()))
                  {
                    drag_state = DRAG_STATE_PLATE_B;
                  }
                }
              }
            }
        }
    }
  }
}

bool SimulationCanvas::on_button_press_event(GdkEventButton *event)
{
  if(event->button == 1)
  {
    mouse_pressed = true;

    grab_focus();

    last_click = Gdk::Point(static_cast<int>(event->x),
                            static_cast<int>(event->y));

    if(mouse_over >= 0)
    {
      if(mouse_over < 1024)
      {
        drag_offset = Gdk::Point(static_cast<int>(bodies[mouse_over].pos.get_x()-event->x),
                                 static_cast<int>(bodies[mouse_over].pos.get_y()-event->y));
      }
      else
      {
        if(point_hits_plate_b(plates[mouse_over-1024], static_cast<int>(event->x), static_cast<int>(event->y)))
        {
          drag_offset = Gdk::Point(static_cast<int>(plates[mouse_over-1024].pos_b.get_x()-event->x),
                                   static_cast<int>(plates[mouse_over-1024].pos_b.get_y()-event->y));
        }
        else
        {
          drag_offset = Gdk::Point(static_cast<int>(plates[mouse_over-1024].pos_a.get_x()-event->x),
                                   static_cast<int>(plates[mouse_over-1024].pos_a.get_y()-event->y));
        }
      }
    }

    if((active >= 0) && (active < 1024))
    {
      int x, y;
      x = static_cast<int>(bodies[active].pos.get_x()) - 2*body_radius;
      y = static_cast<int>(bodies[active].pos.get_y()) - 2*body_radius;
      
      pixmap->draw_drawable(gc, lines_pixmap, x-5, y-5, x-5, y-5,
                            4*body_radius+10, 4*body_radius+10);
    }

    if(active >= 1024)
    {
      int x, y;
      x = static_cast<int>(plates[active-1024].pos_a.get_x()) - plate_radius;
      y = static_cast<int>(plates[active-1024].pos_a.get_y()) - plate_radius;

      pixmap->draw_drawable(gc, lines_pixmap, x-2, y-2, x-2, y-2,
                            2*plate_radius+4, 2*plate_radius+4);

      x = static_cast<int>(plates[active-1024].pos_b.get_x()) - plate_radius;
      y = static_cast<int>(plates[active-1024].pos_b.get_y()) - plate_radius;

      pixmap->draw_drawable(gc, lines_pixmap, x-2, y-2, x-2, y-2,
                            2*plate_radius+4, 2*plate_radius+4);
    }

    if(active != mouse_over)
    {
      active = mouse_over;
      sig_selection_changed.emit();
    }
    draw_plates();
    draw_bodies();
  }
}

bool SimulationCanvas::on_button_release_event(GdkEventButton *event)
{
  if(event->button == 1)
  {
    mouse_pressed = false;
    if(drag_state)
    {
      if(active >= 0)
      {
        switch(drag_state)
        {
        case DRAG_STATE_BODY:
          bodies[active].pos = Vec2(event->x+drag_offset.get_x(), event->y+drag_offset.get_y());
          break;
        case DRAG_STATE_PLATE_A:
        case DRAG_STATE_PLATE_B:
        default:
          break;
        }
      }
      drag_state = DRAG_STATE_NONE;
      refresh();
    }
    else
    {
      draw_plates();
      draw_bodies();
    }
  }
}

bool SimulationCanvas::on_scroll_event(GdkEventScroll *event)
{
  if(event->direction == GDK_SCROLL_UP)
  {
    increase_selected_charge();
  }
  else if(event->direction == GDK_SCROLL_DOWN)
  {
    decrease_selected_charge();
  }
}

bool SimulationCanvas::on_key_press_event(GdkEventKey *event)
{
  if(event->keyval == GDK_Delete)
  {
    delete_selected();
  }
  else if(event->keyval == GDK_Up)
  {
    increase_selected_charge(event->state & GDK_SHIFT_MASK);
  }
  else if(event->keyval == GDK_Down)
  {
    decrease_selected_charge(event->state & GDK_SHIFT_MASK);
  }
}

bool SimulationCanvas::point_hits_body(Body& b, int x, int y)
{
  int dx = static_cast<int>(b.pos.get_x()) - x;
  int dy = static_cast<int>(b.pos.get_y()) - y;
              
  if((dx*dx + dy*dy) < (body_radius*body_radius))
    return true;

  return false;
}

bool SimulationCanvas::point_hits_plate_a(PlateBody& p, int x, int y)
{
  int dx = static_cast<int>(p.pos_a.get_x()) - x;
  int dy = static_cast<int>(p.pos_a.get_y()) - y;
              
  if((dx*dx + dy*dy) < (plate_radius*plate_radius))
    return true;

  return false;
}

bool SimulationCanvas::point_hits_plate_b(PlateBody& p, int x, int y)
{
  int dx = static_cast<int>(p.pos_b.get_x()) - x;
  int dy = static_cast<int>(p.pos_b.get_y()) - y;
              
  if((dx*dx + dy*dy) < (plate_radius*plate_radius))
    return true;

  return false;
}

bool SimulationCanvas::point_hits_plate(PlateBody& p, int x, int y)
{
  float u, dx, dy;

  float length = (p.pos_b - p.pos_a).length();

  /* When the two points are too close, only point_hits_plate_a / ...plate_b is needed */
  if(length > 1)
  {
    u = ( (x-p.pos_a.get_x())*(p.pos_b.get_x()-p.pos_a.get_x())
          + (y-p.pos_a.get_y())*(p.pos_b.get_y()-p.pos_a.get_y()) )
      / (length*length);
    if((u >= 0) && (u <= 1))
    {
      dx = p.pos_a.get_x() + u*(p.pos_b.get_x()-p.pos_a.get_x()) - x;
      dy = p.pos_a.get_y() + u*(p.pos_b.get_y()-p.pos_a.get_y()) - y;

      if((dx*dx + dy*dy) < (plate_radius*plate_radius))
        return true;
    }
  }

  if(point_hits_plate_a(p, x, y) || point_hits_plate_b(p, x, y))
    return true;

  return false;
}

int SimulationCanvas::object_at(int x, int y)
{
  for(int i=bodies.size()-1; i>=0; i--)
  {
    if(point_hits_body(bodies[i], x, y))
    {
      return i;
    }
  }

  for(int i=plates.size()-1; i>=0; i--)
  {
    if(point_hits_plate(plates[i], x, y))
    {
      return (i + 1024);
    }
  }

  return -1;
}


}
