#include <QCoreApplication>
#include "server.h"

#define PORT 2323

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    Server server(PORT);

    return app.exec();
}
