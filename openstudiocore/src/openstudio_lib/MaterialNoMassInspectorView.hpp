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

#ifndef OPENSTUDIO_MATERIALNOMASSINSPECTORVIEW_HPP
#define OPENSTUDIO_MATERIALNOMASSINSPECTORVIEW_HPP

#include "ModelObjectInspectorView.hpp"

namespace openstudio {

namespace model {

  class MasslessOpaqueMaterial;

}

class OSComboBox;

class OSLineEdit;

class OSQuantityEdit;

class StandardsInformationMaterialWidget;

class MaterialNoMassInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

  public:

    MaterialNoMassInspectorView(bool isIP, const openstudio::model::Model& model, QWidget * parent = 0);

    virtual ~MaterialNoMassInspectorView() {}

  protected:

    virtual void onClearSelection();

    virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject);

    virtual void onUpdate();

  private:

    void createLayout();

    void attach(openstudio::model::MasslessOpaqueMaterial & masslessOpaqueMaterial);

    void detach();

    void refresh();

    bool m_isIP;

    OSLineEdit * m_nameEdit = nullptr;  

    OSComboBox * m_roughness = nullptr;
        
    OSQuantityEdit * m_thermalResistance = nullptr;

    OSQuantityEdit * m_thermalAbsorptance = nullptr;

    OSQuantityEdit * m_solarAbsorptance = nullptr;

    OSQuantityEdit * m_visibleAbsorptance = nullptr;

    StandardsInformationMaterialWidget * m_standardsInformationWidget = nullptr;

};

} // openstudio

#endif // OPENSTUDIO_MATERIALNOMASSINSPECTORVIEW_HPP

