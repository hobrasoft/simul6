#include "graf.h"
#include "omp.h"
#include "pdebug.h"
#include "constituentseries.h"
#include "conductivityseries.h"
#include "phseries.h"
#include "electricfieldseries.h"
#include "grafdetail.h"
#include "msettings.h"
#include <iostream>
#include <cmath>
#include <math.h>
#include <QValueAxis>
#include <QTimer>
#include "grafstyle.h"
#include "manualscale.h"

#define E_COLOR     "#00ff00"
#define PH_COLOR    "#0000ff"
#define KAPPA_COLOR "#000000"

Graf::Graf(QWidget *parent) : QChartView(parent)
{
    if (MSETTINGS->guiChartAntialiasing()) {
        setRenderHint(QPainter::Antialiasing);
        }
    setRubberBand(QChartView::RectangleRubberBand);
    m_chart = new QChart();
    setChart(m_chart);
    m_rescaleIndividually = false;
    m_rescalePh = false;
    m_rescaleKapa = false;
    m_rescaleE = false;
    m_visiblePh = true;
    m_visibleKapa = true;
    m_visibleE = true;
    m_engine = nullptr;
    m_axis_x = nullptr;
    m_axis_y = nullptr;

//  connect(m_chart, &QChart::plotAreaChanged, this, &Graf::subselected);
//  connect(m_chart, &QChart::geometryChanged, this, &Graf::subselected);

    m_actionRescale = new QAction(tr("Auto scale"), this);
    connect(m_actionRescale, &QAction::triggered, this, &Graf::autoscale);
    addAction(m_actionRescale);
    m_actionRescale->setEnabled(false);

    m_actionManualScale = new QAction(tr("Manual scale"), this);
    connect(m_actionManualScale, &QAction::triggered, this, &Graf::manualScale);
    addAction(m_actionManualScale);
    m_actionManualScale->setEnabled(false);

    #ifdef SET_AXIS_LABELS_MANUALLY
    m_actionSetAxisLabels = new QAction(tr("Adjust axis labels"), this);
    connect(m_actionSetAxisLabels, &QAction::triggered, this, &Graf::setAxisLabels);
    addAction(m_actionSetAxisLabels);
    m_actionSetAxisLabels->setEnabled(false);
    #endif

    m_actionApplyNiceNumbers = new QAction(tr("Apply nice numbers to axes"), this);
    connect(m_actionApplyNiceNumbers, &QAction::triggered, this, &Graf::applyNiceNumbers);
    addAction(m_actionApplyNiceNumbers);
    m_actionApplyNiceNumbers->setEnabled(false);

    setContextMenuPolicy(Qt::ActionsContextMenu);
    m_rescaleEnabled = true;
    setMouseTracking(true);
}


