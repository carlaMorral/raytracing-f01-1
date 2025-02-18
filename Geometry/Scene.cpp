#include "Scene.h"

Scene::Scene()
{
    pmin.x = -0.5f;  pmin.y = -0.5f; pmin.z = -0.5f;
    pmax.x = 0.5f;  pmax.y = 0.5f; pmax.z = 0.5f;
}


/*
** TODO: FASE 1: Metode que testeja la interseccio contra tots els objectes de l'escena
**
** Si un objecte es intersecat pel raig, el parametre  de tipus IntersectInfo conte
** la informació sobre la interesccio.
** El metode retorna true si algun objecte es intersecat o false en cas contrari.
**
*/
bool Scene::hit(const Ray& raig, float t_min, float t_max, HitInfo& info) const {


    // TODO FASE 0 i FASE 1: Heu de codificar la vostra solucio per aquest metode substituint el 'return true'
    // Una possible solucio es cridar el mètode intersection per a tots els objectes i quedar-se amb la interseccio
    // mes propera a l'observador, en el cas que n'hi hagi més d'una.
    // Cada vegada que s'intersecta un objecte s'ha d'actualitzar el IntersectionInfo del raig,
    // pero no en aquesta funcio.

    float minim_t = t_max;
    for (unsigned int i = 0; i< objects.size(); i++) {
        if (objects[i]->hit(raig, t_min, minim_t, info)) {
            minim_t = info.t;
            info.indObject = i;
        }
    }
    // hit of the floor
    if (floor.isVisible()) {
        if (floor.hit(raig, t_min, minim_t, info)) {
            minim_t = info.t;
        }
    }
    if (abs(minim_t - t_max) < 0.0001) {
        return false;
    }
    return true;
}


/*
** TODO: Funcio ComputeColorRay es la funcio recursiva del RayTracing.
** TODO: FASE 0 per a fer el degradat del fons
** TODO: FASE 1 per a cridar a la intersecció amb l'escena i posar el color de l'objecte
** TODO: Fase 2 de l'enunciat per incloure Blinn-Phong  i ombres
** TODO: Fase 2 per a tractar reflexions i transparències
**
*/
vec3 Scene::ComputeColor (Ray &ray, int depth, vec3 lookFrom, vec3 accCol) {
    numCompColors++;
    vec3 color;
    vec3 recColor = vec3(0);
    vec3 scatterColor;
    vec3 ray2;
    vector<Ray> reflected;
    ray2 = normalize(ray.dirVector());

    HitInfo info;
    if (hit(ray, 0, 500, info)){
        //Segons el color que ens dona Blinn-Phong:
        color = blinn_phong(ray, info, lookFrom);

        if(depth == MAXDEPTH || dynamic_cast<MaterialTextura*>(info.mat_ptr) || length(accCol) < ACCCOLOR){
            return color;
        }else{
            info.mat_ptr->scatter(ray, info, scatterColor, reflected);
            int reflectedAmount = reflected.size();
            for (int i = 0; i < reflectedAmount; i++) {
                recColor += ComputeColor(reflected[i], depth+1, lookFrom, accCol*scatterColor);
            }
            recColor /= max({1, reflectedAmount});
            //return (vec3(1)-info.mat_ptr->k)*color + recColor * scatterColor;
            return color + recColor * scatterColor;// Blinn-phong al transparent
        }

    } else {
        if (depth != 0 && AMBIENTSECRAYS) {
            return globalLight;
        } else {
            vec3 color1 = vec3(0.5, 0.7, 1);
            vec3 color2 = vec3(1, 1 ,1);
            // TODO: A canviar el càlcul del color en les diferents fases
            double y = 0.5*(ray2.y+1);
            color = (float)y*color1 + (float)(1-y)*color2;
            return color;
        }
    }

}

