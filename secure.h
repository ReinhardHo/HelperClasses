/*
HelperClasses.dll, a collection of useful classes and routines.
Copyright (C) 2023 - 2026    Reinhard Hölscher

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/


/*! \file secure.h
 *  \brief Stellt Klassen bereit, die einige Grundfunktionen der __OpenSSL__-Bibliothek nutzen.
 *
 * Die folgenden Klassen sind vorhanden:
 *
 * 1. Die Klasse __AES__, ein symmetrischer Ver- und Entschlüsselungsalgorithmus.
 *
 * 2. Die Klasse __PBKDF2__, stellt die Schlüsselableitungsfunktion PBKDF2 bereit.
 *
 * 3. Die Klasse __Random__, liefert Zufallszahlen.
 *
 * 4. Die Klasse __Secure__, bietet Methoden zum Ver- und Entschlüsseln von Daten mit drei verschiedenen Schlüsseln.
 *
 * 5. Die Klasse __SHA384__, ein Hash-Algorithmus.
 *
 * 6. Die Klasse __SHA512__, ein Hash-Algorithmus.
 *
 */
#ifndef RH_SECURE_H
#define RH_SECURE_H
#include "HelperClasses_global.h"
#include "bytearray.h"
#include "openssl/crypto.h"
//#include "openssl/core.h"
//#include "openssl/evp.h"
#include "openssl/kdf.h"
#include "openssl/rand.h"
//#include "openssl/provider.h"
#include "array.t"
//#include "worker.h"

namespace RH {

        /*! \brief Die Klasse __AES__ stellt Methoden für die symmetrische Ver- und Entschlüsselung mit dem __AES__-Algorithmus bereit.
         *
         *  Diese Klasse kapselt den in der OpenSSL-Bibliothek bereitgestellten AES-Algorithmus und macht ihn so leichter nutzbar. Neben dem
         *  Konstruktor und Destruktor gibt es Methoden um Schlüssel und Initialisierungsvektor zu übergeben. Mit den Methoden `Encrypt()` und `Decrypt()`
         *  können danach Ver- und Entschlüsselungen durchgeführt werden. Mit der Methode `Clear()` können Schlüssel und Initialisierungsvektor wieder
         *  sicher gelöscht werden. Auch der Destruktor stellt die sichere Löschung von Schlüssel und Initialisierungsvektor sicher.
         */
        class HELPERCLASSES_EXPORT AES {
        public:
                AES();
                virtual ~AES();
                void Clear();
                bool Decrypt(ByteArray &ref);
                bool Encrypt(ByteArray &ref);
                void SetIV(ByteArray &ref);
                void SetKey(ByteArray &ref);
                void SetKeyAndIV(ByteArray &refKey, ByteArray &refIV);
        private:
                /*!
                 * \brief Zeiger auf eine OpenSSL-Entschlüsselungsmethode (AES-256-CBC, siehe OpenSSL-Dokumentation).
                 */
                EVP_CIPHER *evp_ciper_dec;
                /*!
                 * \brief Zeiger auf eine OpenSSL-Verschlüsselungsmethode (AES-256-CBC, siehe OpenSSL-Dokumentation).
                 */
                EVP_CIPHER *evp_ciper_enc;
                /*!
                 * \brief Zeiger auf einen OpenSSL-Entschlüsselungskontext (siehe OpenSSL-Dokumentation).
                 */
                EVP_CIPHER_CTX *evp_cipher_ctx_dec;
                /*!
                 * \brief Zeiger auf einen OpenSSL-Verschlüsselungskontext (siehe OpenSSL-Dokumentation).
                 */
                EVP_CIPHER_CTX *evp_cipher_ctx_enc;
                /*!
                 * \brief Zeiger auf eine ByteArray-Instanz, die einen 16 Byte langen Initialisierungsvektor enthält.
                 */
                ByteArray *iv;
        };

