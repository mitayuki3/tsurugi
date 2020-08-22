#include <QApplication>
#include <QDebug>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QMenu>
#include <QPixmap>
#include <QScroller>
#include <QSettings>

class MainView : public QGraphicsView
{
    QGraphicsItem const *whatToFit;

  public:
    explicit MainView(QGraphicsScene *scene, QWidget *parent = nullptr)
        : QGraphicsView(scene, parent), whatToFit(nullptr),
          zoomMode(ZoomMode::DotByDot)
    {
    }
    ~MainView() override {}
    void fitAlways(QGraphicsItem const *obj)
    {
        zoomMode = ZoomMode::Fit;
        whatToFit = obj;
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }

  protected:
    enum class ZoomMode { DotByDot, Fit, Fill } zoomMode;
    void resizeEvent(QResizeEvent *event) override
    {
        switch (zoomMode) {
        case ZoomMode::Fit:
            if (whatToFit) {
                fitInView(whatToFit, Qt::KeepAspectRatio);
            }
            break;
        }
    }
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    auto const &&arguments = a.arguments();
    auto const &&fileName = arguments.value(1);
    QSettings settings("mitayuki3", "tsurugi");
    QGraphicsScene scene;
    QGraphicsPixmapItem *pixmapItem = scene.addPixmap(QPixmap(fileName));
    MainView view(&scene);
    view.setRenderHints(QPainter::Antialiasing |
                        QPainter::SmoothPixmapTransform);
    view.setContextMenuPolicy(Qt::CustomContextMenu);
    if (settings.value("zoomMode", "fit").toString() == "fit") {
        view.fitAlways(pixmapItem);
    }
    QScroller::grabGesture(&view, QScroller::LeftMouseButtonGesture);
    QMenu menu;
    auto fitAction = menu.addAction("Fit");
    fitAction->setShortcut(Qt::Key_Home);
    auto *mainWidget = &view;
    mainWidget->addAction(fitAction);
    QObject::connect(&view, &QWidget::customContextMenuRequested,
                     [&menu, &view](const QPoint &pos) {
                         menu.popup(view.mapToGlobal(pos));
                     });
    QObject::connect(fitAction, &QAction::triggered,
                     [&view, pixmapItem]() { view.fitAlways(pixmapItem); });
    QObject::connect(fitAction, &QAction::triggered, [&settings]() {
        settings.setValue("zoomMode", "fit");
    });
    view.show();
    return a.exec();
}
