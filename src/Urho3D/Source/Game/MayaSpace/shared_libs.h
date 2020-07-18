//
// C++ Implementation by Ajay Bhaga
//


#pragma once

#include <string>
#include <list>
#include <iostream>
#include <fstream>
#include <ctime>
#include <filesystem>
#include <functional>
#include <string>
#include <map>
#include <chrono> // std::chrono::microseconds
#include <thread> // std::this_thread::sleep_for
#include <vector>
#include <cassert>

#include <stdlib.h>     /* atoi */
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include "ai/genotype.h"
#include "ai/neural_layer.h"
#include "ai/genetic_algorithm.h"
#include "ai/neural_network.h"
#include "ai/sensor.h"
#include "ai/agent.h"
//#include "agent_movement.h"
//#include "agent_controller.h"
#include "ai/evolution_manager.h"

//#include "evolution_manager.h"

// Config variables
#define TRAINING_DATA_DIR "data/"
#define NUM_NEURAL_LAYERS 4

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
static const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}