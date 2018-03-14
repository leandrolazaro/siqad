// @file:     settings_dialog.h
// @author:   Samuel
// @created:  2018.02.23
// @editted:  2018.02.23  - Samuel
// @license:  GNU LGPL v3
//
// @desc:     Settings dialog for users to alter settings


#ifndef _SETTINGS_DIALOG_H_
#define _SETTINGS_DIALOG_H_

#include <QtWidgets>

#include "settings.h"

namespace settings{

  class SettingsDialog: public QWidget
  {
    Q_OBJECT

  public:

    enum SettingsCategory{App, GUI, Lattice};

    SettingsDialog(QWidget *parent=0);
    ~SettingsDialog() {};

    //! A struct that holds pending changes to one setting
    struct PendingChange {
      PendingChange(const SettingsCategory &category, const QString &name,
          const QVariant &value)
        : category(category), name(name), value(value)
      {}
      SettingsCategory category;
      QString name;
      QVariant value;
    };

  public slots:
    void addPendingBoolUpdate(bool new_state);

    void applyPendingChanges();
    void applyAndClose();
    void discardAndClose();

  private:
    // initialise the dialog and panes
    void initSettingsDialog();
    QWidget *appSettingsPane();
    QWidget *guiSettingsPane();
    QWidget *latticeSettingsPane();

    // Return the Settings class pointer to the specified category
    settings::Settings *settingsCategoryPointer(SettingsCategory);

    // VARS
    AppSettings *app_settings=0;
    GUISettings *gui_settings=0;
    LatticeSettings *lattice_settings=0;

    QWidget *app_settings_pane=0;
    QWidget *gui_settings_pane=0;
    QWidget *lattice_settings_pane=0;

    QList<PendingChange> pending_changes;
  };

} // end of settings namespace

#endif
