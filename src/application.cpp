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

#include "application.hpp"
#include "window.hpp"

namespace xafile {

Application::Application() {
    app_ = adw_application_new("io.github.xafile", G_APPLICATION_DEFAULT_FLAGS);
    
    g_signal_connect(app_, "activate", G_CALLBACK(on_activate), this);
    g_signal_connect(app_, "startup", G_CALLBACK(on_startup), this);
}

Application* Application::create() {
    return new Application();
}

int Application::run(int argc, char* argv[]) {
    int status = g_application_run(G_APPLICATION(app_), argc, argv);
    g_object_unref(app_);
    return status;
}

void Application::on_startup(GtkApplication* app, gpointer user_data) {
    (void)app;
    (void)user_data;
}

void Application::on_activate(GtkApplication* app, gpointer user_data) {
    (void)user_data;
    
    auto* window = Window::create(GTK_APPLICATION(app));
    gtk_window_present(GTK_WINDOW(window->get_widget()));
}

}
