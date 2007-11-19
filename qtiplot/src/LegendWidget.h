/***************************************************************************
    File                 : LegendWidget.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Ion Vasilief
    Email (use @ for *)  : ion_vasilief*yahoo.fr
    Description          : A 2D Plot Legend Widget

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
#ifndef LEGENDWIDGET_H
#define LEGENDWIDGET_H

#include <QWidget>
#include <qwt_plot.h>
#include <qwt_text.h>

#include "Graph.h"
#include "Plot.h"

//class SelectionMoveResizer;

class LegendWidget: public QWidget
{	
	Q_OBJECT

public:
    LegendWidget(Plot *);
	~LegendWidget();

	QwtPlot *plot(){return d_plot;};

	//! The kinds of frame a LegendWidget can draw around the Text.
	enum FrameStyle{None = 0, Line = 1, Shadow = 2};

	QString text(){return d_text->text();};
	void setText(const QString& s);

	void setOrigin(const QPoint & p);
	//! Sets the position of the top left corner in axis coordinates
	void setOriginCoord(double x, double y);

	QColor textColor(){return d_text->color();};
	void setTextColor(const QColor& c);

	QColor backgroundColor(){return d_text->backgroundBrush().color();};
	void setBackgroundColor(const QColor& c);

	int frameStyle(){return d_frame;};
	void setFrameStyle(int style);

	QFont font(){return d_text->font();};
	void setFont(const QFont& font);

	int angle(){return d_angle;};
	void setAngle(int ang){d_angle = ang;};

	double xValue();
	double yValue();
	
	void setSelected(bool on = true);
	
	void showTextDialog(){emit showDialog();};
	void showContextMenu(){emit showMenu();};
	
private:
	PlotCurve* getCurve(const QString& s, int &point);
	void drawFrame(QPainter *p, int type, const QRect& rect);
	void drawVector(PlotCurve *c, QPainter *p, int x, int y, int l);
	void drawSymbol(PlotCurve *c, int point, QPainter *p, int x, int y, int l);
	void drawText(QPainter *, const QRect&, QwtArray<long>, int);

	QwtArray<long> itemsHeight(int symbolLineLength, int &width, int &height);
	int symbolsMaxWidth();
	QString parse(const QString& str);

	virtual void paintEvent(QPaintEvent *e);
    void mousePressEvent(QMouseEvent *);
	void contextMenuEvent(QContextMenuEvent * ){emit showMenu();};

	//! Parent plot
	Plot *d_plot;

	//! Frame type
	int d_frame;

	//! Rotation angle: not implemented yet
	int d_angle;

	//! Pointer to the QwtText object
	QwtText* d_text;

	//! Distance between symbols and legend text
	int h_space;

	//! Distance between frame and content
	int left_margin, top_margin;

	//! Length of the symbol line
	int line_length;

	double d_x, d_y;

	SelectionMoveResizer *d_selector;
	
signals:
	void showDialog();
	void showMenu();
};

#endif
