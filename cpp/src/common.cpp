#include <iostream>
#include <glm/glm.hpp>
#include <stdio.h>
#include "common.h"
#include "coins.h"

using namespace std;

void packTypechar(vch *dest, unsigned char typechar)
{
    packToVch(dest, "C", typechar);
}

void debugOutputVch(vch out)
{
    cout << "0x";
    for (unsigned int i = 0; i < out.size(); i++)
    {
        cout << hex << (unsigned int)(out[i]);
    }
}
void debugOutputVector(const char *label, vector2f v)
{
    fprintf(stdout, label);
    fprintf(stdout, ": %f,%f\n", v.x, v.y);
}
void debugOutputVector(const char *label, vector3f v)
{
    fprintf(stdout, label);
    fprintf(stdout, ": %f,%f,%f\n", v.x, v.y, v.z);
}
void debugOutputVector(const char *label, glm::vec3 v)
{
    fprintf(stdout, label);
    fprintf(stdout, ": %f,%f,%f\n", v.x, v.y, v.z);
}

void prependVchWithSize(vch *vchDest)
{
    uint64_t packetSize = vchDest->size();
    vch sizeData;
    packToVch(&sizeData, "Q", packetSize);

    vchDest->insert(vchDest->begin(), sizeData.begin(), sizeData.end());
}

void packVector2f(vch *destVch, const vector2f &v)
{
    packToVch(destVch, "ff", v.x, v.y);
}

vchIter unpackVector2f(vchIter src, vector2f *v)
{
    return unpackFromIter(src, "ff", &v->x, &v->y);
}

vchIter unpackTypecharFromIter(vchIter src, unsigned char *typechar)
{
    return unpackFromIter(src, "C", typechar);
}

void packEntityRef(vch *destVch, EntityRef ref)
{
    packToVch(destVch, "H", ref);
}
vchIter unpackEntityRef(vchIter iter, EntityRef *ref)
{
    return unpackFromIter(iter, "H", ref);
}

void packStringToVch(std::vector<unsigned char> *vch, string s)
{
    char* cstr;
    cstr = &s[0];
    packToVch(vch, "s", cstr);
}
vchIter unpackStringFromIter(vchIter iter, uint16_t maxSize, string *s)
{
    char formatStr[7];
    snprintf(formatStr, sizeof(formatStr), "%ds", maxSize);

    char cstr[maxSize+1];
    vchIter newIter = unpackFromIter(iter, formatStr, cstr);
    *s = string(cstr);

    return newIter;
}

bool entityRefIsNull(EntityRef ref)
{
    return ref == 0;
}

std::optional<unsigned int> safeUIntAdd(unsigned int a, unsigned int b)
{
    unsigned int sum = a + b;
    if (sum < a) {
        return {};
    }
    else {
        return {sum};
    }
}

glm::vec3 toGlmVec3(vector3f v)
{
    return glm::vec3(v.x, v.y, v.z);
}

void BalanceUpdate::pack(vch *dest)
{
    packStringToVch(dest, userAddress);
    packToVch(dest, "L", amount);
    packToVch(dest, "C", (unsigned char)isDeposit);
}
void BalanceUpdate::unpackAndMoveIter(vchIter *iter)
{
    *iter = unpackStringFromIter(*iter, 50, &userAddress);
    cout << "userAddress after unpack: " << userAddress << endl;
    *iter = unpackFromIter(*iter, "L", &amount);
    unsigned char isDepositBoolChar;
    *iter = unpackFromIter(*iter, "C", &isDepositBoolChar);
    isDeposit = (bool)isDepositBoolChar;
}

BalanceUpdate::BalanceUpdate(string userAddress, coinsInt amount, bool isDeposit)
    : userAddress(userAddress), amount(amount), isDeposit(isDeposit) {}

BalanceUpdate::BalanceUpdate(vchIter *iter)
{
    unpackAndMoveIter(iter);
}