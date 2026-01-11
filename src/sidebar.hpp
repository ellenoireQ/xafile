#pragma once

#include <adwaita.h>
#include <gtk/gtk.h>

namespace xafile {

class Sidebar {
public:
    static Sidebar* create();
    GtkWidget* get_widget() const { return GTK_WIDGET(scrolled_window_); }

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
};

} // namespace xafile
