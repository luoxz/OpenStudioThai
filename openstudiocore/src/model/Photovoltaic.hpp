#ifndef MODEL_PHOTOVOLTAIC_HPP
#define MODEL_PHOTOVOLTAIC_HPP

#include "ModelAPI.hpp"
#include "ResourceObject.hpp"

namespace openstudio {
namespace model {

namespace detail {

  class Photovoltaic_Impl;

} // detail

class MODEL_API Photovoltaic : public ResourceObject{
 public:
  /** @name Constructors and Destructors */
  //@{

  explicit Photovoltaic(const Model& model);

  virtual ~Photovoltaic() {}

  //@}

  static IddObjectType iddObjectType();
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
  /// @cond
  typedef detail::Photovoltaic_Impl ImplType;

  explicit Photovoltaic(std::shared_ptr<detail::Photovoltaic_Impl> impl);

  friend class detail::Photovoltaic_Impl;
  friend class Model;
  friend class IdfObject;
  friend class openstudio::detail::IdfObject_Impl;
  /// @endcond
 private:
  REGISTER_LOGGER("openstudio.model.Photovoltaic");
};

/** \relates Photovoltaic*/
typedef boost::optional<Photovoltaic> OptionalPhotovoltaic;

/** \relates Photovoltaic*/
typedef std::vector<Photovoltaic> PhotovoltaicVector;

} // model
} // openstudio

#endif // MODEL_PHOTOVOLTAIC_HPP

