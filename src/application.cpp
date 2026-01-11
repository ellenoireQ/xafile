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
