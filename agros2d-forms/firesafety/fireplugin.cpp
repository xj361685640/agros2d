// This file is part of Agros2D.
//
// Agros2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Agros2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Agros2D.  If not, see <http://www.gnu.org/licenses/>.
//
// hp-FEM group (http://hpfem.org/)
// University of Nevada, Reno (UNR) and University of West Bohemia, Pilsen
// Email: agros2d@googlegroups.com, home page: http://hpfem.org/agros2d/

#include "fireplugin.h"

#include "scene.h"
#include "scenenode.h"

#include "util/constants.h"
#include "util/global.h"
#include "util/conf.h"
#include "util.h"

PropertyDialog::PropertyDialog()
{
    setModal(true);
    setWindowTitle(tr("Set properties"));
    createControls();
}

void PropertyDialog::createControls()
{
    txtHeight = new QLineEdit();
    txtFireLoad = new QLineEdit();
    txtPenalty = new QLineEdit();

    lstFireCurve = new QListWidget(this);
    lstFireCurve->setMouseTracking(true);
    lstFireCurve->setMaximumWidth(200);
    lstFireCurve->setMaximumHeight(50);
    QListWidgetItem *item = new QListWidgetItem("Fire Curve ISO");
    lstFireCurve->addItem(item);
    item = new QListWidgetItem("Fire Curve ISO");
    lstFireCurve->addItem(item);

    QGridLayout *layoutNameAndDescription = new QGridLayout();
    layoutNameAndDescription->addWidget(new QLabel(tr("Height:")), 0, 0);
    layoutNameAndDescription->addWidget(txtHeight, 0, 1);
    layoutNameAndDescription->addWidget(new QLabel(tr("Fire Load:")), 1, 0);
    layoutNameAndDescription->addWidget(txtFireLoad, 1, 1);
    layoutNameAndDescription->addWidget(new QLabel(tr("Penalty:")), 2, 0);
    layoutNameAndDescription->addWidget(txtPenalty, 2, 1);
    layoutNameAndDescription->addWidget(new QLabel(tr("Fire Curve:")), 3, 0);
    layoutNameAndDescription->addWidget(lstFireCurve, 3, 1);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));


    QVBoxLayout *layout = new QVBoxLayout();
    layout->addLayout(layoutNameAndDescription);
    layout->addWidget(buttonBox);
    layout->addStretch();

    setLayout(layout);
}

void PropertyDialog::getData(double & height, double & fireLoad, double & penalty, FireCurve & FireCurve)
{
    height = txtHeight->text().toDouble();;
    fireLoad = txtFireLoad->text().toDouble();
    penalty = txtPenalty->text().toDouble();

}

void PropertyDialog::setData(const double & height, const double & fireLoad, const double & penalty, const FireCurve & fireCurve) const
{
    txtHeight->setText(QString::number(height));
    txtFireLoad->setText(QString::number(fireLoad));
    txtPenalty->setText(QString::number(penalty));
    QListWidgetItem *item = new QListWidgetItem("Fire Curve ISO");
    if (fireCurve == FireCurve_ISO)
        lstFireCurve->setCurrentItem(item);
}

SceneViewFireSafety::SceneViewFireSafety(QWidget *parent) : SceneViewCommon2D(NULL, parent)
{
    this->propertyDialog = new PropertyDialog();
}

SceneViewFireSafety::~SceneViewFireSafety()
{
}


void SceneViewFireSafety::clear()
{
    SceneViewCommon2D::clear();
    SceneViewCommon::refresh();

    m_points.clear();

    doZoomBestFit();
}

void SceneViewFireSafety::refresh()
{
    foreach (SceneEdge *edge, m_properties.keys())
    {
        FireProperty prop =  m_properties[edge];
        FireSafety fs(prop);

        m_points[edge] = fs.calculateArea();
    }


    SceneViewCommon2D::refresh();
}

