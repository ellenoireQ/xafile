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

private:
  Window(GtkApplication *app);

  void setup_headerbar();
  void setup_content();
  void setup_actions();

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
