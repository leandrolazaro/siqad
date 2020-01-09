/** @file:     layer.h
 *  @author:   Jake
 *  @created:  2016.11.15
 *  @editted:  2017.06.07  - Jake
 *  @license:  GNU LGPL v3
 *
 *  @desc:     Container class for different layers of the design. The surface
 *             and subtrate lattice are special cases.
 */

// NOTE: may want to distinguish between different types of layers using enum


#ifndef _GUI_PR_LAYER_H_
#define _GUI_PR_LAYER_H_


#include <QtWidgets>
#include <QtCore>
#include <QMetaEnum>
#include "item.h"


namespace prim{



  //! Base Class for design layers, layers do not delete their items when destroyed
  class Layer : public QObject
  {
    Q_OBJECT  // enables tr without using QObject::tr

  public:

    //! enum type for different layer contents
    enum LayerType{Lattice, DB, Electrode, AFMTip, Misc, NoType};
    Q_ENUM(LayerType)

    struct LayerProps
    {
      LayerProps() {};
      LayerProps(LayerType content_type, float zoffset, float zheight)
        : content_type(content_type), zoffset(zoffset), zheight(zheight) {};
      LayerProps(LayerProps &other) {
        content_type = other.content_type;
        zoffset = other.zoffset;
        zheight = other.zheight;
      }

      bool operator=(LayerProps &other) {
        return content_type == other.content_type &&
          zoffset == other.zoffset &&
          zheight == other.zheight;
      }

      // layer properties
      LayerType content_type;
      float zoffset=0;
      float zheight=0;
    };

    //! constructor, create a Layer with the given name. If no name is given,
    //! use the default naming scheme with layer_count.
    Layer(const QString &nm = QString(), const LayerType cnt_type=DB, float z_offset=0, float z_height=0, int lay_id=-1, QObject *parent=0);

    //! Construct layer from XML stream.
    Layer(QXmlStreamReader *, int lay_id=-1);

    // destructor
    ~Layer();

    // RESET
    //! reset layers for new document
    static void resetLayers();

    // accessors

    //! set layer index and update layer_id of contained items
    void setLayerID(int lay_id);

    //! get the layer id of this layer
    int layerID() {return layer_id;}

    //! set the zoffset of the layer
    void setZOffset(const float z_offset) {props.zoffset = z_offset;}
    //! get the zoffset of the layer
    float zOffset() const {return props.zoffset;}

    //! set the zheight of the layer
    void setZHeight(const float z_height) {props.zheight = z_height;}
    //! get the zheight of the layer
    float zHeight() const {return props.zheight;}

    //! add a new Item to the current layer. If the Item is already in the layer,
    //! do nothing.
    void addItem(prim::Item *item, int index=-1);

    //! attempt to remove the given Item from the layer. Returns true if the Item
    //! is found and removed, false otherwise.
    bool removeItem(prim::Item *item);

    //! pop the Item given by the index. Returns 0 if invalid index.
    prim::Item *takeItem(int ind=-1);

    //! update the layer visibility, calls setVisible(vis) for all Items in the
    //! layer. If vis==visibility, do nothing.
    void setVisible(bool vis);

    //! check if the layer should be visible
    bool isVisible() const {return visible;}

    //! update the layer activity, calls setActive(act) for all Items in the layer
    void setActive(bool act);

    //! check if the layer should be active
    bool isActive() const {return active;}

    //! get the Layer name
    const QString& getName() const {return name;}

    //! set the Layer content type
    void setContentType(LayerType type) {props.content_type = type;}

    //! get the Layer content type, like "electrode", "dbdots", etc.
    LayerType contentType() const {return props.content_type;}
    const QString contentTypeString() const {return QString(QMetaEnum::fromType<LayerType>().valueToKey(props.content_type));}

    //! if i is within bounds, return a pointer to the indexed item in the Layer
    //! item stack; otherwise, return 0
    prim::Item *getItem(int i) { return i >= 0 && i<items.size() ? items.at(i) : 0;}

    //! get index of an item with the item's pointer
    int getItemIndex(prim::Item *item) {return items.indexOf(item);}

    //! get the Layer's items, needs to be a copy rather than a reference for Layer removal
    QStack<prim::Item*> &getItems() {return items;}

    // SAVE LOAD
    virtual void saveLayer(QXmlStreamWriter *) const;
    void saveLayerProperties(QXmlStreamWriter *) const;
    virtual void saveItems(QXmlStreamWriter *, gui::DesignInclusionArea) const;
    virtual void loadItems(QXmlStreamReader *, QGraphicsScene *);

  private:

    int layer_id;     // layer index in design panel's layers stack
    //static uint layer_count;  // number of created Layer() objects, does not decrement
    LayerProps props; // layer properties
    //float zoffset=0;  // layer distance from surface. +ve for above, -ve for below.
    //float zheight=0;  // layer height, +ve for height in top direction, -ve for bot direction

    QString name;     // arbitrary layer name, layers can be selected by name
    //LayerType content_type;

    // list of items in order of insertion, should probably be a linked list
    QStack<prim::Item*> items;

    // flags
    bool visible=true;// layer is shown. If false, active should aso be false
    bool active=true; // layer is edittable
  };


} // end prim namespace

#endif