void Graf::init(const Engine *pEngine) {
    disconnect(m_chart, &QChart::plotAreaChanged, this, &Graf::subselected);
    m_chart->removeAllSeries();

    m_engine = pEngine;
    pEngine->lock();
    size_t p = pEngine->getNp(); // points
    double inc_x = pEngine->getCapLen() / p;

    int id = 0;
    for (auto &sample : pEngine->getMix().getSamples()) {
        ConstituentSeries *series = new ConstituentSeries(sample, this);
        connect(series, &ConstituentSeries::clicked, this, &Graf::seriesClicked);
        m_chart->addSeries(series);

        double x = 0;
        for (unsigned int i = 0; i <= p; i++){
            series->append(QPointF(x * 1000.0, sample.getA(0, i)));
            x += inc_x;
            }
        id += 1;
        }

    ConstituentSeries *series = new PhSeries(this);
    connect(series, &ConstituentSeries::clicked, this, &Graf::seriesClicked);
    series->setName(tr("pH"));
    series->setUseOpenGL(true);
    series->setVisible(m_visiblePh);
    QBrush phbrush = series->brush();
    phbrush.setColor(PH_COLOR);
    QPen phpen = series->pen();
    phpen.setColor(PH_COLOR);
    phpen.setWidthF(PENWIDTH);
    series->setPen(phpen);
    series->setBrush(phbrush);
    double x = 0;
    auto hpl = pEngine->getHpl();
    for (unsigned int i = 0; i <= p; i++){
        if (hpl[i] > 0) {
            double pH = -log(hpl[i]) / log(10);
            series->append(QPointF(x * 1000.0, pH));
            }
        x += inc_x;
        }
    m_chart->addSeries(series);

    series = new ConductivitySeries(this);
    connect(series, &ConstituentSeries::clicked, this, &Graf::seriesClicked);
    series->setName(tr("κ"));
    series->setUseOpenGL(true);
    series->setVisible(m_visibleKapa);
    QBrush kappabrush = series->brush();
    kappabrush.setColor(KAPPA_COLOR);
    QPen kappapen = series->pen();
    kappapen.setColor(KAPPA_COLOR);
    kappapen.setWidthF(PENWIDTH);
    series->setPen(kappapen);
    series->setBrush(kappabrush);
    x = 0;
    auto kapa = pEngine->getKapa();
    for (unsigned int i = 0; i <= p; i++){
        series->append(QPointF(x * 1000.0, kapa[i]*100.0));
        x += inc_x;
        }
    m_chart->addSeries(series);

    series = new ElectricFieldSeries(this);
    connect(series, &ConstituentSeries::clicked, this, &Graf::seriesClicked);
    series->setName(tr("Electric field"));
    series->setUseOpenGL(true);
    series->setVisible(m_visibleE);
    QBrush efieldbrush = series->brush();
    efieldbrush.setColor(E_COLOR);
    QPen efieldpen = series->pen();
    efieldpen.setColor(E_COLOR);
    efieldpen.setWidthF(PENWIDTH);
    series->setPen(efieldpen);
    series->setBrush(efieldbrush);
    x = 0;
    auto efield = pEngine->getE();
    for (unsigned int i = 0; i <= p; i++){
        series->append(QPointF(x * 1000.0, fabs(efield[i]/1000.0)));
        x += inc_x;
        }
    m_chart->addSeries(series);

    pEngine->unlock();
    m_chart->legend()->setVisible(false);

    setAxisLabels();
//  autoscale();
    QTimer::singleShot(1000, this, [this]() { connect(m_chart, &QChart::plotAreaChanged, this, &Graf::subselected); });
}


void Graf::autoscale() {
    disconnect(m_chart, &QChart::plotAreaChanged, this, &Graf::subselected);
    if (m_engine == nullptr) { return; }
    double maximum = 0;
    double minimum = 99999;
    m_engine->lock();
    size_t p    = m_engine->getNp();
    auto hpl    = m_engine->getHpl();
    auto mix    = m_engine->getMix();
    auto kapa   = m_engine->getKapa();
    auto efield = m_engine->getE();
    double mcaplen = 1000 * m_engine->getCapLen();
    m_engine->unlock();

    unsigned int xleft = 0;
    unsigned int xright = p;
    if (m_rescaleIndividually && m_axis_x != nullptr) {
        double d_xleft  = m_axis_x->min();
        double d_xright = m_axis_x->max();
        xleft = p * d_xleft /mcaplen;
        xright = p * d_xright /mcaplen;
        }

    for (auto &sample : mix.getSamples()) {
        if (!sample.visible()) { continue; }
        if (m_rescaleIndividually && sample.getId() != m_rescaleId) { continue; }
        for (unsigned int i = xleft; i <= xright; i++){
            if (sample.getA(0, i) > maximum)  {
                maximum = sample.getA(0,i);
                }
            if (sample.getA(0, i) < minimum)  {
                minimum = sample.getA(0,i);
                }
            }
        }

    bool rescalePh = (m_rescaleIndividually && m_rescalePh && m_visiblePh) ||
                     (!m_rescaleIndividually && m_visiblePh);
    for (unsigned int i = xleft; rescalePh && i <= xright; i++) {
        if (hpl[i] > 0) {
            double pH = -log(hpl[i]) / log(10);
            if (pH > maximum) { 
                maximum = pH;
                }
            if (pH < minimum) { 
                minimum = pH;
                }
            }
        }

    bool rescaleKapa = (m_rescaleIndividually && m_rescaleKapa && m_visibleKapa) ||
                       (!m_rescaleIndividually && m_visibleKapa);
    for (unsigned int i = xleft; rescaleKapa && i <= xright; i++) {
        if (kapa[i] * 100 > maximum) {
            maximum = kapa[i] * 100.0;
            }
        if (kapa[i] * 100 < minimum) {
            minimum = kapa[i] * 100.0;
            }
        }

    bool rescaleE = (m_rescaleIndividually && m_rescaleE && m_visibleE) ||
                    (!m_rescaleIndividually && m_visibleE);
    for (unsigned int i = xleft; rescaleE && i <= xright; i++) {
        if (fabs(efield[i] / 1000.0) > maximum) {
            maximum = fabs(efield[i] / 1000.0);
            }
        if (fabs(efield[i] / 1000.0) < minimum) {
            minimum = fabs(efield[i] / 1000.0);
            }
        }

    QRectF rect;
    rect.setTop    (minimum - 0.09 * maximum);
    rect.setBottom (1.09 * maximum);
    rect.setLeft   (0);
    rect.setRight  (mcaplen);
    setScale(rect.normalized());

    if (m_rescaleIndividually && m_axis_x != nullptr) {
        double d_xleft  = m_axis_x->min();
        double d_xright = m_axis_x->max();
        rect.setLeft (d_xleft);
        rect.setRight(d_xright);
        }
    repaint();
    QTimer::singleShot(1000, this, [this]() { connect(m_chart, &QChart::plotAreaChanged, this, &Graf::subselected); });
}


