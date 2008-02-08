/***************************************************************************
    File                 : QwtPieCurve.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2004 - 2008 by Ion Vasilie
    Email (use @ for *)  : ion_vasilief*yahoo.fr
    Description          : Pie plot class

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor,                    *
 *   Boston, MA  02110-1301  USA                                           *
 *                                                                         *
 ***************************************************************************/
#include "QwtPieCurve.h"
#include "ColorBox.h"
#include "Table.h"

#include <QPaintDevice>
#include <QPainter>
#include <QPainterPath>
#include <QVarLengthArray>

QwtPieCurve::QwtPieCurve(Table *t, const QString& name, int startRow, int endRow):
	DataCurve(t, QString(), name, startRow, endRow),
	d_pie_ray(50),
	d_first_color(0),
	d_start_azimuth(270),
	d_view_angle(30),
	d_thickness(30),
	d_horizontal_offset(0),
	d_edge_dist(25),
	d_counter_clockwise(false),
	d_auto_labeling(true),
	d_values(false),
	d_percentages(true),
	d_fixed_labels_pos(true)
{
	setPen(QPen(QColor(Qt::black), 1, Qt::SolidLine));
	setBrush(QBrush(Qt::black, Qt::SolidPattern));
    setStyle(QwtPlotCurve::UserCurve);
	setType(Graph::Pie);
}

void QwtPieCurve::clone(QwtPieCurve* c)
{
    if (!c)
        return;

    d_pie_ray = c->radius();
	d_first_color = c->firstColor();
	d_start_azimuth = c->startAzimuth();
	d_view_angle = c->viewAngle();
	d_thickness = c->thickness();
	d_horizontal_offset = c->horizontalOffset();
	d_edge_dist = c->labelsEdgeDistance();
	d_counter_clockwise = c->counterClockwise();
	d_auto_labeling = c->labelsAutoFormat();
	d_values = c->labelsValuesFormat();
	d_percentages = c->labelsPercentagesFormat();
	d_fixed_labels_pos = c->fixedLabelsPosition();
}

void QwtPieCurve::draw(QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, int from, int to) const
{
    int size = dataSize();
	if ( !painter ||  size <= 0 )
		return;

	if (to < 0)
		to = size - 1;

    if (size > 1)
        drawSlices(painter, xMap, yMap, from, to);
    else
        drawDisk(painter, xMap, yMap);
}

void QwtPieCurve::drawDisk(QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap) const
{
    const int x_width = abs(xMap.p1() - xMap.p2());
	const int x_center = int((xMap.p1() + xMap.p2())/2 + d_horizontal_offset*0.01*x_width);
	const int y_center = int((yMap.p1() + yMap.p2())/2);
	const int ray_x = int(d_pie_ray*0.005*qMin(x_width, abs(yMap.p1() - yMap.p2())));
	const double view_angle_rad = d_view_angle*M_PI/180.0;
	const int ray_y = ray_x*sin(view_angle_rad);
	const double thick = 0.01*d_thickness*ray_x*cos(view_angle_rad);

	QRect pieRect;
	pieRect.setX(x_center - ray_x);
	pieRect.setY(y_center - ray_y);
	pieRect.setWidth(2*ray_x);
	pieRect.setHeight(2*ray_y);

	QRect pieRect2 = pieRect;
    pieRect2.translate(0, thick);

    painter->save();

    painter->setPen(QwtPlotCurve::pen());
    painter->setBrush(QBrush(color(0), QwtPlotCurve::brush().style()));

    QPointF start(x_center + ray_x, y_center);
    QPainterPath path(start);
    path.lineTo(start.x(), start.y() + thick);
    path.arcTo(pieRect2, 0, -180.0);
    QPointF aux = path.currentPosition();
    path.lineTo(aux.x(), aux.y() - thick);
    path.arcTo(pieRect, -180.0, 180.0);
    painter->drawPath(path);

    painter->drawEllipse(pieRect);

    if (d_texts_list.size() > 0){
        LegendWidget* l = d_texts_list[0];
        if (l){
            QString s;
            if (d_auto_labeling){
                if (d_values && d_percentages)
                    s += ((Plot *)plot())->locale().toString(y(0), 'g', 4) + " (100%)";
                else if (d_values)
                    s += ((Plot *)plot())->locale().toString(y(0), 'g', 4);
                else if (d_percentages)
                    s += "100%";
                l->setText(s);
            }
            if (d_fixed_labels_pos){
                double a_deg = d_start_azimuth + 180.0;
                if (a_deg > 360)
                    a_deg -= 360;
                double a_rad = a_deg*M_PI/180.0;
                double rx = ray_x*(1 + 0.01*d_edge_dist);
                const int x = int(x_center + rx*cos(a_rad));
                double ry = ray_y*(1 + 0.01*d_edge_dist);
                int y = int(y_center + ry*sin(a_rad));
                if (a_deg > 0 && a_deg < 180)
                    y += thick;

                double dx = xMap.invTransform(x - l->width()/2);
                double dy = yMap.invTransform(y - l->height()/2);
                l->setOriginCoord(dx, dy);
            }
        }
    }

    painter->restore();
}

