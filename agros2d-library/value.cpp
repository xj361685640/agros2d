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

#include "value.h"

#include "util/global.h"
#include "logview.h"
#include "pythonlab/pythonengine_agros.h"
#include "hermes2d/problem_config.h"
#include "parser/lex.h"

Value::Value(double value)
    : m_isEvaluated(true), m_isTimeDependent(false), m_isCoordinateDependent(false), m_time(0.0), m_point(Point()), m_table(DataTable()), m_problem(Agros2D::problem())
{
    m_text = QString::number(value);
    m_number = value;      
}

Value::Value(double value, std::vector<double> x, std::vector<double> y, DataTableType type, bool splineFirstDerivatives, bool extrapolateConstant)
    : m_isEvaluated(true), m_isTimeDependent(false), m_isCoordinateDependent(false), m_time(0.0), m_point(Point()), m_table(DataTable()), m_problem(Agros2D::problem())
{
    assert(x.size() == y.size());

    m_text = QString::number(value);
    m_number = value;
    m_table.setValues(x, y);
    m_table.setType(type);
    m_table.setSplineFirstDerivatives(splineFirstDerivatives);
    m_table.setExtrapolateConstant(extrapolateConstant);       
}

Value::Value(const QString &value)
    : m_isEvaluated(false), m_isTimeDependent(false), m_isCoordinateDependent(false), m_time(0.0), m_point(Point()), m_table(DataTable()), m_problem(Agros2D::problem())
{
    parseFromString(value.isEmpty() ? "0" : value);
    evaluateAndSave();
}

Value::Value(const QString &value, std::vector<double> x, std::vector<double> y, DataTableType type, bool splineFirstDerivatives, bool extrapolateConstant)
    : m_isEvaluated(false), m_isTimeDependent(false), m_isCoordinateDependent(false), m_time(0.0), m_point(Point()), m_table(DataTable()), m_problem(Agros2D::problem())
{
    assert(x.size() == y.size());

    parseFromString(value.isEmpty() ? "0" : value);
    m_table.setValues(x, y);
    m_table.setType(type);
    m_table.setSplineFirstDerivatives(splineFirstDerivatives);
    m_table.setExtrapolateConstant(extrapolateConstant);
    evaluateAndSave();
}

Value::Value(const QString &value, const DataTable &table)
    : m_isEvaluated(false), m_isTimeDependent(false), m_isCoordinateDependent(false), m_time(0.0), m_point(Point()), m_table(table), m_problem(Agros2D::problem())
{
    parseFromString(value.isEmpty() ? "0" : value);
}

Value::Value(const Value &origin)
{
    *this = origin;
    evaluateAndSave();
//    qDebug() << "Copy Value" << this->m_text << ", " << this->m_number;
}

Value& Value::operator =(const Value &origin)
{
    m_problem = origin.m_problem;

    m_text = origin.m_text;
    m_time = origin.m_time;
    m_point = origin.m_point;
    m_isTimeDependent = origin.m_isTimeDependent;
    m_isCoordinateDependent = origin.m_isCoordinateDependent;
    m_table = origin.m_table;

    evaluateAndSave();

    return *this;
//    qDebug() << "operator= Value" << this->m_text << ", " << this->m_number;
}

Value::~Value()
{
    m_table.clear();
}

bool Value::isNumber()
{
    bool isInt = false;
    text().toInt(&isInt);
    if (isInt)
        return true;

    bool isDouble = false;
    text().toDouble(&isDouble);

    if (isDouble)
        return true;

    return false;
}

bool Value::hasTable() const
{
    return (!m_table.isEmpty());
}

bool Value::evaluateAtPoint(const Point &point)
{
    m_point = point;
    return evaluateAndSave();
}

bool Value::evaluateAtTime(double time)
{
    m_time = time;
    return evaluateAndSave();
}

bool Value::evaluateAtTimeAndPoint(double time, const Point &point)
{
    m_time = time;
    m_point = point;
    return evaluateAndSave();
}

void Value::setNumber(double value)
{
    setText(QString::number(value));
}

double Value::number() const
{
    assert(m_isEvaluated);
    return m_number;
}

double Value::numberAtPoint(const Point &point) const
{
    double result;

    // force evaluate
    evaluate(0, point, result);

    return result;
}

double Value::numberAtTime(double time) const
{
    double result;

    // force evaluate
    evaluate(time, Point(), result);

    return result;
}

double Value::numberAtTimeAndPoint(double time, const Point &point) const
{
    double result;

    // force evaluate
    evaluate(time, point, result);

    return result;
}

double Value::numberFromTable(double key) const
{
    if (m_problem->isNonlinear() && hasTable())
        return m_table.value(key);
    else
        return number();
}

Hermes::Ord Value::numberFromTable(Hermes::Ord key) const
{
    return Hermes::Ord(1);
}

double Value::derivativeFromTable(double key) const
{
    if (m_problem->isNonlinear() && hasTable())
        return m_table.derivative(key);
    else
        return 0.0;
}

Hermes::Ord Value::derivativeFromTable(Hermes::Ord key) const
{
    return Hermes::Ord(1);
}

