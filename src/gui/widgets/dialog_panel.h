#ifndef _GUI_DIALOG_PANEL_H_
#define _GUI_DIALOG_PANEL_H_

#include <QObject>
#include <QWidget>
#include <QScrollArea>
#include <QPlainTextEdit>
#include <QString>
#include <QMouseEvent>
#include <QFile>

namespace gui{

class DialogPanel : public QPlainTextEdit
{
  Q_OBJECT

public:

  // constructor
  explicit DialogPanel(QWidget *parent = 0);

  // deconstructor
  ~DialogPanel();

  // public methods
  void echo(const QString& s);

protected:

  // interrupts
  void mousePressEvent(QMouseEvent *e);

private:

  QFile *file;
};


} // end gui namespace

#endif