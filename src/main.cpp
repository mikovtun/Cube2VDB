#include <openvdb/openvdb.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace openvdb;

// Function to read data from the Gaussian Cube file
void readCubeFile(const std::string& filename, std::vector<float>& data, openvdb::Vec3f& origin, size_t& dim, float& voxelSize) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        exit(1);
    }

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
    std::cout << origin << std::endl;

    // Read voxel size
    std::vector<float> sizes(3);
    std::getline(file, line);
    std::cout << line;
    std::istringstream sizeStream(line);

    sizeStream >> dim;
    sizeStream >> voxelSize;
    std::cout << dim << " " << voxelSize << std::endl;
    
    std::getline(file, line);
    std::getline(file, line);
    
    // skip over atoms
    for( auto i = 0; i < numAtoms; i++ )
      std::getline(file, line);

    // Read data
    float value;
    while (file >> value) {
        data.push_back(value);
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_cube_file> <output_vdb_file>" << std::endl;
        return 1;
    }

    // Read data from the Gaussian Cube file
    std::string inputCubeFile = argv[1];
    std::string outputVdbFile = argv[2];
    std::vector<float> data;
    openvdb::Vec3f origin;
    float voxelSize;
    size_t dim;
    readCubeFile(inputCubeFile, data, origin, dim, voxelSize);
    
    std::cout << "origin: " << origin << std::endl;
    std::cout << "voxelSize: " << voxelSize << std::endl;
    
    // Create an OpenVDB grid
    openvdb::initialize();
    openvdb::FloatGrid::Ptr grid = openvdb::FloatGrid::create();
    grid->setName("Hello!");
    openvdb::FloatGrid::Accessor accessor = grid->getAccessor();
    openvdb::Coord ijk;
    for (float value : data) {
        ++ijk[0];
        accessor.setValue(ijk, 1.0e6 * value);
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
    openvdb::io::File file(outputVdbFile);
    file.write({grid});
    file.close();

    std::cout << "Conversion completed. OpenVDB file saved as " << outputVdbFile << std::endl;

    return 0;
}