        /*! \brief Die Klasse __PBKDF2__ stellt Methoden zur Ableitung von Schlüsseln aus einem Salt und einem Passwort bereit.
         *
         * "PBKDF2" (Password-Based Key Derivation Function 2) ist eine genormte kryptografische Funktion, um aus einem Passwort einen sicheren Schlüssel abzuleiten.
         * Durch wiederholtes Hashing (Iteration) und das Hinzufügen eines Zufallswertes (Salt) macht der Algorithmus Brute-Force-Angriffe extrem zeitaufwendig
         * und unrentabel.
         *
         * Diese Funktion ist in OpenSSL realisiert und in dieser Klasse __PBKDF2__ wird bei der Schlüsselgenerierung darauf zurück gegriffen.
         */
        class HELPERCLASSES_EXPORT PBKDF2 {
        public:
                PBKDF2();
                virtual ~PBKDF2();
                ByteArray *GetKeys();
                void SetIteration(size_t i);
                void SetPW(const ByteArray &ref);
                void SetPW(const QString &ref);
                void SetSalt(const ByteArray &ref);
        private:
                /*!
                 * \brief Zeiger auf einen OpenSSL-KDF-Algorithmus (hier wird "PBKDF2" verwendet) (siehe OpenSSL-Dokumentation).
                 */
                EVP_KDF *evp_kdf;
                /*!
                 * \brief Zeiger auf einen OpenSSL-KDF-Kontext (siehe OpenSSL-Dokumentation).
                 */
                EVP_KDF_CTX *evp_kdf_ctx;
                /*!
                 * \brief Anzahl der Iterationen (Hash-Runden) die bei der Schlüsselerzeugung mit dem Salt und dem Passwort durchgeführt werden.
                 */
                size_t iteration;
                /*!
                 * \brief Zeiger auf eine ByteArray-Instanz, die einen 16 Byte langen Initialisierungsvektor und einen 32 Byte langen Schlüssel enthält.
                 */
                ByteArray *ivKey;
                /*!
                 * \brief Zeiger auf eine ByteArray-Instanz, die das Passwort enthält.
                 */
                ByteArray *pw;
                /*!
                 * \brief Zeiger auf eine ByteArray-Instanz, die Salt-Bytes enthält.
                 */
                ByteArray *salt;
        };

        /*! \brief Die Klasse __Random__ bietet eine Methode zur Erzeugung einer beliebigen Anzahl von Zufallsbytes.
         *
         * Diese Klasse kapselt den in der OpenSSL-Bibliothek bereitgestellten Random-Algorithmus und macht ihn so leichter nutzbar.
         */
        class HELPERCLASSES_EXPORT Random {
        public:
                Random();
                virtual ~Random();
                bool GetRandomBytes(ByteArray &ref, size_t size);
        private:
                /*!
                 * \brief Zeiger auf einen OpenSSL-Hash-Algorithmus (hier wird "SHA512" verwendet) (siehe OpenSSL-Dokumentation).
                 */
                EVP_MD *evp_md;
                /*!
                 * \brief Zeiger auf einen OpenSSL-Hash-Kontext (siehe OpenSSL-Dokumentation).
                 */
                EVP_MD_CTX *evp_md_ctx;
                /*!
                 * \brief Zeiger auf eine ByteArray-Instanz, die einen 64 Byte langen Hashwert enthält.
                 */
                ByteArray *hash;
                /*!
                 * \brief Zeiger auf ein ByteArray, das einen 32 Byte langen Salt-Wert zur Erzeugung von Zufallszahlen enthält.
                 */
                ByteArray *randomSalt;
                char Salt1[32] = { '\x08', '\x7E', '\xB1', '\x4E', '\xE6', '\x52', '\xC9', '\xB8', '\x1E', '\x13', '\x24', '\x90', '\xA8', '\xB7', '\x16', '\x68',
                                   '\x80', '\x1B', '\xF5', '\xF1', '\x94', '\xB9', '\x41', '\xEB', '\x84', '\x7B', '\xEC', '\xD9', '\xCE', '\x5C', '\xD2', '\xCB'};
                /*!
                 * \brief Zeiger auf ein ByteArray, das 128 Byte lange Zufallszahlen aus OpenSSL enthält.
                 */
                ByteArray *randomOpenSSL;
                void GetHash(ByteArray &ref, ByteArray &Hash);
        };

