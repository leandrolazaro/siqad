/** @file:     lattice.h
 *  @author:   Jake
 *  @created:  2016.11.15
 *  @editted:  2017.06.07  - Jake
 *  @license:  GNU LGPL v3
 *
 *  @desc:     Specialized Layer class, Lattice, which details the dangling
 *             bond locations.
 */

#ifndef _GUI_PR_LATTICE_H_
#define _GUI_PR_LATTICE_H_

#include "layer.h"
#include <QHash>

namespace prim{

  class DBDot;

  struct LatticeCoord {
    //! Construct a lattice coordinate with n, m and l coordinates.
    LatticeCoord(int n, int m, int l) : n(n), m(m), l(l) {};

    //! Construct an empty and invalid lattice coordinate
    LatticeCoord() : n(-1), m(-1), l(-1) {};

    int n;
    int m;
    int l;  // invalid if l < 0

    bool isValid() const {
      return (n < 0 || m < 0 || l < 0) ? false : true;
    }

    bool operator==(const LatticeCoord &other) const {
      if (other.n == n && other.m == m && other.l == l)
        return true;
      return false;
    }

    //! Lattice coordinate addition, returned lattice coordinate might not be
    //! valid!
    LatticeCoord operator+(const LatticeCoord other) const {
      return LatticeCoord(n+other.n, m+other.m, l+other.l);
    }

    //! Lattice coordinate subtraction, returned lattice coordinate might not be
    //! valid!
    LatticeCoord operator-(const LatticeCoord other) const {
      return LatticeCoord(n-other.n, m-other.m, l-other.l);
    }

    LatticeCoord operator*(int k) const{
      return LatticeCoord(n*k, m*k, l*k);
    }
  };

  class Lattice : public prim::Layer
  {
  public:

    //! Construct layer from given properties.
    Lattice(const QString &fname = QString(), int lay_id=-1);

    //! destructor
    ~Lattice() {}

    //! Save the lattice layer to XML stream.
    void saveLayer(QXmlStreamWriter *) const override;

    //! Return specified lattice vector after graphical scaling
    QPoint sceneLatticeVector(int dim) const {return a_scene[dim];}

    //! Return specified site vector after graphical scaling
    QPoint sceneSiteVector(int ind) const {return b_scene[ind];}

    //! Identify the nearest lattice site to the given scene position.
    LatticeCoord nearestSite(const QPointF &pos, bool is_scene_pos) const;

    //! Identify the nearest lattice site to the given scene position, returns
    //! it in lattice coordinates and updates the reference QPointF site_pos.
    LatticeCoord nearestSite(const QPointF &pos, QPointF &nearest_site_pos,
        bool is_scene_pos) const;

    //! Return a QList of lattice site coordinates enclosed in a given QRectF 
    //! in scene coordinates. WARNING this won't work with rotated lattices!
    QList<LatticeCoord> enclosedSites(const QRectF &scene_rect) const;

    //! Return all sites enclosed in given lattice coordinates. WARNING this won't
    //! work with rotated lattices!
    QList<prim::LatticeCoord> enclosedSites(const prim::LatticeCoord &coord1,
        const prim::LatticeCoord &coord2) const;

    //! Convert lattice coordinates to scene position in QPointF. Does not check 
    //! for validity.
    QPointF latticeCoord2ScenePos(const prim::LatticeCoord &l_coord) const;

    //! Convert lattice coordinates to physical location in angstrom. Does not 
    //! check for validity.
    QPointF latticeCoord2PhysLoc(const prim::LatticeCoord &coord) const;

    //! Return whether a given scene_pos collides with the given lattice position
    bool collidesWithLatticeSite(const QPointF &scene_pos, const LatticeCoord &l_coord) const;

    //! Set lattice dot location to be occupied
    void setOccupied(const prim::LatticeCoord &l_coord, prim::DBDot *dbdot) {
      qDebug() << tr("set occupied %1, %2, %3").arg(l_coord.n).arg(l_coord.m).arg(l_coord.l);
      occ_latdots.insert(l_coord, dbdot);
    }

