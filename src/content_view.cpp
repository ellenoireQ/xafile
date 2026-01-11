#include "content_view.hpp"
#include "gio/gio.h"
#include "glib.h"
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

namespace fs = std::filesystem;

std::vector<std::string> scan_folder(const std::string &path) {
  std::vector<std::string> result;
  for (const auto &entry : fs::directory_iterator(path)) {
    if (entry.is_directory()) {
      result.push_back(entry.path().filename().string());
    }
  }
  return result;
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
  const char *homedir;

  homedir = getenv("HOME");
  std::string path = homedir;
  std::stringstream ss(path);
  std::string segment;
  std::vector<std::string> result;

  while (std::getline(ss, segment, '/')) {
    if (!segment.empty()) {
      result.push_back(segment);
    }
  }

  path_bar_ = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
  gtk_widget_add_css_class(GTK_WIDGET(path_bar_), "linked");
  gtk_widget_set_margin_start(GTK_WIDGET(path_bar_), 12);
  gtk_widget_set_margin_end(GTK_WIDGET(path_bar_), 12);
  gtk_widget_set_margin_top(GTK_WIDGET(path_bar_), 12);
  gtk_widget_set_margin_bottom(GTK_WIDGET(path_bar_), 12);
  for (size_t i = 0; i < result.size(); i++) {
    if (i > 0) {
      auto *sep1 = gtk_image_new_from_icon_name("go-next-symbolic");
      gtk_widget_set_opacity(sep1, 0.5);
      gtk_box_append(path_bar_, sep1);
    }
    auto *home_btn = gtk_button_new_with_label(result[i].c_str());
    gtk_widget_add_css_class(home_btn, "flat");
    gtk_box_append(path_bar_, home_btn);
  }

  gtk_box_append(content_box_, GTK_WIDGET(path_bar_));

  auto *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_box_append(content_box_, separator);
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
  const auto sc = scan_folder("/home/alien/");

  for (auto &s : sc) {
    g_list_store_append(file_store_,
                        file_item_new(s.c_str(), "folder-symbolic", "Folder",
                                      "--", "Today", TRUE));
  }
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