        /*! \brief Die Klasse __Secure__ bietet Methoden zum Ver- und Entschlüsseln von Daten mit drei verschiedenen Schlüsseln.
         *
         *  Die genaue Funktionsweise kann bei den beiden Methoden `Encrypt()` und `Decrypt()` nachgelesen werden.
         */
        class HELPERCLASSES_EXPORT Secure {
        public:
                Secure();
                virtual ~Secure();
                bool Decrypt(ByteArray &in, ByteArray &out);
                bool Encrypt(ByteArray &in, ByteArray &out);
                bool SetLabelKey(ByteArray &ref);
                bool SetOneTimePadKey(ByteArray &ref);
                bool SetPermutationKey(ByteArray &ref);
                bool SetRedundanz(ByteArray &ref);
        private:
                /*!
                 * \brief Zeiger auf eine ByteArray-Instanz, die einen 256-Byte-Schlüssel enthält,
                 * bei dem ein Byte dazu dient, das Ende der Daten
                 * in einem Datenfeld zu markieren, das zudem Redundanzbytes enthält.
                 * Alle Byte-Werte in diesem Schlüssel sind unterschiedlich und kommen genau einmal vor.
                 */
                ByteArray *labelKey;
                /*!
                 * \brief Zeiger auf eine ByteArray-Instanz, die einen Schlüssel enthält.
                 */
                ByteArray *oneTimePadKey;
                /*!
                 * \brief Zeiger auf eine ByteArray-Instanz, die einen 256 Byte langen Schlüssel enthält, der zum Vertauschen eines 256 Byte langen Arrays verwendet wird.
                 * Alle Byte-Werte in diesem Schlüssel sind unterschiedlich und kommen genau einmal vor.
                 */
                ByteArray *permutationKey;
                /*!
                 * \brief Zeiger auf eine ByteArray-Instanz, die Redundanzbytes enthält.
                 */
                ByteArray *redundanz;
                /*!
                 *  \brief Zeiger auf eine Instanz der Klasse RH::Random zur Generierung von Zufallszahlen.
                 */
                Random *rand;

                char Salt1[64] = { '\x08', '\x7E', '\xB1', '\x4E', '\xE6', '\x52', '\xC9', '\xB8', '\x1E', '\x13', '\x24', '\x90', '\xA8', '\xB7', '\x16', '\x68',
                                   '\x80', '\x1B', '\xF5', '\xF1', '\x94', '\xB9', '\x41', '\xEB', '\x84', '\x7B', '\xEC', '\xD9', '\xCE', '\x5C', '\xD2', '\xCB',
                                   '\xD0', '\x1C', '\x70', '\x64', '\x67', '\x72', '\x27', '\x40', '\x8A', '\xFB', '\x82', '\x74', '\xA3', '\xE8', '\x84', '\x7E',
                                   '\x72', '\xCC', '\x74', '\x22', '\x82', '\x8D', '\xC3', '\xF3', '\x26', '\x85', '\x23', '\xC0', '\x09', '\x81', '\x50', '\xAF'};


