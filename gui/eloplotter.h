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
    void clearPlot();
    void setPlotColor(int id, const QColor &color);

signals:
    void plotColorChanged(int id, const QColor &color);

private:
    void initPlotter();
    void initColorList();

    const QColor &nextDefaultColor() const;

    void updateRanges(qreal x, qreal y);

private:
    QMap<int, QCPGraph *> graphMap;
    QList<QColor> plotColors;

};

#endif // ELOPLOTTER_H
