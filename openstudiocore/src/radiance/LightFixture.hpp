/**********************************************************************
*  Copyright (c) 2008-2015, Alliance for Sustainable Energy.  
*  All rights reserved.
*  
*  This library is free software; you can redistribute it and/or
*  modify it under the terms of the GNU Lesser General Public
*  License as published by the Free Software Foundation; either
*  version 2.1 of the License, or (at your option) any later version.
*  
*  This library is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*  Lesser General Public License for more details.
*  
*  You should have received a copy of the GNU Lesser General Public
*  License along with this library; if not, write to the Free Software
*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
**********************************************************************/

#ifndef RADIANCE_LIGHTFIXTURE_HPP
#define RADIANCE_LIGHTFIXTURE_HPP

#include "RadianceAPI.hpp"

#include "../utilities/data/Vector.hpp"
#include "../utilities/core/Path.hpp"

namespace openstudio{
namespace radiance{

  /** LightFixture represents the physical location, orientation, and illuminance distribution of a light fixture.
  */ 
  class RADIANCE_API LightFixture{
    public:
      openstudio::Vector location;
      openstudio::Vector orientation;
      openstudio::path iesFile;
  };

  // vector of LightFixture
  typedef std::vector<LightFixture> LightFixtureVector;


} // radiance
} // openstudio

#endif //RADIANCE_LIGHTFIXTURE_HPP
