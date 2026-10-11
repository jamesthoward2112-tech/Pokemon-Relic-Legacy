// Visual slots recovered from the approved 38-background beta.
// This never changes gBattleEnvironment or its mechanical data.
static const u8 sPrlCrystalLocations[][3] = {
    {35, 0, 27},
    {35, 1, 31},
    {35, 2, 31},
    {35, 3, 31},
    {35, 9, 22},
    {35, 12, 15},
    {35, 13, 15},
    {35, 42, 13},
    {35, 43, 34},
    {35, 44, 13},
    {35, 45, 34},
    {35, 52, 10},
    {35, 57, 33},
    {35, 63, 28},
    {35, 64, 27},
    {35, 65, 28},
    {35, 66, 27},
    {35, 83, 14},
    {35, 84, 21},
    {35, 85, 18},
    {35, 86, 21},
    {35, 87, 18},
    {35, 89, 16},
    {35, 90, 16},
    {35, 91, 16},
    {35, 92, 25},
    {35, 93, 25},
    {35, 97, 20},
    {35, 101, 20},
    {35, 110, 17},
    {35, 111, 30},
    {35, 112, 30},
    {37, 15, 25},
    {37, 20, 24},
    {37, 21, 23},
    {37, 22, 26},
    {37, 23, 27},
    {37, 24, 28},
    {37, 25, 29},
    {37, 26, 23},
    {37, 27, 26},
    {37, 28, 30},
    {37, 29, 35},
    {37, 30, 24},
    {37, 35, 35},
    {37, 36, 26},
    {37, 43, 28},
    {37, 44, 29},
    {40, 2, 11},
    {41, 5, 21},
    {43, 6, 32},
    {44, 16, 37},
    {45, 3, 36},
    {46, 0, 12},
    {48, 3, 33},
};

static u8 PrlCrystalVisualSlot(u8 environment, u8 group, u8 number)
{
    unsigned i;
    if (environment == 3 || environment == 4 || environment == 5)
        return environment; // Underwater, sea and pond retain their water art.
    if (environment < 10 || environment == 11 || environment == 12)
        for (i = 0; i < ARRAY_COUNT(sPrlCrystalLocations); i++)
            if (sPrlCrystalLocations[i][0] == group && sPrlCrystalLocations[i][1] == number)
                return sPrlCrystalLocations[i][2];
    return environment < 38 ? environment : 9;
}
