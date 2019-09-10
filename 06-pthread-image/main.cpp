#include <QDebug>
#include <QImage>
#include <QDir>
#include <QRgb>
#include <pthread.h>

typedef struct work {
    int n;
    int rank;
    QImage *image;
} work_t;

void *effect(void *arg)
{
    work_t *work = static_cast<work_t*>(arg);
    QImage *img = work->image;

    unsigned long size = img->height();

    /* Find the horizontal boundary for this thread. */
    int h0 = size * (work->rank)     / work->n;
    int h1 = size * (work->rank + 1) / work->n;

    qDebug() << "rank" << work->rank << "h0" << h0 << "h1" << h1 << "work size:" << h1 - h0;

    /*
     * Based on the rank and number of total thread(n) calculate a red delta to
     * apply.
     *
     * In other word, create a colour gradient of step 255/n increase of red.
     */
    int dr = (255 / work->n * work->rank) % 255;
    int dg = 0;
    int db = 0;
    int r, g, b;
    for (int y = h0; y < h1; y++) {
        //QRgb line = img->scanLine(y);
        for (int x = 0; x < img->width(); x++) {
            if (!img->valid(x, y)) {
                qDebug() << QString("invalid (%1,%2)").arg(x).arg(y);
            }
            QRgb rgb = img->pixel(x, y);

	    /*
	     * Only the red intensity will change here.
	     * qBound is used to cap the value to 255
	     */
            r = qBound(0, qRed(rgb) + dr, 255);
            g = qBound(0, qGreen(rgb) + dg, 255);
            b = qBound(0, qBlue(rgb) + db, 255);

            img->setPixel(x, y, qRgb(r, g, b));
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    (void) argc; (void) argv;

    int n = 10;
    pthread_t threads[n];
    work_t work_items[n];

    QImage image;

    QString orig("../../inf8601-scratchpad/res/bsod.png");
    QString modz(orig + ".modz.png");

    if (!image.load(orig)) {
        qDebug() << "error loading image";
        return -1;
    }
    image = image.convertToFormat(QImage::Format_RGB32);

    for (int i = 0; i < n; i++) {
        work_items[i].n = n;
        work_items[i].rank = i;
        work_items[i].image = &image;
        pthread_create(&threads[i], NULL, effect, &work_items[i]);
    }

    for (int i = 0; i < n; i++) {
        pthread_join(threads[i], NULL);
    }

    image.save(modz);

    return 0;
}
