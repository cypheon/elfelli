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

char *SimulationCanvas::color_names[] = {
  "#0000ff",
  "#ccccff",
  "#000066",
  "#ff0000",
  "#ffcccc",
  "#660000"};

SimulationCanvas::SimulationCanvas():
  mouse_pressed(false), body_radius(10),
  dragging(false), mouse_over(-1), selected(-1)
{
  signal_realize().connect(sigc::mem_fun(*this, &SimulationCanvas::after_realize_event));
}

SimulationCanvas::~SimulationCanvas()
{
}

void SimulationCanvas::refresh()
{
  run();
  plot();
}

void SimulationCanvas::clear()
{
  bodies.clear();
  paths.clear();

  mouse_over = selected = -1;
  dragging = false;

  refresh();
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

  if(selected == n)
    selected = -1;
  if(mouse_over == n)
    mouse_over = -1;

  dragging = false;

  refresh();
}

bool SimulationCanvas::delete_selected()
{
  bool r = delete_body(selected);
  selected = -1;
  return r;
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
      if(selected == i)
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
  if(selected >= 0 && draw_selected)
    {
      const Body& body = bodies[selected];
      draw_body(selected);
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

  if(dragging)
    {
      int x, y;
      x = static_cast<int>(bodies[selected].pos.get_x()) - 2*body_radius;
      y = static_cast<int>(bodies[selected].pos.get_y()) - 2*body_radius;
      
      bodies[selected].pos = Vec2(event->x+drag_offset.get_x(), event->y+drag_offset.get_y());

      pixmap->draw_drawable(gc, lines_pixmap, x-5, y-5, x-5, y-5,
                            4*body_radius+10, 4*body_radius+10);

      get_window()->invalidate_rect(Gdk::Rectangle(x-5, y-5, 4*body_radius+10, 4*body_radius+10), false);
      
      draw_bodies();
    }
  else
    {
      if(!mouse_pressed)
        {
          mouse_over = -1;
          
          for(int i=bodies.size()-1; i>=0; i--)
            {
              Body& body = bodies[i];
              int dx = static_cast<int>(body.pos.get_x() - event->x);
              int dy = static_cast<int>(body.pos.get_y() - event->y);
              
              if((dx*dx + dy*dy) < (body_radius*body_radius))
                {
                  mouse_over = i;
                  break;
                }
            }
          if(old != mouse_over)
            {
              draw_bodies();
            }
        }
      else
        {
          if((abs(static_cast<int>(event->x)-last_click.get_x()) + abs(static_cast<int>(event->y)-last_click.get_y())) > 5)
            {
              dragging = true;
            }
        }
    }
}

bool SimulationCanvas::on_button_press_event(GdkEventButton *event)
{
  mouse_pressed = true;

  grab_focus();

  last_click = Gdk::Point(static_cast<int>(event->x),
                          static_cast<int>(event->y));
  if(mouse_over >= 0)
    drag_offset = Gdk::Point(static_cast<int>(bodies[mouse_over].pos.get_x()-event->x),
                             static_cast<int>(bodies[mouse_over].pos.get_y()-event->y));

  if(selected >= 0)
    {
      int x, y;
      x = static_cast<int>(bodies[selected].pos.get_x()) - 2*body_radius;
      y = static_cast<int>(bodies[selected].pos.get_y()) - 2*body_radius;
      
      pixmap->draw_drawable(gc, lines_pixmap, x-5, y-5, x-5, y-5,
                            4*body_radius+10, 4*body_radius+10);
    }

  selected = mouse_over;
  draw_bodies();
}

bool SimulationCanvas::on_button_release_event(GdkEventButton *event)
{
  mouse_pressed = false;
  if(dragging)
    {
      if(selected >= 0)
        bodies[selected].pos = Vec2(event->x+drag_offset.get_x(), event->y+drag_offset.get_y());
      dragging = false;
      refresh();
    }
  else
    {
      draw_bodies();
    }
}

bool SimulationCanvas::on_key_press_event(GdkEventKey *event)
{
  if(event->keyval == GDK_Delete)
    {
      delete_selected();
    }
}


}
