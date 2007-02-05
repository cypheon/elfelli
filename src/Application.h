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

#include <libintl.h>
#define _(String) gettext(String)
#define N_(String) String

#include "SimulationCanvas.h"

namespace Elfelli
{

namespace Stock
{
using namespace Gtk::Stock;
extern const Gtk::StockID EXPORT_PNG;
extern const Gtk::StockID EXPORT_SVG;

extern const Gtk::StockID ADD_NEGATIVE;
extern const Gtk::StockID ADD_POSITIVE;
extern const Gtk::StockID ADD_NEGATIVE_PLATE;
extern const Gtk::StockID ADD_POSITIVE_PLATE;

}

class Application
{
public:
  Application(int argc, char **argv);
  int main();
  void quit();

  void load_file(std::string filename);

  static const std::string find_datafile(const std::string& fname);

  static const std::string appname;
  static const std::string version;
  static const std::string datadir;

private:
  void setup_gettext();

  Gtk::Widget *build_object_toolbar();
  bool build_gui();
  bool setup_ui_actions();
  void setup_file_chooser_dialogs();
  void setup_stock_items();

  void reset_simulation();

  bool on_expose(GdkEventExpose *event);
  void on_remove_selected_activate();
  void on_add_positive_body_clicked();
  void on_add_negative_body_clicked();
  void on_add_positive_plate_clicked();
  void on_add_negative_plate_clicked();

  void on_about_activate();
  void on_quit_activate();
  void on_export_png_activate();
  void on_open_activate();
  void on_save_activate();

  Gtk::Main gtk_main;
  Gtk::Window main_win;
  Gtk::Statusbar sbar;

  Gtk::FileChooserDialog export_png_dlg, save_dlg, open_dlg;
  Gtk::FileFilter elfelli_xml, all;

  Glib::RefPtr<Gtk::ActionGroup> general_actions, object_actions;
  Glib::RefPtr<Gtk::UIManager> ui_manager;

  SimulationCanvas sim_canvas;
};

}
