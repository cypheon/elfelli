/*
 * Toolbox.cpp
 * Copyright (C) 2007  Johann Rudloff
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

#include "Toolbox.h"

namespace Elfelli
{

Toolbox::Toolbox(Gtk::Toolbar *toolbar)
{
  add(*toolbar);
}

void Toolbox::on_child_attached(Gtk::Widget *child)
{
  Gtk::Toolbar *tb = static_cast<Gtk::Toolbar *>(child);

  tb->set_show_arrow(true);
}

void Toolbox::on_child_detached(Gtk::Widget *child)
{
  Gtk::Toolbar *tb = static_cast<Gtk::Toolbar *>(child);

  tb->set_show_arrow(false);
}

}
