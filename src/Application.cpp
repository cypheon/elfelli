/*
 * Application.cpp
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

#include "Application.h"
#include "Simulation.h"
#include "XmlLoader.h"
#include "XmlWriter.h"

using namespace Gtk;

#include <vector>
#include <cstring>
#include <fstream>
#include <iostream>

namespace Elfelli
{

const std::string Application::appname("Elfelli");
const std::string Application::version("0.2");
const std::string Application::datadir(DATADIR);

namespace Stock
{
const Gtk::StockID EXPORT_PNG("export-png");
const Gtk::StockID EXPORT_SVG("export-svg");

const Gtk::StockID ADD_NEGATIVE("add-negative");
const Gtk::StockID ADD_POSITIVE("add-positive");
const Gtk::StockID ADD_NEGATIVE_PLATE("add-negative-plate");
const Gtk::StockID ADD_POSITIVE_PLATE("add-positive-plate");
}

Application::Application(int argc, char **argv):
  gtk_main(argc, argv),
  export_png_dlg(main_win, "", FILE_CHOOSER_ACTION_SAVE),
  save_dlg(main_win, "", FILE_CHOOSER_ACTION_SAVE),
  open_dlg(main_win, "", FILE_CHOOSER_ACTION_OPEN)
{
  setup_gettext();
  build_gui();
  reset_simulation();

  if(argc >= 2)
  {
    load_file(argv[1]);
  }
}

void Application::setup_gettext()
{
  setlocale(LC_ALL, "");

  std::ifstream in("po/locale/de/LC_MESSAGES/elfelli.mo");
  if(in)
    bindtextdomain("elfelli", "po/locale");
  else
    bindtextdomain("elfelli", LOCALEDIR);
  in.close();

  textdomain("elfelli");
}

const std::string Application::find_datafile(const std::string& fname)
{
  std::vector<std::string> dirs;
  dirs.push_back(Glib::get_current_dir() + "/" + "data");
  dirs.push_back(datadir);

  std::string fullpath;
  
  for(int i=0; i< dirs.size(); i++)
    {
      fullpath = dirs[i] + "/" + fname;
#ifdef DEBUG
      std::cerr << "Checking for file `" << fullpath << "'." << std::endl;
#endif // DEBUG
      std::ifstream in(fullpath.c_str());
      if(in)
        {
          in.close();
#ifdef DEBUG
          std::cerr << "Found file `" << fname << "' in `" << fullpath << "'." << std::endl;
#endif // DEBUG
          return fullpath;
        }
      else
        {
          in.close();
        }
    }
  std::cerr << "Warning: Could not find data file `" << fname << "'." << std::endl;
  return "";
}

void Application::on_export_png_activate()
{
  std::string filename = "";

  int result = export_png_dlg.run();
  if(result == RESPONSE_OK)
    {
      filename = export_png_dlg.get_filename();
#ifdef DEBUG
      std::cerr << "Exporting PNG to file `" << filename << "'." << std::endl;
#endif // DEBUG
      sim_canvas.save(filename, "png");
    }

  export_png_dlg.hide();
}

void Application::on_open_activate()
{
  int result = open_dlg.run();
  if(result == RESPONSE_OK)
  {
    load_file(open_dlg.get_filename());
  }

  open_dlg.hide();
}

void Application::load_file(std::string filename)
{
#ifdef DEBUG
  std::cerr << "Loading file `" << filename << "'." << std::endl;
#endif // DEBUG

  XmlLoader loader;

  Simulation *tmp_sim = new Simulation;

  if(loader.load(filename.c_str(), tmp_sim) == 0)
  {
    sim_canvas = *tmp_sim;
    sim_canvas.refresh();
  }

  delete tmp_sim;
}

void Application::on_save_activate()
{
  std::string filename = "";

  while(1){
    int result = save_dlg.run();
    if(result == RESPONSE_OK)
    {
      filename = save_dlg.get_filename();
      if(save_dlg.get_filter())
      {
        if(save_dlg.get_filter()->get_name() == _("Elfelli XML (*.elfelli)"))
        {
          if(filename.find_last_of('.') == std::string::npos)
          {
            filename += ".elfelli";
          }
        }
      }

      char buf[1024];
      std::snprintf(buf, 1024, "A file named \"%s\" already exists. Do you want to overwrite it?", Glib::filename_display_basename(filename).c_str());

      MessageDialog overwrite_dlg(main_win, buf, false, MESSAGE_WARNING, BUTTONS_OK_CANCEL, true);
      if(!Glib::file_test(filename, Glib::FILE_TEST_EXISTS)
         || (overwrite_dlg.run() == RESPONSE_OK))
      {
#ifdef DEBUG
        std::cerr << "Saving file `" << filename << "'." << std::endl;
#endif // DEBUG

        XmlWriter::write(filename, &sim_canvas);
        break;
      }
    }
    else
    {
      break;
    }
  }
  save_dlg.hide();
}

void Application::on_quit_activate()
{
  quit();
}

void Application::on_add_positive_body_clicked()
{
  sim_canvas.add_body(Vec2(rand()%sim_canvas.get_width(), rand()%sim_canvas.get_height()), 4);
  sim_canvas.refresh();
}

void Application::on_add_negative_body_clicked()
{
  sim_canvas.add_body(Vec2(rand()%sim_canvas.get_width(), rand()%sim_canvas.get_height()), -4);
  sim_canvas.refresh();
}

void Application::on_add_positive_plate_clicked()
{
  Vec2 a(rand() % (sim_canvas.get_width() - 200)+100, rand() % (sim_canvas.get_height() - 200)+100);

  float angle = 2*PI*((float)rand()/RAND_MAX);
  Vec2 b(100*cos(angle), 100*sin(angle));

  sim_canvas.add_plate(a, a+b, 4);
  sim_canvas.refresh();
}

void Application::on_add_negative_plate_clicked()
{
  Vec2 a(rand() % (sim_canvas.get_width() - 200)+100, rand() % (sim_canvas.get_height() - 200)+100);

  float angle = 2*PI*((float)rand()/RAND_MAX);
  Vec2 b(100*cos(angle), 100*sin(angle));

  sim_canvas.add_plate(a, a+b, -4);
  sim_canvas.refresh();
}

void Application::on_about_activate()
{
  AboutDialog dlg;

  const char *license_parts[] = {
    N_("This program is free software; you can redistribute it and/or modify\n"
       "it under the terms of the GNU General Public License as published by\n"
       "the Free Software Foundation; either version 2 of the License, or\n"
       "at your option) any later version.\n"),
 
    N_("This program is distributed in the hope that it will be useful,\n"
       "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
       "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
       "GNU General Public License for more details.\n"),

    N_("You should have received a copy of the GNU General Public License\n"
       "along with this program; if not, write to the Free Software\n"
       "Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA\n")};

  const std::string license_text =
    std::string(_(license_parts[0])) + "\n"
    + _(license_parts[1]) + "\n"
    + _(license_parts[2]);

  dlg.set_name(appname);
  dlg.set_version(version);
  dlg.set_authors(std::vector<std::string>(1, "Johann Rudloff"));
  dlg.set_copyright("Copyright \xC2\xA9 2006 Johann Rudloff");
  dlg.set_license(license_text);
  dlg.set_logo(Gdk::Pixbuf::create_from_file(find_datafile("elfelli-logo.svg")));

  dlg.run();
}

void Application::quit()
{
  Main::quit();
}

void Application::reset_simulation()
{
  sim_canvas.clear();
  sim_canvas.refresh();
}

void Application::setup_stock_items()
{
  Glib::RefPtr<IconFactory> fact = IconFactory::create();
  IconSource src;


  Stock::add(Gtk::StockItem(Stock::EXPORT_PNG, _("Export _PNG")));
  IconSet iset_export_png;
  src.set_icon_name("gnome-mime-image-png");
  iset_export_png.add_source(src);
  fact->add(Stock::EXPORT_PNG, iset_export_png);

  Stock::add(Gtk::StockItem(Stock::EXPORT_SVG, _("Export S_VG")));
  IconSet iset_export_svg;
  src.set_icon_name("gnome-mime-image-svg");
  iset_export_svg.add_source(src);
  fact->add(Stock::EXPORT_SVG, iset_export_svg);


  Stock::add(Gtk::StockItem(Stock::ADD_NEGATIVE, _("Negative body")));
  IconSet iset_add_negative;
  src.set_icon_name("AddNegative");
  src.set_filename(find_datafile("negative.svg"));
  iset_add_negative.add_source(src);
  fact->add(Stock::ADD_NEGATIVE, iset_add_negative);

  Stock::add(Gtk::StockItem(Stock::ADD_POSITIVE, _("Positive body")));
  IconSet iset_add_positive;
  src.set_icon_name("AddPositive");
  src.set_filename(find_datafile("positive.svg"));
  iset_add_positive.add_source(src);
  fact->add(Stock::ADD_POSITIVE, iset_add_positive);

  Stock::add(Gtk::StockItem(Stock::ADD_NEGATIVE_PLATE, _("Negative plate")));
  IconSet iset_add_negative_plate;
  src.set_icon_name("AddNegativePlate");
  src.set_filename(find_datafile("negative-plate.svg"));
  iset_add_negative_plate.add_source(src);
  fact->add(Stock::ADD_NEGATIVE_PLATE, iset_add_negative_plate);

  Stock::add(Gtk::StockItem(Stock::ADD_POSITIVE_PLATE, _("Positive plate")));
  IconSet iset_add_positive_plate;
  src.set_icon_name("AddPositivePlate");
  src.set_filename(find_datafile("positive-plate.svg"));
  iset_add_positive_plate.add_source(src);
  fact->add(Stock::ADD_POSITIVE_PLATE, iset_add_positive_plate);

  fact->add_default();
}

bool Application::setup_ui_actions()
{
  action_group = ActionGroup::create();

  action_group->add( Action::create("MenuScene", _("_Scene")) );
  action_group->add( Action::create("New", Stock::NEW) , sigc::mem_fun(*this, &Application::reset_simulation));
  action_group->add( Action::create("Open", Stock::OPEN) , sigc::mem_fun(*this, &Application::on_open_activate));
  action_group->add( Action::create("SaveAs", Stock::SAVE_AS) , sigc::mem_fun(*this, &Application::on_save_activate));
  action_group->add( Action::create("ExportPNG", Stock::EXPORT_PNG) , sigc::mem_fun(*this, &Application::on_export_png_activate));
  action_group->add( Action::create("ExportSVG", Stock::EXPORT_SVG) );
  action_group->add( Action::create("Quit", Stock::QUIT) , sigc::mem_fun(*this, &Application::quit));

  action_group->add( Action::create("MenuEdit", _("E_dit")) );
  action_group->add( Action::create("Clear", Stock::CLEAR) , sigc::mem_fun(*this, &Application::reset_simulation));
  action_group->add( Action::create("AddNegative", Stock::ADD_NEGATIVE) , sigc::mem_fun(*this, &Application::on_add_negative_body_clicked));
  action_group->add( Action::create("AddPositive", Stock::ADD_POSITIVE) , sigc::mem_fun(*this, &Application::on_add_positive_body_clicked));
  action_group->add( Action::create("AddNegativePlate", Stock::ADD_NEGATIVE_PLATE) , sigc::mem_fun(*this, &Application::on_add_negative_plate_clicked));
  action_group->add( Action::create("AddPositivePlate", Stock::ADD_POSITIVE_PLATE) , sigc::mem_fun(*this, &Application::on_add_positive_plate_clicked));

  action_group->add( Action::create("MenuHelp", _("_Help")) );
  action_group->add( Action::create("About", Stock::ABOUT) , sigc::mem_fun(*this, &Application::on_about_activate));

  ui_manager = UIManager::create();
  ui_manager->insert_action_group(action_group);
  main_win.add_accel_group(ui_manager->get_accel_group());

  ui_manager->add_ui_from_file(find_datafile("ui.xml"));

  return true;
}

void Application::setup_file_chooser_dialogs()
{
  export_png_dlg.set_do_overwrite_confirmation();
  export_png_dlg.add_button(Stock::CANCEL, RESPONSE_CANCEL);
  export_png_dlg.add_button(Stock::SAVE, RESPONSE_OK);
  export_png_dlg.set_title(_("Export PNG"));


  elfelli_xml.set_name(_("Elfelli XML (*.elfelli)"));
  elfelli_xml.add_pattern("*.elfelli");

  all.set_name(_("All files"));
  all.add_pattern("*");

  save_dlg.set_title(_("Save scene"));
  save_dlg.add_button(Stock::CANCEL, RESPONSE_CANCEL);
  save_dlg.add_button(Stock::SAVE, RESPONSE_OK);
  save_dlg.add_filter(elfelli_xml);
  save_dlg.add_filter(all);


  open_dlg.add_button(Stock::CANCEL, RESPONSE_CANCEL);
  open_dlg.set_title(_("Open scene"));
  open_dlg.add_button(Stock::OPEN, RESPONSE_OK);
  open_dlg.add_filter(elfelli_xml);
  open_dlg.add_filter(all);
}

bool Application::build_gui()
{
  VBox *vbox1 = manage(new VBox);

  main_win.set_title(appname + " " + version);
  main_win.add(*vbox1);

  setup_stock_items();
  setup_ui_actions();

  setup_file_chooser_dialogs();

  vbox1->pack_start(*(ui_manager->get_widget("/MenuBar")), false, false);
  vbox1->pack_start(*(ui_manager->get_widget("/ToolBar")), false, false);

  vbox1->pack_start(sim_canvas);
  sim_canvas.set_size_request(640, 480);

  vbox1->pack_start(sbar, false, false);

  main_win.show_all();

  return true;
}

int Application::main()
{
  Main::run(main_win);

  return 0;
}

}
