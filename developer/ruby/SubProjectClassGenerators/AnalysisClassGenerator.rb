######################################################################
#  Copyright (c) 2008-2015, Alliance for Sustainable Energy.  
#  All rights reserved.
#  
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#  
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#  
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
######################################################################

######################################################################
# == Synopsis 
#
#   Generator for analysis::AnalysisObject classes.
#
######################################################################

require File.dirname(__FILE__) + '/SubProjectClassGenerator.rb'

class AnalysisClassGenerator < SubProjectClassGenerator

  def initialize(className, baseClassName, pImpl, qobject)
    super(className, baseClassName, pImpl, qobject)
  end
  
  def hppConstructors()
    result = String.new
    
    result << "  // TODO: Delete if this class is abstract. Otherwise, add other items necessary for construction.\n"
    result << "  // Algorithms have fixed names, and DataPoints have no names by default, so delete the name\n"
    result << "  // argument in those cases.\n"
    result << "  " << @className << "(const std::string& name);\n\n"
    
    buffer = "   " + " " * @className.size

    result << "  // TODO: Delete if this class is abstract. Otherwise, add other items necessary for construction.\n"    
    result << "  /** Constructor provided for deserialization; not for general use. */\n"
    result << "  " << @className << "(const UUID& uuid,\n"
    result << buffer << "const UUID& versionUUID,\n"
    result << buffer << "const std::string& name,\n"
    result << buffer << "const std::string& displayName,\n"
    result << buffer << "const std::string& description);\n\n"
    
    return result
  end
  
  def implHppConstructors()
    result = String.new
    
    result << "    // TODO: Add other items necessary for construction.\n"
    result << "    " << @className << "_Impl(const std::string& name);\n\n"
    
    buffer = "          " + " " * @className.size
    
    result << "    // TODO: Add other items necessary for construction.\n"
    result << "    " << @className << "_Impl(const UUID& uuid,\n"
    result << buffer << "const UUID& versionUUID,\n"
    result << buffer << "const std::string& name,\n"
    result << buffer << "const std::string& displayName,\n"
    result << buffer << "const std::string& description);\n\n"
    
    return result
  end  
  
  def cppConstructors()
    result = String.new
    
    result << "  " << @className << "_Impl::" << @className << "_Impl(const std::string& name)\n"
    result << "    : " << @baseClassName << "_Impl(name)\n"
    result << "  {}\n\n"
    
    buffer = "               " + " " * (@className.size * 2)
    
    result << "  " << @className << "_Impl::" << @className << "_Impl(const UUID& uuid,\n"
    result << buffer << "const UUID& versionUUID,\n"
    result << buffer << "const std::string& name,\n"
    result << buffer << "const std::string& displayName,\n"
    result << buffer << "const std::string& description)\n"    
    result << "    : " << @baseClassName << "_Impl(uuid,versionUUID,name,displayName,description)\n"
    result << "  {}\n\n"
    
    return result
  end
  
  def cppPublicClassConstructors()
    result = String.new
    
    result << "// TODO: Delete if this class is abstract.\n"
    result << @className << "::" << @className << "(const std::string& name)\n"
    result << "  : " << @baseClassName << "(boost::shared_ptr<detail::" << @className << "_Impl>(\n"
    result << "        " << "new detail::" << @className << "_Impl(name)))\n"
    result << "{}\n\n"
    
    buffer = "   " + " " * (@className.size * 2)
    
    result << "// TODO: Delete if this class is abstract.\n"
    result << @className << "::" << @className << "(const UUID& uuid,\n"
    result << buffer << "const UUID& versionUUID,\n"
    result << buffer << "const std::string& name,\n"
    result << buffer << "const std::string& displayName,\n"
    result << buffer << "const std::string& description)\n"
    
    buffer = "                          " + " " * @className.size
    
    result << "  : " << @baseClassName << "(boost::shared_ptr<detail::" << @className << "_Impl>(\n"
    result << "        " << "new detail::" << @className << "_Impl(uuid,\n"
    result << buffer << "versionUUID,\n"
    result << buffer << "name,\n"
    result << buffer << "displayName,\n"
    result << buffer << "description)\n"
    result << "{}\n\n"
        
    return result
  end  
  
  def hppPublicMethods()
    result = String.new
    result << "  /** @name Getters and Queries */\n"
    result << "  //@{\n\n"
    result << "  // TODO: Provide getters for all items needed by the deserialization constructor.\n\n"
    result << "  //@}\n"
    result << "  /** @name Actions */\n"
    result << "  //@{\n\n"
    result << "  // TODO: Declare methods used by AnalysisDriver et al. in running analyses.\n\n"
    result << "  //@}\n"
    return result
  end
  
  def implHppPublicMethods()
    result = String.new
    
    result << "    /** @name Virtual Methods */\n"
    result << "    //@{\n\n"
    result << "    // TODO: Declare virtual methods that need to be overridden.\n\n"
    result << "    //@}\n"
    result << "    // TODO: Copy, and possibly modify, declarations made in the public class.\n"    
    
    return result
  end  
  
  def hppProtectedFriends()
    result = String.new
    result << "  friend class AnalysisObject;\n"
    result << "  friend class detail::AnalysisObject_Impl;\n"
    return result
  end  
  
  def implHppProtectedMethods()
    result = String.new
    result << "    // TODO: Add protected data members.\n"    
    return result
  end  
  
end
