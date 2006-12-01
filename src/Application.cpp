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

using namespace Gtk;

#include <vector>
#include <fstream>
#include <iostream>

namespace Elfelli
{

const std::string Application::appname("Elfelli");
const std::string Application::version("0.2");
const std::string Application::datadir(DATADIR);

Application::Application(int argc, char **argv):
  gtk_main(argc, argv),
  save_dlg(main_win, "", FILE_CHOOSER_ACTION_SAVE)
{
  setup_gettext();
  build_gui();
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
  dirs.push_back("data");
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

  save_dlg.set_title(_("Export PNG"));

  int result = save_dlg.run();
  if(result == RESPONSE_OK)
    {
      filename = save_dlg.get_filename();
#ifdef DEBUG
      std::cout << "Exporting PNG to file `" << filename << "'." << std::endl;
#endif // DEBUG
      sim_canvas.save(filename, "png");
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

bool Application::setup_ui_actions()
{
  action_group = ActionGroup::create();

  action_group->add( Action::create("MenuScene", _("_Scene")) );
  action_group->add( Action::create("New", Stock::NEW) , sigc::mem_fun(*this, &Application::reset_simulation));
  action_group->add( Action::create("Open", Stock::OPEN) );
  action_group->add( Action::create("SaveAs", Stock::SAVE_AS) );
  action_group->add( Action::create("ExportPNG", _("Export _PNG")) , sigc::mem_fun(*this, &Application::on_export_png_activate));
  action_group->add( Action::create("ExportSVG", _("Export S_VG")) );
  action_group->add( Action::create("Quit", Stock::QUIT) , sigc::mem_fun(*this, &Application::quit));

  action_group->add( Action::create("MenuEdit", _("E_dit")) );
  action_group->add( Action::create("Clear", Stock::CLEAR) , sigc::mem_fun(*this, &Application::reset_simulation));
  action_group->add( Action::create("AddNegative", _("Negative body")) , sigc::mem_fun(*this, &Application::on_add_negative_body_clicked));
  action_group->add( Action::create("AddPositive", _("Positive body")) , sigc::mem_fun(*this, &Application::on_add_positive_body_clicked));
  action_group->add( Action::create("AddNegativePlate", _("Negative plate")) , sigc::mem_fun(*this, &Application::on_add_negative_plate_clicked));
  action_group->add( Action::create("AddPositivePlate", _("Positive plate")) , sigc::mem_fun(*this, &Application::on_add_positive_plate_clicked));

  action_group->add( Action::create("MenuHelp", _("_Help")) );
  action_group->add( Action::create("About", Stock::ABOUT) , sigc::mem_fun(*this, &Application::on_about_activate));

  ui_manager = UIManager::create();
  ui_manager->insert_action_group(action_group);
  main_win.add_accel_group(ui_manager->get_accel_group());

  ui_manager->add_ui_from_file(find_datafile("ui.xml"));

  int w, h;
  IconSize::lookup(dynamic_cast<Toolbar *>(ui_manager->get_widget("/ToolBar"))->get_icon_size(), w, h);

  Image *img;
  Glib::RefPtr<Gdk::Pixbuf> pixbuf;

  if (ui_manager->get_widget("/MenuBar/MenuScene/ExportPNG"))
    {
      img = manage(new Image);
      img->set_from_icon_name("gnome-mime-image-png", ICON_SIZE_MENU);
      dynamic_cast<ImageMenuItem *>(ui_manager->get_widget("/MenuBar/MenuScene/ExportPNG"))->set_image(*img);
    }

  if (ui_manager->get_widget("/MenuBar/MenuScene/ExportSVG"))
    {
      img = manage(new Image);
      img->set_from_icon_name("gnome-mime-image-svg", ICON_SIZE_MENU);
      dynamic_cast<ImageMenuItem *>(ui_manager->get_widget("/MenuBar/MenuScene/ExportSVG"))->set_image(*img);
    }


  if (ui_manager->get_widget("/ToolBar/AddNegative"))
    {
      pixbuf = Gdk::Pixbuf::create_from_file(find_datafile("negative.svg"), w, h);
      img = manage(new Image(pixbuf));
      img->show();
      dynamic_cast<ToolButton *>(ui_manager->get_widget("/ToolBar/AddNegative"))->set_icon_widget(*dynamic_cast<Widget *>(img));
    }

  if (ui_manager->get_widget("/ToolBar/AddPositive"))
    {
      pixbuf = Gdk::Pixbuf::create_from_file(find_datafile("positive.svg"), w, h);
      img = manage(new Image(pixbuf));
      img->show();
      dynamic_cast<ToolButton *>(ui_manager->get_widget("/ToolBar/AddPositive"))->set_icon_widget(*dynamic_cast<Widget *>(img));
    }

  if (ui_manager->get_widget("/ToolBar/AddNegativePlate"))
    {
      pixbuf = Gdk::Pixbuf::create_from_file(find_datafile("negative-plate.svg"), w, h);
      img = manage(new Image(pixbuf));
      img->show();
      dynamic_cast<ToolButton *>(ui_manager->get_widget("/ToolBar/AddNegativePlate"))->set_icon_widget(*dynamic_cast<Widget *>(img));
    }

  if (ui_manager->get_widget("/ToolBar/AddPositivePlate"))
    {
      pixbuf = Gdk::Pixbuf::create_from_file(find_datafile("positive-plate.svg"), w, h);
      img = manage(new Image(pixbuf));
      img->show();
      dynamic_cast<ToolButton *>(ui_manager->get_widget("/ToolBar/AddPositivePlate"))->set_icon_widget(*dynamic_cast<Widget *>(img));
    }

  return true;
}

void Application::setup_file_chooser_dialogs()
{
  save_dlg.set_do_overwrite_confirmation();
  save_dlg.add_button(Stock::CANCEL, RESPONSE_CANCEL);
  save_dlg.add_button(Stock::SAVE, RESPONSE_OK);
}

bool Application::build_gui()
{
  VBox *vbox1 = manage(new VBox);
  HBox *hbox1 = manage(new HBox);

  main_win.set_title(appname + " " + version);
  main_win.add(*vbox1);

  setup_ui_actions();

  setup_file_chooser_dialogs();

  vbox1->pack_start(*(ui_manager->get_widget("/MenuBar")), false, false);
  vbox1->pack_start(*(ui_manager->get_widget("/ToolBar")), false, false);

  vbox1->pack_start(sim_canvas);
  sim_canvas.set_size_request(640, 480);

  //vbox1->pack_start(*hbox1, false, false);

  Button *button = manage(new Button("Neuer positiver Körper"));
  hbox1->pack_end(*button, false, false);
  button->signal_clicked().connect(sigc::mem_fun(*this, &Application::on_add_positive_body_clicked));
  button->set_border_width(1);

  button = manage(new Button("Neuer negativer Körper"));
  hbox1->pack_end(*button, false, false);
  button->signal_clicked().connect(sigc::mem_fun(*this, &Application::on_add_negative_body_clicked));
  button->set_border_width(1);

  vbox1->pack_start(sbar, false, false);

  main_win.show_all();

  return true;
}

int Application::main()
{
  reset_simulation();

  Main::run(main_win);
}

}
