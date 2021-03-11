#include "Main.h"

// Metode principal del programa

int main(int argc, char **argv) {


    cout << "Init..." <<endl;
    Q_INIT_RESOURCE(resources);

    //auto controller = make_shared<Controller>("://resources/dataBCN.txt", "://resources/configMappingData.txt", "://resources/configVis.txt");
    auto controller = make_shared<Controller>("://resources/spheres.txt", "://resources/configMapping.txt", "://resources/configVis.txt");
    //auto controller = make_shared<Controller>("://resources/boundaryObject.txt", "://resources/configMapping.txt", "://resources/configVis.txt");
    controller->start(argc, argv);

    return(0);
}
