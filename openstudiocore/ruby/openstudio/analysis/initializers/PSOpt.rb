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

require "#{File.dirname(__FILE__)}/../DakotaInitializer.rb"

require 'pathname'

class PSOpt < DakotaInitializer

  def initialize(out_dir="",prob_file="",x0=[],
                 driver=true,nodes=1,cores=4,verbose=false,restart_file=OpenStudio::Path.new)
    super(out_dir,prob_file,x0,driver,nodes,cores,verbose,restart_file)
  end
  
  def in_file_method()
    result = String.new
    result << "method,\n"
    result << "        coliny_pattern_search\n"
    result << "          max_iterations = 1000\n"
    result << "          max_function_evaluations = 2000\n"
    result << "          solution_accuracy = 1.0e-4\n"
    result << "          initial_delta = 0.5\n"
    result << "          threshold_delta = 1.0e-4\n"
    result << "          exploratory_moves basic_pattern\n"
    result << "          contraction_factor = 0.75\n\n"
    return result
  end  
  
  def in_file_variables()
    result = super
    if not @problem.x0.empty?
      result << "          initial_point "
      for i in 1..@problem.x0.size()
        result << @problem.x0[i-1].to_s.rjust(8)
      end
      result << "\n"
    end
    result << "          lower_bounds"
    for i in 1..@problem.nx
      result << (-2.0).to_s.rjust(8)
    end
    result << "\n          upper_bounds" 
    for i in 1..@problem.nx
      result << (2.0).to_s.rjust(8)
    end
    result << "\n\n"
    return result
  end  
  
  def in_file_responses()  
    result = String.new
    result << "responses,\n"
    result << "        num_objective_functions = 1\n"
    result << "        num_nonlinear_inequality_constraints = " + @problem.ng.to_s + "\n" if @problem.ng > 0
    result << "        num_nonlinear_equality_constraints = " + @problem.nh.to_s + "\n" if @problem.nh > 0    
    result << "        no_gradients\n"
    result << "        no_hessians\n\n"
    return result  
  end  
      
end

PSOpt.new