vec3 Scene::blinn_phong(Ray &ray, HitInfo &info, vec3 lookFrom){
    vec3 ca = vec3(0,0,0);
    vec3 cd = vec3(0,0,0);
    vec3 cs = vec3(0,0,0);
    vec3 diffuse;
    vector<HitInfo> infoOmbra;
    vec3 cout;
    vec3 cin;
    double alphain;
    double alphaout;
    double alpha;
    vec3 id;
    //Per cada Light
    for(int i=0; i<pointLights.size(); i++){
        bool ambientTextura = false;
        //Component ambient
        if(MaterialTextura* mat = dynamic_cast<MaterialTextura*>(info.mat_ptr)){
            if(mat->ignoreLights){ //IGNOREM OMBRES I INTERACCIONS AMB LLUM SI ES MATERIAL TEXTURA AMB FLAG DE IGNORELIGHTS
                continue;
            }
            ambientTextura = AMBIENTTEXTURA; //Utilitzar la imatge de la textura com a component ambient també
            if(ambientTextura){
                ca += mat->getAmbient(info.uv) * this->pointLights[i]->ambient;
            }
        }
        if(!ambientTextura){
            ca += info.mat_ptr->ambient * this->pointLights[i]->ambient;
        }

        diffuse = info.mat_ptr->getDiffuse(info.uv);

        float atenuacio = this->pointLights[i]->get_atenuation(info.p);

        float factorOmbra;
        //Opcional 4: Color shadows
        if(COLORSHADOWASCTIVATED && hitOmbra(infoOmbra, info.p, info.indObject, this->pointLights[i]->position)) {
            //TODO: Calcular Cout i alpha
            alphain = 0;
            cin.x = 0;
            cin.y = 0;
            cin.z = 0;
            for(int k=0; k<infoOmbra.size(); k++) {
                alpha = 1-infoOmbra[k].t/(dynamic_cast<Transparent*>(infoOmbra[k].mat_ptr))->dmax;
                cout.x = cin.x - ((1-alphain)*infoOmbra[k].mat_ptr->diffuse.x*alpha);
                cout.y = cin.y - ((1-alphain)*infoOmbra[k].mat_ptr->diffuse.y*alpha);
                cout.z = cin.z - ((1-alphain)*infoOmbra[k].mat_ptr->diffuse.z*alpha);
                cin = cout;
                alphaout = alphain + (1-alphain)*alpha;
                alphain = alphaout;
            }
            id = cout*this->pointLights[i]->diffuse;
            factorOmbra = 1;
            infoOmbra.clear();
        } else {
            id = this->pointLights[i]->diffuse;
            //IGNOREM OMBRES I INTERACCIONS AMB LLUM SI ES MATERIAL TEXTURA AMB FLAG DE IGNORELIGHTS
            bool ignoraOmbra = false;
            if(MaterialTextura* mat = dynamic_cast<MaterialTextura*>(info.mat_ptr)){
                if(mat->ignoreLights){
                    ignoraOmbra = true;
                }
            }
            if(ignoraOmbra){
                factorOmbra = 1;
            }else{
                factorOmbra = shadowCalculation(info.p, this->pointLights[i]);
            }
        }

        //Component difusa amb atenuacio
        cd += factorOmbra*atenuacio*id * diffuse*
                std::max(dot(info.normal, glm::normalize(pointLights[i]->get_vector_L(info.p))), 0.0f);
        vec3 H = normalize(lookFrom-info.p + pointLights[i]->get_vector_L(info.p));

        //Component especular amb atenuacio
        cs += factorOmbra*atenuacio*this->pointLights[i]->specular * info.mat_ptr->specular*
                pow(std::max(dot(info.normal, H), 0.0f), info.mat_ptr->shineness);
    }

    vec3 global;
    if(MaterialTextura* mat = dynamic_cast<MaterialTextura*>(info.mat_ptr)){
        global = mat->getAmbient(info.uv);
        if(!mat->ignoreLights){ //IGNOREM OMBRES I INTERACCIONS AMB LLUM SI ES MATERIAL TEXTURA AMB FLAG DE IGNORELIGHTS
            global *= this->globalLight;
        }
    }else{
       global = this->globalLight*info.mat_ptr->ambient;
    }

    //Opcional 5: Ambient Occlusion. Suposem escenes outdoor i llancem n raigs aleatoris des de p cap al 'cel'
    float AOFactor = 1;
    if(AOACTIVATED) {
        AOFactor = ambientOcclusionFactor(info);
    }

    //Retornem la llum ambient global més les tres components
    return  AOFactor*global + ca + cd + cs;
}

