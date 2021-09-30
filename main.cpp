#include <QCoreApplication>
#include "e_course.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    E_course course;
    course.start();
    return a.exec();
}