void SceneViewFireSafety::paintGL()
{
    if (!isVisible()) return;
    makeCurrent();

    glClearColor(COLORBACKGROUND[0], COLORBACKGROUND[1], COLORBACKGROUND[2], 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // geometry
    paintGeometry();

    // rulers
    if (Agros2D::configComputer()->value(Config::Config_ShowRulers).toBool()) paintRulers();

    // axes
    if (Agros2D::configComputer()->value(Config::Config_ShowAxes).toBool()) paintAxes();
}


void SceneViewFireSafety::resizeGL(int w, int h)
{
    SceneViewCommon::resizeGL(w, h);
}

void SceneViewFireSafety::keyPressEvent(QKeyEvent *event)
{
    SceneViewCommon2D::keyPressEvent(event);
}

void SceneViewFireSafety::mousePressEvent(QMouseEvent *event)
{
    SceneViewCommon2D::mousePressEvent(event);

    m_lastPos = event->pos();
    Point p = transform(Point(event->pos().x(), event->pos().y()));

    if (event->buttons() & Qt::LeftButton)
    {
        //  find edge marker
        SceneEdge *edge = SceneEdge::findClosestEdge(p);

        edge->setSelected(!edge->isSelected());
        updateGL();

        emit mousePressed();
    }
}

void SceneViewFireSafety::mouseDoubleClickEvent(QMouseEvent *event)
{
    SceneViewCommon2D::mouseDoubleClickEvent(event);

    m_lastPos = event->pos();
    Point p = transform(Point(event->pos().x(), event->pos().y()));

    if (event->buttons() & Qt::LeftButton)
    {
        //  find edge marker
        SceneEdge *edge = SceneEdge::findClosestEdge(p);

        double pv = 90;
        double height = edge->length() / 2;
        double penalty = 0;
        FireCurve fireCurve = FireCurve_ISO;

        if(!this->hasProperty(edge))
        {
            propertyDialog->setData(height, pv, penalty, fireCurve);
            int dialogCode = propertyDialog->exec();
            if(dialogCode == QDialog::Accepted) {


                propertyDialog->getData(height, pv, penalty, fireCurve);
                this->setProperty(edge, FireProperty(edge->length(), height,
                                                     pv + penalty, FireCurve_ISO, 18500, 1.0, 0.0));
            }
        }
        else
        {
            this->removeProperty(edge);
        }

        edge->setSelected(!edge->isSelected());
        refresh();

        emit mousePressed();
    }
}

void SceneViewFireSafety::paintGeometry()
{
    loadProjection2d(true);

    // edges
    foreach (SceneEdge *edge, Agros2D::scene()->edges->items())
    {
        if (edge->isSelected())
        {
            // selected line
            glColor3d(COLORSELECTED[0], COLORSELECTED[1], COLORSELECTED[2]);
            glLineWidth(3.0);
        }
        else
        {
            glColor3d(COLOREDGE[0], COLOREDGE[1], COLOREDGE[2]);
            glLineWidth(EDGEWIDTH);
        }

        if (edge->isStraight())
        {
            glBegin(GL_LINES);
            glVertex2d(edge->nodeStart()->point().x, edge->nodeStart()->point().y);
            glVertex2d(edge->nodeEnd()->point().x, edge->nodeEnd()->point().y);
            glEnd();

            foreach (SceneEdge *edge, m_properties.keys())
            {
                // fire safety
                Point dvector = edge->vector();
                Point normalVector(dvector.y / edge->length(), - dvector.x / edge->length());

                Point center((edge->nodeStart()->point().x + edge->nodeEnd()->point().x) / 2,
                             (edge->nodeStart()->point().y + edge->nodeEnd()->point().y) / 2);

                // blended rectangle
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                glColor4d(COLORCROSSED[0], COLORCROSSED[1], COLORCROSSED[2], 0.4);
                glBegin(GL_POLYGON);
                for (int i = 0; i < m_points[edge].count(); i++)
                {
                    EnvelopePoint point1 = m_points[edge][i];

                    // left point
                    Point pl1(center.x - point1.position * dvector.x / edge->length() + point1.distance * normalVector.x,
                              center.y - point1.position * dvector.y / edge->length() + point1.distance * normalVector.y);
                    glVertex2d(pl1.x, pl1.y);
                }

                for (int i = m_points[edge].count() - 1; i >=0; i--)
                {
                    EnvelopePoint point1 = m_points[edge][i];

                    // right point
                    Point pr1(center.x + point1.position * dvector.x / edge->length() + point1.distance * normalVector.x,
                              center.y + point1.position * dvector.y / edge->length() + point1.distance * normalVector.y);
                    glVertex2d(pr1.x, pr1.y);
                }
                glEnd();

                glDisable(GL_BLEND);
            }
        }
        else
        {
            Point center = edge->center();
            double radius = edge->radius();
            double startAngle = atan2(center.y - edge->nodeStart()->point().y, center.x - edge->nodeStart()->point().x) / M_PI*180 - 180;

            drawArc(center, radius, startAngle, edge->angle());
        }

        glLineWidth(1.0);
    }
}

// ***************************************************************************************************************

ToolFireSafety::ToolFireSafety(QWidget *parent) : ToolInterface(parent)
{
    setWindowTitle(tr("Fire Safety"));
    setModal(true);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    // connect(buttonBox, SIGNAL(accepted()), this, SLOT(acceptForm()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));

    sceneViewFireSafety = new SceneViewFireSafety(this);

    treeWindows = new QTreeWidget(this);

    QHBoxLayout *layoutMain = new QHBoxLayout();
    layoutMain->addWidget(treeWindows);
    layoutMain->addWidget(sceneViewFireSafety, 1);

    QVBoxLayout *layoutAll = new QVBoxLayout();
    layoutAll->addLayout(layoutMain, 1);
    layoutAll->addWidget(buttonBox);

    setLayout(layoutAll);
    setMinimumSize(800, 450);

    QSettings settings;
    restoreGeometry(settings.value("ToolFireSafety/Geometry", saveGeometry()).toByteArray());

    sceneViewFireSafety->doZoomBestFit();
}

ToolFireSafety::~ToolFireSafety()
{
    QSettings settings;
    settings.setValue("ToolFireSafety/Geometry", saveGeometry());
}

void ToolFireSafety::keyPressEvent(QKeyEvent *event)
{
    if (event->key() != Qt::Key_Escape)
        QDialog::keyPressEvent(event);
}


int ToolFireSafety::show()
{
    this->showMaximized();
    sceneViewFireSafety->refresh();
    sceneViewFireSafety->showMaximized();
    return exec();
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(tool_fire_safety, ToolFireSafety)
#endif