void Graf::rescalePh() {
    m_rescaleIndividually = true;
    m_rescalePh = true;
    autoscale();
    m_rescalePh = false;
    m_rescaleIndividually = false;
}

void Graf::rescaleE() {
    m_rescaleIndividually = true;
    m_rescaleE = true;
    autoscale();
    m_rescaleE = false;
    m_rescaleIndividually = false;
}

void Graf::rescaleKapa() {
    m_rescaleIndividually = true;
    m_rescaleKapa = true;
    autoscale();
    m_rescaleKapa = false;
    m_rescaleIndividually = false;
}

void Graf::rescale(int internalId) {
    m_rescaleIndividually = true;
    m_rescaleId = internalId;
    autoscale();
    m_rescaleId = 0;
    m_rescaleIndividually = false;
}




void Graf::manualScale() {
    double caplen = 100;
    if (m_axis_y == nullptr || m_axis_y == nullptr) {
        return;
        }
    if (m_engine != nullptr) {
        m_engine->lock();
        caplen =  m_engine->getCapLen() * 1000;
        m_engine->unlock();
        }

    QRectF rect;
    rect.setBottom(m_axis_y->min());
    rect.setTop(m_axis_y->max());
    rect.setLeft(m_axis_x->min());
    rect.setRight(m_axis_x->max());

    ManualScale *d = new ManualScale(this);
    d->setRect(rect);
    d->setCaplen(caplen);
    if (d->exec() == QDialog::Accepted) {
        setScale(d->rect().normalized());
        }
    d->deleteLater();
}


// prevents the context menu 
void Graf::mouseReleaseEvent(QMouseEvent *event) { 
    if (event->button() != Qt::RightButton) { 
        QChartView::mouseReleaseEvent(event);
        return; 
        }
}


void Graf::setAxisLabels() {
    if (m_axis_y == nullptr || m_axis_x == nullptr) { 
        return;
        }
    QPointF topleft(m_axis_x->min(), m_axis_y->min());
    QPointF rightbottom(m_axis_x->max(), m_axis_y->max());
    QRectF rect(topleft, rightbottom);
    setScale(rect.normalized());
}


void Graf::applyNiceNumbers() {
    if (m_axis_y == nullptr || m_axis_x == nullptr) { return; }
    m_axis_y->applyNiceNumbers();
    m_axis_x->applyNiceNumbers();
}


void Graf::subselected() {
    if (m_axis_x == nullptr) { return; }
    if (m_axis_y == nullptr) { return; }

    QRectF  rect        = m_chart->geometry(); // m_chart->plotArea();
    QPointF topLeft     = m_chart->mapToValue(rect.topLeft());
    QPointF bottomRight = m_chart->mapToValue(rect.bottomRight());
    QRectF rext(topLeft, bottomRight);
    setScale(rext.normalized());

}


