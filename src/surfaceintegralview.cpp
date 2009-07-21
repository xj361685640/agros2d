#include "surfaceintegralview.h"

SurfaceIntegralValueView::SurfaceIntegralValueView(QWidget *parent): QDockWidget(tr("Surface Integral"), parent)
{
    setMinimumWidth(280);
    setObjectName("SurfaceIntegralValueView");

    trvWidget = new QTreeWidget();
    trvWidget->setHeaderHidden(false);
    trvWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    trvWidget->setMouseTracking(true);
    trvWidget->setColumnCount(3);
    trvWidget->setColumnWidth(0, 150);
    trvWidget->setColumnWidth(1, 80);
    trvWidget->setColumnWidth(2, 20);

    QStringList labels;
    labels << tr("Label") << tr("Value") << tr("Unit");
    trvWidget->setHeaderLabels(labels);

    setWidget(trvWidget);
}

void SurfaceIntegralValueView::doShowSurfaceIntegral(SurfaceIntegralValue *surfaceIntegralValue)
{
    trvWidget->clear();

    // point
    QTreeWidgetItem *pointGeometry = new QTreeWidgetItem(trvWidget);
    pointGeometry->setText(0, tr("Geometry"));
    pointGeometry->setExpanded(true);

    addValue(pointGeometry, tr("Length:"), tr("%1").arg(surfaceIntegralValue->length, 0, 'e', 3), tr("m"));
    addValue(pointGeometry, tr("Surface:"), tr("%1").arg(surfaceIntegralValue->surface, 0, 'e', 3), tr("m2"));

    trvWidget->insertTopLevelItem(0, pointGeometry);

    if (Util::scene()->sceneSolution()->isSolved())
    {
        if (SurfaceIntegralValueElectrostatic *surfaceIntegralValueElectrostatic = dynamic_cast<SurfaceIntegralValueElectrostatic *>(surfaceIntegralValue))
            showElectrostatic(surfaceIntegralValueElectrostatic);
        if (SurfaceIntegralValueMagnetostatic *surfaceIntegralValueMagnetostatic = dynamic_cast<SurfaceIntegralValueMagnetostatic *>(surfaceIntegralValue))
            showMagnetostatic(surfaceIntegralValueMagnetostatic);
        if (SurfaceIntegralValueHeat *surfaceIntegralValueHeat = dynamic_cast<SurfaceIntegralValueHeat *>(surfaceIntegralValue))
            showHeat(surfaceIntegralValueHeat);
        if (SurfaceIntegralValueCurrent *surfaceIntegralValueCurrent = dynamic_cast<SurfaceIntegralValueCurrent *>(surfaceIntegralValue))
            showCurrent(surfaceIntegralValueCurrent);
    }
}

void SurfaceIntegralValueView::showElectrostatic(SurfaceIntegralValueElectrostatic *surfaceIntegralValueElectrostatic)
{
    // electrostatic
    QTreeWidgetItem *electrostaticNode = new QTreeWidgetItem(trvWidget);
    electrostaticNode->setText(0, tr("Electrostatic Field"));
    electrostaticNode->setExpanded(true);

    addValue(electrostaticNode, tr("Charge:"), tr("%1").arg(surfaceIntegralValueElectrostatic->surfaceCharge, 0, 'e', 3), tr("C"));
}

void SurfaceIntegralValueView::showMagnetostatic(SurfaceIntegralValueMagnetostatic *surfaceIntegralValueMagnetostatic)
{
    // electrostatic
    QTreeWidgetItem *magnetostaticNode = new QTreeWidgetItem(trvWidget);
    magnetostaticNode->setText(0, tr("Magnetostatic Field"));
    magnetostaticNode->setExpanded(true);

}

void SurfaceIntegralValueView::showHeat(SurfaceIntegralValueHeat *surfaceIntegralValueHeat)
{
    // heat
    QTreeWidgetItem *heatNode = new QTreeWidgetItem(trvWidget);
    heatNode->setText(0, tr("Heat Transfer"));
    heatNode->setExpanded(true);

    addValue(heatNode, tr("Temperature avg.:"), tr("%1").arg(surfaceIntegralValueHeat->averageTemperature, 0, 'e', 3), tr("C"));
    addValue(heatNode, tr("Temperature dif.:"), tr("%1").arg(surfaceIntegralValueHeat->temperatureDifference, 0, 'e', 3), tr("C"));
    addValue(heatNode, tr("Heat flux:"), tr("%1").arg(surfaceIntegralValueHeat->heatFlux, 0, 'e', 3), tr("C"));
}

void SurfaceIntegralValueView::showCurrent(SurfaceIntegralValueCurrent *surfaceIntegralValueCurrent)
{
    // current field
    QTreeWidgetItem *currentNode = new QTreeWidgetItem(trvWidget);
    currentNode->setText(0, tr("Current Field"));
    currentNode->setExpanded(true);

    addValue(currentNode, tr("Current:"), tr("%1").arg(surfaceIntegralValueCurrent->currentDensity, 0, 'e', 3), tr("A"));
}

void SurfaceIntegralValueView::addValue(QTreeWidgetItem *parent, QString name, QString text, QString unit)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(parent);
    item->setText(0, name);
    item->setText(1, text);
    item->setTextAlignment(1, Qt::AlignRight);
    item->setText(2, unit + " ");
    item->setTextAlignment(2, Qt::AlignLeft);
}

