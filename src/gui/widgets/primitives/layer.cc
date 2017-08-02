// @file:     layer.cc
// @author:   Jake
// @created:  2016.11.15
// @editted:  2017.06.07  - Jake
// @license:  GNU LGPL v3
//
// @desc:     Layer implementations

#include "layer.h"


// statics
uint prim::Layer::layer_count = 0;


prim::Layer::Layer(const QString &nm, int lay_id, QObject *parent)
  : QObject(parent), visible(true), active(false)
{
  layer_id = lay_id;
  name = nm.isEmpty() ? nm : QString("Layer %1").arg(layer_count++);
}


void prim::Layer::setLayerIndex(int lay_id){
  layer_id = lay_id;
  for(prim::Item *item : items)
    item->setLayerIndex(lay_id);
}

// NOTE: in future, it might be worth keeping the items in a binary tree, sorted
// by pointer address (which should not be modifiable).

void prim::Layer::addItem(prim::Item *item, int index)
{
  if(!items.contains(item)){
    if(index <= items.size()){
      items.insert(index < 0 ? items.size() : index, item);

      // set item flags to agree with layer
      item->setActive(active);
      item->setVisible(visible);
    }
    else
      qCritical() << tr("Layer item index invalid");
  }
  else
    qDebug() << tr("item aleady contained in layer...");
}



bool prim::Layer::removeItem(prim::Item *item)
{
  bool found = items.removeOne(item);
  if(!found)
    qDebug() << tr("item not found in layer...");
  return found;
}


prim::Item *prim::Layer::takeItem(int ind)
{
  if(ind==-1 && !items.isEmpty())
    return items.takeLast();
  if(ind < 0 || ind >= items.count()){
    qCritical() << tr("Invalid item index...");
    return 0;
  }
  return items.takeAt(ind);
}

void prim::Layer::setVisible(bool vis)
{
  if(vis!=visible){
    visible = vis;
    for(prim::Item *item : items)
      item->setVisible(vis);
  }
}


void prim::Layer::setActive(bool act)
{
  if(act!=active){
    active=act;
    for(prim::Item *item : items)
      item->setActive(act);
  }
}

void prim::Layer::saveToFile(QXmlStreamWriter *stream) const
{
  stream->writeStartElement("layer");

  // TODO layer ID

  stream->writeTextElement("name", name);
  stream->writeTextElement("visible", QString::number(visible));
  stream->writeTextElement("active", QString::number(active));

  // TODO contained item ids (might actually not need this for layers?)

  stream->writeEndElement();
}


void prim::Layer::loadFromFile(QXmlStreamReader *stream)
{
  QString name_ld; //name
  bool visible_ld, active_ld;

  while(!stream->atEnd()){
    if(stream->isStartElement()){
      if(stream->name() == "id"){
        // TODO add layer id to id -> pointer conversion table
      }
      else if(stream->name() == "name"){
        name_ld = stream->readElementText();
      }
      else if(stream->name() == "visible"){
        visible_ld = (stream->readElementText() == "1")?1:0;
      }
      else if(stream->name() == "active"){
        active_ld = (stream->readElementText() == "1")?1:0;
      }
    }
    else if(stream->isEndElement())
      stream->readNext();
  }
  // TODO this code might keep reading past the end of the intended element, look at the logic again

  if(stream->hasError()){
    qCritical() << tr("XML error: ") << stream->errorString().data();
  }

  // make layer object using loaded information
  prim::Layer *layer_ld = new prim::Layer(name_ld);
  layer_ld->setVisible(visible_ld);
  layer_ld->setActive(active_ld);

  // TODO add it to scene or whatever
}
