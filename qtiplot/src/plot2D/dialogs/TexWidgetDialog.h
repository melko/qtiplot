/***************************************************************************
    File                 : TexWidgetDialog.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2008 by Ion Vasilief
    Email (use @ for *)  : ion_vasilief*yahoo.fr
    Description          : A dialog for the TextWidget, based on article 
						  "Using a Simple Web Service with Qt" in Qt Quaterly, Issue 23, Q3 2007

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

#ifndef TEXWIDGETDIALOG_H
#define TEXWIDGETDIALOG_H

#include <QDialog>

class QComboBox;
class QHttp;
class QLabel;
class QPushButton;
class QTextEdit;
class QTabWidget;
class QCheckBox;
class QLineEdit;
	
class Graph;
class FrameWidget;
class ColorButton;
class DoubleSpinBox;
	
class TexWidgetDialog : public QDialog
{
    Q_OBJECT

public:
	enum WidgetType{Frame, Text, Image, Tex};
	
    TexWidgetDialog(WidgetType wt, Graph *g, QWidget *parent = 0);
	void setWidget(FrameWidget *w);

private slots:
    void clearForm();
    void fetchImage();
    void updateForm(bool error);
	void addImage();
	void apply();
	void customButtons(QWidget *w);
	void chooseImageFile(const QString& fn = QString::null);
	void displayCoordinates(int unit);

private:
	void initEditorPage();
	void initImagePage();
	void initFramePage();
	void initGeometryPage();
	void setCoordinates(int unit);

    QHttp *http;
    QLabel *outputLabel;
	QPushButton *addButton;
    QPushButton *clearButton;
    QPushButton *updateButton;
	QPushButton *cancelButton;
    QTextEdit *equationEditor;
	QComboBox *frameBox;
	QTabWidget* tabWidget;
	QWidget *editPage, *framePage, *geometryPage, *imagePage;
	ColorButton *frameColorBtn;
	QCheckBox *boxSaveImagesInternally;
	QLineEdit *imagePathBox;
	DoubleSpinBox *xBox, *yBox, *widthBox, *heightBox;
	QComboBox *unitBox;

	Graph *d_plot;
	FrameWidget *d_widget;
	WidgetType d_widget_type;
};

#endif
