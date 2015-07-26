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

#ifndef OPENSTUDIO_PHOTOVOLTAICINSPECTORVIEW_HPP
#define OPENSTUDIO_PHOTOVOLTAICINSPECTORVIEW_HPP

#include "ModelObjectInspectorView.hpp"

namespace openstudio {

namespace model {

class Photovoltaic;

}

class OSComboBox;

class OSLineEdit;

class OSQuantityEdit;

class OSDropZone;

class PhotovoltaicInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

  public:

    PhotovoltaicInspectorView(bool isIP, const openstudio::model::Model& model, QWidget * parent = 0 );

    virtual ~PhotovoltaicInspectorView() {}

  protected:

    virtual void onClearSelection();

    virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject);

    virtual void onUpdate();

  private:

    void attach(openstudio::model::Photovoltaic & Photovoltaic);

    void detach();

    void refresh();

    OSLineEdit* m_nameEdit;

	OSQuantityEdit * m_surfaceAreaEdit;

	OSQuantityEdit * m_factionActiveEdit;

	OSQuantityEdit * m_inverterEfficiencyEdit;

	OSQuantityEdit * m_azimuthAngleEdit;

	OSQuantityEdit * m_inclinationAngleEdit;

	OSQuantityEdit * m_cellEfficiencyEdit;

	OSQuantityEdit * m_gtEfficiencyEdit;

	OSQuantityEdit * m_systemEfficiencyEdit;

	OSComboBox * m_PVTypeComboBox;

	bool m_isIP;

  public slots:

    void toggleUnits(bool displayIP);
};

} // openstudio

#endif // OPENSTUDIO_PHOTOVOLTAICINSPECTORVIEW_HPP

