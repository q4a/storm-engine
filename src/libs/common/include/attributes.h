#pragma once

#include <cstdint>
#include <cstdio>
#include <vector>

#include "storm_assert.h"
#include "storm/string_compare.hpp"

class VSTRING_CODEC
{
  public:
    VSTRING_CODEC(){};

    virtual ~VSTRING_CODEC(){};
    virtual uint32_t GetNum() = 0;
    virtual uint32_t Convert(const char *pString) = 0;
    virtual uint32_t Convert(const char *pString, int32_t iLen) = 0;
    virtual const char *Convert(uint32_t code) = 0;

    virtual void VariableChanged() = 0;
};

// here for now
constexpr size_t TSE_MAX_EVENT_LENGTH = 64;

#define INVALID_ATTRIBUTE_HANDLE 0xffffffff

//--------------------------------------------------------------------------------------------------------------
// Each attribute is a class with Name and Attribute string values. Class can contain list of ... later
//
// GetAttributeClass(attribute_name) - return class ATTRIBUTES pointer for specified attribute
// GetAttribute(attribute_name) - return string value of specified attribute
// SetAttribute(attribute_name,attribute) - create or modify attribute in attributes set
// GetAttributeAsDword(attribute_name, DWORD def = 0) - return attribute value, converted from string to uint32_t
// GetAttributeAsFloat(attribute_name, DWORD def = 0) - return attribute value, converted from string to float
// SetAttributeUseDword(attribute_name, DWORD val) - create or modify attribute, converted from uint32_t to string
// SetAttributeUseFloat(attribute_name, FLOAT val) - create or modify attribute, converted from float to string

class ATTRIBUTES
{
    // TODO: remove with another iteration of rewriting this
    friend class COMPILER;

    VSTRING_CODEC *pVStringCodec;
    uint32_t nNameCode;
    char *Attribute;
    std::vector<ATTRIBUTES *> pAttributes;
    ATTRIBUTES *pParent;
    bool bBreak;

    void SetParent(ATTRIBUTES *pP)
    {
        pParent = pP;
    }

    ATTRIBUTES *CreateNewAttribute(uint32_t name_code)
    {
        auto *attr = pAttributes.emplace_back(new ATTRIBUTES(pVStringCodec));

        attr->SetParent(this);
        attr->nNameCode = name_code;

        return attr;
    }

  public:
    ATTRIBUTES(VSTRING_CODEC *p)
    {
        pVStringCodec = p;
        Attribute = nullptr;
        pParent = nullptr;
        bBreak = false;
        nNameCode = pVStringCodec->Convert("root");
    }

    ATTRIBUTES(const ATTRIBUTES &) = delete;
    ATTRIBUTES(ATTRIBUTES &&) = delete;
    ATTRIBUTES &operator=(const ATTRIBUTES &) = delete;

    ~ATTRIBUTES()
    {
        Release();
    }

    void SetBreak(bool bBreak)
    {
        this->bBreak = bBreak;
    }

    auto GetParent() const
    {
        return pParent;
    }

    bool operator==(const char *str) const
    {
        if (!str || !str[0])
            return false;
        return storm::iEquals(pVStringCodec->Convert(nNameCode), str);
    }

    auto GetThisName() const
    {
        Assert(pVStringCodec != nullptr);
        return pVStringCodec->Convert(nNameCode);
    }

    auto GetThisAttr() const
    {
        return Attribute;
    }

    void SetName(const char *_name)
    {
        if (_name)
            nNameCode = pVStringCodec->Convert(_name);
    }

    void SetValue(const char *_val)
    {
        if (_val == nullptr)
        {
            delete Attribute;
            Attribute = nullptr;
            return;
        }

        const auto len = GetLen(strlen(_val) + 1);
        auto *const oldPtr = Attribute;
        Attribute = new char[len];
        strcpy_s(Attribute, len, _val);
        delete[] oldPtr;

        if (bBreak)
            pVStringCodec->VariableChanged();
    }

    void Release()
    {
        if (bBreak)
            pVStringCodec->VariableChanged();
        ReleaseLeafs();
        delete Attribute;
        Attribute = nullptr;
    }

    void ReleaseLeafs()
    {
        for (const auto &attribute : pAttributes)
            delete attribute;
        pAttributes.clear();
    }

    auto GetAttributesNum() const
    {
        return pAttributes.size();
    }

    ATTRIBUTES *GetAttributeClass(const char *name)
    {
        for (const auto &attribute : pAttributes)
            if (storm::iEquals(name, attribute->GetThisName()))
                return attribute;
        return nullptr;
    }

    ATTRIBUTES *GetAttributeClass(uint32_t n)
    {
        return n >= pAttributes.size() ? nullptr : pAttributes[n];
    }

