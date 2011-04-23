/**
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 **/

#pragma once

/**
 * @file ImplicitShapeFactory.h
 * @author David Coeurjolly (\c david.coeurjolly@liris.cnrs.fr )
 * Laboratoire d'InfoRmatique en Image et Systèmes d'information - LIRIS (CNRS, UMR 5205), CNRS, France
 *
 * @date 2011/03/22
 *
 * Header file for module ImplicitShapeFactory.cpp
 *
 * This file is part of the DGtal library.
 */

#if defined(ImplicitShapeFactory_RECURSES)
#error Recursive header files inclusion detected in ImplicitShapeFactory.h
#else // defined(ImplicitShapeFactory_RECURSES)
/** Prevents recursive inclusion of headers. */
#define ImplicitShapeFactory_RECURSES

#if !defined ImplicitShapeFactory_h
/** Prevents repeated inclusion of headers. */
#define ImplicitShapeFactory_h

//////////////////////////////////////////////////////////////////////////////
// Inclusions
#include <iostream>
#include "DGtal/base/Common.h"
#include "DGtal/helpers/implicitShapes/ImplicitBall.h"
#include "DGtal/helpers/implicitShapes/ImplicitNorm1Ball.h"
#include "DGtal/helpers/implicitShapes/ImplicitHyperCube.h"
#include "DGtal/helpers/implicitShapes/ImplicitRoundedHyperCube.h"
#include "DGtal/helpers/parametricShapes/Ball2D.h"
#include "DGtal/helpers/parametricShapes/Flower2D.h"
#include "DGtal/helpers/parametricShapes/NGon2D.h"

//////////////////////////////////////////////////////////////////////////////

namespace DGtal
{

  /////////////////////////////////////////////////////////////////////////////
  // template class ImplicitShapeFactory
  /**
   * Description of template class 'ImplicitShapeFactory' <p>
   * \brief Aim: collects all models of CImplicitShape concept to create a shape factory.
   *
   */
  
  

} // namespace DGtal
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#endif // !defined ImplicitShapeFactory_h

#undef ImplicitShapeFactory_RECURSES
#endif // else defined(ImplicitShapeFactory_RECURSES)
