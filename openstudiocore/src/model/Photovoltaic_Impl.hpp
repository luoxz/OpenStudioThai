#ifndef MODEL_PHOTOVOLTAIC_IMPL_HPP
#define MODEL_PHOTOVOLTAIC_IMPL_HPP

#include "ModelAPI.hpp"
#include "ResourceObject_Impl.hpp"

namespace openstudio {
namespace model {

namespace detail {

	class MODEL_API Photovoltaic_Impl : public ResourceObject_Impl {
	  Q_OBJECT;
	  Q_PROPERTY(double designLevel READ designLevel WRITE setDesignLevel);
   public:
    /** @name Constructors and Destructors */
    //@{

    Photovoltaic_Impl(const IdfObject& idfObject,
                         Model_Impl* model,
                         bool keepHandle);

    Photovoltaic_Impl(const openstudio::detail::WorkspaceObject_Impl& other,
                         Model_Impl* model,
                         bool keepHandle);

    Photovoltaic_Impl(const Photovoltaic_Impl& other,
                         Model_Impl* model,
                         bool keepHandle);

    virtual ~Photovoltaic_Impl() {}

    //@}
    /** @name Virtual Methods */
    //@{

	virtual const std::vector<std::string>& outputVariableNames() const;

    virtual IddObjectType iddObjectType() const;

	//@}
	/** @name Getters */
	//@{

	double designLevel() const;

	//@}
	/** @name Setters */
	//@{

	bool setDesignLevel(double designLevel);

	//@}
	/** @name Other */
	//@{

	//@}
   protected:
   private:
    REGISTER_LOGGER("openstudio.model.Photovoltaic");
  };

} // detail

} // model
} // openstudio

#endif // MODEL_PHOTOVOLTAIC_IMPL_HPP

