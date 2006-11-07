// -*- C++ -*-
/*
 * Application.h
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

#include <gtkmm.h>

#include "SimulationCanvas.h"

namespace Elfelli
{

class Application
{
 public:
  Application(int argc, char **argv);
  int main();
  void quit();

  static const std::string find_datafile(const std::string& fname);

  static const std::string appname;
  static const std::string version;
  static const std::string datadir;

 private:
  bool build_gui();
  bool setup_ui_actions();
  void setup_file_chooser_dialogs();

  void reset_simulation();

  bool on_expose(GdkEventExpose *event);
  void on_add_positive_body_clicked();
  void on_add_negative_body_clicked();

  void on_about_activate();
  void on_quit_activate();
  void on_export_png_activate();

  Gtk::Main gtk_main;
  Gtk::Window main_win;
  Gtk::Statusbar sbar;

  Gtk::FileChooserDialog save_dlg;

  Glib::RefPtr<Gtk::ActionGroup> action_group;
  Glib::RefPtr<Gtk::UIManager> ui_manager;

  SimulationCanvas sim_canvas;
};

}
