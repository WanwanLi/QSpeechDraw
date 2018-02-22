#include <QDir>
#include <QFile>
#include <QDebug>
#include <QStringList>
#include <QBasicTimer>
#include <QTextStream>
#include "QSpeech.h"
QString speechTools="python SpeechTools/";

QSpeech::QSpeech(QThread* thread)
{
	this->moveToThread(thread);
	connect(thread, &QThread::started, this, &QSpeech::listen);
}
void  QSpeech::listen()
{
	QProcess process;
	process.start(speechTools+"Listener.py");
	process.waitForFinished(-1);
	this->read("../audio.txt");
}
void QSpeech::read(QString fileName)
{
	QFile file(fileName);
	if(file.open(QIODevice::ReadOnly))
	{
		QTextStream textStream(&file); 
		QString text=textStream.readLine();
		emit listenEvent(new QListenEvent(text.split(" ")));
		file.close(); file.remove();
	}
}
void QSpeech::speak(QString text)
{
	QProcess process;
	process.start(speechTools+"Speaker.py "+text);
	process.waitForFinished(-1);
}
