#include <QApplication>
#include <QPixmap>
#include <QGraphicsView>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    auto const &&arguments = a.arguments();
    auto const &&fileName = arguments.value(1);
    QGraphicsScene scene;
    scene.addPixmap(QPixmap(fileName));
    QGraphicsView view(&scene);
    view.show();
    return a.exec();
}