    auto VerifyAttributeClass(const char *name)
    {
        auto *const pTemp = GetAttributeClass(name);
        return (pTemp) ? pTemp : CreateAttribute(name, "");
    }

    char *GetAttribute(size_t n)
    {
        return n >= pAttributes.size() ? nullptr : pAttributes[n]->Attribute;
    }

    const char *GetAttributeName(size_t n)
    {
        return n >= pAttributes.size() ? nullptr : pAttributes[n]->GetThisName();
    }

    char *GetAttribute(const char *name)
    {
        if (name == nullptr)
            return nullptr;
        for (const auto &attribute : pAttributes)
            if (storm::iEquals(name, attribute->GetThisName()))
                return attribute->Attribute;
        return nullptr;
    }

    auto GetAttributeAsDword(const char *name = nullptr, uint32_t def = 0)
    {
        uint32_t vDword;
        char *pAttribute;
        vDword = def;
        if (name)
        {
            pAttribute = GetAttribute(name);
            if (pAttribute)
                vDword = atol(pAttribute);
        }
        else
        {
            vDword = atol(Attribute);
        }
        return vDword;
    }

    auto GetAttributeAsPointer(const char *name = nullptr, uintptr_t def = 0)
    {
        uintptr_t ptr;
        char *pAttribute;
        ptr = def;
        if (name)
        {
            pAttribute = GetAttribute(name);
            if (pAttribute)
                ptr = atoll(pAttribute);
        }
        else
        {
            ptr = atoll(Attribute);
        }
        return ptr;
    }

    auto GetAttributeAsFloat(const char *name = nullptr, float def = 0)
    {
        float vFloat;
        char *pAttribute;
        vFloat = def;
        if (name)
        {
            pAttribute = GetAttribute(name);
            if (pAttribute)
                vFloat = static_cast<float>(atof(pAttribute));
        }
        else
        {
            vFloat = static_cast<float>(atof(Attribute));
        }
        return vFloat;
    }

    auto SetAttributeUseDword(const char *name, uint32_t val)
    {
        std::string buffer;
        buffer = std::to_string(val);
        if (name)
            return SetAttribute(name, buffer.c_str()) != 0;
        SetValue(buffer.c_str());
        return true;
    }

    auto SetAttributeUseFloat(const char *name, float val)
    {
        char buffer[128];
        sprintf_s(buffer, "%g", val);
        if (name)
            return SetAttribute(name, buffer) != 0;
        SetValue(buffer);

        return true;
    }

    ATTRIBUTES *CreateAttribute(const char *name, const char *attribute)
    {
        if (name == nullptr)
            return nullptr;
        auto *attr = pAttributes.emplace_back(new ATTRIBUTES(pVStringCodec));

        attr->SetParent(this);
        attr->SetName(name);

        if (attribute)
        {
            const auto len = GetLen(strlen(attribute) + 1);
            attr->Attribute = new char[len];
            strcpy_s(attr->Attribute, len, attribute);
        }

        return attr;
    }

    size_t SetAttribute(const char *name, const char *attribute)
    {
        return SetAttribute(pVStringCodec->Convert(name), attribute);
    }

    void Copy(ATTRIBUTES *pASource)
    {
        if (pASource == nullptr)
            return;
        ReleaseLeafs();
        for (const auto &attribute : pASource->pAttributes)
        {
            const auto i = SetAttribute(attribute->GetThisName(), attribute->Attribute);
            pAttributes[i]->Copy(attribute);
        }
    }

    bool DeleteAttributeClassX(ATTRIBUTES *pA)
    {
        if (pA == nullptr)
            return false;
        if (pA == this)
        {
            for (const auto &attribute : pAttributes)
                delete attribute;
            pAttributes.clear();
        }
        else
        {
            for (uint32_t n = 0; n < pAttributes.size(); n++)
            {
                if (pAttributes[n] == pA)
                {
                    delete pA;
                    for (auto i = n; i < pAttributes.size() - 1; i++)
                        pAttributes[i] = pAttributes[i + 1];

                    pAttributes.pop_back();
                    return true;
                }
                if (pAttributes[n]->DeleteAttributeClassX(pA))
                    return true;
            }
        }
        return false;
    }

