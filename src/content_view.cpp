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

#include "content_view.hpp"
#include "gio/gio.h"
#include "glib.h"
#include "glibconfig.h"
#include "utility/utilitas.hpp"
#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace xafile {

#define FILE_ITEM_TYPE (file_item_get_type())
G_DECLARE_FINAL_TYPE(FileItemObject, file_item, FILE, ITEM, GObject)

struct _FileItemObject {
  GObject parent_instance;
  char *name;
  char *icon_name;
  char *file_type;
  char *size;
  char *modified;
  gboolean is_directory;
};

G_DEFINE_TYPE(FileItemObject, file_item, G_TYPE_OBJECT)

static void file_item_finalize(GObject *object) {
  FileItemObject *self = FILE_ITEM(object);
  g_free(self->name);
  g_free(self->icon_name);
  g_free(self->file_type);
  g_free(self->size);
  g_free(self->modified);
  G_OBJECT_CLASS(file_item_parent_class)->finalize(object);
}
Utility utly{};
static void file_item_class_init(FileItemObjectClass *klass) {
  GObjectClass *object_class = G_OBJECT_CLASS(klass);
  object_class->finalize = file_item_finalize;
}

static void file_item_init(FileItemObject *self) {
  self->name = nullptr;
  self->icon_name = nullptr;
  self->file_type = nullptr;
  self->size = nullptr;
  self->modified = nullptr;
  self->is_directory = FALSE;
}

static FileItemObject *file_item_new(const char *name, const char *icon_name,
                                     const char *file_type, const char *size,
                                     const char *modified,
                                     gboolean is_directory) {
  FileItemObject *item = FILE_ITEM(g_object_new(FILE_ITEM_TYPE, nullptr));
  item->name = g_strdup(name);
  item->icon_name = g_strdup(icon_name);
  item->file_type = g_strdup(file_type);
  item->size = g_strdup(size);
  item->modified = g_strdup(modified);
  item->is_directory = is_directory;
  return item;
}

ContentView::ContentView() : is_grid_mode_(true) {
  content_box_ = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
  file_store_ = g_list_store_new(FILE_ITEM_TYPE);

  setup_path_bar();
  setup_grid_view();
  setup_list_view();
  add_sample_items();

  view_stack_ = GTK_STACK(gtk_stack_new());
  gtk_stack_set_transition_type(view_stack_,
                                GTK_STACK_TRANSITION_TYPE_CROSSFADE);

  auto *grid_scroll = gtk_scrolled_window_new();
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(grid_scroll),
                                 GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(grid_scroll),
                                GTK_WIDGET(grid_view_));
  gtk_widget_set_vexpand(grid_scroll, TRUE);

  auto *list_scroll = gtk_scrolled_window_new();
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(list_scroll),
                                 GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(list_scroll),
                                GTK_WIDGET(list_view_));
  gtk_widget_set_vexpand(list_scroll, TRUE);

  gtk_stack_add_named(view_stack_, grid_scroll, "grid");
  gtk_stack_add_named(view_stack_, list_scroll, "list");
  gtk_stack_set_visible_child_name(view_stack_, "grid");

  gtk_box_append(content_box_, GTK_WIDGET(view_stack_));
}

ContentView *ContentView::create() { return new ContentView(); }
void ContentView::setup_path_bar() {
  const auto result = utly.getParsedCurDir();
  path_bar_ = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
  gtk_widget_add_css_class(GTK_WIDGET(path_bar_), "linked");
  gtk_widget_set_margin_start(GTK_WIDGET(path_bar_), 12);
  gtk_widget_set_margin_end(GTK_WIDGET(path_bar_), 12);
  gtk_widget_set_margin_top(GTK_WIDGET(path_bar_), 12);
  gtk_widget_set_margin_bottom(GTK_WIDGET(path_bar_), 12);
  for (size_t i = 0; i < result.size(); i++) {
    if (i > 0) {
      auto *arrowRight = gtk_image_new_from_icon_name("go-next-symbolic");
      gtk_widget_set_opacity(arrowRight, 0.5);
      gtk_box_append(path_bar_, arrowRight);
    }
    auto *breadcrumbs = gtk_button_new_with_label(result[i].c_str());
    gtk_widget_add_css_class(breadcrumbs, "flat");
    gtk_box_append(path_bar_, breadcrumbs);
  }
  gtk_box_append(content_box_, GTK_WIDGET(path_bar_));

  auto *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_box_append(content_box_, separator);
}

