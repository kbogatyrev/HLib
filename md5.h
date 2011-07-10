#pragma once

#include "StdAfx.h"
#include <wincrypt.h>

using namespace std;

class CMD5
{

private:
    HCRYPTPROV hCryptProv;
    HCRYPTHASH hHash;

public:
    CMD5()
    {
        hCryptProv = NULL;
        hHash = NULL;
        Init();
    }

    ~CMD5()
    {
        Null();
    }

    void Init()
    {
        BOOL uiRet = CryptAcquireContext (&hCryptProv, NULL, NULL, PROV_RSA_FULL, 0);
        if (!uiRet)
        {
            throw CException (-1, L"CMD5::Init(): CryptAcquireContext() failed.");
        }

        uiRet = CryptCreateHash (hCryptProv, CALG_MD5, 0, 0, &hHash);
        if (!uiRet)
        {
            throw CException (-1, L"CMD5::Init(): CryptCreateHash() failed.");
        }

    }   //  Init()

    CEString sHash (const CEString& sSource)
    {
        BOOL uiRet = CryptHashData (hHash, 
                                    (BYTE *)sSource.pToBytes(), 
                                    sSource.uiLength()* sizeof(wchar_t), 0);
        if (!uiRet)
        {
            throw CException (-1, L"CMD5::sHash():CryptHashData() failed.");
        }

        BYTE byteHash[16];
        DWORD dwLength = 16;
        uiRet = CryptGetHashParam (hHash, HP_HASHVAL, byteHash, &dwLength, 0);
        if (!uiRet)
        {
            throw CException (-1, L"CT_Hash::sHash(): CryptGetHashParam() failed.");
        }

        wstringstream io_;
        io_.fill ('0');
        for (int iByte = 0; iByte < 16; ++iByte)
        {
            io_.width (2);
	        io_ << hex << byteHash[iByte];
        }

        return io_.str().c_str();

    }   // sHash()

    void Null()
    {
        if (hHash)
        {
            CryptDestroyHash (hHash);
            hHash = NULL;
        }
        if (hCryptProv) 
        {
            CryptReleaseContext (hCryptProv, 0);
            hCryptProv = NULL;
        }
    
    }   //  Null()

};
