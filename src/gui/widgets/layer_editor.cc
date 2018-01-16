// @file:     layer_editor.cc
// @author:   Samuel
// @created:  2017.12.22
// @editted:  2017.12.22 - Samuel
// @license:  GNU LGPL v3
//
// @desc:     widget for configuring 

#include "layer_editor.h"

namespace gui {

// constructor
LayerEditor::LayerEditor(gui::DesignPanel *design_pan, QWidget *parent)
  : QWidget(parent, Qt::Dialog), dp(design_pan)
{
  layers = dp->getLayers();
  initLayerEditor();
}

// destructor
LayerEditor::~LayerEditor()
{
  for (auto row_item : table_row_items)
    delete row_item;

  // NOTE Pointers within *layers are deleted in design_panel
}


// PRIVATE

// initialize widget
void LayerEditor::initLayerEditor()
{
  // top buttons
  QPushButton *bt_add = new QPushButton(tr("Add")); // TODO implement function

  QHBoxLayout *top_buttons_hl = new QHBoxLayout;
  top_buttons_hl->addWidget(bt_add);

  // grid layout that show all layers
  layer_table = new QTableWidget(this);

  // Main layout
  QVBoxLayout *main_vl = new QVBoxLayout;
  //main_vl->addLayout(top_buttons_hl); TODO add this back when add function is implemented
  main_vl->addWidget(layer_table);

  setLayout(main_vl);

  // TODO change add/remove to signal based
}


void LayerEditor::initLayerTable()
{
  qDebug() << "Initializing layer table";
  QStringList table_headers;
  // TODO take enum type instead of this stringlist
  table_headers << 
    "ID" <<       // Layer ID, layer's position in layers* stack
    "Type" <<     // Type (lattice, db, electrode) TODO types of default layers can't be changed
    "Name" <<     // Name TODO names of default layers can't be changed
    "Z-Height" << // Z-Height (vertical offset from surface) TODO surface zheight can't be changed
    "" <<  // Visibility
    "";   // Editability

  layer_table->setColumnCount(table_headers.count());
  layer_table->setHorizontalHeaderLabels(table_headers);
  layer_table->setColumnHidden(static_cast<int>(ID), true);
  layer_table->resizeColumnToContents(static_cast<int>(Visibility)); // reduce width of visibility column
  layer_table->resizeColumnToContents(static_cast<int>(Editability)); // reduce width of visibility column

  // header tooltips
  layer_table->horizontalHeaderItem(static_cast<int>(Type))->setToolTip("Layer Type: lattice, db, or electrode");
  layer_table->horizontalHeaderItem(static_cast<int>(Name))->setToolTip("Layer Name");
  layer_table->horizontalHeaderItem(static_cast<int>(ZHeight))->setToolTip("Z-Height: vertical offset from surface.\nPositive for objects above surface, negative for objects under surface.");
  layer_table->horizontalHeaderItem(static_cast<int>(Visibility))->setToolTip("Visibility of the layer");
  layer_table->horizontalHeaderItem(static_cast<int>(Editability))->setToolTip("Editability of the layer");

  // populate table with layer info
  qDebug() << "Populating layer table";
  int layer_i = 0;
  for (prim::Layer* layer : *layers)
    addLayerRow(layer);

  // signals originating from the table
  connect(layer_table, SIGNAL(cellChanged(int,int)), this, SLOT(updateLayerPropFromTable(int,int)));
  // TODO UNDOable z-height change
  // TODO UNDOable layer creation in DesignPanel
}


void LayerEditor::refreshLayerTable()
{
  // reload all rows and update with changes, including added/deleted layers
  // TODO
}


void LayerEditor::clearLayerTable()
{
  // delete layer rows and disconnect all signals within the table
  // TODO
}


void LayerEditor::updateLayerPropFromTable(int row, int column)
{
  // TODO really need to not hard code layer ID and column position, need some sort of table that translates between readable name and row/col number
  QString layer_name = layer_table->item(row, static_cast<int>(Name))->text();
  qDebug() << QObject::tr("Row=%1, Col=%2, Layer name=%3").arg(row).arg(column).arg(layer_name);
  prim::Layer* layer = dp->getLayer(layer_name); // get layer according to Layer Name

  switch(column) {
    case static_cast<int>(ID):
      // not supposed to get this
      break;
    case static_cast<int>(Type):
      // not supposed to get this
      break;
    case static_cast<int>(Name):
      // not supposed to get this
      break;
    case static_cast<int>(ZHeight):
      layer->setZHeight(layer_table->item(row, column)->text().toInt());
      break;
    case static_cast<int>(Visibility):
      // not supposed to get this
      break;
    case static_cast<int>(Editability):
      // not supposed to get this
      break;
    default:
      // not supposed to get this
      break;
  }
  // TODO edit layer property
}


// update widget
void LayerEditor::addLayerRow(prim::Layer *layer)
{
  LayerTableRowItems *curr_row_items = new LayerTableRowItems;
  curr_row_items->layer = layer;

  //qDebug() << tr("Constructing layer row GUI elements for layer %1").arg(layer->getName());

  // items that require signal disconnection at removal
  curr_row_items->bt_visibility = new QPushButton(QIcon(":/ico/visible.svg"), "", this);
  curr_row_items->bt_editability = new QPushButton(QIcon(":/ico/editable.svg"), "", this);

  curr_row_items->bt_visibility->setCheckable(true);
  curr_row_items->bt_visibility->setChecked(layer->isVisible());
  curr_row_items->bt_editability->setCheckable(true);
  curr_row_items->bt_editability->setChecked(layer->isActive());

  connect(curr_row_items->bt_visibility, SIGNAL(toggled(bool)), layer, SLOT(visibilityPushButtonChanged(bool)));
  connect(curr_row_items->bt_editability, SIGNAL(toggled(bool)), layer, SLOT(editabilityPushButtonChanged(bool)));

  // other items
  curr_row_items->type = new QTableWidgetItem(layer->getContentTypeString());
  curr_row_items->type->setIcon(layerType2Icon(layer->getContentType()));
  curr_row_items->type->setToolTip(layer->getContentTypeString());
  
  curr_row_items->name = new QTableWidgetItem(layer->getName());
  curr_row_items->zheight = new QTableWidgetItem(QString::number(layer->getZHeight()));

  // add to table
  addLayerRow(curr_row_items);
}


void LayerEditor::addLayerRow(LayerTableRowItems *row_items)
{
  table_row_items.append(row_items);

  // add elems to table
  int curr_row = layer_table->rowCount();

  //qDebug() << tr("Inserting layer GUI elements to table into row %1").arg(curr_row);

  layer_table->insertRow(curr_row);
  layer_table->setItem(curr_row, static_cast<int>(Type), row_items->type);
  layer_table->setItem(curr_row, static_cast<int>(Name), row_items->name);
  layer_table->setItem(curr_row, static_cast<int>(ZHeight), row_items->zheight);

  layer_table->setCellWidget(curr_row, static_cast<int>(Visibility), row_items->bt_visibility);
  layer_table->setCellWidget(curr_row, static_cast<int>(Editability), row_items->bt_editability);
}


QIcon LayerEditor::layerType2Icon(const prim::Layer::LayerType layer_type)
{
  // TODO make enumerated layer type instead of hard code string
  if (layer_type == prim::Layer::Lattice)
    return QIcon(":/ico/lattice.svg");
  /*else if (layer_type == prim::Layer::DB)
    return QIcon(":/ico/db.svg");
  else if (layer_type == prim::Layer::Electrode)
    return QIcon(":/ico/electrode.svg");*/
  else
    return QIcon(":/ico/unknown.svg");
}

// TODO
// z-height of db-surface is 0
// +ve for overhanging layers, -ve for buried
// ability to edit z-height for non-surface layers
// option to "always show layer distance from surface"
// states (checkboxes): visible, editable

}
