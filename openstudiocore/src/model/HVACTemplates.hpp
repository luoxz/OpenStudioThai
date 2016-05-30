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

#ifndef MODEL_HVACTEMPLATES_HPP
#define MODEL_HVACTEMPLATES_HPP

#include "ModelAPI.hpp"
#include <vector>

namespace openstudio {

namespace model {

class Loop;

class Model;

class ThermalZone;

MODEL_API void addSystemType1(Model & model, std::vector<ThermalZone> zones);

MODEL_API void addSystemType2(Model & model, std::vector<ThermalZone> zones);

MODEL_API Loop addSystemType3(Model & model);

MODEL_API Loop addSystemType4(Model & model);

MODEL_API Loop addSystemType5(Model & model);

MODEL_API Loop addSystemType6(Model & model);

MODEL_API Loop addSystemType7(Model & model);

MODEL_API Loop addSystemType8(Model & model);

MODEL_API Loop addSystemType9(Model & model);

MODEL_API Loop addSystemType10(Model & model);

MODEL_API Loop airLoopHVACTHAI(Model & model);


/*Split type/DX Type*/
MODEL_API Loop addSplitTypeHVACTHAIType0(Model & model);

/*Split type/DX Type (Inverter)*/
MODEL_API Loop addSplitTypeHVACTHAIType1(Model & model);

/*Split type/DX Type (Inverter) with ERV*/
MODEL_API Loop addSplitTypeHVACTHAIType2(Model & model);

/*Central System (Air Loop and Water Loop)*/
MODEL_API Loop addCentralSystemHVACTHAI(Model & model);

/*Central System (Thai Central A/C Air Cooled System)*/
MODEL_API Loop addCentralSystemWithAirCooled(Model & model);

/*Central System (Thai  Central  A/C Water Cooled System)*/
MODEL_API Loop addCentralSystemWithWaterCooled(Model & model);

/*Thai Air Loop*/
MODEL_API Loop addThaiAirLoop(Model & model);

/*Thai Air Loop with ERV*/
MODEL_API Loop addThaizAirLoopWithERV(Model & model);


} // model

} // openstudio

#endif // MODEL_HVACTEMPLATES_HPP
