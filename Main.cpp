#include "Main.h"

// Metode principal del programa

int main(int argc, char **argv) {


    cout << "Init..." <<endl;
    Q_INIT_RESOURCE(resources);

    auto controller = make_shared<Controller>("://resources/data0.txt", "://resources/configMappingData0.txt", "://resources/configVisData0.txt");
    //auto controller = make_shared<Controller>("://resources/spheres.txt", "://resources/configMapping_Fase2A.txt", "://resources/configVis.txt");
    //auto controller = make_shared<Controller>("://resources/boundaryObject.txt", "://resources/configMapping.txt", "://resources/configVis.txt");
    //auto controller = make_shared<Controller>("://resources/3D_Scene_myScene.txt", "://resources/configMapping.txt", "://resources/configVis.txt");
    //auto controller = make_shared<Controller>("://resources/Scene_Fase2A.txt", "://resources/configMapping_Fase2A.txt", "://resources/configVis.txt");
    cout << "hola" << endl;
    controller->start(argc, argv);

    return(0);
}
