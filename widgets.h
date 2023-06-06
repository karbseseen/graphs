#pragma once
#include <QButtonGroup>
#include <QComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <limits>


struct UintEdit : QLineEdit {
	uint min_v, max_v;
	UintEdit(
		uint min_v = std::numeric_limits<uint>::min(),
		uint max_v = std::numeric_limits<uint>::max(),
		QWidget *parent = 0);
	uint get();
};
struct DoubleEdit : QLineEdit {
	double min_v, max_v;
	DoubleEdit(
		double min_v = -std::numeric_limits<double>::infinity(),
		double max_v = std::numeric_limits<double>::infinity(),
		QWidget *parent = 0);
	double get();
};

struct Methods : QGroupBox {
	QVBoxLayout layout;
	QRadioButton nn;
	QRadioButton sa;
	QRadioButton ac;
	Methods(QWidget *parent = 0);
};
struct NewVerts : QGroupBox {
	QGridLayout layout;
	QLabel text;
	UintEdit vert_n;
	QPushButton create;
	NewVerts(QWidget *parent = 0);
};
struct Annealing : QGroupBox {
	QFormLayout layout;
	DoubleEdit t0;
	DoubleEdit c;
	UintEdit iters;
	QComboBox dep;
	Annealing(QWidget *parent = 0);
};
struct Ants : QGroupBox {
	QFormLayout layout;
	DoubleEdit a;
	DoubleEdit b;
	DoubleEdit p;
	UintEdit ants;
	UintEdit iters;
	Ants(QWidget *parent = 0);
};

struct Input : QWidget {
	Q_OBJECT
public:
	enum Type { Nearest, Anneal, Ant };

	QVBoxLayout layout;
	NewVerts initializer;
	Methods method;
	Annealing annealing;
	Ants ants;
	QPushButton go;
	QProgressBar progr;
	QLabel score;

	Type type;
	Input(QWidget *parent = 0);

	void set_type(Type type);
public slots:
	void set_nearest();
	void set_anneal();
	void set_ant();
};