void QwtPieCurve::drawSlices(QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, int from, int to) const
{
    const int x_width = abs(xMap.p1() - xMap.p2());
	const int x_center = int((xMap.p1() + xMap.p2())/2 + d_horizontal_offset*0.01*x_width);
	const int y_center = int((yMap.p1() + yMap.p2())/2);
	const int ray_x = int(d_pie_ray*0.005*qMin(x_width, abs(yMap.p1() - yMap.p2())));
	const double view_angle_rad = d_view_angle*M_PI/180.0;
	const int ray_y = ray_x*sin(view_angle_rad);
	const double thick = 0.01*d_thickness*ray_x*cos(view_angle_rad);

	QRect pieRect;
	pieRect.setX(x_center - ray_x);
	pieRect.setY(y_center - ray_y);
	pieRect.setWidth(2*ray_x);
	pieRect.setHeight(2*ray_y);

	QRect pieRect2 = pieRect;
    pieRect2.translate(0, thick);

	double sum = 0.0;
	for (int i = from; i <= to; i++)
		sum += y(i);

	const int sign = d_counter_clockwise ? 1 : -1;

	const int size = dataSize();
    double *start_angle = new double[size];
    double *end_angle = new double[size];
    double aux_angle = d_start_azimuth;
	for (int i = from; i <= to; i++){
	    double a = -sign*y(i)/sum*360.0;
		start_angle[i] = aux_angle;

		double end = aux_angle + a;
		if (end >= 360)
            end -= 360;
        else if (end < 0)
            end += 360;

		end_angle[i] = end;
		aux_angle = end;
	}

	int angle = (int)(5760 * d_start_azimuth/360.0);
	if (d_counter_clockwise)
		angle = (int)(5760 * (1 - d_start_azimuth/360.0));

	painter->save();

	QLocale locale = ((Plot *)plot())->locale();
	for (int i = from; i <= to; i++){
		const double yi = y(i);
		const double q = yi/sum;
		const int value = (int)(q*5760);

		painter->setPen(QwtPlotCurve::pen());
		painter->setBrush(QBrush(color(i), QwtPlotCurve::brush().style()));

		double deg = q*360;
		double start_3D_view_angle = start_angle[i];
		double end_3D_view_angle = end_angle[i];
		if (d_counter_clockwise){
		    start_3D_view_angle = end_angle[i];
            end_3D_view_angle = start_angle[i];
		}

        bool draw3D = false;
        if (deg <= 180 && start_3D_view_angle >= 0 && start_3D_view_angle < 180){
            if ((end_3D_view_angle > 180 && end_3D_view_angle > start_3D_view_angle)){
                deg = 180 - start_3D_view_angle;
                end_3D_view_angle = 180.0;
            }
            draw3D = true;
		} else if (start_3D_view_angle >= 180 && end_3D_view_angle < start_3D_view_angle){
		    if (end_3D_view_angle > 180)
                end_3D_view_angle = 180;
            deg = end_3D_view_angle;
            start_3D_view_angle = 0;
            draw3D = true;
		} else if (deg > 180 && start_3D_view_angle >= 180){
            deg = 180;
            end_3D_view_angle = 180;
            start_3D_view_angle = 0;
            draw3D = true;
		}

		if (draw3D){
            double rad = start_3D_view_angle/180.0*M_PI;
			QPointF start(x_center + ray_x*cos(rad), y_center + ray_y*sin(rad));
			QPainterPath path(start);
			path.lineTo(start.x(), start.y() + thick);
			path.arcTo(pieRect2, -start_3D_view_angle, -deg);
        	QPointF aux = path.currentPosition();
        	path.lineTo(aux.x(), aux.y() - thick);
        	path.arcTo(pieRect, -end_3D_view_angle, deg);
        	painter->drawPath(path);
        } else {
            if (start_3D_view_angle >= 0 && start_3D_view_angle < 180){
                if (end_3D_view_angle > 180)
                    end_3D_view_angle = 0;

                double rad = start_3D_view_angle/180.0*M_PI;
                QPointF start(x_center + ray_x*cos(rad), y_center + ray_y*sin(rad));
                QPainterPath path(start);
                path.lineTo(start.x(), start.y() + thick);

                deg = 180 - start_3D_view_angle;
                path.arcTo(pieRect2, -start_3D_view_angle, -deg);
                QPointF aux = path.currentPosition();
                path.lineTo(aux.x(), aux.y() - thick);
                path.arcTo(pieRect, -180, deg);
                painter->drawPath(path);

                path.moveTo(QPointF(x_center + ray_x, y_center));
                aux = path.currentPosition();
                path.lineTo(aux.x(), aux.y() + thick);
                path.arcTo(pieRect2, 0, -end_3D_view_angle);
                aux = path.currentPosition();
                path.lineTo(aux.x(), aux.y() - thick);
                path.arcTo(pieRect, -end_3D_view_angle, end_3D_view_angle);
                painter->drawPath(path);
            }
        }

		painter->drawPie(pieRect, sign*angle, sign*value);
		angle += value;

		int li = i - from;
		if (li >= d_texts_list.size())
			continue;

		PieLabel* l = d_texts_list[li];
		if (l){
			QString s;
			if (d_auto_labeling){
				if (d_values && d_percentages)
					s += locale.toString(yi, 'g', 4) + " (" + locale.toString(q*100, 'g', 4) + "%)";
				else if (d_values)
					s += locale.toString(yi, 'g', 4);
				else if (d_percentages)
					s += locale.toString(q*100, 'g', 4) + "%";
                l->setText(s);
			} else
				l->setCustomText(l->customText());
			
            if (d_fixed_labels_pos){
                double a_deg = start_angle[i] - sign*q*180.0;
                if (a_deg > 360)
                    a_deg -= 360.0;
                double a_rad = a_deg*M_PI/180.0;

                double rx = ray_x*(1 + 0.01*d_edge_dist);
                const int x = int(x_center + rx*cos(a_rad));

                double ry = ray_y*(1 + 0.01*d_edge_dist);
                int y = int(y_center + ry*sin(a_rad));
                if (a_deg > 0 && a_deg < 180)
                    y += thick;

                double dx = xMap.invTransform(x - l->width()/2);
                double dy = yMap.invTransform(y - l->height()/2);
                l->setOriginCoord(dx, dy);
            }
		}
	}
	painter->restore();
	delete [] start_angle;
	delete [] end_angle;
}