void Graf::setScale(const QRectF& rect) {
    PDEBUG << rect;
    if (rect.isNull()) {
        return;
        }
    if (rect.height() <= 0 || rect.width() <= 0) {
        PDEBUG << "Invalid rect dimensions" << rect << rect.height() << rect.width();
        return;
        }
    Q_ASSERT (m_engine != nullptr);
    if (m_engine == nullptr) {
        return;
        }

    m_actionManualScale->setEnabled(true);
    m_actionRescale->setEnabled(true);
    m_actionApplyNiceNumbers->setEnabled(true);
    #ifdef SET_AXIS_LABELS_MANUALLY
    m_actionSetAxisLabels->setEnabled(true);
    #endif

    QList<QAbstractSeries *> serieslist = m_chart->series();
    for (int i=0; i<serieslist.size(); i++) {
        QList<QAbstractAxis *> axes = serieslist[i]->attachedAxes();
        for (int a=0; a<axes.size(); a++) {
            serieslist[i]->detachAxis(axes[a]);
            }
        }
    QList<QAbstractAxis *> axislistX = m_chart->axes(Qt::Horizontal);
    for (int i=0; i<axislistX.size(); i++) {
        m_chart->removeAxis(axislistX[i]);
        }
    QList<QAbstractAxis *> axislistY = m_chart->axes(Qt::Vertical);
    for (int i=0; i<axislistY.size(); i++) {
        m_chart->removeAxis(axislistY[i]);
        }

    if (m_axis_x != nullptr) { m_axis_x->deleteLater(); }
    if (m_axis_y != nullptr) { m_axis_y->deleteLater(); }

    m_axis_y = new QValueAxis(this);
    m_axis_x = new QValueAxis(this);
    m_axis_y->setRange(rect.top(), rect.bottom());
    m_axis_x->setRange(rect.left(), rect.right());

    #if QT_VERSION > 0x050c00
    // Not supported in Qt < 5.12
    double ix = pow(10, round(log10(rect.width() / 8)));
    double iy = pow(10, round(log10(rect.height() / 8)));
    m_axis_x->setTickInterval(ix);
    m_axis_y->setTickInterval(iy);

    double anchor_x = (rect.left() <= 0) ? 0 : pow(10, round(log10(rect.left())) );
    double anchor_y = (rect.bottom() <= 0) ? 0 : pow(10, round(log10(rect.bottom())) );
    m_axis_x->setTickAnchor(anchor_x);
    m_axis_y->setTickAnchor(anchor_y);
    m_axis_x->setTickType(QValueAxis::TicksDynamic);
    m_axis_y->setTickType(QValueAxis::TicksDynamic);
    #endif

    m_chart->addAxis(m_axis_x, Qt::AlignBottom);
    m_chart->addAxis(m_axis_y, Qt::AlignLeft);
    for (int i=0; i<serieslist.size(); i++) {
        serieslist[i]->attachAxis(m_axis_y);
        serieslist[i]->attachAxis(m_axis_x);
        }

}


void Graf::setVisible(int id, bool visible) {
    QList<QAbstractSeries *> list = m_chart->series();
    for (int i=0; i<list.size(); i++) {
        ConstituentSeries *series = qobject_cast<ConstituentSeries *>(m_chart->series()[i]);
        if (series->internalId() == id) {
            series->setVisible(visible);
            // autoscale();
            return;
            }
        }
}


void Graf::setVisiblePh(bool visible) {
    m_visiblePh = visible;
    QList<QAbstractSeries *> list = m_chart->series();
    int i = list.size()-3;
    if (i<0) { return; }
    list[i]->setVisible(visible);
    // autoscale();
}


void Graf::setVisibleKapa(bool visible) {
    m_visibleKapa = visible;
    QList<QAbstractSeries *> list = m_chart->series();
    int i = list.size()-2;
    if (i<0) { return; }
    list[i]->setVisible(visible);
    // autoscale();
}


void Graf::setVisibleE(bool visible) {
    m_visibleE = visible;
    QList<QAbstractSeries *> list = m_chart->series();
    int i = list.size()-1;
    if (i<0) { return; }
    list[i]->setVisible(visible);
    // autoscale();
}