void ContentView::refresh_path_bar() {
  GtkWidget *child = gtk_widget_get_first_child(GTK_WIDGET(path_bar_));
  while (child != nullptr) {
    GtkWidget *next = gtk_widget_get_next_sibling(child);
    gtk_box_remove(path_bar_, child);
    child = next;
  }

  const auto result = utly.getParsedCurDir();
  for (size_t i = 0; i < result.size(); i++) {
    if (i > 0) {
      auto *arrowRight = gtk_image_new_from_icon_name("go-next-symbolic");
      gtk_widget_set_opacity(arrowRight, 0.5);
      gtk_box_append(path_bar_, arrowRight);
    }
    auto *breadcrumbs = gtk_button_new_with_label(result[i].c_str());
    gtk_widget_add_css_class(breadcrumbs, "flat");
    gtk_box_append(path_bar_, breadcrumbs);
  }
}

void ContentView::on_item_activated(GtkGridView *view, guint position,
                                    gpointer user_data) {
  (void)view;
  namespace fs = std::filesystem;
  auto *self = static_cast<ContentView *>(user_data);
  auto *item = FILE_ITEM(
      g_list_model_get_item(G_LIST_MODEL(self->file_store_), position));

  if (!item)
    return;

  std::string cur_dir = utly.getCurDir();
  if (cur_dir.empty() || cur_dir.back() != '/') {
    cur_dir += '/';
  }

  if (item->is_directory) {
    fs::path new_path = cur_dir + item->name + '/';
    utly.setCurDir(new_path.c_str());
    self->reload_items();
  } else {
    std::string full_path = cur_dir + item->name;
    GFile *file = g_file_new_for_path(full_path.c_str());
    g_app_info_launch_default_for_uri(g_file_get_uri(file), NULL, NULL);
    g_object_unref(file);
  }
}

void ContentView::setup_grid_view() {
  auto *factory = gtk_signal_list_item_factory_new();

  g_signal_connect(
      factory, "setup",
      G_CALLBACK(
          +[](GtkSignalListItemFactory *, GtkListItem *list_item, gpointer) {
            auto *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
            gtk_widget_set_size_request(box, 100, 100);
            gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
            gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
            gtk_widget_set_margin_start(box, 8);
            gtk_widget_set_margin_end(box, 8);
            gtk_widget_set_margin_top(box, 8);
            gtk_widget_set_margin_bottom(box, 8);

            auto *icon = gtk_image_new();
            gtk_image_set_pixel_size(GTK_IMAGE(icon), 64);
            gtk_widget_set_halign(icon, GTK_ALIGN_CENTER);
            gtk_box_append(GTK_BOX(box), icon);

            auto *label = gtk_label_new("");
            gtk_label_set_max_width_chars(GTK_LABEL(label), 14);
            gtk_label_set_ellipsize(GTK_LABEL(label), PANGO_ELLIPSIZE_END);
            gtk_label_set_wrap(GTK_LABEL(label), TRUE);
            gtk_label_set_wrap_mode(GTK_LABEL(label), PANGO_WRAP_WORD_CHAR);
            gtk_label_set_lines(GTK_LABEL(label), 2);
            gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
            gtk_box_append(GTK_BOX(box), label);

            gtk_list_item_set_child(list_item, box);
          }),
      nullptr);

  g_signal_connect(factory, "bind",
                   G_CALLBACK(+[](GtkSignalListItemFactory *,
                                  GtkListItem *list_item, gpointer) {
                     auto *box = gtk_list_item_get_child(list_item);
                     auto *item = FILE_ITEM(gtk_list_item_get_item(list_item));

                     auto *icon = gtk_widget_get_first_child(box);
                     auto *label = gtk_widget_get_next_sibling(icon);

                     gtk_image_set_from_icon_name(GTK_IMAGE(icon),
                                                  item->icon_name);
                     gtk_label_set_text(GTK_LABEL(label), item->name);
                   }),
                   nullptr);

  auto *selection = gtk_multi_selection_new(G_LIST_MODEL(file_store_));
  grid_view_ =
      GTK_GRID_VIEW(gtk_grid_view_new(GTK_SELECTION_MODEL(selection), factory));
  gtk_grid_view_set_min_columns(grid_view_, 3);
  gtk_grid_view_set_max_columns(grid_view_, 10);
  gtk_widget_add_css_class(GTK_WIDGET(grid_view_), "content-view");

  g_signal_connect(grid_view_, "activate", G_CALLBACK(on_item_activated), this);
}

