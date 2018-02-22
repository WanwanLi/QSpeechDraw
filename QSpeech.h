#include <QObject>
#include <QThread>
#include <QProcess>
#include <QBasicTimer>

class QListenEvent
{
	public:
	QStringList text;
	QListenEvent(QStringList text):text(text){}
};
class QSpeech : public QObject
{
	Q_OBJECT

	signals:
	void listenEvent(QListenEvent* event);

	public:
	void listen();
	void speak(QString text);
	QSpeech(QThread* thread);

	private:
	void read(QString fileName);
};
