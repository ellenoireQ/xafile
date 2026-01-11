#pragma once

#include <adwaita.h>
#include <gtk/gtk.h>

namespace xafile {

class Application {
public:
    static Application* create();
    int run(int argc, char* argv[]);

private:
    Application();
    
    static void on_activate(GtkApplication* app, gpointer user_data);
    static void on_startup(GtkApplication* app, gpointer user_data);
    
    AdwApplication* app_;
};

} // namespace xafile
