#include <QCoreApplication>
#include <QMap>
#include <QVector>
#include <QImage>
#include <QDir>
#include <QFile>
#include <QThread>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QDebug>

struct ImageSlice
{
    uchar* bits;
    quint32 start;
    quint32 end;
};

void image_slicing(uchar *bits, const quint32 &bits_qty, const quint32 &bits_per_thread);
void run_tasks(const QList<ImageSlice> &slices);
QList<QMap<uchar, quint32>> count_bgra(const ImageSlice &slice);
void save_results(const QList<QList<QMap<uchar, quint32>>> &results);

static const quint8 THREAD_COUNT = QThread::idealThreadCount();

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    const QStringList args = a.arguments();

    if(args.size() < 2)
        return 1;

    QDir exe_path(QCoreApplication::applicationDirPath());
    QString image_path = args[1];

    QImage img;

    if(img.load(exe_path.absoluteFilePath(image_path)))
    {
        uchar *bits = img.bits();
        quint32 bits_qty = img.width() * img.height() * 4;
        quint32 bits_per_thread = bits_qty / THREAD_COUNT;

        image_slicing(bits, bits_qty, bits_per_thread);
    }
    else
        return 2;

    return 0;
}

void image_slicing(uchar *bits, const quint32 &bits_qty, const quint32 &bits_per_thread)
{
    QList<ImageSlice> slices;

    for (quint8 i = 0; i <= THREAD_COUNT; i++)
    {
        quint32 start = i * bits_per_thread;
        quint32 end = bits_qty;

        if (i != THREAD_COUNT)
            end = (i + 1) * bits_per_thread;

        if (start != end)
            slices.append(ImageSlice{bits, start, end});
    }

    run_tasks(slices);
}

void run_tasks(const QList<ImageSlice> &slices)
{
    QFuture<QList<QMap<uchar, quint32>>> future = QtConcurrent::mapped(slices, count_bgra);
    future.waitForFinished();

    save_results(future.results());
}

QList<QMap<uchar, quint32>> count_bgra(const ImageSlice &slice)
{
    QList<QMap<uchar, quint32>> hist_slice(4);

    for (quint32 i = slice.start; i < slice.end; i++)
    {
        quint8 channel = i % 4;
        hist_slice[channel][slice.bits[i]]++;
    }

    return hist_slice;
}

void save_results(const QList<QList<QMap<uchar, quint32>>> &results)
{
    QFile outf("output.csv");

    if(!outf.open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        exit(1);

    QTextStream out(&outf);

    for (qint8 colour = 2; colour >= 0; colour--)
    {
        for (quint16 intensity = 0; intensity < 256; intensity++)
        {
            quint32 qty = 0;

            for (auto const &result : results)
                qty += result[colour][intensity];

            out << qty;

            if (intensity != 255)
                out << ',';
        }

        out << '\n';
    }

    outf.close();
}