struct Compare {
    vec3 point;
    Compare(vec3 point) {
        this->point = point;
    }
    bool operator()(const HitInfo& info1, const HitInfo& info2) const {
        return length(info1.p-point) < length(info2.p-point);
    }
};

bool Scene::hitOmbra(vector<HitInfo>& infoOmbra, vec3 point, int ind, vec3 lightPosition) {
    vec3 director = normalize(lightPosition - point);
    float tMax = length(lightPosition - point);
    Ray shadowRay = Ray(point, director);
    HitInfo info;
    int indBefore = -1;
    vec3 pBefore;
    while (hit(shadowRay, EPSILON, tMax, info)) {
        //Si hi ha un objecte Lambertian no calcularem aquesta ombra
        if(!dynamic_cast<Transparent*>(info.mat_ptr)) {
            return false;
        }
        if(indBefore != info.indObject) {
            infoOmbra.push_back(info);
        } else {
            infoOmbra[infoOmbra.size()-1].t = length(info.p-pBefore);
        }
        pBefore = info.p;
        shadowRay.origin = info.p;
        tMax = length(lightPosition - info.p);
        indBefore = info.indObject;
    }
    if(infoOmbra.size() == 0) {
        return false;
    }

    //Ara toca ordenar el vector d'objectes de mes proper a menys proper
    std::sort(infoOmbra.begin(), infoOmbra.end(), Compare(point));
    return true;
}

float Scene::ambientOcclusionFactor(HitInfo info) {
    vec3 rayOrigin;
    vec3 rayDir;
    HitInfo rayInfo;
    int numSkyRays = 0;
    for(int i = 0; i < NUMRAYSAO; i++) {
        rayDir = info.normal + info.mat_ptr->RandomInSphere();
        while (dot(rayDir, info.normal) < 0) {
            rayDir = info.normal + info.mat_ptr->RandomInSphere();
        }
        rayOrigin = info.p + 0.01f*rayDir;
        if(!hit(Ray(rayOrigin, rayDir), 0, 500, rayInfo)) {
            numSkyRays ++;
        }
    }
    return numSkyRays/NUMRAYSAO;
}

float Scene::shadowCalculation(vec3 point, shared_ptr<Light> light) {
    vec3 lightPosition = light->position;
    vec3 director = normalize(lightPosition - point);
    float tMax = length(lightPosition - point);
    float tMin = 0.01; //Should be 0.0 but to avoid shadow acne must be some small epsilon
    Ray shadowRay = Ray(point, director);
    HitInfo info = HitInfo();
    if (this->hit(shadowRay, tMin, tMax, info)) {
        //IGNOREM OMBRES I INTERACCIONS AMB LLUM SI ES MATERIAL TEXTURA AMB FLAG DE IGNORELIGHTS
        if(auto tex_mat = dynamic_cast<MaterialTextura*>(info.mat_ptr)){
            if(tex_mat->ignoreLights){
                return 1.0;
            }
        }
        //
        return 0.0;
    } else {
        if (light->isPointInCone(point)) {
            return 1.0;
        } else {
            return 0.0;
        }
    }
}

void Scene::update(int nframe) {
    //NO IMPLEMENTAT
    //(Això seria per implementar Temporal per realWorldData
    //for (unsigned int i = 0; i< objects.size(); i++) {
    //    objects[i]->update(nframe);
    //}
    //METODOLOGIA PROPIA PER ANIMAR OBJECTES DE VIRTUALWORLD:
    for (unsigned int i = 0; i< objects.size(); i++) {
        objects[i]->applyAnimations(nframe);
    }
}

void Scene::setDimensions(vec3 p1, vec3 p2) {
    pmin = p1;
    pmax = p2;
}

void Scene::setGlobalLight(vec3 globalLight){
    this->globalLight = globalLight;
}

void Scene::setPointLights(vector<shared_ptr<Light>> pointLights){
    for (unsigned int i = 0; i< pointLights.size(); i++){
        this->pointLights.push_back(pointLights[i]);
    }
}
