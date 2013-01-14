/*!****************************************************************************

  This source file is part of the Avogadro project.

  Copyright 2008-2009 Marcus D. Hanwell
  Copyright 2012 Kitware, Inc.

  This source code is released under the New BSD License, (the "License").

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

******************************************************************************/

#ifndef AVOGADRO_QTGUI_MESHGENERATOR_H
#define AVOGADRO_QTGUI_MESHGENERATOR_H

#include "avogadroqtguiexport.h"

#include <avogadro/core/vector.h>

#include <QtCore/QThread>

#include <vector>

namespace Avogadro {
namespace QtGui {

class Cube;
class Mesh;

/*!
 * \class MeshGenerator meshgenerator.h <avogadro/qtgui/meshgenerator.h>
 * \brief Class that can generate Mesh objects from Cube objects.
 * \author Marcus D. Hanwell
 *
 * This class implements a method of generating an isosurface Mesh from
 * volumetric data using the marching cubes algorithm. In the case of the
 * MeshGenerator class it expects a Cube as an input and an isosurface value.
 * The tables and the basic code is taken from the public domain code written
 * by Cory Bloyd (marchingsource.cpp) and available at,
 * http://local.wasp.uwa.edu.au/~pbourke/geometry/polygonise/
 *
 * You must first initialize the class and then call run() to actually
 * polygonize the isosurface. Connect to the classes finished() signal to
 * do something once the polygonization is complete.
 */

class AVOGADROQTGUI_EXPORT MeshGenerator : public QThread
{
  Q_OBJECT
public:
  /*!
   * Constructor.
   */
  explicit MeshGenerator(QObject *parent = 0);

  /*!
   * Constructor. Can be used to initialize the MeshGenerator.
   * \param cube The source Cube with the volumetric data.
   * \param mesh The Mesh that will hold the isosurface.
   * \param iso The iso value of the surface.
   * \return True if the MeshGenerator was successfully initialized.
   */
  MeshGenerator(const Cube *cube, Mesh *mesh, float iso,
                bool reverse = false, QObject *parent = 0);

  /*!
   * Destructor.
   */
  ~MeshGenerator();

  /*!
   * Initialization function, set up the MeshGenerator ready to find an
   * isosurface of the supplied Cube.
   * \param cube The source Cube with the volumetric data.
   * \param mesh The Mesh that will hold the isosurface.
   * \param iso The iso value of the surface.
   */
  bool initialize(const Cube *cube, Mesh *mesh, float iso,
                  bool reverse = false);

  /*!
   * Use this function to begin Mesh generation. Uses an asynchronous thread,
   * and so avoids locking the user interface while the isosurface is found.
   */
  void run();

  /*!
   * \return The Cube being used by the class.
   */
  const Cube * cube() const { return m_cube; }

  /*!
   * \return The Mesh being generated by the class.
   */
  Mesh * mesh() const { return m_mesh; }

  /*!
   * Clears the contents of the MeshGenerator.
   */
  void clear();

  /*!
   * \return The minimum value of the progress value.
   */
  int progressMinimum() { return m_progmin; }

  /*!
   * \return The maximum value of the progress value.
   */
  int progressMaximum() { return m_progmax; }

signals:
  /*!
   * The current value of the calculation's progress.
   */
  void progressValueChanged(int);

protected:
  /*!
   * Get the normal to the supplied point. This operation is quite expensive
   * and so should be avoided wherever possible.
   * \param pos The position of the vertex whose normal is needed.
   * \return The normal vector for the supplied point.
   */
  Vector3f normal(const Vector3f &pos);

  /*!
   * Get the offset, i.e. the approximate point of intersection of the surface
   * between two points.
   * \param val1 The position of the vertex whose normal is needed.
   * \return The normal vector for the supplied point.
   */
  float offset(float val1, float val2);

  unsigned long duplicate(const Vector3i &c, const Vector3f &pos);

  /*!
   * Perform a marching cubes step on a single cube.
   */
  bool marchingCube(const Vector3i &pos);

  float m_iso;           /*! The value of the isosurface. */
  bool m_reverseWinding; /*! Whether the winding and normals are reversed */
  const Cube *m_cube;    /*! The cube that we are generating a Mesh from. */
  Mesh *m_mesh;          /*! The mesh that is being generated. */
  float m_stepSize;      /*! The step size of the cube. */
  Vector3f m_min; /*! The minimum point in the cube. */
  Vector3i m_dim; /*! The dimensions of the cube. */
  std::vector<Vector3f> m_vertices, m_normals;
  std::vector<unsigned int> m_indices;
  int m_progmin;
  int m_progmax;

  /*!
   * These are the tables of constants for the marching cubes and tetrahedra
   * algorithms. They are taken from the public domain source at
   * http://local.wasp.uwa.edu.au/~pbourke/geometry/polygonise/
   */
  static const float a2fVertexOffset[8][3];
  static const int   a2iVertexOffset[8][3];
  static const int   a2iEdgeConnection[12][2];
  static const float a2fEdgeDirection[12][3];
  static const int   a2iTetrahedronEdgeConnection[6][2];
  static const int   a2iTetrahedronsInACube[6][4];
  static const long  aiTetrahedronEdgeFlags[16];
  static const int   a2iTetrahedronTriangles[16][7];
  static const long  aiCubeEdgeFlags[256];
  static const int   a2iTriangleConnectionTable[256][16];
};

} // End namespace QtGui
} // End namespace Avogadro

#endif // AVOGADRO_QTGUI_MESHGENERATOR_H
