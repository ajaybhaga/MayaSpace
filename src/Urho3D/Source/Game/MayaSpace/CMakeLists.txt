#
# Copyright (c) 2008-2018 the Urho3D project.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

if (NOT URHO3D_URHO2D)
    return ()
endif ()

# Define target name
set (TARGET_NAME MayaSpace)

# Define source files
define_source_files (EXTRA_H_FILES ${COMMON_SAMPLE_H_FILES} ./Sample2D.h ./Sample2D.cpp ../Utilities2D/Mover.h ../Utilities2D/Mover.cpp ai/genotype.cpp ai/genotype.h util/random_d.h ai/genetic_algorithm.cpp ai/genetic_algorithm.h ai/evolution_manager.cpp ai/evolution_manager.h ai/agent.cpp ai/agent.h ai/neural_layer.cpp ai/neural_layer.h ai/neural_network.cpp ai/neural_network.h util/math_helper.cpp util/math_helper.h util/event.cpp util/event.h ai/agent_controller.cpp ai/agent_controller.h shared_libs.h ai/sensor.cpp ai/sensor.h app.cpp app.h ai/agent_movement.cpp ai/agent_movement.h ai/fsm_event_data.cpp ai/fsm_event_data.h ai/fsm.cpp ai/fsm.h util/semaphore.h ai/agent_fsm.cpp ai/agent_fsm.h util/event.cpp util/event.h)

# Setup target with resource copying
setup_main_executable ()

# Setup test cases
setup_test ()
