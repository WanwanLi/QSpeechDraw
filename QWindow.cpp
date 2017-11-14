#include "QWindow.h"
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>

QWindow::QWindow(QWidget* widget) : QWidget(widget)
{
 	this->resize(500, 400);
 	this->setMinimumSize(100, 100);
	this->setWindowTitle("QCanvas");
	this->canvas=new QCanvas(this);
	this->setLayout(newQBoxLayout());
}
QBoxLayout* QWindow::newQBoxLayout()
{
	QBoxLayout* VBoxLayout=new QVBoxLayout();
	VBoxLayout->setMenuBar(newQMenuBar());
	VBoxLayout->addWidget(canvas);
	VBoxLayout->setMargin(0);
	return VBoxLayout;
}
QMenuBar* QWindow::newQMenuBar()
{
	QMenuBar* menuBar = new QMenuBar();
	QMenu* menu=new QMenu("Image", this);
	QAction* action=menu->addAction("Open", this,  &QWindow::openImage, QKeySequence(tr("Ctrl+o")));
	action=menu->addAction("Resize", canvas, &QCanvas::resizeImage, QKeySequence(tr("Ctrl+r")));
	action=menu->addAction("Save", this,  &QWindow::saveImage, QKeySequence(tr("Ctrl+s")));
	action=menu->addAction("Clear", canvas, &QCanvas::clear, QKeySequence(tr("Ctrl+c")));
	action=menu->addAction("Undo", canvas, &QCanvas::undo, QKeySequence(tr("Ctrl+z")));
	menuBar->addMenu(menu);
	return menuBar;
}
bool QWindow::openImage()
{
	QString fileName = QFileDialog::getOpenFileName(this, "Load Image", QDir::currentPath());
	if(fileName.isEmpty()||!this->canvas->loadImage(fileName))
	{
		QMessageBox::critical(this, "Error", "Can not load image."); return false;
	}
	return true;
}
bool QWindow::saveImage()
{
	QString fileName = QFileDialog::getSaveFileName(this, "Save Image", QString(), "SKY(*.sky);; JEPG(*.jpg)");
	if(!fileName.isEmpty())
	{
		QStringList fileNames=fileName.split("."); 
		QByteArray fileFormat=fileNames[fileNames.size()-1].toLatin1();
		if(this->canvas->saveImage(fileName, fileFormat.data()))return true;
	}
	QMessageBox::critical(this, "Error", "Can not save image."); return false;
}
void QWindow::closeEvent(QCloseEvent* event)
{
	if(canvas->isModified()) 
	{
		QMessageBox::StandardButton button=QMessageBox::warning
		(
			this, "Close", "Save image?", QMessageBox::Save|
			QMessageBox::Discard|QMessageBox::Cancel
		);
		if(button==QMessageBox::Save&&saveImage())event->accept();
		else if(button==QMessageBox::Discard)event->accept();
		else if(button==QMessageBox::Cancel)event->ignore();
		else event->ignore();
	}
	else event->accept();
}