void ContentView::setup_list_view() {
  auto *selection =
      gtk_multi_selection_new(G_LIST_MODEL(g_object_ref(file_store_)));
  list_view_ =
      GTK_COLUMN_VIEW(gtk_column_view_new(GTK_SELECTION_MODEL(selection)));
  gtk_column_view_set_show_column_separators(list_view_, FALSE);
  gtk_column_view_set_show_row_separators(list_view_, FALSE);

  auto *name_factory = gtk_signal_list_item_factory_new();
  g_signal_connect(name_factory, "setup",
                   G_CALLBACK(+[](GtkSignalListItemFactory *,
                                  GtkListItem *list_item, gpointer) {
                     auto *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
                     gtk_widget_set_margin_start(box, 8);
                     gtk_widget_set_margin_end(box, 8);
                     gtk_widget_set_margin_top(box, 6);
                     gtk_widget_set_margin_bottom(box, 6);

                     auto *icon = gtk_image_new();
                     gtk_box_append(GTK_BOX(box), icon);

                     auto *label = gtk_label_new("");
                     gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
                     gtk_widget_set_hexpand(label, TRUE);
                     gtk_box_append(GTK_BOX(box), label);

                     gtk_list_item_set_child(list_item, box);
                   }),
                   nullptr);
  g_signal_connect(name_factory, "bind",
                   G_CALLBACK(+[](GtkSignalListItemFactory *,
                                  GtkListItem *list_item, gpointer) {
                     auto *box = gtk_list_item_get_child(list_item);
                     auto *item = FILE_ITEM(gtk_list_item_get_item(list_item));

                     auto *icon = gtk_widget_get_first_child(box);
                     auto *label = gtk_widget_get_next_sibling(icon);

                     gtk_image_set_from_icon_name(GTK_IMAGE(icon),
                                                  item->icon_name);
                     gtk_label_set_text(GTK_LABEL(label), item->name);
                   }),
                   nullptr);
  g_signal_connect(list_view_, "activate", G_CALLBACK(on_item_activated), this);
  auto *name_col = gtk_column_view_column_new("Name", name_factory);
  gtk_column_view_column_set_expand(name_col, TRUE);
  gtk_column_view_column_set_resizable(name_col, TRUE);
  gtk_column_view_append_column(list_view_, name_col);

  auto *size_factory = gtk_signal_list_item_factory_new();
  g_signal_connect(size_factory, "setup",
                   G_CALLBACK(+[](GtkSignalListItemFactory *,
                                  GtkListItem *list_item, gpointer) {
                     auto *label = gtk_label_new("");
                     gtk_label_set_xalign(GTK_LABEL(label), 1.0f);
                     gtk_widget_set_margin_start(label, 8);
                     gtk_widget_set_margin_end(label, 8);
                     gtk_list_item_set_child(list_item, label);
                   }),
                   nullptr);
  g_signal_connect(size_factory, "bind",
                   G_CALLBACK(+[](GtkSignalListItemFactory *,
                                  GtkListItem *list_item, gpointer) {
                     auto *label = gtk_list_item_get_child(list_item);
                     auto *item = FILE_ITEM(gtk_list_item_get_item(list_item));
                     gtk_label_set_text(GTK_LABEL(label), item->size);
                   }),
                   nullptr);

  auto *size_col = gtk_column_view_column_new("Size", size_factory);
  gtk_column_view_column_set_resizable(size_col, TRUE);
  gtk_column_view_append_column(list_view_, size_col);

  auto *type_factory = gtk_signal_list_item_factory_new();
  g_signal_connect(type_factory, "setup",
                   G_CALLBACK(+[](GtkSignalListItemFactory *,
                                  GtkListItem *list_item, gpointer) {
                     auto *label = gtk_label_new("");
                     gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
                     gtk_widget_set_margin_start(label, 8);
                     gtk_widget_set_margin_end(label, 8);
                     gtk_list_item_set_child(list_item, label);
                   }),
                   nullptr);
  g_signal_connect(type_factory, "bind",
                   G_CALLBACK(+[](GtkSignalListItemFactory *,
                                  GtkListItem *list_item, gpointer) {
                     auto *label = gtk_list_item_get_child(list_item);
                     auto *item = FILE_ITEM(gtk_list_item_get_item(list_item));
                     gtk_label_set_text(GTK_LABEL(label), item->file_type);
                   }),
                   nullptr);

  auto *type_col = gtk_column_view_column_new("Type", type_factory);
  gtk_column_view_column_set_resizable(type_col, TRUE);
  gtk_column_view_append_column(list_view_, type_col);

  auto *modified_factory = gtk_signal_list_item_factory_new();
  g_signal_connect(modified_factory, "setup",
                   G_CALLBACK(+[](GtkSignalListItemFactory *,
                                  GtkListItem *list_item, gpointer) {
                     auto *label = gtk_label_new("");
                     gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
                     gtk_widget_set_margin_start(label, 8);
                     gtk_widget_set_margin_end(label, 8);
                     gtk_list_item_set_child(list_item, label);
                   }),
                   nullptr);
  g_signal_connect(modified_factory, "bind",
                   G_CALLBACK(+[](GtkSignalListItemFactory *,
                                  GtkListItem *list_item, gpointer) {
                     auto *label = gtk_list_item_get_child(list_item);
                     auto *item = FILE_ITEM(gtk_list_item_get_item(list_item));
                     gtk_label_set_text(GTK_LABEL(label), item->modified);
                   }),
                   nullptr);

  auto *modified_col = gtk_column_view_column_new("Modified", modified_factory);
  gtk_column_view_column_set_resizable(modified_col, TRUE);
  gtk_column_view_append_column(list_view_, modified_col);
}

