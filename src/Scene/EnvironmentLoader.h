
#ifndef TP_IG_M2_S2_ENVIRONMENTLOADER_H
#define TP_IG_M2_S2_ENVIRONMENTLOADER_H


#include "Environment.h"

class EnvironmentLoader {
public:
    EnvironmentLoader();

    Environment* loadFromFile(QString filePath);
};


#endif //TP_IG_M2_S2_ENVIRONMENTLOADER_H
