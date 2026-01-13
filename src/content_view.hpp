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

#include "glib.h"
#include <adwaita.h>
#include <gtk/gtk.h>

namespace xafile {

class ContentView {
public:
  void reload_items();

  static ContentView *create();
  GtkWidget *get_widget() const { return GTK_WIDGET(content_box_); }

  void set_view_mode(bool grid_mode);

private:
  ContentView();

  void setup_path_bar();
  void setup_grid_view();
  void setup_list_view();
  void add_sample_items();
  void refresh_path_bar();
  static void on_item_activated(GtkGridView *view, guint position,
                                gpointer user_data);
  static void on_item_right_click(GtkGestureClick *gesture, int n_press,
                                  double x, double y, gpointer user_data);
  static void setup_item_factory(GtkSignalListItemFactory *factory,
                                 GtkListItem *list_item, gpointer user_data);
  static void bind_item_factory(GtkSignalListItemFactory *factory,
                                GtkListItem *list_item, gpointer user_data);

  GtkBox *content_box_;
  GtkBox *path_bar_;
  GtkStack *view_stack_;
  GtkGridView *grid_view_;
  GtkColumnView *list_view_;
  GListStore *file_store_;

  bool is_grid_mode_;
};

struct FileItem {
  GObject parent;
  char *name;
  char *icon_name;
  char *file_type;
  char *size;
  char *modified;
  bool is_directory;
};

} // namespace xafile
