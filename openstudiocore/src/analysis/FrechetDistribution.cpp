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

#include "FrechetDistribution.hpp"
#include "UncertaintyDescription_Impl.hpp"

#include "../utilities/core/Assert.hpp"

namespace openstudio {
namespace analysis {

FrechetDistribution::FrechetDistribution() 
  : UncertaintyDescription(std::shared_ptr<detail::UncertaintyDescription_Impl>(
        new detail::UncertaintyDescription_Impl(FrechetDistribution::type())))
{}

FrechetDistribution::FrechetDistribution(double alpha, double beta) 
  : UncertaintyDescription(std::shared_ptr<detail::UncertaintyDescription_Impl>(
        new detail::UncertaintyDescription_Impl(FrechetDistribution::type())))
{
  setAlpha(alpha);
  setBeta(beta);
}

UncertaintyDescriptionType FrechetDistribution::type() {
  return UncertaintyDescriptionType(UncertaintyDescriptionType::frechet_uncertain);
}

double FrechetDistribution::alpha() const {
  return impl()->getAttribute("alphas",false).valueAsDouble();
}

double FrechetDistribution::beta() const {
  return impl()->getAttribute("betas",false).valueAsDouble();
}

bool FrechetDistribution::setAlpha(double value) {
  if (value > 2) {
    impl()->setAttribute(Attribute("alphas",value),false);
    return true;
  }
  else {
    LOG(Warn,"The alphas of a frechet distribution must be greater than 2.");
    return false;
  } 
}

void FrechetDistribution::setBeta(double value) {
  impl()->setAttribute(Attribute("betas",value),false);
}

FrechetDistribution::FrechetDistribution(std::shared_ptr<detail::UncertaintyDescription_Impl> impl)
  : UncertaintyDescription(impl)
{
  OS_ASSERT(type() == FrechetDistribution::type());
}

} // analysis
} // openstudio