QColor QwtPieCurve::color(int i) const
{
	return ColorBox::color((d_first_color + i) % ColorBox::numPredefinedColors());
}

void QwtPieCurve::setBrushStyle(const Qt::BrushStyle& style)
{
	QBrush br = QwtPlotCurve::brush();
	if (br.style() == style)
		return;

	br.setStyle(style);
	setBrush(br);
}

void QwtPieCurve::loadData()
{
	QLocale locale = ((Plot *)plot())->locale();
	QVarLengthArray<double> X(abs(d_end_row - d_start_row) + 1);
	int size = 0;
	int ycol = d_table->colIndex(title().text());
	for (int i = d_start_row; i <= d_end_row; i++ ){
		QString xval = d_table->text(i, ycol);
		bool valid_data = true;
		if (!xval.isEmpty()){
            X[size] = locale.toDouble(xval, &valid_data);
            if (valid_data){
                size++;
			}
		}
	}
	X.resize(size);
	setData(X.data(), X.data(), size);
}

void QwtPieCurve::addLabel(PieLabel *l, bool clone)
{
	if (clone){
		PieLabel *newLabel = new PieLabel((Plot *)plot());
		newLabel->clone(l);
		newLabel->setCustomText(l->customText());
		d_texts_list << newLabel;	
	} else
		d_texts_list << l;
}

void QwtPieCurve::removeLabel(PieLabel *l)
{
	
}

void QwtPieCurve::initLabels()
{
	int size = abs(d_end_row - d_start_row) + 1;
	double sum = 0.0;
	for (int i = 0; i < size; i++)
		sum += y(i);

    Plot *d_plot = (Plot *)plot();
	QLocale locale = d_plot->locale();
	for (int i = 0; i <size; i++ ){
		PieLabel* l = new PieLabel(d_plot);
		l->setFrameStyle(0);
		d_texts_list << l;
		if (i < dataSize())
            l->setText(locale.toString(y(i)/sum*100, 'g', 4) + "%");
	}
}

PieLabel::PieLabel(Plot *plot):LegendWidget(plot),
	d_custom_text(QString::null),
	d_custom_position(QPoint())
{
}

QString PieLabel::customText()
{
	if (d_custom_text.isEmpty())
		return text();
	
	return d_custom_text;
}

void PieLabel::setCustomPosition(const QPoint& p)
{
	d_custom_position = p;
}