void Value::setText(const QString &str)
{
    m_isEvaluated = false;
    m_text = str;

    m_isTimeDependent = false;
    m_isCoordinateDependent = false;

    LexicalAnalyser lex;

    // ToDo: Improve
    try
    {
        lex.setExpression(m_text);
    }

    catch(ParserException e)
    {
        // Nothing to do at this point.
    }

    foreach (Token token, lex.tokens())
    {
        if (token.type() == ParserTokenType_VARIABLE)
        {
            if (token.toString() == "time")
                m_isTimeDependent = true;
            if (m_problem->config()->coordinateType() == CoordinateType_Planar)
            {
                if (token.toString() == "x" || token.toString() == "y")
                    m_isCoordinateDependent = true;
            }
            else
            {
                if (token.toString() == "r" || token.toString() == "z")
                    m_isCoordinateDependent = true;
            }
        }
    }

    evaluateAndSave();
}

QString Value::toString() const
{
    if (m_table.isEmpty())
        return m_text;
    else
        return m_text + ";" + m_table.toString();
}

void Value::parseFromString(const QString &str)
{
    if (str.contains(";"))
    {
        // string and table
        QStringList lst = str.split(";");
        this->setText(lst.at(0));

        if (lst.size() > 2)
        {
            // try
            {
                m_table.fromString((lst.at(1) + ";" + lst.at(2)));
            }
            // catch (...)
            {
                // do nothing
            }
        }
    }
    else
    {
        // just string
        this->setText(str);
    }
}

bool Value::evaluate(double time, const Point &point, double& result) const
{
    return evaluateExpression(m_text, time, point, result);
}

bool Value::evaluateAndSave()
{
    m_isEvaluated = false;
    m_isEvaluated = evaluateExpression(m_text, m_time, m_point, m_number);
    return m_isEvaluated;
}

bool Value::evaluateExpression(const QString &expression, double time, const Point &point, double &evaluationResult) const
{
    // speed up - int number
    bool isInt = false;
    double numInt = expression.toInt(&isInt);
    if (isInt)
    {
        evaluationResult = numInt;
        return true;
    }

    // speed up - double number
    bool isDouble = false;
    double numDouble = expression.toDouble(&isDouble);
    if (isDouble)
    {
        evaluationResult = numDouble;
        return true;
    }

    bool signalBlocked = currentPythonEngineAgros()->signalsBlocked();
    currentPythonEngineAgros()->blockSignals(true);

    QString command;

    if (m_isCoordinateDependent && !m_isTimeDependent)
    {
        if (m_problem->config()->coordinateType() == CoordinateType_Planar)
            command = QString("x = %1; y = %2").arg(point.x).arg(point.y);
        else
            command = QString("r = %1; z = %2").arg(point.x).arg(point.y);
    }

    if (m_isTimeDependent && !m_isCoordinateDependent)
    {
        command = QString("time = %1").arg(time);
    }

    if (m_isCoordinateDependent && m_isTimeDependent)
    {
        if (m_problem->config()->coordinateType() == CoordinateType_Planar)
            command = QString("time = %1; x = %2; y = %3").arg(time).arg(point.x).arg(point.y);
        else
            command = QString("time = %1; r = %2; z = %3").arg(time).arg(point.x).arg(point.y);
    }

    // eval expression
    bool successfulRun = currentPythonEngineAgros()->runExpression(expression, &evaluationResult, command);

    // delete reserved variables
    if (!command.isEmpty())
    {
        QString commandDel;

        if (m_isCoordinateDependent && !m_isTimeDependent)
        {
            if (m_problem->config()->coordinateType() == CoordinateType_Planar)
                commandDel = QString("del x; del y");
            else
                commandDel = QString("del r; del z");
        }

        if (m_isTimeDependent && !m_isCoordinateDependent)
        {
            commandDel = QString("del time");
        }

        if (m_isCoordinateDependent && m_isTimeDependent)
        {
            if (m_problem->config()->coordinateType() == CoordinateType_Planar)
                commandDel = QString("del time; del x; del y");
            else
                commandDel = QString("del time; del r; del z");
        }

        currentPythonEngineAgros()->runExpression(commandDel);
    }

    if (!signalBlocked)
        currentPythonEngineAgros()->blockSignals(false);

    return successfulRun;
}

// ************************************************************************************************

PointValue::PointValue(double x, double y)
    : m_x(Value(x)), m_y(Value(y))
{
}

PointValue::PointValue(const Point &point)
{
    m_x.setNumber(point.x);
    m_y.setNumber(point.y);
}

PointValue::PointValue(const Value &x, const Value &y)
    : m_x(x), m_y(y)
{
}

PointValue& PointValue::operator =(const PointValue &origin)
{
    m_x = origin.m_x;
    m_y = origin.m_y;

    return *this;
}

void PointValue::setPoint(double x, double y)
{
    m_x.setNumber(x);
    m_y.setNumber(y);
}

void PointValue::setPoint(const Point &point)
{
    m_x.setNumber(point.x);
    m_y.setNumber(point.y);
}

void PointValue::setPoint(const Value &x, const Value &y)
{
    m_x = x;
    m_y = y;
}

void PointValue::setPoint(const QString &x, const QString &y)
{
    m_x.setText(x);
    m_y.setText(y);
}

QString PointValue::toString() const
{
    return QString("[%1, %2]").arg(m_x.toString()).arg(m_y.toString());
}
