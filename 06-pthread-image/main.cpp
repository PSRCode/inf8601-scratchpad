#include <QDebug>
#include <QImage>
#include <QDir>
#include <QRgb>
#include <pthread.h>

typedef struct work {\
    int n;
    int rank;
    QImage *image;
} work_t;

void *effect(void *arg)
{
    work_t *work = static_cast<work_t*>(arg);
    QImage *img = work->image;
    (void) img;

    /* Split work */

    //qDebug() << "rank" << work->rank << "h0" << h0 << "h1" << h1;

    /*
    int dr = (255 / work->n * work->rank) % 255;
    int dg = 0; int db = 0;
    int r, g, b;
    */
    /* loop over pixels */

    /*
            if (!img->valid(x, y)) {
                qDebug() << QString("invalid (%1,%2)").arg(x).arg(y);
            }
            QRgb rgb = img->pixel(x, y);

            r = qBound(0, qRed(rgb) + dr, 255);
            g = qBound(0, qGreen(rgb) + dg, 255);
            b = qBound(0, qBlue(rgb) + db, 255);

            img->setPixel(x, y, qRgb(r, g, b));
    */

    return 0;
}

int main(int argc, char *argv[])
{
    (void) argc; (void) argv;

    int n = 10;
    pthread_t threads[n];
    work_t work_items[n];

    (void) threads;
    (void) work_items;

    QImage image;

    QString orig("../../inf8601-scratchpad/res/bsod.png");
    QString modz(orig + ".modz.png");

    if (!image.load(orig)) {
        qDebug() << "error loading image";
        return -1;
    }
    image = image.convertToFormat(QImage::Format_RGB32);

    for (int i = 0; i < n; i++) {
        /* Create thread */
    }

    for (int i = 0; i < n; i++) {
        /* Wait thread */
    }

    image.save(modz);

    return 0;
}
