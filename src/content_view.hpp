#pragma once

#include <adwaita.h>
#include <gtk/gtk.h>

namespace xafile {

class ContentView {
public:
  static ContentView *create();
  GtkWidget *get_widget() const { return GTK_WIDGET(content_box_); }

  void set_view_mode(bool grid_mode);

private:
  ContentView();

  void setup_path_bar();
  void setup_grid_view();
  void setup_list_view();
  void add_sample_items();

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
