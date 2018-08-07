#ifndef ELOPLOTTER_H
#define ELOPLOTTER_H

#include <qcustomplot/qcustomplot.h>

class ELOPlotter : public QCustomPlot
{
    Q_OBJECT

public:
    ELOPlotter(QWidget *parent = Q_NULLPTR);

public slots:
    void addDataToPlot(int id, qreal x, qreal y);
    void setPlotColor(int id, const QColor &color);
    void clearPlot();
    void fitInPlots();
    void pause();
    void run();

signals:
    void plotColorChanged(int id, const QColor &color);

private slots:
    void onMousePressed(QMouseEvent *event);
    void onSelectionChanged();

private:
    void initPlotter();
    void initColorList();

    const QColor &nextDefaultColor() const;

    void updateRanges(qreal x, qreal y);

private:
    QMap<int, QCPGraph *> graphMap;
    QList<QColor> plotColors;
    bool fVerticalAutoScroll = true;
    qreal maxRangeY = 0.0;
};

#endif // ELOPLOTTER_H
