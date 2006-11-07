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

const std::string Application::appname = "Elfelli";
const std::string Application::version = "0.1";
const std::string Application::datadir = DATADIR;

Application::Application(int argc, char **argv):
  gtk_main(argc, argv),
  save_dlg(main_win, "", FILE_CHOOSER_ACTION_SAVE)
{
  build_gui();
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

  save_dlg.set_title("PNG exportieren");

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

void Application::on_about_activate()
{
  AboutDialog dlg;

  const std::string license_parts[3] = {
    "Dieses Programm ist freie Software. Sie können es unter den Bedingungen\n"
    "der GNU General Public License, wie von der Free Software Foundation ver-\n"
    "öffentlicht, weitergeben und/oder modifizieren; entweder gemäß Version 2\n"
    "der Lizenz, oder (nach Ihrem Ermessen) gemäß jeder späteren Version.\n",

    "Die Veröffentlichung dieses Programms erfolgt in der Hoffnung, dass es\n"
    "Ihnen von Nutzen sein wird, aber OHNE IRGENDEINE GARANTIE, sogar ohne\n"
    "die implizite Garantie der MARKTREIFE oder der VERWENDBARKEIT FÜR EINEN\n"
    "BESTIMMTEN ZWECK. Details finden Sie in der GNU General Public License.\n",

    "Sie sollten ein Exemplar der GNU General Public License zusammen mit\n"
    "diesem Programm erhalten haben, Falls nicht, schreiben Sie an die\n"
    "Free Software Foundation, Inc.\n"
    "51 Franklin Street, Fifth Floor, Boston, MA 02110, USA.\n"};

  const std::string license_text =
    license_parts[0] + "\n"
    + license_parts[1] + "\n"
    + license_parts[2];

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

  action_group->add( Action::create("MenuScene", "_Szene") );
  action_group->add( Action::create("New", Stock::NEW) , sigc::mem_fun(*this, &Application::reset_simulation));
  action_group->add( Action::create("Open", Stock::OPEN) );
  action_group->add( Action::create("SaveAs", Stock::SAVE_AS) );
  action_group->add( Action::create("ExportPNG", "Exportieren als _PNG") , sigc::mem_fun(*this, &Application::on_export_png_activate));
  action_group->add( Action::create("ExportSVG", "Exportieren als S_VG") );
  action_group->add( Action::create("Quit", Stock::QUIT) , sigc::mem_fun(*this, &Application::quit));

  action_group->add( Action::create("MenuEdit", "_Bearbeiten") );
  action_group->add( Action::create("Clear", Stock::CLEAR) , sigc::mem_fun(*this, &Application::reset_simulation));
  action_group->add( Action::create("AddNegative", "Negativer Körper") , sigc::mem_fun(*this, &Application::on_add_negative_body_clicked));
  action_group->add( Action::create("AddPositive", "Positiver Körper") , sigc::mem_fun(*this, &Application::on_add_positive_body_clicked));

  action_group->add( Action::create("MenuHelp", "_Hilfe") );
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

  if (ui_manager->get_widget("/ToolBar/AddNegative"))
    {
      pixbuf = Gdk::Pixbuf::create_from_file(find_datafile("positive.svg"), w, h);
      img = manage(new Image(pixbuf));
      img->show();
      dynamic_cast<ToolButton *>(ui_manager->get_widget("/ToolBar/AddPositive"))->set_icon_widget(*dynamic_cast<Widget *>(img));
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