                char Salt2[64] = { '\x6B', '\xBD', '\xDA', '\x4A', '\xE5', '\xA6', '\x01', '\x03', '\x5F', '\x02', '\x3A', '\x2D', '\x29', '\xE2', '\xA9', '\x33',
                                   '\x0D', '\x0C', '\x5A', '\x37', '\xDA', '\x39', '\xA3', '\xEE', '\x5E', '\x6B', '\x4B', '\x0D', '\x32', '\x55', '\xBF', '\xEF',
                                   '\x95', '\x60', '\x18', '\x90', '\xAF', '\xD8', '\x07', '\x09', '\x27', '\xE4', '\xB4', '\xD1', '\xA3', '\xC9', '\x0F', '\xB6',
                                   '\x51', '\xE4', '\x74', '\x4B', '\xB8', '\xA3', '\xC8', '\x11', '\x2F', '\x2C', '\x26', '\x49', '\x6C', '\x44', '\x27', '\xB5'};

                size_t CreateRedundancy(ByteArray &source, size_t readIndex, ByteArray &target, unsigned char arrayNumber);
                bool DeleteRedundancy(ByteArray &source, ByteArray &target, size_t &index, unsigned char arrayNumber);
                void InsertRedundancy(ByteArray &ref, size_t index);
                bool IsByteInArray(ByteArray &ref, char c, size_t count);
                bool IsByteMultipleInArray(ByteArray &ref);
                bool TestAndDeleteRedundancy(ByteArray &source, ByteArray &target, size_t &index, unsigned char arrayNumber);
        };

        /*! \brief Die Klasse __SHA384__ bietet Methoden zur Erzeugung und Überprüfung von Hash-Werten gemäß dem SHA384-Algorithmus.
         *
         * Diese Klasse kapselt den in der OpenSSL-Bibliothek bereitgestellten SHA384-Algorithmus und macht ihn so leichter nutzbar.
         */
        class HELPERCLASSES_EXPORT SHA384 {
        public:
                SHA384();
                virtual ~SHA384();
                void AddHash(ByteArray &ref);
                void GetHash(ByteArray &ref, ByteArray &Hash);
                bool TestHash(ByteArray &ref);
                bool TestHash(ByteArray &ref, ByteArray &Hash);
        private:
                /*!
                 * \brief Zeiger auf einen OpenSSL-Hash-Algorithmus (hier wird "SHA384" verwendet) (siehe OpenSSL-Dokumentation).
                 */
                EVP_MD *evp_md;
                /*!
                 * \brief Zeiger auf einen OpenSSL-Hash-Kontext (siehe OpenSSL-Dokumentation).
                 */
                EVP_MD_CTX *evp_md_ctx;
                /*!
                 * \brief  Zeiger auf ein ByteArray, das einen 48 Byte langen Hash-Wert enthält.
                 */
                ByteArray *hash;
        };

        /*! \brief Die Klasse __SHA512__ bietet Methoden zur Erzeugung und Überprüfung von Hash-Werten gemäß dem SHA512-Algorithmus.
         *
         * Diese Klasse kapselt den in der OpenSSL-Bibliothek bereitgestellten SHA512-Algorithmus und macht ihn so leichter nutzbar.
         */
        class HELPERCLASSES_EXPORT SHA512 {
        public:
                SHA512();
                virtual ~SHA512();
                void AddHash(ByteArray &ref);
                void GetHash(ByteArray &ref, ByteArray &Hash);
                bool TestHash(ByteArray &ref);
                bool TestHash(ByteArray &ref, ByteArray &Hash);
        private:
                /*!
                 * \brief Zeiger auf einen OpenSSL-Hash-Algorithmus (hier wird "SHA512" verwendet) (siehe OpenSSL-Dokumentation).
                 */
                EVP_MD *evp_md;
                /*!
                 * \brief Zeiger auf einen OpenSSL-Hash-Kontext (siehe OpenSSL-Dokumentation).
                 */
                EVP_MD_CTX *evp_md_ctx;
                /*!
                 * \brief  Zeiger auf ein ByteArray, das einen 64 Byte langen Hash-Wert enthält.
                 */
                ByteArray *hash;
        };


} // end of namespace RH

#endif // RH_SECURE_H