    ATTRIBUTES *CreateSubAClass(ATTRIBUTES *pRoot, const char *access_string)
    {
        uint32_t dwNameCode;
        uint32_t n = 0;
        ATTRIBUTES *pResult = nullptr;
        ATTRIBUTES *pTemp;
        if (pRoot == nullptr)
            return nullptr;
        if (access_string == nullptr)
            return nullptr;

        while (true)
        {
            switch (access_string[n])
            {
            case '.':
                dwNameCode = pVStringCodec->Convert(access_string, n);
                pTemp = pRoot->GetAttributeClassByCode(dwNameCode);
                if (!pTemp)
                    pTemp = pRoot->CreateNewAttribute(dwNameCode);
                pResult = CreateSubAClass(pTemp, &access_string[n + 1]);
                return pResult;

            case 0:
                dwNameCode = pVStringCodec->Convert(access_string);
                pResult = pRoot->GetAttributeClassByCode(dwNameCode);
                return (pResult) ? pResult : pRoot->CreateNewAttribute(dwNameCode);
            }
            n++;
        }
        return nullptr;
    }

    ATTRIBUTES *FindAClass(ATTRIBUTES *pRoot, const char *access_string)
    {
        uint32_t n = 0;
        ATTRIBUTES *pResult = nullptr;
        ATTRIBUTES *pTemp = nullptr;

        if (!pRoot || !access_string)
            return nullptr;
        if (!access_string[0])
            return pRoot;

        while (true)
        {
            switch (access_string[n])
            {
            case '.':
                pTemp = pRoot->GetAttributeClassByCode(pVStringCodec->Convert(access_string, n));
                if (!pTemp)
                    return nullptr;
                pResult = FindAClass(pTemp, &access_string[n + 1]);
                return pResult;
            case 0:
                pResult = pRoot->GetAttributeClassByCode(pVStringCodec->Convert(access_string));
                return pResult;
            }
            n++;
        }
        return nullptr;
    }

    ATTRIBUTES *GetAttributeClassByCode(uint32_t name_code)
    {
        for (const auto &attribute : pAttributes)
            if (name_code == attribute->nNameCode)
                return attribute;
        return nullptr;
    }

    ATTRIBUTES *VerifyAttributeClassByCode(uint32_t name_code)
    {
        ATTRIBUTES *pTemp;
        pTemp = GetAttributeClassByCode(name_code);
        if (pTemp)
            return pTemp;
        return CreateAttribute(name_code, "");
    }

    ATTRIBUTES *CreateAttribute(uint32_t name_code, const char *attribute)
    {
        auto *attr = pAttributes.emplace_back(new ATTRIBUTES(pVStringCodec));

        attr->SetParent(this);
        attr->nNameCode = name_code;
        if (attribute)
        {
            const auto len = GetLen(strlen(attribute) + 1);
            attr->Attribute = new char[len];
            strcpy_s(attr->Attribute, len, attribute);
        }

        return attr;
    }

    size_t GetALen(size_t dwLen)
    {
        return (pParent) ? (1 + dwLen / 2) * 2 : (1 + dwLen / 8) * 8;
    }

    size_t GetLen(size_t dwLen, uint8_t dwAlign = 8)
    {
        return (1 + dwLen / dwAlign) * dwAlign;
    }

    size_t SetAttribute(uint32_t name_code, const char *attribute)
    {
        size_t len = 0;
        if (attribute)
            len = GetLen(strlen(attribute) + 1);

        size_t n;
        for (n = 0; n < pAttributes.size(); n++)
        {
            if (pAttributes[n]->nNameCode == name_code)
            {
                if (attribute)
                {
                    auto *const oldPtr = pAttributes[n]->Attribute;
                    pAttributes[n]->Attribute = new char[len];
                    strcpy_s(pAttributes[n]->Attribute, len, attribute);
                    delete[] oldPtr;
                }
                else
                {
                    delete[] pAttributes[n]->Attribute;

                    pAttributes[n]->Attribute = nullptr;
                }
                return n;
            }
        }

        auto *attr = pAttributes.emplace_back(new ATTRIBUTES(pVStringCodec));

        attr->SetParent(this);
        attr->nNameCode = name_code;
        if (attribute)
        {
            pAttributes[n]->Attribute = new char[len];
            strcpy_s(pAttributes[n]->Attribute, len, attribute);
        }

        return pAttributes.size() - 1;
    }

    uint32_t GetThisNameCode()
    {
        return nNameCode;
    }

    void SetNameCode(uint32_t n)
    {
        nNameCode = n;
    }

    /*void Dump(ATTRIBUTES *pA, int32_t level)
    {
        char buffer[128];
        if (pA == nullptr)
            return;

        if (level >= 128)
            level = 127;
        if (level != 0)
            memset(buffer, ' ', level);
        buffer[level] = 0;

        for (uint32_t n = 0; n < pA->GetAttributesNum(); n++)
        {
            //~!~
            xtrace("%s%s = %s", buffer, pA->GetAttributeName(n), pA->GetAttribute(n));
            Dump(pA->GetAttributeClass(pA->GetAttributeName(n)), level + 2);
        }
    }*/
};