void Graf::drawGraph(const Engine *pEngine)
{
    pEngine->lock(); 
    size_t p = pEngine->getNp(); // points
    QLineSeries *series;

    int id = 0;
    double inc_x = pEngine->getCapLen() / p;
    for (auto &sample : pEngine->getMix().getSamples()) {
        series = qobject_cast<QLineSeries *>(m_chart->series()[id]);
        // PDEBUG << sample.getName() << sample.getInternalId();
        double x = 0;
        QList<double> vlist;
        QVector<QPointF> plist;
        for (unsigned int i = 0; i <= p; i++){
            plist << QPointF(x * 1000.0, sample.getA(0, i));
            vlist << x;
            x += inc_x;
            }
        series->replace(plist);
        series->setVisible(sample.visible());
        id += 1;
        }

    series = qobject_cast<QLineSeries *>(m_chart->series()[id]);
    double x = 0;
    auto hpl = pEngine->getHpl();
    QVector<QPointF> plist;
    for (unsigned int i = 0; i <= p; i++) {
        double pH = (hpl[i] > 0) ? (-log(hpl[i]) / log(10)) : 0;
        plist << QPointF(x * 1000.0, pH);
        x += inc_x;
        }
    series->replace(plist);
    id += 1;

    x = 0;
    series = qobject_cast<QLineSeries *>(m_chart->series()[id]);
    auto kapa = pEngine->getKapa();
    plist.clear();;
    for (unsigned int i = 0; i <= p; i++) {
        plist << QPointF(x * 1000.0, kapa[i]*100.0);
        x += inc_x;
        }
    series->replace(plist);
    id += 1;

    x = 0;
    series = qobject_cast<QLineSeries *>(m_chart->series()[id]);
    auto efield = pEngine->getE();
    plist.clear();;
    for (unsigned int i = 0; i <= p; i++) {
        plist << QPointF(x * 1000.0, fabs(efield[i]/1000.0));
        x += inc_x;
        }
    series->replace(plist);
    id += 1;

    pEngine->unlock(); 
}


void Graf::seriesClicked(const QPointF& point) {
    // PDEBUG;
    QLineSeries *s1 = qobject_cast<QLineSeries *>(sender());
    if (s1 == nullptr) { return; }
    if (m_engine == nullptr) { return; }

    m_engine->lock();
    QPoint position = mapToGlobal(QPoint(15,15));
    double caplen   = m_engine->getCapLen() * 1000.0;
    int    np       = m_engine->getNp();
    int    node     = (np+1) * point.x() / caplen;
    double x        = ((double)node) * (caplen / ((double)np));
    double y        = point.y();
    auto   hpl      = m_engine->getHpl();
    auto   mix      = m_engine->getMix();
    auto   kapa     = m_engine->getKapa();
    auto   efield   = m_engine->getE();
    m_engine->unlock();

    ConstituentSeries *s2 = qobject_cast<ConstituentSeries *>(s1);
    if (s2 != nullptr && s2->internalId() != 0) {
        double minimumd = 1e99;
        double minimumy = 1e99;
        m_engine->lock();
        for (auto &sample : m_engine->getMix().getSamples()) {
            double sample_y = sample.getA(0, node);
            double distance = fabs(y - sample_y);
            if (distance < minimumd) {
                minimumd = distance;
                minimumy = sample_y;
                }
            }
        m_engine->unlock();
        PDEBUG << s2->name() << s2->internalId();
        GrafDetail *d = new GrafDetail(this, s2->name(), "mM", x, minimumy, node);
        d->move(position);
        d->show();
        connect(d, &QObject::destroyed, s2, &ConstituentSeries::setNormalWidth);
        return;
        }

    int seriescount = m_chart->series().size();
    if (s1 == m_chart->series()[seriescount-3]) {
        double pH = -log(hpl[node]) / log(10);
        GrafDetail *d = new GrafDetail(this, tr("pH"), "", x, pH, node);
        d->move(position);
        d->show();
        connect(d, &QObject::destroyed, s2, &ConstituentSeries::setNormalWidth);
        return;
        }

    if (s1 == m_chart->series()[seriescount-2]) {
        double k = kapa[node] * 1000.0;
        GrafDetail *d = new GrafDetail(this, tr("Conductivity"), "mS/m", x, k, node);
        d->move(position);
        d->show();
        connect(d, &QObject::destroyed, s2, &ConstituentSeries::setNormalWidth);
        return;
        }

    if (s1 == m_chart->series()[seriescount-1]) {
        double e = efield[node];
        GrafDetail *d = new GrafDetail(this, tr("Electric field"), "V/m", x, e, node);
        d->move(position);
        d->show();
        connect(d, &QObject::destroyed, s2, &ConstituentSeries::setNormalWidth);
        return;
        }

}

