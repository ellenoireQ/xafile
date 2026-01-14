/*
 * xafile - Xavier File Manager
 * Copyright (C) 2026 Fitrian Musya
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <adwaita.h>
#include <gtk/gtk.h>

namespace xafile {

class Sidebar;
class ContentView;

class Window {
public:
  static Window *create(GtkApplication *app);
  GtkWidget *get_widget() const { return GTK_WIDGET(window_); }
  Window *reload();

private:
  Window(GtkApplication *app);

  void setup_headerbar();
  void setup_content();
  void setup_actions();

  void update_nav_buttons(bool can_back, bool can_forward);

  static void on_back_clicked(GtkButton *button, gpointer user_data);
  static void on_forward_clicked(GtkButton *button, gpointer user_data);
  static void on_search_toggled(GtkToggleButton *button, gpointer user_data);
  static void on_view_mode_changed(GtkToggleButton *button, gpointer user_data);

  AdwApplicationWindow *window_;
  AdwHeaderBar *headerbar_;
  AdwNavigationSplitView *split_view_;
  GtkSearchBar *search_bar_;
  GtkSearchEntry *search_entry_;

  Sidebar *sidebar_;
  ContentView *content_view_;

  GtkWidget *back_btn_;
  GtkWidget *forward_btn_;

  GtkWidget *grid_view_btn_;
  GtkWidget *list_view_btn_;
};

} // namespace xafile
