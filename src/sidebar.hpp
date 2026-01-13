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

class ContentView;  // forward declaration

class Sidebar {
public:
    static Sidebar* create();
    GtkWidget* get_widget() const { return GTK_WIDGET(scrolled_window_); }
    void set_content_view(ContentView* view) { content_view_ = view; }

private:
    Sidebar();
    
    void setup_places();
    GtkWidget* create_section_header(const char* title);
    GtkWidget* create_place_row(const char* icon_name, const char* label, bool is_ejectable = false);
    
    static void on_row_activated(GtkListBox* list_box, GtkListBoxRow* row, gpointer user_data);
    
    GtkScrolledWindow* scrolled_window_;
    GtkBox* content_box_;
    GtkListBox* places_list_;
    GtkListBox* devices_list_;
    GtkListBox* bookmarks_list_;
    ContentView* content_view_ = nullptr;
};

} // namespace xafile
