#include "Photovoltaic.hpp"
#include "Photovoltaic_Impl.hpp"

#include <utilities/idd/OS_Exterior_PV_FieldEnums.hxx>
#include <utilities/idd/IddEnums.hxx>

#include "../utilities/core/Assert.hpp"

namespace openstudio {
	namespace model {

		namespace detail {

			Photovoltaic_Impl::Photovoltaic_Impl(const IdfObject& idfObject,
				Model_Impl* model,
				bool keepHandle)
				: ResourceObject_Impl(idfObject, model, keepHandle)
			{
				OS_ASSERT(idfObject.iddObject().type() == Photovoltaic::iddObjectType());
			}

			Photovoltaic_Impl::Photovoltaic_Impl(const openstudio::detail::WorkspaceObject_Impl& other,
				Model_Impl* model,
				bool keepHandle)
				: ResourceObject_Impl(other, model, keepHandle)
			{
				OS_ASSERT(other.iddObject().type() == Photovoltaic::iddObjectType());
			}

			Photovoltaic_Impl::Photovoltaic_Impl(const Photovoltaic_Impl& other,
				Model_Impl* model,
				bool keepHandle)
				: ResourceObject_Impl(other, model, keepHandle)
			{}

			const std::vector<std::string>& Photovoltaic_Impl::outputVariableNames() const
			{
				static std::vector<std::string> result;
				if (result.empty()){
				}
				return result;
			}

			IddObjectType Photovoltaic_Impl::iddObjectType() const {
				return Photovoltaic::iddObjectType();
			}

			double Photovoltaic_Impl::designLevel() const {
				boost::optional<double> value = getDouble(OS_Exterior_PVFields::DesignLevel, true);
				OS_ASSERT(value);
				return value.get();
			}

			bool Photovoltaic_Impl::setDesignLevel(double designLevel) {
				bool result = setDouble(OS_Exterior_PVFields::DesignLevel, designLevel);
				return result;
			}

		} // detail

		Photovoltaic::Photovoltaic(const Model& model)
			: ResourceObject(Photovoltaic::iddObjectType(), model)
		{
			OS_ASSERT(getImpl<detail::Photovoltaic_Impl>());

			bool ok = setDesignLevel(0.0);
			OS_ASSERT(ok);
		}

		IddObjectType Photovoltaic::iddObjectType() {
			IddObjectType result(IddObjectType::OS_Exterior_PV);
			return result;
		}

		double Photovoltaic::designLevel() const {
			return getImpl<detail::Photovoltaic_Impl>()->designLevel();
		}

		bool Photovoltaic::setDesignLevel(double designLevel) {
			return getImpl<detail::Photovoltaic_Impl>()->setDesignLevel(designLevel);
		}

/// @cond
Photovoltaic::Photovoltaic(std::shared_ptr<detail::Photovoltaic_Impl> impl)
	: ResourceObject(impl)
{}
/// @endcond

} // model
} // openstudio

