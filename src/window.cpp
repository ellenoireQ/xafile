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

#include "window.hpp"
#include "content_view.hpp"
#include "sidebar.hpp"

namespace xafile {

Window::Window(GtkApplication *app) {
  window_ = ADW_APPLICATION_WINDOW(adw_application_window_new(app));
  gtk_window_set_title(GTK_WINDOW(window_), "Files");
  gtk_window_set_default_size(GTK_WINDOW(window_), 1000, 700);

  setup_headerbar();
  setup_content();
  setup_actions();
}

Window *Window::create(GtkApplication *app) { return new Window(app); }

void Window::setup_headerbar() {
  headerbar_ = ADW_HEADER_BAR(adw_header_bar_new());

  auto *nav_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_add_css_class(nav_box, "linked");

  back_btn_ = gtk_button_new_from_icon_name("go-previous-symbolic");
  gtk_widget_set_tooltip_text(back_btn_, "Back");
  gtk_widget_set_sensitive(back_btn_, FALSE);
  g_signal_connect(back_btn_, "clicked", G_CALLBACK(on_back_clicked), this);

  forward_btn_ = gtk_button_new_from_icon_name("go-next-symbolic");
  gtk_widget_set_tooltip_text(forward_btn_, "Forward");
  gtk_widget_set_sensitive(forward_btn_, FALSE);
  g_signal_connect(forward_btn_, "clicked", G_CALLBACK(on_forward_clicked),
                   this);

  gtk_box_append(GTK_BOX(nav_box), back_btn_);
  gtk_box_append(GTK_BOX(nav_box), forward_btn_);

  adw_header_bar_pack_start(headerbar_, nav_box);

  auto *view_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_add_css_class(view_box, "linked");

  grid_view_btn_ = gtk_toggle_button_new();
  gtk_button_set_icon_name(GTK_BUTTON(grid_view_btn_), "view-grid-symbolic");
  gtk_widget_set_tooltip_text(grid_view_btn_, "Grid View");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(grid_view_btn_), TRUE);
  g_signal_connect(grid_view_btn_, "toggled", G_CALLBACK(on_view_mode_changed),
                   this);

  list_view_btn_ = gtk_toggle_button_new();
  gtk_button_set_icon_name(GTK_BUTTON(list_view_btn_), "view-list-symbolic");
  gtk_widget_set_tooltip_text(list_view_btn_, "List View");
  gtk_toggle_button_set_group(GTK_TOGGLE_BUTTON(list_view_btn_),
                              GTK_TOGGLE_BUTTON(grid_view_btn_));

  gtk_box_append(GTK_BOX(view_box), grid_view_btn_);
  gtk_box_append(GTK_BOX(view_box), list_view_btn_);

  adw_header_bar_pack_end(headerbar_, view_box);

  auto *search_btn = gtk_toggle_button_new();
  gtk_button_set_icon_name(GTK_BUTTON(search_btn), "system-search-symbolic");
  gtk_widget_set_tooltip_text(search_btn, "Search");
  g_signal_connect(search_btn, "toggled", G_CALLBACK(on_search_toggled), this);
  adw_header_bar_pack_end(headerbar_, search_btn);

  auto *menu_btn = gtk_menu_button_new();
  gtk_menu_button_set_icon_name(GTK_MENU_BUTTON(menu_btn),
                                "open-menu-symbolic");
  gtk_widget_set_tooltip_text(GTK_WIDGET(menu_btn), "Main Menu");

  auto *menu = g_menu_new();
  auto *section1 = g_menu_new();
  g_menu_append(section1, "New Folder", "win.new-folder");
  g_menu_append(section1, "New File", "win.new-file");
  g_menu_append_section(menu, NULL, G_MENU_MODEL(section1));

  auto *section2 = g_menu_new();
  g_menu_append(section2, "Show Hidden Files", "win.show-hidden");
  g_menu_append(section2, "Sort By...", "win.sort");
  g_menu_append_section(menu, NULL, G_MENU_MODEL(section2));

  auto *section3 = g_menu_new();
  g_menu_append(section3, "Properties", "win.properties");
  g_menu_append(section3, "About Files", "win.about");
  g_menu_append_section(menu, NULL, G_MENU_MODEL(section3));

  gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(menu_btn), G_MENU_MODEL(menu));
  adw_header_bar_pack_end(headerbar_, GTK_WIDGET(menu_btn));

  g_object_unref(section1);
  g_object_unref(section2);
  g_object_unref(section3);
  g_object_unref(menu);
}

void Window::setup_content() {
  auto *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

  gtk_box_append(GTK_BOX(main_box), GTK_WIDGET(headerbar_));

  search_bar_ = GTK_SEARCH_BAR(gtk_search_bar_new());
  search_entry_ = GTK_SEARCH_ENTRY(gtk_search_entry_new());
  gtk_widget_set_hexpand(GTK_WIDGET(search_entry_), TRUE);
  gtk_widget_set_size_request(GTK_WIDGET(search_entry_), 400, -1);

  auto *search_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_set_halign(search_box, GTK_ALIGN_CENTER);
  gtk_box_append(GTK_BOX(search_box), GTK_WIDGET(search_entry_));

  gtk_search_bar_set_child(search_bar_, search_box);
  gtk_search_bar_connect_entry(search_bar_, GTK_EDITABLE(search_entry_));
  gtk_box_append(GTK_BOX(main_box), GTK_WIDGET(search_bar_));

  split_view_ = ADW_NAVIGATION_SPLIT_VIEW(adw_navigation_split_view_new());
  gtk_widget_set_vexpand(GTK_WIDGET(split_view_), TRUE);

  sidebar_ = Sidebar::create();
  auto *sidebar_page =
      adw_navigation_page_new(sidebar_->get_widget(), "Places");
  adw_navigation_split_view_set_sidebar(split_view_, sidebar_page);

  content_view_ = ContentView::create();
  auto *content_page =
      adw_navigation_page_new(content_view_->get_widget(), "Files");
  adw_navigation_split_view_set_content(split_view_, content_page);

  gtk_box_append(GTK_BOX(main_box), GTK_WIDGET(split_view_));

  auto *status_bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
  gtk_widget_add_css_class(status_bar, "toolbar");
  gtk_widget_set_margin_start(status_bar, 12);
  gtk_widget_set_margin_end(status_bar, 12);
  gtk_widget_set_margin_top(status_bar, 6);
  gtk_widget_set_margin_bottom(status_bar, 6);

  gtk_box_append(GTK_BOX(main_box), status_bar);

  adw_application_window_set_content(window_, main_box);
}

void Window::setup_actions() {
  auto *action_group = g_simple_action_group_new();

  auto *new_folder_action = g_simple_action_new("new-folder", NULL);
  g_action_map_add_action(G_ACTION_MAP(action_group),
                          G_ACTION(new_folder_action));

  auto *new_file_action = g_simple_action_new("new-file", NULL);
  g_action_map_add_action(G_ACTION_MAP(action_group),
                          G_ACTION(new_file_action));

  auto *show_hidden_action = g_simple_action_new_stateful(
      "show-hidden", NULL, g_variant_new_boolean(FALSE));
  g_action_map_add_action(G_ACTION_MAP(action_group),
                          G_ACTION(show_hidden_action));

  auto *sort_action = g_simple_action_new("sort", NULL);
  g_action_map_add_action(G_ACTION_MAP(action_group), G_ACTION(sort_action));

  auto *properties_action = g_simple_action_new("properties", NULL);
  g_action_map_add_action(G_ACTION_MAP(action_group),
                          G_ACTION(properties_action));

  auto *about_action = g_simple_action_new("about", NULL);
  g_action_map_add_action(G_ACTION_MAP(action_group), G_ACTION(about_action));

  gtk_widget_insert_action_group(GTK_WIDGET(window_), "win",
                                 G_ACTION_GROUP(action_group));
}

void Window::on_back_clicked(GtkButton *button, gpointer user_data) {
  (void)button;
  (void)user_data;
}

void Window::on_forward_clicked(GtkButton *button, gpointer user_data) {
  (void)button;
  (void)user_data;
}

void Window::on_search_toggled(GtkToggleButton *button, gpointer user_data) {
  auto *self = static_cast<Window *>(user_data);
  gboolean active = gtk_toggle_button_get_active(button);
  gtk_search_bar_set_search_mode(self->search_bar_, active);
}

void Window::on_view_mode_changed(GtkToggleButton *button, gpointer user_data) {
  (void)button;
  (void)user_data;
}

} // namespace xafile