SurfaceIntegralValue::SurfaceIntegralValue()
{
    length = 0;
    surface = 0;
    for (int i = 0; i<Util::scene()->edges.length(); i++)
    {
        if (Util::scene()->edges[i]->isSelected)
        {
            length += Util::scene()->sceneSolution()->surfaceIntegral(i, PHYSICFIELDINTEGRAL_SURFACE_LENGTH);
            surface += Util::scene()->sceneSolution()->surfaceIntegral(i, PHYSICFIELDINTEGRAL_SURFACE_SURFACE);
        }
    }
}

QString SurfaceIntegralValue::toString()
{
    return ""; // QString::number(point.x, 'f', 5) + "; " + QString::number(point.y, 'f', 5);
}

// ****************************************************************************************************************

SurfaceIntegralValueElectrostatic::SurfaceIntegralValueElectrostatic() : SurfaceIntegralValue()
{
    surfaceCharge = 0;

    if (Util::scene()->sceneSolution()->isSolved())
    {
        for (int i = 0; i<Util::scene()->edges.length(); i++)
        {
            if (Util::scene()->edges[i]->isSelected)
            {
                surfaceCharge += Util::scene()->sceneSolution()->surfaceIntegral(i, PHYSICFIELDINTEGRAL_SURFACE_ELECTROSTATIC_CHARGE_DENSITY);
            }
        }
    }
}

QString SurfaceIntegralValueElectrostatic::toString()
{
    return ""; // QString::number(point.x, 'f', 5) + "; " + QString::number(point.y, 'f', 5);
}

// ****************************************************************************************************************

SurfaceIntegralValueMagnetostatic::SurfaceIntegralValueMagnetostatic() : SurfaceIntegralValue()
{
    if (Util::scene()->sceneSolution()->isSolved())
    {

    }
}

QString SurfaceIntegralValueMagnetostatic::toString()
{
    return ""; // QString::number(point.x, 'f', 5) + "; " + QString::number(point.y, 'f', 5);
}

// ****************************************************************************************************************

SurfaceIntegralValueHarmonicMagnetic::SurfaceIntegralValueHarmonicMagnetic() : SurfaceIntegralValue()
{
    if (Util::scene()->sceneSolution()->isSolved())
    {

    }
}

QString SurfaceIntegralValueHarmonicMagnetic::toString()
{
    return ""; // QString::number(point.x, 'f', 5) + "; " + QString::number(point.y, 'f', 5);
}

// ****************************************************************************************************************

SurfaceIntegralValueCurrent::SurfaceIntegralValueCurrent() : SurfaceIntegralValue()
{
    currentDensity = 0;

    if (Util::scene()->sceneSolution()->isSolved())
    {
        for (int i = 0; i<Util::scene()->edges.length(); i++)
        {
            if (Util::scene()->edges[i]->isSelected)
            {
                currentDensity += Util::scene()->sceneSolution()->surfaceIntegral(i, PHYSICFIELDINTEGRAL_SURFACE_CURRENT_CURRENT_DENSITY);
            }
        }
    }
}

QString SurfaceIntegralValueCurrent::toString()
{
    return ""; // QString::number(point.x, 'f', 5) + "; " + QString::number(point.y, 'f', 5);
}

// ****************************************************************************************************************

SurfaceIntegralValueHeat::SurfaceIntegralValueHeat() : SurfaceIntegralValue()
{
    averageTemperature = 0;
    temperatureDifference = 0;
    heatFlux = 0;

    if (Util::scene()->sceneSolution()->isSolved())
    {
        for (int i = 0; i<Util::scene()->edges.length(); i++)
        {
            if (Util::scene()->edges[i]->isSelected)
            {
                averageTemperature += Util::scene()->sceneSolution()->surfaceIntegral(i, PHYSICFIELDINTEGRAL_SURFACE_HEAT_TEMPERATURE);
                temperatureDifference += Util::scene()->sceneSolution()->surfaceIntegral(i, PHYSICFIELDINTEGRAL_SURFACE_HEAT_TEMPERATURE_DIFFERENCE);
                heatFlux += Util::scene()->sceneSolution()->surfaceIntegral(i, PHYSICFIELDINTEGRAL_SURFACE_HEAT_FLUX);
            }
        }

        if (length > 0)
        {
            averageTemperature /= length;
        }
    }
}

QString SurfaceIntegralValueHeat::toString()
{
    return ""; // QString::number(point.x, 'f', 5) + "; " + QString::number(point.y, 'f', 5);
}

// ***********************************************************************************************************************

SurfaceIntegralValue *surfaceIntegralValueFactory()
{
    switch (Util::scene()->problemInfo().physicField)
    {
    case PHYSICFIELD_ELECTROSTATIC:
        return new SurfaceIntegralValueElectrostatic();
        break;
    case PHYSICFIELD_MAGNETOSTATIC:
        return new SurfaceIntegralValueMagnetostatic();
        break;
    case PHYSICFIELD_HARMONIC_MAGNETIC:
        return new SurfaceIntegralValueHarmonicMagnetic();
        break;
    case PHYSICFIELD_HEAT_TRANSFER:
        return new SurfaceIntegralValueHeat();
        break;
    case PHYSICFIELD_CURRENT:
        return new SurfaceIntegralValueCurrent();
        break;
    default:
        cerr << "Physical field '" + physicFieldStringKey(Util::scene()->problemInfo().physicField).toStdString() + "' is not implemented. SurfaceIntegralValue *surfaceIntegralValueFactory()" << endl;
        throw;
        break;
    }
}