void ContentView::add_sample_items() {
  const auto [sc, s] = utly.scan(utly.getCurDir());

  for (auto &s : sc) {
    g_list_store_append(file_store_,
                        file_item_new(s.c_str(), "folder-symbolic", "Folder",
                                      "--", "Today", TRUE));
  }
  for (auto &c : s) {
    g_list_store_append(file_store_,
                        file_item_new(c.c_str(), "text-x-generic", "file", "--",
                                      "today", false));
  }
}

void ContentView::reload_items() {
  g_list_store_remove_all(file_store_);
  const auto [sc, s] = utly.scan(utly.getCurDir());

  for (auto &s : sc) {
    std::cout << s << std::endl;
    g_list_store_append(file_store_,
                        file_item_new(s.c_str(), "folder-symbolic", "Folder",
                                      "--", "Today", TRUE));
  }
  for (auto &c : s) {
    g_list_store_append(file_store_,
                        file_item_new(c.c_str(), "text-x-generic", "file", "--",
                                      "today", false));
  }

  refresh_path_bar();
}

void ContentView::set_view_mode(bool grid_mode) {
  is_grid_mode_ = grid_mode;
  gtk_stack_set_visible_child_name(view_stack_, grid_mode ? "grid" : "list");
}

void ContentView::on_item_right_click(GtkGestureClick *gesture, int n_press,
                                      double x, double y, gpointer user_data) {
  (void)gesture;
  (void)n_press;
  (void)x;
  (void)y;
  (void)user_data;
}

} // namespace xafile
