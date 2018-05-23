// @file:     property_editor.cc
// @author:   Samuel
// @created:  2018.03.29
// @editted:  2018.03.29  - Samuel
// @license:  GNU LGPL v3
//
// @desc:     Allows users to edit item properties.

#include "property_editor.h"

namespace gui{

// Constructor
PropertyEditor::PropertyEditor(QWidget *parent)
  : QWidget(parent, Qt::Dialog)
{
  initPropertyEditor();
}

// Generate a user-editable form for the provided property map, and connect
// appropriate signals to update the target item.
void PropertyEditor::showForms(QList<prim::Item*> target_items)
{
  for (prim::Item *item : target_items) {
    if (!item || !item->classPropertyMap()) {
      continue;
    }
    form_item_pair.append(qMakePair(new PropertyForm(item->properties(), this), item));
    form_tab_widget->addTab(form_item_pair.back().first, item->getQStringItemType());
  }
  show();
}

void PropertyEditor::closeEvent(QCloseEvent *event)
{
  cancel();
  event->accept();
}

void PropertyEditor::applyForms()
{
  /*for (PropertyForm *form : form_item_pair)
    form->pushPropertyChanges();*/
  for (QPair<PropertyForm*, prim::Item*> p : form_item_pair) {
    PropertyMap final_map = p.first->finalProperties();
    prim::Item *item = p.second;

    for (const QString &key : item->properties().keys()) {
      if (item->getProperty(key).value != final_map.value(key).value) {
        item->setProperty(key, final_map.value(key).value);
      }
    }
  }
}


void PropertyEditor::discardForms()
{
  form_tab_widget->clear();
  while (!form_item_pair.isEmpty())
    delete form_item_pair.takeLast().first;
}


void PropertyEditor::initPropertyEditor()
{
  // tab for showing forms
  form_tab_widget = new QTabWidget(this);

  // editor buttons
  QHBoxLayout *buttons_hl = new QHBoxLayout;
  QPushButton *pb_apply = new QPushButton("Apply");
  QPushButton *pb_ok = new QPushButton("OK");
  QPushButton *pb_cancel = new QPushButton("Cancel");
  buttons_hl->addWidget(pb_apply);
  buttons_hl->addWidget(pb_ok);
  buttons_hl->addWidget(pb_cancel);

  // full form structure
  QVBoxLayout *editor_container = new QVBoxLayout;
  editor_container->addWidget(form_tab_widget);
  editor_container->addLayout(buttons_hl);
  setLayout(editor_container);

  // connect signals to parse form submission (generate a list of changed items)
  connect(pb_apply, &QAbstractButton::clicked,
          this, &PropertyEditor::applyForms);
  connect(pb_ok, &QAbstractButton::clicked,
          this, &PropertyEditor::okay);
  connect(pb_cancel, &QAbstractButton::clicked,
          this, &PropertyEditor::cancel);
}


} // end of gui namespace
