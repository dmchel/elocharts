#ifndef ELOPLOTTER_H
#define ELOPLOTTER_H

#include <qcustomplot/qcustomplot.h>

class ELOPlotter : public QCustomPlot
{
    Q_OBJECT

public:
    ELOPlotter(QWidget *parent = Q_NULLPTR);

    void setUpdatingPeriod(int msec);
    int updatingInterval() const;

public slots:
    void start(int msec = 50);
    void addDataToPlot(int id, qreal x, qreal y);
    void setPlotColor(int id, const QColor &color);
    void clearPlot();
    void fitInPlots();
    void pause();
    void run();

signals:
    void plotColorChanged(int id, const QColor &color);

private slots:
    void onUpdateTimeout();
    void onMousePressed(QMouseEvent *event);
    void onSelectionChanged();

private:
    void initPlotter();
    void initColorList();

    const QColor &nextDefaultColor() const;

    void updateRanges(qreal x, qreal y);

private:
    QTimer *updateTimer = Q_NULLPTR;
    int replotIntervalMs = 50;
    QMap<int, QCPGraph *> graphMap;
    QCPGraph *timeLine = Q_NULLPTR;
    QVector<qreal> tlKeys;
    QVector<qreal> tlData;
    QList<QColor> plotColors;
    bool fVerticalAutoScroll = true;
    qreal maxRangeY = 0.0;
    bool fRunning = true;
};

#endif // ELOPLOTTER_H
