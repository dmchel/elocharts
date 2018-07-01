import QtQuick 2.9
import QtQuick.Window 2.2
import QtCharts 2.2

Window {
    id: mainWindow
    visible: true
    width: 800
    height: 480
    title: qsTr("ELO Charts")

    ChartView {
        id: chartView
        title: "Chart"
        anchors.fill: parent
        antialiasing: true

        ValueAxis {
            id: axisY1
            min: -1
            max: 4
        }

        ValueAxis {
            id: axisX
            min: 0
            max: 1024
        }

        LineSeries {
            id: lineSeries1
            name: "signal 1"
            axisX: axisX
            axisY: axisY1
            useOpenGL: chartView.openGL
        }
    }

    function addDataToLineSeries(x, y) {
        lineSeries1.append(x, y);
        var seriesData = chartView.series(0);
        seriesData.append(x, y);
        chartView.removeSeries(chartView.series(0));
        chartView.createSeries()
        chartView.series(0).append(x, y);
        chartView.update();
    }

    function removeDataFromLineSeries(x, y) {
        //lines.remove(x, y);
        chartView.update();
    }

    function clearLineSeries() {
        //lines.clear();
        chartView.update();
    }
}
