#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include <QProcess>
#include "SettingDlg.h"

class MyThread : public QThread
{
	Q_OBJECT

public:
	MyThread(QObject* parent = 0) : QThread(parent) {
		connect(this, SIGNAL(finished()), this, SLOT(onFinish()));
	}

	virtual void run()
	{
		UserSetting* setting = MySetting<UserSetting>::getInstance("Global");
		QString calc = setting->value("Calculator").toString();
		if(!calc.isEmpty())
			QProcess::execute(calc);
	}

private slots:
	void onFinish()	{
		delete this;
	}
};

#endif // MYTHREAD_H
