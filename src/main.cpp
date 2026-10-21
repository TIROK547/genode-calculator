#include <QApplication>
#include "main_window.h"

// ─────────────────────────────────────────────────────────────────────────────
// calcirok — entry point
//
// On Genode/NOVA the linker entry is _start (provided by Genode's crt0),
// but Component::construct() is the Genode idiom. For a Qt component we
// simply call qApp() via the Genode Qt5 platform plugin which maps Qt's
// event loop onto Genode's signal framework.
//
// On the host (dev build, GENODE_BUILD=OFF) this is a plain Qt app.
// ─────────────────────────────────────────────────────────────────────────────

#ifdef GENODE_BUILD
// Forward declaration for Genode's Component interface
namespace Genode { struct Env; }
extern "C" void genode_component_construct(Genode::Env &);
#endif

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("calcirok");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("TIROK");

    MainWindow window;
    window.show();

    return app.exec();
}
