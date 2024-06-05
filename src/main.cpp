#include <openvdb/openvdb.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <future>

using namespace openvdb;

// Function to read data from the Gaussian Cube file
std::vector<float> readCubeFile(const std::string& filename,  openvdb::Vec3f& origin, size_t& dim, float& voxelSize, const bool doLog) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        exit(1);
    }
    
    std::vector<float> data;

    std::string line;
    std::getline(file, line); // Skip header lines
    std::getline(file, line); // Skip header lines
    std::getline(file, line);

    // Read origin
    std::istringstream iss(line);
    size_t numAtoms;
    iss >> numAtoms;
    iss >> origin.x();
    iss >> origin.y();
    iss >> origin.z();

    // Read voxel size
    std::vector<float> sizes(3);
    std::getline(file, line);
    std::istringstream sizeStream(line);

    sizeStream >> dim;
    sizeStream >> voxelSize;
    //std::cout << dim << " " << voxelSize << std::endl;
    
    std::getline(file, line);
    std::getline(file, line);
    
    // skip over atoms
    for( auto i = 0; i < numAtoms; i++ )
      std::getline(file, line);

    // Read data
    float value;
    while (file >> value) {
        value = abs(value);  // yuck.....
        if(doLog) value = log10(value);
        data.push_back(value);
    }
    return data;
}

int main(int argc, char** argv) {
    if (argc < 1) {
        std::cerr << "Usage: " << argv[0] << " <input_cube_file_1> [input_cube_file_2 ...]" << std::endl;
        return 1;
    }

    std::vector<std::string> inputFiles;
    for( size_t id = 1; id < argc; ++id ) {
      inputFiles.emplace_back(argv[id]);
    }
    bool doLogarithm = false;
    size_t id = 1;
    for (auto& instr : inputFiles) {
    }
    auto findFlag = [&](std::string& c) {
        if (c == "-L") {
            doLogarithm = true;
            return true;
        }
        return false;
    };
      
    erase_if(inputFiles, findFlag);
    
    
    auto processFile = [&doLogarithm](const std::string fileName) {
      openvdb::Vec3f origin;
      float voxelSize;
      size_t dim;
      std::vector<float> data = readCubeFile(fileName, origin, dim, voxelSize, doLogarithm);

      // If log, shift data up by its minimum
      if( doLogarithm ) {
        auto datamin = std::min_element(data.begin(), data.end());
        for(auto& c : data)
          c += *datamin;
      }
      
      // Create an OpenVDB grid
      openvdb::initialize();
      openvdb::FloatGrid::Ptr grid = openvdb::FloatGrid::create();
      grid->setName("density");
      openvdb::FloatGrid::Accessor accessor = grid->getAccessor();
      openvdb::Coord ijk;
      for (float value : data) {
          ++ijk[0];
          accessor.setValue(ijk, value);
          if (ijk[0] == dim) {
              ++ijk[1];
              ijk[0] = 0;
              if (ijk[1] == dim) {
                  ++ijk[2];
                  ijk[1] = 0;
              }
          }
      }

      // Set metadata
      grid->setTransform(openvdb::math::Transform::createLinearTransform(voxelSize));
      grid->setGridClass(openvdb::GRID_FOG_VOLUME);
      
      // Write the OpenVDB grid to file
      size_t lastDotPos = fileName.find_last_of(".");
      auto outputFileName = fileName;
      if( !(lastDotPos == std::string::npos) ) 
        outputFileName = fileName.substr(0, lastDotPos);
      outputFileName += ".vdb";

      std::cout << "Processing " << fileName << " -> " << outputFileName << std::endl;
      openvdb::io::File file(outputFileName);
      file.write({grid});
      file.close();

    };
      
    std::vector<std::future<void>> futures;
    for( auto& n : inputFiles )
      futures.push_back(std::async(std::launch::async, processFile, std::ref(n)));
    
    for(auto& future : futures )
      future.wait();
    
    return 0;
}

