#pragma once

#include "widgets.h"
#include "graphedit.h"
#include <QMainWindow>
#include <QScrollArea>
#include <thread>

class MainWindow : public QMainWindow {
	Q_OBJECT
	QScrollArea input_scroll;
	Input input;
	GraphEdit ge;

	void resizeEvent(QResizeEvent *event);
private slots:
	void init();
	void calc();
	void update(uint p);
public:
	MainWindow(QWidget *parent = nullptr);
private:
	inline void update() { update(input.progr.maximum()); }
};
