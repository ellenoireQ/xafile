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

#include "sidebar.hpp"

namespace xafile {

Sidebar::Sidebar() {
    scrolled_window_ = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new());
    gtk_scrolled_window_set_policy(scrolled_window_, GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(GTK_WIDGET(scrolled_window_), 240, -1);
    
    content_box_ = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    gtk_widget_add_css_class(GTK_WIDGET(content_box_), "navigation-sidebar");
    
    setup_places();
    
    gtk_scrolled_window_set_child(scrolled_window_, GTK_WIDGET(content_box_));
}

Sidebar* Sidebar::create() {
    return new Sidebar();
}

void Sidebar::setup_places() {
    places_list_ = GTK_LIST_BOX(gtk_list_box_new());
    gtk_widget_add_css_class(GTK_WIDGET(places_list_), "navigation-sidebar");
    gtk_list_box_set_selection_mode(places_list_, GTK_SELECTION_SINGLE);
    g_signal_connect(places_list_, "row-activated", G_CALLBACK(on_row_activated), this);
    
    gtk_list_box_append(places_list_, create_place_row("user-home-symbolic", "Home"));
    
    gtk_box_append(content_box_, GTK_WIDGET(places_list_));
}

GtkWidget* Sidebar::create_section_header(const char* title) {
    auto* label = gtk_label_new(title);
    gtk_widget_add_css_class(label, "heading");
    gtk_widget_add_css_class(label, "dim-label");
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_widget_set_margin_start(label, 12);
    gtk_widget_set_margin_end(label, 12);
    gtk_widget_set_margin_top(label, 18);
    gtk_widget_set_margin_bottom(label, 6);
    
    return label;
}

GtkWidget* Sidebar::create_place_row(const char* icon_name, const char* label, bool is_ejectable) {
    auto* box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_widget_set_margin_start(box, 12);
    gtk_widget_set_margin_end(box, 12);
    gtk_widget_set_margin_top(box, 8);
    gtk_widget_set_margin_bottom(box, 8);
    
    auto* icon = gtk_image_new_from_icon_name(icon_name);
    gtk_box_append(GTK_BOX(box), icon);
    
    auto* text = gtk_label_new(label);
    gtk_label_set_xalign(GTK_LABEL(text), 0.0f);
    gtk_widget_set_hexpand(text, TRUE);
    gtk_box_append(GTK_BOX(box), text);
    
    if (is_ejectable) {
        auto* eject_btn = gtk_button_new_from_icon_name("media-eject-symbolic");
        gtk_widget_add_css_class(eject_btn, "flat");
        gtk_widget_add_css_class(eject_btn, "circular");
        gtk_box_append(GTK_BOX(box), eject_btn);
    }
    
    return box;
}

void Sidebar::on_row_activated(GtkListBox* list_box, GtkListBoxRow* row, gpointer user_data) {
    (void)list_box;
    (void)row;
    (void)user_data;
}

}