    //! Set lattice dot location to be unoccupied
    void setUnoccupied(const prim::LatticeCoord &l_coord) {
      occ_latdots.remove(l_coord);
    }

    //! Clear occupation list (the pointers aren't actually deleted).
    void clearOccupation() {
      occ_latdots.clear();
    }

    //! Return whether lattice dot location is occupied.
    bool isOccupied(const prim::LatticeCoord &l_coord) {
      return occ_latdots.contains(l_coord);
    }

    //! Return whether the given lattice coordinate is a valid coordinate.
    bool isValid(const prim::LatticeCoord &l_coord) const {
      return (l_coord.l >= 0 && l_coord.l < b.length());
    }

    //! Return the DBDot pointer at the specified lattice coord, or nullptr if none.
    prim::DBDot *dbAt(const prim::LatticeCoord &l_coord) {
      return occ_latdots.contains(l_coord) ? occ_latdots.value(l_coord) : nullptr;
    }

    //! Return a list of DBDot pointers at specified physical locations (angstrom).
    QList<prim::DBDot*> dbsAtPhysLocs(const QList<QPointF> &physlocs);

    //! identify the bounding rect of an approximately rectangular supercell
    QRectF tileApprox();

    //! Return a tileable image that represents the lattice.
    QImage tileableLatticeImage(QColor bkg_col, bool publish=false);

    //! Set the visiblity of the lattice
    void setVisible(bool);

  private:

    int n_cell;         // number of atoms in unit cell

    QPointF a[2];       // lattice vectors
    QPoint a_scene[2];  // lattice vector after graphical scaling
    QList<QPointF> b;   // unit cell site vectors
    QList<QPoint> b_scene;  // unit cell site vectors after graphical scaling

    qreal Lx;           // x-bound on lattice vectors, in Angstroms
    qreal Ly;           // y-bound on lattice vectors, in Angstroms

    qreal coth;         // cotangent of angle between lattice vectors
    bool orthog;        // lattice vectors are orthogonal
    qreal a2[2];        // square magnitudes of lattice vectors

    QHash<prim::LatticeCoord, prim::DBDot*> occ_latdots; // set of occupied lattice dots

    // constants

    static qreal rtn_acc;     // termination precision for rationalize
    static int rtn_iters;     // max iterations of rationalize

    // construct lattice from lattice settings
    void construct();

    // Find a rational approximation of a given float
    QPair<int,int> rationalize(qreal x, int k=0);

    // construct lattice statics
    void constructStatics();

    // GUI statics
    static qreal lat_diam;
    static qreal lat_diam_pb;
    static qreal lat_edge_width;
    static qreal lat_edge_width_pb;
    static QColor lat_edge_col;
    static QColor lat_edge_col_pb;
    static QColor lat_fill_col;
    static QColor lat_fill_col_pb;
  };  // end of Lattice class

  class LatticeDotPreview : public Item
  {
  public:
    //! Construct a lattice dot preview at the given lattice coordinate.
    LatticeDotPreview(prim::LatticeCoord l_coord);

    //! Destructor.
    ~LatticeDotPreview() {}

    // Accessors

    //! Get the lattice coordinates of the lattice dot preview.
    prim::LatticeCoord latticeCoord() {return lat_coord;}

    // Graphics
    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) override;

  private: 
    //! Construct static variables on first creation.
    void constructStatics();

    // Variables
    prim::LatticeCoord lat_coord; // lattice coordinates of the lattice dot preview.

    // Static class variables
    static QColor fill_col;
    static QColor fill_col_pb;
    static QColor edge_col;
    static QColor edge_col_pb;

    static qreal diameter;
    static qreal diameter_pb;
    static qreal edge_width;
    static qreal edge_width_pb;
  };  // end of LatticeDotPreview class

  //! Hash function for lattice coordinates
  inline uint qHash(const prim::LatticeCoord &l_coord, uint seed=0)
  {
    return ::qHash(l_coord.n, seed) + ::qHash(l_coord.m, seed) + ::qHash(l_coord.l, seed);
  }

} // end prim namespace


#endif
