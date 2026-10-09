 /*
HelperClasses.dll, a collection of useful classes and routines.
Copyright (C) 2023 - 2026    Reinhard Hölscher

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/


#include "secure.h"
#include "QFile"

/*! \file secure.cpp
 *  \brief In dieser Datei sind die Klassen __AES__, __Random__, __PBKDF2__, __Secure__, __SHA384__ und __SHA512__ realisiert.
 */
namespace RH {

        /*! \brief Konstruktor: Initialisiert eine neue Instanz der Klasse __AES__.
         *
         * Die AES-Instanz stellt eine Schnittstelle zur AES-Verschlüsselung in der OpenSSL-Bibliothek bereit.
         */
        AES::AES() {
                evp_ciper_dec = EVP_CIPHER_fetch(NULL, "AES-256-CBC", NULL);
                evp_ciper_enc = EVP_CIPHER_fetch(NULL, "AES-256-CBC", NULL);
                evp_cipher_ctx_dec = EVP_CIPHER_CTX_new();
                evp_cipher_ctx_enc = EVP_CIPHER_CTX_new();
                iv = new ByteArray(16, true);
        }

        /*! \brief Destruktor: Zerstört die existierende Instanz der Klasse __AES__.
         *
         *  Der Destruktor löscht alle sensiblen Daten aus dieser AES-Instanz bevor der Heapspeicher
         *  wieder an das Betriebssystem zurück gegeben wird.
         */
        AES::~AES() {
                delete iv;
                EVP_CIPHER_free(evp_ciper_enc);
                EVP_CIPHER_free(evp_ciper_dec);
                EVP_CIPHER_CTX_free(evp_cipher_ctx_enc);
                EVP_CIPHER_CTX_free(evp_cipher_ctx_dec);
        }

        /*! \brief Diese Methode `Clear()` löscht das gesamte in der AES-Instanz vorhandene Schlüsselmaterial sicher.
         *
         *  Der Methode `Clear()` löscht alle sensiblen Daten aus dieser AES-Instanz.
         */
        void AES::Clear() {
                EVP_CIPHER_CTX_reset(evp_cipher_ctx_enc);
                EVP_CIPHER_CTX_reset(evp_cipher_ctx_dec);
                iv->Clear();
        }

        /*! \brief Diese Methode `Decrypt()` entschlüsselt einen übergebenen verschlüsselten Datenblock im CBC-Modus.
         *
         * Die Methode `Decrypt()` entschlüsselt den in einer ByteArray-Instanz übergebenen Datenblock im CBC-Modus.
         * Wenn dabei ein Fehler auftritt, wird die Verarbeitung abgebrochen und _false_ zurück gegeben.
         * Dies kann dann passieren, wenn der zu entschlüsselnde Datenblock böswillig verändert wurde oder
         * zwischzeitlich ein falscher Schlüssel oder Initialisierungsvektor in diese AES-Instanz geladen wurde.
         *
         * Wenn die Entschlüsselung fehlerfrei verlaufen ist, wird die Längenangabe in der übergebenen ByteArray-Instanz
         * berichtigt und _true_ zurück gegeben
         *
         * \param ref Verweis auf die zu entschlüsselde ByteArray-Instanz.
         * \return _true_ wenn die Verarbeitung fehlerfrei verlief (ansonsten _false_).
         */
        bool AES::Decrypt(ByteArray &ref) {
                int length1;
                int length2;
                EVP_DecryptInit_ex2(evp_cipher_ctx_dec, nullptr,  nullptr, reinterpret_cast<unsigned char*>(&(*iv)[0]), nullptr);
                if (!EVP_DecryptUpdate(evp_cipher_ctx_dec, reinterpret_cast<unsigned char*>(&ref[0]), &length1, reinterpret_cast<unsigned char*>(&ref[0]), static_cast<int>(ref.Size()))) {
                        /* Error */
                        return false;
                }
                if (!EVP_DecryptFinal_ex(evp_cipher_ctx_dec, reinterpret_cast<unsigned char*>(&ref[static_cast<size_t>(length1)]), &length2)) {
                        /* Error */
                        return false;
                }
                ref.SetSize(static_cast<size_t>(length1 + length2));
                return true;
        }

        /*! \brief Diese Methode `Encrypt()` verschlüsselt einen im Klartext übergebenen Datenblock im CBC-Modus.
         *
         * Wenn dabei ein Fehler auftritt, wird die Verarbeitung abgebrochen und _false_ zurück gegeben.
         *
         * \param ref Verweis auf die zu verschlüsselde ByteArray-Instanz.
         * \return _true_ wenn die Verarbeitung fehlerfrei verlief (ansonsten _false_).
         */
        bool AES::Encrypt(ByteArray &ref) {
                int length1;
                int length2;
                size_t lengthA = ref.Size();
                ref.TestCapacity(lengthA + 32);
                EVP_EncryptInit_ex2(evp_cipher_ctx_enc, nullptr, nullptr, reinterpret_cast<unsigned char*>(&(*iv)[0]), nullptr);
                if (!EVP_EncryptUpdate(evp_cipher_ctx_enc, reinterpret_cast<unsigned char*>(&ref[0]), &length1, reinterpret_cast<unsigned char*>(&ref[0]), static_cast<int>(lengthA))) {
                        /* Error */
                        return false;
                }
                if (!EVP_EncryptFinal_ex(evp_cipher_ctx_enc, reinterpret_cast<unsigned char*>(&ref[static_cast<size_t>(length1)]), &length2)) {
                        /* Error */
                        return false;
                }
                ref.SetSize(static_cast<size_t>(length1 + length2));
                return true;
        }

        /*! \brief Diese Methode `SetIV()` kopiert einen 16 Byte langen Initialisierungsvektor in die AES-Instanz.
         *
         * Wurde ein ungültiger Verweis übergeben oder hat der Initialisierungsvektor eine falsche Länge, wird eine Ausnahme (_invalid_argument_) ausgelöst.
         *
         * \param ref Verweis auf eine ByteArray-Instanz, die einen 16 Byte langen Initialisierungsvektor enthält.
         */
        void AES::SetIV(ByteArray &ref) {
                void *p = &ref;
                if ((p == nullptr) || (ref.Size() != 16)) {
                        throw std::invalid_argument("AES: Parameter IV");
                } else {
                        iv->Copy(ref, 0, 0, 16);
                }
        }

        /*! \brief Diese Methode `SetKey()` kopiert einen 32-Byte-Schlüssel in die AES-Instanz.
         *
         * Wenn eine ungültige Referenz übergeben wurde oder der Schlüssel eine falsche Länge hat, wird eine Ausnahme (_invalid_argument_) ausgelöst.
         *
         * \param ref Verweis auf eine ByteArray-Instanz, die einen 32 Byte langen Schlüssel enthält.
         */
        void AES::SetKey(ByteArray &ref) {
                void *p = &ref;
                if ((p == nullptr) || (ref.Size() != 32)) {
                        throw std::invalid_argument("AES: Parameter Key");
                } else {
                        EVP_EncryptInit_ex2(evp_cipher_ctx_enc, evp_ciper_enc, reinterpret_cast<unsigned char*>(&ref[0]), nullptr, nullptr);
                        EVP_DecryptInit_ex2(evp_cipher_ctx_dec, evp_ciper_dec, reinterpret_cast<unsigned char*>(&ref[0]), nullptr, nullptr);
                }
        }

        /*! \brief Diese Methode `SetKeyAndIV()` kopiert einen 32-Byte-Schlüssel und einen 16-Byte-Initialisierungsvektor in die AES-Instanz.
         *
         * Diese Methode ruft intern die Methoden `SetKey()` und `SetIV()` auf.
         *
         * \param refKey Verweis auf eine ByteArray-Instanz, die einen 32 Byte langen Schlüssel enthält.
         * \param refIV Verweis auf eine ByteArray-Instanz, die einen 16 Byte langen Initialisierungsvektor enthält.
         */
        void AES::SetKeyAndIV(ByteArray &refKey, ByteArray &refIV) {
                SetKey(refKey);
                SetIV(refIV);
        }




        /*! \brief Konstruktor: Initialisiert eine neue Instanz der Klasse __PBKDF2__.
         *
         * Die PBKDF2-Instanz stellt eine Schnittstelle zur PBKDF2-Funktion in der OpenSSL-Bibliothek bereit.
         */
        PBKDF2::PBKDF2() {
                evp_kdf = EVP_KDF_fetch(NULL, "PBKDF2", NULL);
                evp_kdf_ctx = EVP_KDF_CTX_new(evp_kdf);
                iteration = 10;
                ivKey = nullptr;
                pw = nullptr;
                salt = nullptr;
        }

        /*! \brief Destruktor: Zerstört die existierende Instanz der Klasse __PBKDF2__.
         *
         *  Der Destruktor löscht alle sensiblen Daten aus dieser AES-Instanz bevor der Heapspeicher
         *  wieder an das Betriebssystem zurück gegeben wird.
         */
        PBKDF2::~PBKDF2() {
                delete ivKey;
                delete pw;
                delete salt;
                EVP_KDF_CTX_free(evp_kdf_ctx);
                EVP_KDF_free(evp_kdf);
        }

        /*! \brief Diese Methode `GetKeys()` erzeugt aus einem Salt und einem Passwort 48 Byte Schlüsselmaterial (Verfahren: PBKDF2, vgl. OpenSSL-Dokumentation).
         *
         * Die Methode darf nur aufgerufen werden, wenn zuvor der Iterationszähler, das Salt und das Passwort festgelegt wurden.
         * Fehlen diese Daten, gibt diese Methode einen _nullptr_ zurück.
         *
         * \return Zeiger auf eine ByteArray-Instanz, die das Schlüsselmaterial enthält (oder _nullptr_ im Fehlerfall).
         */
        ByteArray *PBKDF2::GetKeys() {
                if (iteration < 100) {
                        return nullptr;
                }
                if (pw == nullptr) {
                        return nullptr;
                }
                if (salt == nullptr) {
                        return nullptr;
                }
                if (ivKey != nullptr) {
                        delete ivKey;
                }
                ivKey = new ByteArray(48, true);
                ivKey->SetSize(48);
                ByteArray digest("SHA384");
                OSSL_PARAM request[] {
                        { "digest", OSSL_PARAM_UTF8_STRING, &(digest)[0], 6, 0 },
                        { "pass", OSSL_PARAM_OCTET_STRING, &(*pw)[0], pw->Size(), 0 },
                        { "salt", OSSL_PARAM_OCTET_STRING, &(*salt)[0], salt->Size(), 0 },
                        { "iter", OSSL_PARAM_UNSIGNED_INTEGER, &this->iteration, sizeof(size_t), 0 },
                        { NULL, 0, NULL, 0, 0 }
                };
                if (!EVP_KDF_derive(evp_kdf_ctx, reinterpret_cast<unsigned char*>(&(*ivKey)[0]), 48, request)) {
                        return nullptr;
                } else {
                        return ivKey;
                }
        }

        /*! \brief Diese Methode `SetIteration()` legt einen neuen Iterationswert fest (es sind nur Werte größer als 100 zulässig).
         *
         * \param i Gewünschter Iterationswert.
         */
        void PBKDF2::SetIteration(size_t i) {
                iteration = i;
                if (iteration < 100) {
                        iteration = 100 + i;
                }
        }

        /*! \brief Diese Methode `SetPW()` legt ein neues Passwort fest, das für die Schlüsselableitung verwendet werden soll.
         *
         * \param ref Verweis auf eine ByteArray-Instanz, die das Passwort enthält.
         */
        void PBKDF2::SetPW(const ByteArray &ref) {
                if (pw != nullptr) {
                        delete pw;
                }
                pw = new ByteArray(ref);
        }

        /*! \brief Diese Methode `SetPW()` legt ein neues Passwort fest, das für die Schlüsselableitung verwendet werden soll.
         *
         * \param ref Verweis auf eine QString-Instanz, die das Passwort enthält.
         */
        void PBKDF2::SetPW(const QString &ref) {
                if (pw != nullptr) {
                        delete pw;
                }
                pw = new ByteArray(ref, true);
        }

        /*! \brief Diese Methode `SetSalt()` legt einen neuen Salt-Wert fest, der für die Schlüsselableitung verwendet werden soll.
         *
         * \param ref Verweis auf eine ByteArray-Instanz mit dem Salt-Wert, der verwendet werden soll.
         */
        void PBKDF2::SetSalt(const ByteArray &ref) {
                if (salt != nullptr) {
                        delete salt;
                }
                salt = new ByteArray(ref);
        }




        /*! \brief Konstruktor: Initialisiert eine neue Instanz der Klasse __Random__.
         *
         * Die Random Klasse stellt eine Schnittstelle zum Zufallszahlengenerator in der OpenSSL-Bibliothek  bereit.
         */
        Random::Random() {
                evp_md = EVP_MD_fetch(NULL, "SHA512", NULL);
                evp_md_ctx = EVP_MD_CTX_new();
                hash = new ByteArray(64, true);
                randomSalt = new ByteArray(32);
                randomSalt->Copy(Salt1, 0, 0, 32);
                randomOpenSSL = new ByteArray(128);
        }

        /*! \brief Destruktor: Zerstört die existierende Instanz der Klasse __Random__.
         *
         *  Der Destruktor löscht alle sensiblen Daten aus dieser Random-Instanz bevor der Heapspeicher
         *  wieder an das Betriebssystem zurück gegeben wird.
         */
        Random::~Random() {
                delete randomOpenSSL;
                delete randomSalt;
                delete hash;
                EVP_MD_CTX_free(evp_md_ctx);
                EVP_MD_free(evp_md);
        }

        /*! \brief Diese private Methode `GetHash()` erzeugt aus einem Datenblock einen Hashwert und trägt diesen in einen anderen Datenblock ein.
         *
         * Die Methode erzeugt anhand des SHA512-Algorithmus einen 64 Byte langen Hashwert aus dem ersten übergebenen Datenblock
         * und fügt diesen Hashwert anschließend in den zweiten Datenblock ein.
         *
         * \param ref Verweis auf eine ByteArray-Instanz, die einen Datenblock enthält, aus dem der Hash-Wert ermittelt werden soll.
         * \param Hash Verweis auf eine ByteArray-Instanz, in die der ermittelte Hash-Wert geschrieben werden soll.
         */
        void Random::GetHash(ByteArray &ref, ByteArray &Hash) {
                EVP_DigestInit(evp_md_ctx, evp_md);
                EVP_DigestUpdate(evp_md_ctx, &ref[0], ref.Size());
                EVP_DigestFinal(evp_md_ctx, reinterpret_cast<unsigned char*>(&Hash.WriteareaReferenz(0, 64)), nullptr);
        }

        /*! \brief Diese Methode `GetRandomBytes()` erzeugt Zufallszahlen und füllt die übergebene ByteArray-Instanz damit.
         *
         * Intern werden Zufallszahlen zunächst über die OpenSSL-Bibliothek abgerufen.
         * Diese Zufallszahlen gelten jedoch als nicht vertrauenswürdig und werden daher erneut zusammen mit einem 32 Byte langen RandomSalt gehasht.
         * Der resultierende Hash-Wert wird zum Teil als Zufallszahl zurückgegeben und zum Teil im nächsten Hash verwendet.
         *
         * \param ref Verweis auf eine ByteArray-Instanz, in die die Zufallszahlen geschrieben werden sollen.
         * \param size Anzahl der zu generierenden Bytes.
         * \return _true_, wenn die Generierung fehlerfrei verlaufen ist.
         */
        bool Random::GetRandomBytes(ByteArray &ref, size_t size) {
                size_t index = 0;
                void *p = &randomOpenSSL->WriteareaReferenz(0, 128);
                unsigned char *pu = reinterpret_cast<unsigned char*>(p);
                if (RAND_bytes(pu, 128) != 1) {
                        return false;
                }
                randomOpenSSL->Copy(*randomSalt, 0, 0, 32);
                while((size - index) > 32) {
                        GetHash(*randomOpenSSL, *hash);
                        ref.Copy(*hash, 0, index, 32);
                        index = index + 32;
                        if (RAND_bytes(pu, 128) != 1) {
                                return false;
                        }
                        randomOpenSSL->Copy(*hash, 32, 0, 32);
                }
                size_t lengthi = size - index;
                GetHash(*randomOpenSSL, *hash);
                ref.Copy(*hash, 0, index, lengthi);
                randomSalt->Copy(*hash, 32, 0, 32);
                return true;
        }




        /*! \brief Konstruktor: Initialisiert eine neue Instanz der Klasse __Secure__.
         *
         * Der Konstruktor besetzt die drei ByteArray-Instanzen für die Schlüssel und die ByteArray-Instanz für die Redundanz zunächst mit
         * Standartwerten. Dies ist historisch bedingt und sorgt dafür, dass eine ganz alte (veraltete) Anwendung immer noch mit dieser Klasse Secure läuft.
         *
         * 256 Byte langer OneTimePad-Schlüssel:
         *
         *      Index  Inhalt (hexadezimal)                             Inhalt (ASCII)
         *          0  f9 e0 93 6b 88 d7 7c d5 7a 61 14 0d 09 58 83 d6  ...k..|.za...X..
         *         16  05 9e eb 72 76 27 84 e2 73 1f ec 06 89 d8 fe 63  ...rv'..s......c
         *         32  0c 60 6d 04 8a d9 ff 1c f3 18 12 fb 0b da 00 e3  ..m.............
         *         48  f4 67 66 75 74 5b 0a e4 f5 11 99 95 fd 24 6e 65  .gfut[.......$ne
         *         64  f6 59 1a 96 2a db 03 9a f7 26 e5 17 01 dc fc 1b  .Y..*....&......
         *         80  f8 52 e6 68 7e 5d 7d 64 79 2d e7 10 81 a2 82 9b  .R.h~]}dy-......
         *         96  86 d2 5a 6f 02 a3 8e 9c 87 d3 25 90 08 a4 8f 9d  ..Zo......%.....
         *        112  a9 d4 53 ee 77 1e ed df 23 55 2c 4c b4 45 0f 22  ..S.w...#U,L.E."
         *        128  f2 aa de 33 b5 ba 70 56 35 ab ea 3e b6 3b a5 29  ...3..pV5..>.;.)
         *        144  ca d1 4f 41 b7 44 a6 48 cb 4b b0 37 b8 34 a7 c2  ..OA.D.H.K.7.4..
         *        160  30 bf 31 c8 39 e1 28 c3 3a c0 4e c9 46 0e 19 c4  0.1.9.(.:.N.F...
         *        176  c5 c1 b1 4a b9 71 5f d0 3f cd 32 2e f1 07 a0 13  ...J.q_.?.2.....
         *        192  7b ce 51 8c 97 2f a1 91 bb cf 20 8d 98 1d ad 92  {.Q../.... .....
         *        208  bc 49 16 af 54 50 ae 38 bd cc e9 c6 2b 6a 80 c7  .I..TP.8....+j..
         *        224  b2 4d 5c dd 15 a8 fa 8b 6c 9f b3 5e 47 57 43 f0  .M\.....l..^GWC.
         *        240  42 ef 85 21 ac 69 3c 40 36 3d 62 78 94 e8 7f be  B..!.i<@6=bx....
         *
         * 256 Byte langer Endemarkenschlüssel:
         *
         *      Index  Inhatl (hexadezimal)                             Inhalt (ASCII)
         *          0  9a 82 97 98 0e 15 f8 57 9b 83 a3 19 f1 6a f9 28  .......W.....j.(
         *         16  1c 84 a4 e6 72 07 fa 50 63 85 a5 67 0d 03 fb af  ....r..Pc..g....
         *         32  9c 86 a6 18 f2 7c 75 30 9d 87 a7 60 f3 fc 0a 4f  .....|u0.......O
         *         48  1e 88 a8 9f 74 7d 6e b0 61 09 a9 a0 8b 8d 91 31  ....t}n.a......1
         *         64  9e f6 2a a1 0c 8e 92 ce 1f f7 55 22 fe 0f 93 cf  ..*.......U"....
         *         80  59 24 aa dd 7f 70 14 d0 b1 5b 2b de 00 8f eb 51  Y$...p...[+....Q
         *         96  b2 1d d4 df 78 10 ec 2e 33 e2 4e e0 e4 6f 6d 4a  ....x...3.N..omJ
         *        112  4c 5c bc 5a 65 90 12 b5 b3 23 bd bb 1a c8 66 b6  L\.Ze....#....f.
         *        128  34 dc 3e c7 5e 49 99 b7 4b e8 41 48 ac 36 c6 38  4.>.^I..K.AH.6.8
         *        144  b4 69 be 37 ad 42 d2 47 c0 16 bf 3a ae d3 53 d6  .i.7.B.G...:..S.
         *        160  2c 62 cb 45 2f ea 25 d7 29 c9 1b ba db 6b 06 d8  ,b.E/.%.)....k..
         *        176  56 ca 64 3b e7 ff 79 52 e3 44 d5 3d 68 80 b9 2d  V.d;..yR.D.=h..-
         *        192  5d 26 17 c2 a2 01 c5 35 b8 d9 ab 43 c1 7e 46 3c  ]&.....5...C.~F<
         *        208  c4 96 02 40 cd 81 da 27 54 c3 fd 3f 39 e5 4d 58  ...@...'T..?9.MX
         *        224  cc 21 6c e1 89 5f e9 20 7b 11 7a ed 8a 0b f5 04  .!l.._. {.z.....
         *        240  d1 ee 05 77 f0 ef 76 94 13 73 8c 08 32 95 f4 71  ...w..v..s..2..q
         *
         * 256 Byte langer Vertauschungsschlüssel:
         *
         *      Index  Inhalt (hexadezimal)                             Inhalt (ASCII)
         *          0  0f 5c bb d4 d8 b3 58 ac 70 23 3c d5 59 34 a7 ad  .\....X.p#<.Y4..
         *         16  08 dc c3 56 26 cb 28 ae f7 5d 44 a9 52 cc 57 2f  ...V&.(..]D.R.W/
         *         32  f8 22 c6 2a b8 4d a8 50 f9 4f 47 e0 39 b2 29 af  .".*.M.P.OG.9.).
         *         48  7a 30 ce 61 d1 33 d6 1b 85 cf 48 9e 4b d7 49 64  z0.a.3....H.K.Id
         *         64  86 d0 37 9f 2d e3 36 9b 87 e7 41 20 d2 3a c9 1c  ..7.-.6...A .:..
         *         80  88 e8 3e df 53 45 ca 63 09 e9 2c eb 25 ba e1 15  ..>.SE.c..,.%...
         *         96  f6 6a 4c 6c 5a f2 62 55 77 0e be 93 a5 73 1d aa  .jLlZ.bUw....s..
         *        112  b4 71 bf 14 a6 8c e2 2b 35 8e c0 6b bd 0d ee 54  .q.....+5..k...T
         *        128  4a 8f c1 06 ea 72 6f ab 2e 90 c2 79 1e 8d b1 b7  J....ro....y....
         *        144  51 11 d9 91 ec 07 c8 38 b9 6e ff 12 6d 78 42 c7  Q......8.n..mxB.
         *        160  17 9c 00 ed 92 9d d3 10 f3 16 0a 04 01 0c f5 ef  ................
         *        176  f4 83 0b fb fe 1f 43 f0 21 84 99 fc a4 60 bc 4e  ......C.!......N
         *        192  de 05 9a fd 02 b5 3d dd 5f 65 f1 7e 7d 1a 3b c5  ......=._e.~}.;.
         *        208  19 b0 13 81 82 e5 c4 46 e6 31 5e 98 db 66 96 32  .......F.1^..f.2
         *        224  67 e4 cd da 40 a2 a0 3f 18 03 fa 5b 97 89 a3 24  g...@..?...[...$
         *        240  95 7c 7b 8a b6 a1 75 74 80 69 94 7f 27 8b 76 68  .|{...ut.i..'.vh
         *
         * 256 Byte lange Redundanz:
         *
         *      Index  Inhalt (hexadezimal)                             Inhalt (ASCII)
         *          0  08 7e b1 4e e6 52 c9 b8 1e 13 24 90 a8 b7 16 68  .~.N.R....$....h
         *         16  80 1b f5 f1 94 b9 41 eb 84 7b ec d9 ce 5c d2 cb  ......A..{...\..
         *         32  d0 1c 70 64 67 72 27 40 8a fb 82 74 a3 e8 84 7e  ..pdgr'@...t...~
         *         48  72 cc 74 22 82 8d c3 f3 26 85 23 c0 09 81 50 af  r.t"....&.#...P.
         *         64  6b bd da 4a e5 a6 01 03 5f 02 3a 2d 29 e2 a9 33  k..J...._.:-)..3
         *         80  0d 0c 5a 37 da 39 a3 ee 5e 6b 4b 0d 32 55 bf ef  ..Z7.9..^kK.2U..
         *         96  95 60 18 90 af d8 07 09 27 e4 b4 d1 a3 c9 0f b6  ........'.......
         *        112  51 e4 74 4b b8 a3 c8 11 2f 2c 26 49 6c 44 27 b5  Q.tK..../,&IlD'.
         *        128  08 7e b1 4e e6 52 c9 b8 1e 13 24 90 a8 b7 16 68  .~.N.R....$....h
         *        144  80 1b f5 f1 94 b9 41 eb 84 7b ec d9 ce 5c d2 cb  ......A..{...\..
         *        160  d0 1c 70 64 67 72 27 40 8a fb 82 74 a3 e8 84 7e  ..pdgr'@...t...~
         *        176  72 cc 74 22 82 8d c3 f3 26 85 23 c0 09 81 50 af  r.t"....&.#...P.
         *        192  6b bd da 4a e5 a6 01 03 5f 02 3a 2d 29 e2 a9 33  k..J...._.:-)..3
         *        208  0d 0c 5a 37 da 39 a3 ee 5e 6b 4b 0d 32 55 bf ef  ..Z7.9..^kK.2U..
         *        224  95 60 18 90 af d8 07 09 27 e4 b4 d1 a3 c9 0f b6  .:......'.......
         *        240  51 e4 74 4b b8 a3 c8 11 2f 2c 26 49 6c 44 27 b5  Q.tK..../,&IlD'.
         *
         * Diese Standardwerte können vor der Benutzung der Secure Klasse selbstverständlich mit anderen Werten belegt werden.
         * \sa SetOneTimePadKey(), SetLabelKey(), SetPermutationKey() und SetRedundanz().
         */
        Secure::Secure() {
                oneTimePadKey = new ByteArray(256, true);
                oneTimePadKey->Append('\371');
                oneTimePadKey->Append('\340');
                oneTimePadKey->Append('\223');
                oneTimePadKey->Append('\153');
                oneTimePadKey->Append('\210');
                oneTimePadKey->Append('\327');
                oneTimePadKey->Append('\174');
                oneTimePadKey->Append('\325');
                InsertRedundancy(*oneTimePadKey, 8);
                oneTimePadKey->SetSize(256);
                labelKey = new ByteArray(256, true);
                labelKey->Append('\232');
                labelKey->Append('\202');
                labelKey->Append('\227');
                labelKey->Append('\230');
                labelKey->Append('\16');
                labelKey->Append('\25');
                labelKey->Append('\370');
                labelKey->Append('\127');
                InsertRedundancy(*labelKey, 8);
                labelKey->SetSize(256);
                permutationKey = new ByteArray(256, true);
                permutationKey->Append('\17');
                permutationKey->Append('\134');
                permutationKey->Append('\273');
                permutationKey->Append('\324');
                permutationKey->Append('\330');
                permutationKey->Append('\263');
                permutationKey->Append('\130');
                permutationKey->Append('\254');
                InsertRedundancy(*permutationKey, 8);
                permutationKey->SetSize(256);
                redundanz = new ByteArray(256, true);
                redundanz->Copy(Salt1, 0, 0, 64);
                redundanz->Copy(Salt2, 0, 64, 64);
                redundanz->Copy(Salt1, 0, 128, 64);
                redundanz->Copy(Salt2, 0, 192, 64);
                rand = new Random();
        }


        /*! \brief Destruktor: Zerstört die existierende Instanz der Klasse __Secure__.
         *
         *  Der Destruktor löscht alle sensiblen Daten aus dieser Secure-Instanz bevor der Heapspeicher
         *  wieder an das Betriebssystem zurück gegeben wird.
         */
        Secure::~Secure() {
                delete rand;
                delete redundanz;
                delete permutationKey;
                delete labelKey;
                delete oneTimePadKey;
        }

        /*! \brief Diese Methode `CreateRedundancy()` kombiniert Datenbytes aus einem Quelldatenfeld mit Redundanzbytes in einem Zieldatenfeld.
         *
         * Die Methode erzeugt zunächst eine Endmarkierung in einem genau 256 Byte großen Zieldatenfeld.
         * Anschließend werden Datenbytes aus dem Quelldatenfeld vor der Endmarkierung im Zieldatenfeld eingefügt,
         * solange sich alle Bytes (Daten und Endmarkierung) unterscheiden (wobei jedoch höchstens 128 Bytes eingefügt werden).
         * Anschließend wird der Rest des Ziel-Datenfeldes mit Redundanzbytes (die sich noch nicht im Zieldatenfeld befinden) aufgefüllt.
         * Nach diesem Vorgang sind alle Byte-Werte im Zieldatenfeld unterschiedlich.
         *
         * \param source Verweis auf eine ByteArray-Instanz, die die Quelldaten enthält.
         * \param readIndex Index im Quelldatenfeld, ab dem Datenbytes entnommen werden sollen.
         * \param target Verweis auf eine ByteArray-Instanz, die die Zieldaten enthält.
         * \param arrayNumber Byte-Wert (zwischen 0 und 255), der die Endemarkierung darstellt.
         * \return Neuer Leseindex im Quelldatenfeld für den nächsten Aufruf dieser Methode.
         */
        size_t Secure::CreateRedundancy(ByteArray &source, size_t readIndex, ByteArray &target, unsigned char arrayNumber) {
                target[0] = (*labelKey)[arrayNumber];
                size_t i = readIndex;
                size_t maxIndex = source.Size();
                size_t j = 0;
                bool stop = false;
                do {
                        char b = source[i];
                        stop = IsByteInArray(target, b, j + 1);
                        if (!stop) {
                                target[j + 1] = target[j];
                                target[j] = b;
                                i++;
                                if (i >= maxIndex) {
                                        stop = true;
                                }
                                j++;
                                if (j == 128) {
                                        stop = true;
                                }
                        }
                } while (!stop);
                ByteArray b(4096);
                bool ready = false;
                while (!ready) {
                        rand->GetRandomBytes(b, 4096);
                        size_t keyindex = j + 1;
                        for (size_t k = 0; k < 4096; k++) {
                                char c = b[k];
                                if (!IsByteInArray(target, c, keyindex)) {
                                        target[keyindex] = c;
                                        keyindex++;
                                }
                                if (keyindex == 256) {
                                        ready = true;
                                        break;
                                }
                        }
                }
                return i;
        }


        /*! \brief Diese Methode `Decrypt()` entschlüsselt Bytes aus dem Eingabe-ByteArray und speichert das Ergebnis im Ausgabe-ByteArray.
         *
         * Es werden folgende Schritte durchgeführt:
         *
         * Es wird geprüft, ob die Länge des Eingabe-ByteArrays durch 256 ganzzahlig teilbar ist.
         * Wenn nicht, wird _false_ zurück gegeben.
         *
         * Das Eingabe-ByteArray wird in Blöcke zu je 256 Bytes aufgeteilt. Für jeden Block werden die folgenden Schritte ausgeführt:
         *
         * Es wird geprüft, ob jeder Byte-Wert [0..255] im Block genau einmal vorkommt. Wenn nicht, wird _false_ zurück gegeben.
         * Mit dem Schlüssel _PermutationKey_ wird die Byte-Vertauschung rückgängig machen.
         * Redundante Bytes werden entfernt und Daten aus der Vernam_Chiffre werden aufgesammelt.
         *
         * Das Sammeln muss zu einem Byte-Array führen, dessen Länge der Länge des in dieser Instanz vorhandenen OneTimePad-Schlüssels entspricht.
         * Wenn nicht, wird _false_ zurück gegeben.
         *
         * Anschließend wird die OneTimePad-Verschlüsselung rückgängig gemacht.
         *
         * Schließlich wird die Länge der Benutzerdaten anhand der letzten Bytes ermittelt. Anschließend werden die entschlüsselten Daten in das Ausgabe-ByteArray kopiert.
         *
         * In dem nachfolgenden Beispiel soll der 13 Byte lange Text: <span style="color:blue">Ein Testtext.</span> entschlüsselt werden.
         *
         * <pre class="fragment"> Ausgabeatenblock:
         *      Index  Inhalt (hexadezimal)                             Inhalt (ASCII)
         *          0  9c f2 eb 62 1b 5b 43 8f 66 87 11 23 b2 81 5d 0b  ...b.[C.f..#..].
         *         16  19 cd 58 1a c7 78 03 b6 c8 cb 63 21 53 0a 1d f4  ..X..x....c!S...
         *         32  45 d1 <span style="color:blue">39</span> 20 b4 86 69 77 b1 5a b8 51 dc 18 76 e4  E.9 ..iw.Z.Q..v.
         *         48  a0 de ab 68 bc 12 99 f9 88 c6 05 b0 e6 55 b3 07  ...h.........U..
         *         64  67 aa bb 48 8b 0e 2f fc 6a 01 8e 7e 16 b9 a5 a4  g..H../.j..~....
         *         80  3f <span style="color:blue">5c</span> 60 fe a2 3b 35 08 4b cc ea 3a ff 73 f0 6e  ?\`..;5.K..:.s.n
         *         96  80 da e5 95 d5 22 34 3d 2c f3 a7 a6 54 9a 9e 90  ......4=,...T...
         *        112  5e e9 d8 8a 5f e1 30 82 91 6d 50 4a ce 98 0c d6  ^..._.0..mPJ....
         *        128  ac d0 4e 2e 75 59 0f 6c <span style="color:blue">f5</span> 9f f1 7a 84 27 64 4c  ..N.uY.l...z.'dL
         *        144  7f ec d3 d2 f7 38 a8 09 8d 8c 1c 74 71 7d b7 <span style="color:blue">97</span>  .....8.....tq}..
         *        160  13 c0 61 e3 93 31 cf <span style="color:blue">e7</span> 83 46 fa 2b 9b 1f 00 b5  ..a..1...F.+....
         *        176  bd fd df 06 d9 9d 70 65 e8 ef ee 89 7b ed 41 ca  ......pe....{.A.
         *        192  c9 44 17 72 a3 26 40 ba 4f e2 57 d4 37 24 1e 79  .D.r.&..O.W.7$.y
         *        208  c3 bf 85 a1 fb ad 32 92 2d f6 3e 96 af 6f 6b 56  ......2.-.>..okV
         *        224  ae 10 42 94 29 <span style="color:blue">49</span> dd c5 14 33 c1 c2 2a a9 7c 04  ..B.)I...3..*.|.
         *        240  d7 02 e0 4d 25 db 0d f8 52 <span style="color:red">c4</span> 3c 36 be 15 28 47  ...M%...R.<6..(G
         *
         *        256  c2 7f bc 9c 7d f7 fb 5f 9e 6a 25 81 a4 43 d2 62  ....}.._.j%..C.b
         *        272  <span style="color:blue">ec</span> 73 96 c4 a9 b0 04 1f a1 74 80 e0 97 64 86 44  .s.......t...d.D
         *        288  48 <span style="color:blue">8a</span> 21 39 01 84 cd bf 6f bd 18 e4 79 51 95 6b  H.!9....o...yQ.k
         *        304  26 2d b9 75 05 27 40 6e <span style="color:blue">82</span> 4a d4 4e 7c <span style="color:red">0a</span> 50 53  &-.u.'.n.J.N|.PS
         *        320  12 f3 e9 f9 52 56 00 3c 8f 11 06 db 94 88 5d e5  ....RV.<......].
         *        336  <span style="color:blue">49</span> 08 f8 0b 4b 10 b8 <span style="color:blue">60</span> de 29 ce b7 61 30 ee a0  I...K..`.)..a0..
         *        352  0c b3 77 b6 28 1a da 1b 66 83 16 89 5b 3a fe 32  ..w.(...f...[:.2
         *        368  f1 2f e6 02 54 c7 c9 07 fd 17 a7 45 a5 <span style="color:blue">90</span> ca 65  ./..T......E...e
         *        384  c0 b5 bb c3 09 2e ac <span style="color:blue">e1</span> f4 87 31 7e cb <span style="color:blue">c6</span> f6 7a  ..........1~...z
         *        400  9d 5e aa a3 ed eb dd 93 cc 0f ab 7b 99 b2 <span style="color:blue">15</span> 98  .^.........{....
         *        416  5a 42 4c f2 9b ea <span style="color:blue">fa</span> 70 8d 2c c1 d5 72 b4 0e 6d  ZBL....p.,..r..m
         *        432  57 d8 71 8b f5 78 03 c8 e7 33 4d c5 d9 e2 38 34  W.q..x...3M...84
         *        448  ae 46 69 ad 76 14 d7 f0 2a d3 9a 20 ba 35 6c a2  .Fi.v...*.. .5l.
         *        464  ff 5c 3b dc 3d a8 67 41 13 8e 9f 4f 2b 37 d0 af  .\;.=.gA...O+7..
         *        480  91 55 63 fc <span style="color:blue">df</span> 8c 1e 1d d6 e3 <span style="color:blue">be 19</span> b1 3f 24 e8  .Uc..........?$.
         *        496  0d 3e ef 1c a6 58 59 23 <span style="color:blue">68</span> 47 d1 85 92 cf 22 36  .>...XY#hG.....6
         *
         *        512  95 16 f8 de 61 96 94 ce 17 26 ca d1 f0 85 cb f4  ....a....&......
         *        528  a3 66 aa 0e 29 28 79 2e 9b 5a 67 b5 37 86 8f 01  .f..)(y..Zg.7...
         *        544  <span style="color:blue">fb</span> cc bf e6 a0 05 23 d7 d5 e5 fa 9f fd 06 5f 8c  ......#......._.
         *        560  c7 1f 19 4d c1 d8 e9 b0 34 a6 73 71 ea bd 81 93  ...M....4.sq....
         *        576  c3 dc 58 eb 82 d4 b7 91 c4 53 89 65 55 64 b6 <span style="color:blue">15</span>  ..X......S.eUd..
         *        592  18 5e 7b 3d 0b a9 5c 8a a2 3a 8b 43 9c 6e b9 2c  .^{=..\..:.C.n.,
         *        608  1a bc e1 b2 d9 54 9d 9e 12 ef 32 35 ad 8e e3 dd  .....T....25....
         *        624  00 02 84 7e 3e df fe c8 04 97 ac 21 f6 74 22 da  ...~>......!.t..
         *        640  1b 78 4f be f3 77 <span style="color:blue">60</span> 49 e0 1d 3f f5 2f fc 98 ec  .xO..w`I..?./...
         *        656  0f 39 4e 2a a5 c0 27 47 2b 4b 25 ba bb <span style="color:blue">62</span> f7 57  .9N*..'G+K%..b.W
         *        672  0d 0a a1 2d 07 af 9a 69 52 92 6f ae 3c 99 d2 f1  ...-...iR.o.<...
         *        688  33 09 51 6a 38 4c 48 cd e4 75 31 f9 41 36 d0 30  3.Qj8LH..u1.A6.0
         *        704  40 e2 72 13 a7 ff 7f 42 70 03 c6 44 45 e7 59 63  ..r....Bp..DE.Yc
         *        720  24 83 ab ee a8 68 b4 c9 0c 8d 56 46 db b3 b8 7d  $....h....VF...}
         *        736  a4 e8 76 <span style="color:red">ed</span> 3b 11 d3 5b c5 5d f2 4a 1c 6b 80 7a  ..v.;..[.].J.k.z
         *        752  87 1e cf c2 6d 6c 20 50 10 b1 7c 90 d6 08 14 88  ....ml P..|.....
         *      <span style="color:blue">Vernam-Chiffre blau</span>, <span style="color:red">Endemarken rot</span></pre>
         *
         * Erster Verarbeitungschritt:
         * Die Vertauschung wird rückgängig gemacht. In diesem Beispiel entstehen dann die nachfolgenden drei Datenblöcke:
         *
         * <pre class="fragment"> Datenblock1:
         *       Index  Inhalt (hexadezimal)                             Inhalt (ASCII)
         *           0  <span style="color:blue">5c 97 39 f5 49 e7</span> <span style="color:red">c4</span> <span style="color:green">0c 64 4b c6 cd 2a c2 b3 b7</span>  <span style="color:blue">\.9.I.</span><span style="color:red">.</span><span style="color:green">.dK..*...</span>
         *          16  <span style="color:green">23 21 ec ed 05 bc be 2d 8e bd 2c 47 5d 0f 20 dd  #!.....-..,G]. .</span>
         *          32  <span style="color:green">00 fe 85 02 1c 98 b0 ae 80 93 d3 96 a7 9e 09 7e  ...............~</span>
         *          48  <span style="color:green">9b 18 fb 75 68 c5 a4 3c 6b 28 7d 4a 48 f7 f0 c7  ...uh..<k(}JH...</span>
         *          64  <span style="color:green">ad 01 df 71 88 92 0b da 91 69 1f cc 43 8b b6 5b  ...q.....i..C..[</span>
         *          80  <span style="color:green">56 c1 27 e9 0d 3b 9c 15 b5 c9 7a 54 d1 c8 b2 35  V.'..;....zT...5</span>
         *          96  <span style="color:green">e5 f3 b4 46 40 bb 76 db ac 16 73 eb 82 03 a0 4e  ...F@.v...s....N</span>
         *         112  <span style="color:green">bf 55 07 d7 70 f6 fd f1 d8 b8 31 11 2e ef 4d a9  .U..p.....1...M.</span>
         *         128  <span style="color:green">e6 5f 4c e0 72 7c 9a ea ce 66 50 3d 65 b9 d6 25  ._L.r|...fP=e..%</span>
         *         144  <span style="color:green">cf dc 42 44 51 61 6f fa 22 89 6d 17 9d 24 a6 f2  ..BDQao...m..$..</span>
         *         160  <span style="color:green">ee 6c e1 3a f4 e4 12 1e 45 de 8c 94 60 ab 3e 08  .l.:....E.....>.</span>
         *         176  <span style="color:green">f8 30 19 5e 8a 6a 67 06 1a fc 62 af 74 0e 10 77  .0.^.jg...b.t..w</span>
         *         192  <span style="color:green">36 ca 41 a3 32 33 29 aa 63 c3 0a 58 d2 95 c0 84  6.A.23).c..X....</span>
         *         208  <span style="color:green">8f d5 83 d4 86 1d 04 ba 13 6e 1b 9f 5a 34 2b ff  .........n..Z4+.</span>
         *         224  <span style="color:green">78 d0 8d 7b 14 e3 a2 87 d9 a8 38 53 26 52 59 37  x..{......8S&RY7</span>
         *         240  <span style="color:green">99 a5 e2 e8 3f 81 79 4f 2f b1 cb 57 a1 7f 90 f9  ....?.yO/..W....</span></pre>
         *
         * <pre class="fragment"> Datenblock2:
         *       Index  Inhalt (hexadezimal)                             Inhalt (ASCII)
         *           0  <span style="color:blue">49 15 8a e1 df fa 68 90 c6 60 82 ec 19 be</span> <span style="color:red">0a</span> <span style="color:green">b2</span>  <span style="color:blue">I.....h.......</span><span style="color:red">.</span><span style="color:green">.</span>
         *          16  <span style="color:green">25 80 9d d9 4a 75 85 41 11 6d 1b 22 43 2e 21 8c  %...Ju.A.m..C.!.</span>
         *          32  <span style="color:green">b4 f8 5c 0d 0f a5 d4 af a0 f2 5e 9f 83 3a dd 06  ..\.......^..:..</span>
         *          48  <span style="color:green">d5 79 dc c3 b9 1e 5d 47 37 cf 99 a7 e9 a3 30 c4  .y....]G7.....0.</span>
         *          64  <span style="color:green">3d 8f d8 7b 6e 67 d2 0c 07 84 72 de f7 f9 04 7d  =..{ng....r....}</span>
         *          80  <span style="color:green">d0 e3 cb f1 58 4b 36 92 0e 34 31 89 48 1f 81 10  ....XK6..41.H...</span>
         *          96  <span style="color:green">b3 66 39 8d 14 f3 51 a6 65 db 61 7f c9 b0 6b b5  .f9...Q.e.a...k.</span>
         *         112  <span style="color:green">ff 7c 50 e8 78 13 57 87 2f bd 9b 6a bb e7 ef b1  .|P.x.W./..j....</span>
         *         128  <span style="color:green">4e 02 f6 3e 69 3f 5b 29 45 5f 17 da 03 94 ca 1c  N..>i?[)E_......</span>
         *         144  <span style="color:green">ea e4 55 ae 18 42 2b 2c 28 4d fd 46 f5 ba 16 c2  ..U..B+,(M.F....</span>
         *         160  <span style="color:green">33 ac 54 ce 86 95 05 35 44 26 cc 63 08 2d 8e b8  3.T....5D&.c.-..</span>
         *         176  <span style="color:green">59 c7 62 32 e6 3c 53 71 96 00 bc 4f ab 52 91 cd  Y.b2.<Sq...O.R..</span>
         *         192  <span style="color:green">d1 38 e2 ad a8 d6 fc 12 74 a2 97 73 aa 77 5a 7e  .8......t..s.wZ~</span>
         *         208  <span style="color:green">fb b6 70 9a 01 64 24 d7 98 ee 9c f4 6f 1a c1 b7  ..p..d$.....o...</span>
         *         224  <span style="color:green">a9 c0 93 c5 1d 4c 0b 9e 8b eb ed e0 76 23 09 20  .....L......v#. </span>
         *         240  <span style="color:green">27 88 2a c8 e5 a4 6c f0 56 bf a1 d3 3b 7a fe 40  '.*...l.V...;z..</span></pre>
         *
         * <pre class="fragment"> Datenblock3:
         *       Index  Inhalt (hexadezimal)                             Inhalt (ASCII)
         *           0  <span style="color:blue">15 62 fb 60</span> <span style="color:red">ed</span> <span style="color:green">af 50 f6 2f 5c b0 f4 f2 5d ea bb</span>  <span style="color:blue">.b..</span><span style="color:red">.</span><span style="color:green">.P./\...]..</span>
         *          16  <span style="color:green">26 5a ec f9 34 19 7c b4 c4 d2 9d 08 f0 f3 cc 3b  &Z..4.|........;</span>
         *          32  <span style="color:green">3c 5e 24 7a 2b 21 a6 b8 b9 a1 0f 8d 12 ad c0 53  <^$z+!.........S</span>
         *          48  <span style="color:green">6f 9f ab 4f 1f 11 64 10 db d6 ba 97 dc 4e 9c aa  o..O..d......N..</span>
         *          64  <span style="color:green">ee 91 33 25 e9 68 85 2c fe a0 ae 8a 61 58 28 de  ..3%.h.,....aX(.</span>
         *          80  <span style="color:green">b3 c5 f5 dd 6d 3d 14 90 99 d0 1d 32 01 79 ca 0b  ....m=.....2.y..</span>
         *          96  <span style="color:green">1a 9e bf 69 a7 c3 fd c2 22 89 43 95 df 29 5f 1b  ...i......C..)_.</span>
         *         112  <span style="color:green">63 71 bd 80 38 c9 f1 e0 00 d5 2d 17 78 cd 1e 4a  cq..8.....-.x..J</span>
         *         128  <span style="color:green">73 84 fc 87 e2 1c 35 a2 ac 94 04 54 4c 65 74 cf  s.....5....TLet.</span>
         *         144  <span style="color:green">07 fa a4 30 e5 0d 46 52 b2 75 c8 40 6a 44 ef 88  ...0..FR.u..jD..</span>
         *         160  <span style="color:green">e4 77 7e 3a 86 06 4d 45 8f 8c 47 e8 18 c7 0c a9  .w~:..ME..G.....</span>
         *         176  <span style="color:green">6c 3e cb e3 02 b7 81 09 66 d4 16 56 4b eb 7d 05  l>......f..VK.}.</span>
         *         192  <span style="color:green">b1 36 41 72 a8 5b 76 93 9b 59 b5 a3 39 bc 57 3f  .6Ar.[v..Y..9.W?</span>
         *         208  <span style="color:green">96 e1 9a 03 e6 37 6b ff f7 6e f8 49 d7 d9 92 8b  .....7k..n.I....</span>
         *         224  <span style="color:green">0e da 27 31 d3 0a 7b ce 51 a5 2a 67 13 20 be c6  ..'1..{.Q.*g. ..</span>
         *         240  <span style="color:green">c1 55 42 48 b6 d1 e7 7f 82 23 2e 70 83 98 8e d8  .UBH.....#.p....</span>
         *      <span style="color:green">Redundanzbytes grün</span>, <span style="color:blue">Vernam-Chiffre blau</span>, <span style="color:red">Endemarken rot</span></pre>
         *
         * Zweiter Verarbeitungschritt:
         * Die Daten der Vernam-Chiffre werden aus den drei Blöcken aufgesammelt. In diesem Beispiel entsteht dann der nachfolgende Datenblock:
         *
         *      24 Byte lange Vernam-Chiffre:
         *       Index  Inhalt (hexadezimal)                             Inhalt (ASCII)
         *           0  5c 97 39 f5 49 e7 49 15 8a e1 df fa 68 90 c6 60  \.9.I.I.....h...
         *          16  82 ec 19 be 15 62 fb 60                          .....b..
         *
         * Dritter Verarbeitungschritt:
         * Die Daten der Vernam-Chiffre werden mit dem OneTimePad-Schlüssel wieder entschlüsselt. In diesem Beispiel entsteht dann der nachfolgende Datenblock:
         *
         * <pre class="fragment"> Der resultierende 24 Byte lange Klartextblock:
         *   Index  Inhalt (hexadezimal)                             Inhalt (ASCII)
         *       0  <span style="color:green">22 40 07 8d 3a 2f c1 3a da 3d</span> <span style="color:blue">45 69 6e 20 54 65</span>  <span style="color:green">..........</span><span style="color:blue">Ein Te</span>
         *      16  <span style="color:blue">73 74 74 65 78 74 2e</span> <span style="color:red">0d</span>                          <span style="color:blue">sttext.</span><span style="color:red">.</span>
         *      <span style="color:green">Zufallstext grün</span>, <span style="color:blue">Ausgangstext blau</span>, <span style="color:red">Ausgangstextlänge rot</span></pre>
         *
         * Vierter Verarbeitungschritt:
         * Die Nutzdatenlänge wird am Ende des Blockes ermittelt (hier 13) und die Nutzdaten werden in den Ausgabedatenblock kopiert. In diesem Beispiel entsteht dann der nachfolgende Datenblock:
         *
         *      Ein 13 Zeichen langer Text:
         *       Index  Inhalt (hexadezimal)                             Inhalt(ASCII)
         *           0  45 69 6e 20 54 65 73 74 74 65 78 74 2e           Ein Testtext.
         *
         * \param in Ein Verweis auf eine ByteArray-Instanz mit den verschlüsselten Eingabedaten.
         * \param out Ein Verweis auf eine ByteArray-Instanz mit den unverschlüsselten Ausgabedaten.
         * \return _true_, wenn die Methode erfolgreich war (ansonsten _false_).
         * \sa Encrypt()
         */
        bool Secure::Decrypt(ByteArray &in, ByteArray &out) {
                void *p = &in;
                if (p == nullptr) {
                        return false;
                }
                p = &out;
                if (p == nullptr) {
                        return false;
                }
                size_t length = in.Size();
                if ((length & 0xFF) > 0) {
                        return false;           // Length not divisible by 256
                }
                ByteArray permutation(256);
                permutation.SetSize(256);
                ByteArray dataredundanz(256, true);
                dataredundanz.SetSize(256);
                ByteArray onetimepadarray(256, true);
                size_t index = 0;
                size_t writeIndex = 0;
                unsigned char b2 = 0;
                while (index < (length - 1)) {
                        permutation.Copy(in, index, 0, 256);
                        index = index + 256;
                        if (IsByteMultipleInArray(permutation)) {
                                return false;
                        }
                        for (size_t i = 0; i < 256; i++) {      //  Undo swapping
                                unsigned char b1 = static_cast<unsigned char>((*permutationKey)[b2]);
                                dataredundanz[b1] = permutation[i];
                                b2++;
                        }
                        if (!DeleteRedundancy(dataredundanz, onetimepadarray, writeIndex, b2)) {
                                return false;
                        }
                        b2++;
                }
                length = onetimepadarray.Size();
                if (length != oneTimePadKey->Size()) {
                        return false;
                }
                ByteArray plainarray(length, true);
                plainarray.SetSize(length);
                b2 = 0;
                for (size_t i = 0; i < length; i++) {
                        unsigned char b1 = static_cast<unsigned char>(onetimepadarray[i]) - b2 - static_cast<unsigned char>((*oneTimePadKey)[i]);
                        plainarray[i] = static_cast<char>(b1);
                        b2 = static_cast<unsigned char>(onetimepadarray[i]);
                }
                index = length - 1;
                int lng = plainarray.ReadX209Int64(index, true);
                index = index - static_cast<size_t>(lng) + 1;
                out.Copy(plainarray, index, 0, static_cast<size_t>(lng));
                return true;
        }

        /*! \brief Diese Methode `DeleteRedundancy()` speichert die Nutzdatenbytes in einem Zieldatenfeld.
         *
         * Bei dieser Methode wird zunächst in einem 256-Byte-Quellfeld nach der Endemarke gesucht.
         * Dann werden die Benutzerdaten in das Zieldatenfeld (ab dem Schreibindex) übertragen
         * und der neue Schreibindex wird berechnet.
         *
         * \param source Verweis auf eine ByteArray-Instanz, die ein 256 Byte langes Quelldatenfeld enthält.
         * \param target Verweis auf eine ByteArray-Instanz, in die die Nutzdatenbytes kopiert werden sollen.
         * \param index Aktuelle Schreibposition im Zieldatenfeld.
         * \param arrayNumber Bytewert (zwischen 0 und 255) der die Endmarkierung darstellt.
         * \return _true_, wenn keine Fehler festgestellt wurden (ansonsten _false_).
         */
        bool Secure::DeleteRedundancy(ByteArray &source, ByteArray &target, size_t &index, unsigned char arrayNumber) {
                size_t endIndex = 256;
                for (size_t i = 0; i < 129; i++) {
                        if (source[i] == (*labelKey)[arrayNumber]) {
                                endIndex = i;
                                break;
                        }
                }
                if (endIndex == 256) {
                        return false;
                }
                if (endIndex > 0) {
                        target.Copy(source, 0, index, endIndex);
                        index = index + endIndex;
                }
                return true;
        }


        /*! \brief Diese Methode `Encrypt()` verschlüsselt Bytes aus der Eingabe-ByteArray-Instanz und speichert das Ergebnis in der Ausgabe-ByteArray-Instanz.
         *
         * Es werden folgende Schritte durchgeführt:
         *
         * Diese Methode prüft, ob die Länge der Eingabe-ByteArray-Instanz mindestens fünf Bytes kürzer ist als der in dieser Instanz festgelegte OneTimePad-Schlüssel.
         * Wenn nicht, wird die Bearbeitung abgebrochen und _false_ zurück gegeben.
         *
         * Eine neue ByteArray-Instanz (die genauso lang ist wie der OneTimePad-Schlüssel) wird zum Beginn mit Zufallszahlen gefüllt.
         * Am Ende dieser ByteArray-Instanz wird die Länge der Eingabe-ByteArray-Instanz vermerkt.
         * Davor werden die Datenbytes aus der Eingabe-ByteArray-Instanz kopiert.
         * Es wird damit sichergestellt, dass diese neue ByteArray-Instanz mit mindestes einem Zufallsbyte beginnt (meistens sind es aber deutlich mehr Zufallsbytes).
         *
         * Die Daten aus dieser neuen ByteArray-Instanz werden nun Byte für Byte mit dem OneTimePadKey gleicher Größe (im CBC-Modus) verschlüsselt.
         *
         * Die einmal verschlüsselten Daten werden nun mit Redundanzbytes versehen.
         * Dazu werden zunächst mehrere nummerierte 256-Byte-Blöcke teilweise mit Benutzerdaten, einem Byte als Endemarke
         * und den restlichen Redundanzbytes gefüllt (siehe Methode `CreateRedundancy()`).
         * In diesen 256 Byte großen Blöcken kommt jeder mögliche Byte-Wert [0..255] genau einmal vor.
         *
         * Die Bytes in den erstellten Blöcken werden anschließend mit dem Schlüssel _PermutationKey_ vertauscht,
         * und die Blöcke werden dann in die Ausgabe-ByteArray-Instanz geschrieben.
         *
         * In dem nachfolgenden Beispiel soll der 13 Byte lange Text: <span style="color:blue">Ein Testtext.</span> verschlüsselt werden.
         * Für jedes dieser 13 Zeichen wird in der UTF-8 Codierung genau ein Byte belegt. Der Datendump dieser Ausgangsdaten sieht damit wie folgt aus:
         *
         *      Ein 13 Zeichen langer Text:
         *       Index  Inhalt (hexadezimal)                             Inhalt(ASCII)
         *           0  45 69 6e 20 54 65 73 74 74 65 78 74 2e           Ein Testtext.
         *
         * Erster Verschlüsselungsschritt:
         * Ein 24  Byte langer Klartextblock wird wie folgt mit dem Ausgangstext belegt. Am Ende des Klartextblockes wird zunächst die Länge des Ausgangstextes
         * hinterlegt. Dafür sind bis zu einer Länge von 127 Zeichen ein Byte und bis zu einer Länge von 8191 Zeichen zwei Byte erforderlich.
         * In diesem Beispiel reicht also ein Byte (die Länge ist 13) aus. Davor wird der Ausgangstext platziert.
         * Der Anfang des Klartextblockes wird mit Zeichen aus dem Zufallszahlengenerator gefüllt. Dies sind in diesem Falle 10 Byte.
         * Der Datendump des Klartextblockes sieht damit wie folgt aus:
         *
         * <pre class="fragment"> Der resultierende 24 Byte lange Klartextblock:
         *   Index  Inhalt (hexadezimal)                             Inhalt (ASCII)
         *       0  <span style="color:green">22 40 07 8d 3a 2f c1 3a da 3d</span> <span style="color:blue">45 69 6e 20 54 65</span>  <span style="color:green">..........</span><span style="color:blue">Ein Te</span>
         *      16  <span style="color:blue">73 74 74 65 78 74 2e</span> <span style="color:red">0d</span>                          <span style="color:blue">sttext.</span><span style="color:red">.</span>
         *      <span style="color:green">Zufallstext grün</span>, <span style="color:blue">Ausgangstext blau</span>, <span style="color:red">Ausgangstextlänge rot</span></pre>
         *
         * Zweiter Verschlüsselungsschritt:
         * Der Klartextblock aus dem ersten Verschlüsselungsschritt wird nun mit einer Vernam-Chiffre verschlüsselt. Hierzu wurde in diesem Beispiel vom
         * Zufallszahlengenerator folgender 24 Byte langer One-Time-Pad generiert:
         *
         *      24 Byte langer Schlüssel:
         *       Index  Inhalt (hexadezimal)                             Inhalt (ASCII)
         *           0  3a fb 9b 2f 1a 6f a1 92 9b 1a b9 b2 00 08 e2 35  :../.o.........5
         *          16  af f6 b9 40 df d9 6b 58                          ...@..kX
         *
         * Zum Byte 1 des Klartextblockes (0x22) wird das Byte 1 des One-Time-Pad (0x3a) und das vorangehende Byte des Ergebnisses (hier noch 0x00) addiert.
         * Als Ergebnis ergibt sich der Wert 0x5c. Byte 2 des Klartextblockes (0x40) plus Byte 2 des Schlüssels (0xfb) plus
         * das Ergebnis von Byte 1 (0x5c) ergibt das Ergebnis 0x97 (der Übertrag 256 entfällt). Byte 3 des Klartextblockes (0x07) plus Byte 3 des Schlüssels (0x9b) plus das Ergebnis von
         * Byte 2 (0x97) ergibt das Ergebnis 0x39 (der Übertrag 256 entfällt). usw. Für die gesamte Verschlüsselung entsteht folgendes Ergebnis:
         *
         *      24 Byte lange Vernam-Chiffre:
         *       Index  Inhalt (hexadezimal)                             Inhalt (ASCII)
         *           0  5c 97 39 f5 49 e7 49 15 8a e1 df fa 68 90 c6 60  \.9.I.I.....h...
         *          16  82 ec 19 be 15 62 fb 60                          .....b..
         *
         * Dritter Verschlüsselungsschritt:
         * In diesem Verschlüsselungsschritt werden mehrere 256 Byte große Datenblöcke gebildet, in denen jeder mögliche Bytewert (von 0x00 bis 0xff) genau einmal vorkommt.
         * Begonnen wird mit der Einfügung der Endemarke an der Indexposition 0. Block 1 beginnt mit der Endemarke 0xc4, Block 2 mit der Endemarke 0x0a, Block 3
         * würde mit Endemarke 0xed beginnen. usw. Nach 256 Blöcken würde das Spiel wieder von vorne beginnen. So viele Blöcke werden aber in der Regel nicht gebildet.
         *
         * Vor der Endemarke werden nun so lange Datenbytes aus der Vernam-Chiffre in den Block eingefügt, bis es zu einer Bytewiederholung kommen würde.
         * Maximal werden jedoch nur 128 Datenbytes eingefügt (wenn es zu keiner Bytewiederholung gekommen ist).
         * Die Anzahl der Datenbytes aus der Vernam-Chiffre kann also von Block zu Block stark schwanken.
         * Hat das erste Datenbyte, dass aus der Vernam-Chiffre eingefügt werden soll, zufällig denselben Bytewert wie die Endemarke selbst, so wird nur die Endemarke eingefügt.
         *
         * Der Rest der 256 Byte große Datenblöcke wird mit Zufallszahlen aufgefüllt, die noch nicht im Block enthalten waren. So werden Redundanzbytes erzeugt.
         *
         * In diesem Beispiel ergeben sich so drei 256 Byte große Blöcke, deren Datendump nachfolgend gezeigt wird.
         *
         * <pre class="fragment"> Datenblock1:
         *       Index  Inhalt (hexadezimal)                             Inhalt (ASCII)
         *           0  <span style="color:blue">5c 97 39 f5 49 e7</span> <span style="color:red">c4</span> <span style="color:green">0c 64 4b c6 cd 2a c2 b3 b7</span>  <span style="color:blue">\.9.I.</span><span style="color:red">.</span><span style="color:green">.dK..*...</span>
         *          16  <span style="color:green">23 21 ec ed 05 bc be 2d 8e bd 2c 47 5d 0f 20 dd  #!.....-..,G]. .</span>
         *          32  <span style="color:green">00 fe 85 02 1c 98 b0 ae 80 93 d3 96 a7 9e 09 7e  ...............~</span>
         *          48  <span style="color:green">9b 18 fb 75 68 c5 a4 3c 6b 28 7d 4a 48 f7 f0 c7  ...uh..<k(}JH...</span>
         *          64  <span style="color:green">ad 01 df 71 88 92 0b da 91 69 1f cc 43 8b b6 5b  ...q.....i..C..[</span>
         *          80  <span style="color:green">56 c1 27 e9 0d 3b 9c 15 b5 c9 7a 54 d1 c8 b2 35  V.'..;....zT...5</span>
         *          96  <span style="color:green">e5 f3 b4 46 40 bb 76 db ac 16 73 eb 82 03 a0 4e  ...F@.v...s....N</span>
         *         112  <span style="color:green">bf 55 07 d7 70 f6 fd f1 d8 b8 31 11 2e ef 4d a9  .U..p.....1...M.</span>
         *         128  <span style="color:green">e6 5f 4c e0 72 7c 9a ea ce 66 50 3d 65 b9 d6 25  ._L.r|...fP=e..%</span>
         *         144  <span style="color:green">cf dc 42 44 51 61 6f fa 22 89 6d 17 9d 24 a6 f2  ..BDQao...m..$..</span>
         *         160  <span style="color:green">ee 6c e1 3a f4 e4 12 1e 45 de 8c 94 60 ab 3e 08  .l.:....E.....>.</span>
         *         176  <span style="color:green">f8 30 19 5e 8a 6a 67 06 1a fc 62 af 74 0e 10 77  .0.^.jg...b.t..w</span>
         *         192  <span style="color:green">36 ca 41 a3 32 33 29 aa 63 c3 0a 58 d2 95 c0 84  6.A.23).c..X....</span>
         *         208  <span style="color:green">8f d5 83 d4 86 1d 04 ba 13 6e 1b 9f 5a 34 2b ff  .........n..Z4+.</span>
         *         224  <span style="color:green">78 d0 8d 7b 14 e3 a2 87 d9 a8 38 53 26 52 59 37  x..{......8S&RY7</span>
         *         240  <span style="color:green">99 a5 e2 e8 3f 81 79 4f 2f b1 cb 57 a1 7f 90 f9  ....?.yO/..W....</span></pre>
         *
         * <pre class="fragment"> Datenblock2:
         *       Index  Inhalt (hexadezimal)                             Inhalt (ASCII)
         *           0  <span style="color:blue">49 15 8a e1 df fa 68 90 c6 60 82 ec 19 be</span> <span style="color:red">0a</span> <span style="color:green">b2</span>  <span style="color:blue">I.....h.......</span><span style="color:red">.</span><span style="color:green">.</span>
         *          16  <span style="color:green">25 80 9d d9 4a 75 85 41 11 6d 1b 22 43 2e 21 8c  %...Ju.A.m..C.!.</span>
         *          32  <span style="color:green">b4 f8 5c 0d 0f a5 d4 af a0 f2 5e 9f 83 3a dd 06  ..\.......^..:..</span>
         *          48  <span style="color:green">d5 79 dc c3 b9 1e 5d 47 37 cf 99 a7 e9 a3 30 c4  .y....]G7.....0.</span>
         *          64  <span style="color:green">3d 8f d8 7b 6e 67 d2 0c 07 84 72 de f7 f9 04 7d  =..{ng....r....}</span>
         *          80  <span style="color:green">d0 e3 cb f1 58 4b 36 92 0e 34 31 89 48 1f 81 10  ....XK6..41.H...</span>
         *          96  <span style="color:green">b3 66 39 8d 14 f3 51 a6 65 db 61 7f c9 b0 6b b5  .f9...Q.e.a...k.</span>
         *         112  <span style="color:green">ff 7c 50 e8 78 13 57 87 2f bd 9b 6a bb e7 ef b1  .|P.x.W./..j....</span>
         *         128  <span style="color:green">4e 02 f6 3e 69 3f 5b 29 45 5f 17 da 03 94 ca 1c  N..>i?[)E_......</span>
         *         144  <span style="color:green">ea e4 55 ae 18 42 2b 2c 28 4d fd 46 f5 ba 16 c2  ..U..B+,(M.F....</span>
         *         160  <span style="color:green">33 ac 54 ce 86 95 05 35 44 26 cc 63 08 2d 8e b8  3.T....5D&.c.-..</span>
         *         176  <span style="color:green">59 c7 62 32 e6 3c 53 71 96 00 bc 4f ab 52 91 cd  Y.b2.<Sq...O.R..</span>
         *         192  <span style="color:green">d1 38 e2 ad a8 d6 fc 12 74 a2 97 73 aa 77 5a 7e  .8......t..s.wZ~</span>
         *         208  <span style="color:green">fb b6 70 9a 01 64 24 d7 98 ee 9c f4 6f 1a c1 b7  ..p..d$.....o...</span>
         *         224  <span style="color:green">a9 c0 93 c5 1d 4c 0b 9e 8b eb ed e0 76 23 09 20  .....L......v#. </span>
         *         240  <span style="color:green">27 88 2a c8 e5 a4 6c f0 56 bf a1 d3 3b 7a fe 40  '.*...l.V...;z..</span></pre>
         *
         * <pre class="fragment"> Datenblock3:
         *       Index  Inhalt (hexadezimal)                             Inhalt (ASCII)
         *           0  <span style="color:blue">15 62 fb 60</span> <span style="color:red">ed</span> <span style="color:green">af 50 f6 2f 5c b0 f4 f2 5d ea bb</span>  <span style="color:blue">.b..</span><span style="color:red">.</span><span style="color:green">.P./\...]..</span>
         *          16  <span style="color:green">26 5a ec f9 34 19 7c b4 c4 d2 9d 08 f0 f3 cc 3b  &Z..4.|........;</span>
         *          32  <span style="color:green">3c 5e 24 7a 2b 21 a6 b8 b9 a1 0f 8d 12 ad c0 53  <^$z+!.........S</span>
         *          48  <span style="color:green">6f 9f ab 4f 1f 11 64 10 db d6 ba 97 dc 4e 9c aa  o..O..d......N..</span>
         *          64  <span style="color:green">ee 91 33 25 e9 68 85 2c fe a0 ae 8a 61 58 28 de  ..3%.h.,....aX(.</span>
         *          80  <span style="color:green">b3 c5 f5 dd 6d 3d 14 90 99 d0 1d 32 01 79 ca 0b  ....m=.....2.y..</span>
         *          96  <span style="color:green">1a 9e bf 69 a7 c3 fd c2 22 89 43 95 df 29 5f 1b  ...i......C..)_.</span>
         *         112  <span style="color:green">63 71 bd 80 38 c9 f1 e0 00 d5 2d 17 78 cd 1e 4a  cq..8.....-.x..J</span>
         *         128  <span style="color:green">73 84 fc 87 e2 1c 35 a2 ac 94 04 54 4c 65 74 cf  s.....5....TLet.</span>
         *         144  <span style="color:green">07 fa a4 30 e5 0d 46 52 b2 75 c8 40 6a 44 ef 88  ...0..FR.u..jD..</span>
         *         160  <span style="color:green">e4 77 7e 3a 86 06 4d 45 8f 8c 47 e8 18 c7 0c a9  .w~:..ME..G.....</span>
         *         176  <span style="color:green">6c 3e cb e3 02 b7 81 09 66 d4 16 56 4b eb 7d 05  l>......f..VK.}.</span>
         *         192  <span style="color:green">b1 36 41 72 a8 5b 76 93 9b 59 b5 a3 39 bc 57 3f  .6Ar.[v..Y..9.W?</span>
         *         208  <span style="color:green">96 e1 9a 03 e6 37 6b ff f7 6e f8 49 d7 d9 92 8b  .....7k..n.I....</span>
         *         224  <span style="color:green">0e da 27 31 d3 0a 7b ce 51 a5 2a 67 13 20 be c6  ..'1..{.Q.*g. ..</span>
         *         240  <span style="color:green">c1 55 42 48 b6 d1 e7 7f 82 23 2e 70 83 98 8e d8  .UBH.....#.p....</span>
         *      <span style="color:green">Redundanzbytes grün</span>, <span style="color:blue">Vernam-Chiffre blau</span>, <span style="color:red">Endemarken rot</span></pre>
         *
         * Vierter Verschlüsselungsschritt:
         * In diesem Verschlüsselungsschritt werden mit einem Vertauschungsschlüssel mehrere 256 Byte große Datenblöcke gebildet, in denen jeder mögliche
         * Bytewert (von 0x00 bis 0xff) genau einmal vorkommen. Im neuen Block1 wird an Index 0x00 nun das Byte aus Block1 des vorangegangenen Kapitels geschrieben,
         * dass dort am Index 0x56 steht. An Index 0x01 wird das Byte aus Index 0x9f geschrieben usw.
         *
         * Vor der Bildung des neuen zweiten Blockes wird der Vertauschungsschlüssel um 1 Byte rotiert. An Index 0x00 des zweiten Blockes wird das Byte aus Block1 des vorangegangenen Kapitels geschrieben,
         * dass dort am Index 0x9f steht. An Index 0x01 wird das Byte aus Index 0x6b geschrieben usw.
         *
         * In diesem Beispiel ergeben sich so drei 256 Byte große Blöcke, deren Datendump nachfolgend gezeigt wird.
         *
         * <pre class="fragment"> Ausgabeatenblock:
         *      Index  Inhalt (hexadezimal)                             Inhalt (ASCII)
         *          0  9c f2 eb 62 1b 5b 43 8f 66 87 11 23 b2 81 5d 0b  ...b.[C.f..#..].
         *         16  19 cd 58 1a c7 78 03 b6 c8 cb 63 21 53 0a 1d f4  ..X..x....c!S...
         *         32  45 d1 <span style="color:blue">39</span> 20 b4 86 69 77 b1 5a b8 51 dc 18 76 e4  E.9 ..iw.Z.Q..v.
         *         48  a0 de ab 68 bc 12 99 f9 88 c6 05 b0 e6 55 b3 07  ...h.........U..
         *         64  67 aa bb 48 8b 0e 2f fc 6a 01 8e 7e 16 b9 a5 a4  g..H../.j..~....
         *         80  3f <span style="color:blue">5c</span> 60 fe a2 3b 35 08 4b cc ea 3a ff 73 f0 6e  ?\`..;5.K..:.s.n
         *         96  80 da e5 95 d5 22 34 3d 2c f3 a7 a6 54 9a 9e 90  ......4=,...T...
         *        112  5e e9 d8 8a 5f e1 30 82 91 6d 50 4a ce 98 0c d6  ^..._.0..mPJ....
         *        128  ac d0 4e 2e 75 59 0f 6c <span style="color:blue">f5</span> 9f f1 7a 84 27 64 4c  ..N.uY.l...z.'dL
         *        144  7f ec d3 d2 f7 38 a8 09 8d 8c 1c 74 71 7d b7 <span style="color:blue">97</span>  .....8.....tq}..
         *        160  13 c0 61 e3 93 31 cf <span style="color:blue">e7</span> 83 46 fa 2b 9b 1f 00 b5  ..a..1...F.+....
         *        176  bd fd df 06 d9 9d 70 65 e8 ef ee 89 7b ed 41 ca  ......pe....{.A.
         *        192  c9 44 17 72 a3 26 40 ba 4f e2 57 d4 37 24 1e 79  .D.r.&..O.W.7$.y
         *        208  c3 bf 85 a1 fb ad 32 92 2d f6 3e 96 af 6f 6b 56  ......2.-.>..okV
         *        224  ae 10 42 94 29 <span style="color:blue">49</span> dd c5 14 33 c1 c2 2a a9 7c 04  ..B.)I...3..*.|.
         *        240  d7 02 e0 4d 25 db 0d f8 52 <span style="color:red">c4</span> 3c 36 be 15 28 47  ...M%...R.<6..(G
         *
         *        256  c2 7f bc 9c 7d f7 fb 5f 9e 6a 25 81 a4 43 d2 62  ....}.._.j%..C.b
         *        272  <span style="color:blue">ec</span> 73 96 c4 a9 b0 04 1f a1 74 80 e0 97 64 86 44  .s.......t...d.D
         *        288  48 <span style="color:blue">8a</span> 21 39 01 84 cd bf 6f bd 18 e4 79 51 95 6b  H.!9....o...yQ.k
         *        304  26 2d b9 75 05 27 40 6e <span style="color:blue">82</span> 4a d4 4e 7c <span style="color:red">0a</span> 50 53  &-.u.'.n.J.N|.PS
         *        320  12 f3 e9 f9 52 56 00 3c 8f 11 06 db 94 88 5d e5  ....RV.<......].
         *        336  <span style="color:blue">49</span> 08 f8 0b 4b 10 b8 <span style="color:blue">60</span> de 29 ce b7 61 30 ee a0  I...K..`.)..a0..
         *        352  0c b3 77 b6 28 1a da 1b 66 83 16 89 5b 3a fe 32  ..w.(...f...[:.2
         *        368  f1 2f e6 02 54 c7 c9 07 fd 17 a7 45 a5 <span style="color:blue">90</span> ca 65  ./..T......E...e
         *        384  c0 b5 bb c3 09 2e ac <span style="color:blue">e1</span> f4 87 31 7e cb <span style="color:blue">c6</span> f6 7a  ..........1~...z
         *        400  9d 5e aa a3 ed eb dd 93 cc 0f ab 7b 99 b2 <span style="color:blue">15</span> 98  .^.........{....
         *        416  5a 42 4c f2 9b ea <span style="color:blue">fa</span> 70 8d 2c c1 d5 72 b4 0e 6d  ZBL....p.,..r..m
         *        432  57 d8 71 8b f5 78 03 c8 e7 33 4d c5 d9 e2 38 34  W.q..x...3M...84
         *        448  ae 46 69 ad 76 14 d7 f0 2a d3 9a 20 ba 35 6c a2  .Fi.v...*.. .5l.
         *        464  ff 5c 3b dc 3d a8 67 41 13 8e 9f 4f 2b 37 d0 af  .\;.=.gA...O+7..
         *        480  91 55 63 fc <span style="color:blue">df</span> 8c 1e 1d d6 e3 <span style="color:blue">be 19</span> b1 3f 24 e8  .Uc..........?$.
         *        496  0d 3e ef 1c a6 58 59 23 <span style="color:blue">68</span> 47 d1 85 92 cf 22 36  .>...XY#hG.....6
         *
         *        512  95 16 f8 de 61 96 94 ce 17 26 ca d1 f0 85 cb f4  ....a....&......
         *        528  a3 66 aa 0e 29 28 79 2e 9b 5a 67 b5 37 86 8f 01  .f..)(y..Zg.7...
         *        544  <span style="color:blue">fb</span> cc bf e6 a0 05 23 d7 d5 e5 fa 9f fd 06 5f 8c  ......#......._.
         *        560  c7 1f 19 4d c1 d8 e9 b0 34 a6 73 71 ea bd 81 93  ...M....4.sq....
         *        576  c3 dc 58 eb 82 d4 b7 91 c4 53 89 65 55 64 b6 <span style="color:blue">15</span>  ..X......S.eUd..
         *        592  18 5e 7b 3d 0b a9 5c 8a a2 3a 8b 43 9c 6e b9 2c  .^{=..\..:.C.n.,
         *        608  1a bc e1 b2 d9 54 9d 9e 12 ef 32 35 ad 8e e3 dd  .....T....25....
         *        624  00 02 84 7e 3e df fe c8 04 97 ac 21 f6 74 22 da  ...~>......!.t..
         *        640  1b 78 4f be f3 77 <span style="color:blue">60</span> 49 e0 1d 3f f5 2f fc 98 ec  .xO..w`I..?./...
         *        656  0f 39 4e 2a a5 c0 27 47 2b 4b 25 ba bb <span style="color:blue">62</span> f7 57  .9N*..'G+K%..b.W
         *        672  0d 0a a1 2d 07 af 9a 69 52 92 6f ae 3c 99 d2 f1  ...-...iR.o.<...
         *        688  33 09 51 6a 38 4c 48 cd e4 75 31 f9 41 36 d0 30  3.Qj8LH..u1.A6.0
         *        704  40 e2 72 13 a7 ff 7f 42 70 03 c6 44 45 e7 59 63  ..r....Bp..DE.Yc
         *        720  24 83 ab ee a8 68 b4 c9 0c 8d 56 46 db b3 b8 7d  $....h....VF...}
         *        736  a4 e8 76 <span style="color:red">ed</span> 3b 11 d3 5b c5 5d f2 4a 1c 6b 80 7a  ..v.;..[.].J.k.z
         *        752  87 1e cf c2 6d 6c 20 50 10 b1 7c 90 d6 08 14 88  ....ml P..|.....
         *      <span style="color:blue">Vernam-Chiffre blau</span>, <span style="color:red">Endemarken rot</span></pre>
         *
         * \param in Ein Verweis auf eine ByteArray-Instanz, die die unverschlüsselten Eingabedaten enthält.
         * \param out Ein Verweis auf eine ByteArray-Instanz mit den verschlüsselten Ausgabedaten.
         * \return _true_, wenn die Methode erfolgreich war (ansonsten _false_).
         * \sa Decrypt()
         */
        bool Secure::Encrypt(ByteArray &in, ByteArray &out) {
                void *p = &in;
                if (p == nullptr) {
                        return false;
                }
                p = &out;
                if (p == nullptr) {
                        return false;
                }
                size_t length = in.Size();
                size_t oTPKlength = oneTimePadKey->Size();
                if (length > (oTPKlength - 5)) {
                        return false;
                }
                ByteArray text(oTPKlength, true);
                text.SetSize(oTPKlength);
                size_t index = oTPKlength - 1;
                text.WriteX209Int64(index, length, true);
                index = index - length + 1;
                size_t writeindex = 0;
                if (index > 0) {
                        size_t rLength = redundanz->Size();
                        if (index > rLength) {
                                size_t rest = index - writeindex;
                                while (rest > rLength) {
                                        text.Copy(*redundanz, 0, writeindex, rLength);
                                        writeindex = writeindex + rLength;
                                        rest = rest - rLength;
                                }
                                if (rest > 0) {
                                        text.Copy(*redundanz, 0, writeindex, rest);
                                }
                        } else {
                                text.Copy(*redundanz, 0, 0, index);
                        }
                }
                text.Copy(in, 0, index, length);
                unsigned char b2 = 0;
                for (size_t i = 0; i < oTPKlength; i++) {
                        unsigned char b1 = b2 + static_cast<unsigned char>(text[i]) + static_cast<unsigned char>((*oneTimePadKey)[i]);
                        text[i] = static_cast<char>(b1);
                        b2 = b1;
                }
                index = 0;
                b2 = 0;
                writeindex = 0;
                ByteArray dataredundanz(256, true);
                dataredundanz.SetSize(256);
                ByteArray permutation(256);
                permutation.SetSize(256);
                do {
                        index = CreateRedundancy(text, index, dataredundanz, static_cast<unsigned char>(b2));
                        for (size_t i = 0; i < 256; i++) {
                                unsigned char b1 = static_cast<unsigned char>((*permutationKey)[b2]);
                                permutation[i] = dataredundanz[b1];
                                b2++;
                        }
                        b2++;
                        out.Copy(permutation, 0, writeindex, 256);
                        writeindex = writeindex + 256;
                } while (index < oTPKlength);
                out.SetSize(writeindex);
                return true;
        }

        /*! \brief Diese Methode `InsertRedundancy()` füllt ein 256-Byte-Datenfeld beginnend bei einem Startindex mit Redundanzbytes.
         *
         * Ein im ersten Parameter angegebenes 256-Byte-Datenfeld wurde (durch eine andere Methode) am Anfang mit Datenbytes gefüllt, die alle unterschiedlich sind.
         * Der Rest des Datenfeldes wird nun mithilfe der Methode `InsertRedundancy()` mit Redundanzbytes aufgefüllt, und zwar so,
         * dass jeder Byte-Wert von 0 bis 255 genau einmal im Datenfeld vorkommt.
         * Die Reihenfolge der gebildeten Redundanzbytes (abhängig von den bereits vorhandenen Datenbytes) bleibt stets gleich.
         *
         * \param ref Verweis auf eine ByteArray-Instanz, die das zu verarbeitende Datenfeld (256 Byte) enthält.
         * \param index Index im Datenfeld, ab dem Redundanzbytes gebildet werden sollen.
         */
        void Secure::InsertRedundancy(ByteArray &ref, size_t index) {
                int contrastsum = 0;
                int datasum = 0;
                unsigned char b;
                for (size_t i = 0; i < index; i++) {
                        b = static_cast<unsigned char>(ref[i]);
                        datasum = datasum + b;
                        if ((i % 2) == 1) {
                                contrastsum = contrastsum + 128;
                        } else {
                                contrastsum = contrastsum + 127;
                        }
                }
                size_t k = 0;
                bool isInserted;
                for (size_t i = index; i < 256; i++) {
                        if (datasum > contrastsum) {
                                b = static_cast<unsigned char>(ref[k]);
                                if (b > 127) {
                                        b = b - 127;
                                } else {
                                        b = 127 - b;
                                }
                                do {
                                        isInserted = IsByteInArray(ref, static_cast<char>(b), index);
                                        if (isInserted) {
                                                b = b - 7;
                                        }
                                } while (isInserted);
                                ref[index] = static_cast<char>(b);
                        } else {
                                b = static_cast<unsigned char>(ref[k]);
                                if (b > 128) {
                                        b = b + 1;
                                } else {
                                        b = 255 - b;
                                }
                                do {
                                        isInserted = IsByteInArray(ref, static_cast<char>(b), index);
                                        if (isInserted) {
                                                b = b + 11;
                                        }
                                } while (isInserted);
                                ref[index] = static_cast<char>(b);
                        }
                        index++;
                        k++;
                        datasum = datasum + b;
                        if ((i % 2) == 1) {
                                contrastsum = contrastsum + 128;
                        } else {
                                contrastsum = contrastsum + 127;
                        }
                }
        }

        /*! \brief Diese Methode `IsByteInArray()` sucht in einem Array nach dem übergebenen Byte.
         *
         * Die Methode sucht in einem Array, beginnend bei Index 0, nach einem Byte-Wert. Es wird bis zum Index __Count__ minus eins gesucht.
         * Wird der gesuchte Byte-Wert gefunden, wird die Suche sofort abgebrochen und _true_ zurückgegeben.
         * Wird der gesuchte Byte-Wert nicht gefunden, wird _false_ zurückgegeben.
         *
         * \param ref Verweis auf eine ByteArray-Instanz.
         * \param c Zu suchender Byte-Wert.
         * \param count Anzahl der Suchversuche.
         * \return _true_ bei Treffer, andernfalls _false_.
         */
        bool Secure::IsByteInArray(ByteArray &ref, char c, size_t count) {
                for (size_t i = 0; i < count; i++) {
                        if (ref[i] == c) {
                                return true;
                        }
                }
                return false;
        }

        /*! \brief Diese Methode `IsByteMultipleInArray()` prüft, ob ein Byte-Wert mehrfach in einem Datenfeld vorkommt.
         *
         * Wird ein Byte-Wert mehrfach im Datenfeld gefunden, wird die Prüfung abgebrochen und _true_ zurückgegeben.
         * Andernfalls wird am Ende der Prüfung _false_ zurückgegeben.
         *
         * \param ref Verweis auf eine ByteArray-Instanz.
         * \return _true_ bei mehrfachen Vorkommen eines Bytewertes, andernfalls _false_.
         */
        bool Secure::IsByteMultipleInArray(ByteArray &ref) {
                size_t length = ref.Size();
                char b;
                for (size_t i = 0; i < length; i++) {
                        b = ref[i];
                        size_t count = 0;
                        for (size_t j = 0; j < length; j++) {
                                if (ref[j] == b) {
                                        count++;
                                }
                        }
                        if (count > 1) {
                                return true;
                        }
                }
                return false;
        }

        /*! \brief Diese Methode `SetLabelKey()` setzt den Endemarkenblock auf einen neuen Wert.
         *
         * Das übergebene 256-Byte-Datenfeld muss jeden Byte-Wert genau einmal enthalten (d. h., alle Datenbytes müssen unterschiedlich sein).
         * Ist dies nicht der Fall oder entspricht die Länge des Datenfelds nicht 256, wird _false_ zurückgegeben und der Endemarkenblock bleibt unverändert.
         *
         * Für die in dieser Klasse beschriebene Beispielverschlüsselung wird der Endemarkenblock auf die nachfolgenden 256 Byte gesetzt.
         *
         *      Index  Inhalt (hexadezimal)                             Inhalt (ASCII)
         *          0  c4 0a ed 09 2c a9 c6 b2 f2 52 6f 28 e6 32 f4 45  ....,....Ro(.2.E
         *         16  24 23 53 e1 59 ea 12 cd 5b 4b ee a4 d9 ba c0 93  $#S.Y...[K......
         *         32  6a 7f 27 df e9 99 dd 81 ca 83 80 15 2d dc 79 c3  j.'.........-.y.
         *         48  e8 11 b8 3f 1d a1 1b 75 49 cf b5 17 29 67 fb 86  ...?...uI...)g..
         *         64  03 e0 20 90 be 8c cb 63 c7 bc d8 a2 0e 21 7d 04  .. ....c.....!}.
         *         80  85 40 51 ae 16 96 36 bf ad b4 9a 8b 9f 1f 2f 26  .@Q...6......./&
         *         96  37 02 65 0b a5 2b 18 ff 1e b3 a7 87 8f 7a e3 7e  7.e..+.......z.~
         *        112  4a db 4f 78 aa 5e fd 46 d4 33 94 01 5f eb ac 4d  J.Ox.^.F.3.._..M
         *        128  14 de 1a 0f b9 68 31 d5 e4 91 ce 8e b1 3d 92 84  .....h1......=..
         *        144  d0 43 48 fa 55 42 8d 62 08 a3 0d 4c 76 5a b6 d2  .CH.UB.b...LvZ..
         *        160  3e c8 57 7b cc f7 a6 77 38 fc 88 6d c5 f3 5c e7  >.W{...w8..m..\.
         *        176  c9 f1 b0 44 d3 3c 98 69 10 7c 71 64 8a f6 4e 95  ...D.<.i.|qd..N.
         *        192  f0 97 30 66 3a d1 6b f8 da a8 ab 6e 0c 82 50 39  ..0f:.k....n..P9
         *        208  9d ec 6c b7 9b 9e 58 d6 22 06 35 a0 00 5d ef 56  ..l...X.".5..].V
         *        224  af 61 3b 47 bd f9 fe e5 74 25 70 2e 60 bb 13 54  .a;G....t%p....T
         *        240  07 41 1c f5 72 9c 2a c1 34 c2 e2 89 05 d7 19 73  .A..r.*.4......s
         *
         * \param ref Verweis auf eine ByteArray-Instanz, die das zu übernehmende Datenfeld enthält.
         * \return _true_, wenn alles funktioniert hat.
         */
        bool Secure::SetLabelKey(ByteArray &ref) {
                void *p = &ref;
                if ((p == nullptr) || (ref.Size() != 256)) {
                        return false;
                } else {
                        if (IsByteMultipleInArray(ref)) {
                                return false;
                        }
                        labelKey->Copy(ref, 0, 0, 256);
                        return true;
                }
        }

        /*! \brief Diese Methode `SetOneTimePadKey()` setzt den OneTimePadKey auf einen neuen Wert.
         *
         * Für die in dieser Klasse beschriebene Beispielverschlüsselung wird der OneTimePadKey auf die nachfolgenden 24 Byte gesetzt.
         *
         *      Index  Inhalt (hexadezimal)                             Inhalt (ASCII)
         *          0  3a fb 9b 2f 1a 6f a1 92 9b 1a b9 b2 00 08 e2 35  :../.o.........5
         *         16  af f6 b9 40 df d9 6b 58                          ...@..kX
         *
         * \param ref Verweis auf eine ByteArray-Instanz, die das zu übernehmende Datenfeld enthält.
         * \return _true_, wenn alles funktioniert hat.
         */
        bool Secure::SetOneTimePadKey(ByteArray &ref) {
                void *p = &ref;
                if (p == nullptr) {
                        return false;
                } else {
                        oneTimePadKey->Copy(ref, 0, 0, ref.Size());
                        oneTimePadKey->SetSize(ref.Size());
                        return true;
                }
        }

        /*! \brief Diese Methode `SetPermutationKey()` setzt den Vertauschungsschlüssel auf einen neuen Wert.
         *
         * Das übergebene 256-Byte-Datenfeld muss jeden Byte-Wert genau einmal enthalten (d. h., alle Datenbytes müssen unterschiedlich sein).
         * Ist dies nicht der Fall oder entspricht die Länge des Datenfelds nicht 256, wird _false_ zurückgegeben und der Vertauschungsschlüssel bleibt unverändert.
         *
         * Für die in dieser Klasse beschriebene Beispielverschlüsselung wird der Vertauschungsschlüssel auf die nachfolgenden 256 Byte gesetzt.
         *
         *      Index  Inhalt (hexadezimal)                             Inhalt (ASCII)
         *          0  56 9f 6b ba da 4f 4c d0 89 e7 7b 10 5e f5 1c 46  V.k..OL...{.^..F
         *         16  b2 0b cb b8 3f e0 6d 4e 5d fa c8 11 eb ca d5 a4  ....?.mN].......
         *         32  a8 5c 02 1e 62 d4 49 bf f9 dc 79 94 91 31 66 a5  .\..b.I...y..1f.
         *         48  6e a9 ad 34 15 a6 f0 ff 44 0a 14 26 80 71 0e 72  n..4....D..&.q.r
         *         64  b6 c7 65 3c 4d bd f8 b9 b5 41 18 2f 69 8d f1 36  ..e<M....A./i..6
         *         80  f4 00 ac 21 e6 55 5f af 09 4b 87 a3 df 6a 3e d9  ...!.U_..K...j>.
         *         96  28 47 60 cd d1 98 dd 8b 1a 61 2c 9e 5b 86 2d fe  (G.......a,.[.-.
         *        112  b3 53 78 b4 81 a2 b1 6c 48 9a 8a 3b 88 25 07 8e  .Sx....lH..;.%..
         *        128  68 e1 6f 7c 33 ee 1d a1 03 db 77 5a cf 52 08 82  h.o|3.....wZ.R..
         *        144  fd 12 2a cc 3d ea e9 2e e2 aa 24 bc 43 3a 0f 01  ..*.=.....$.C:..
         *        160  d8 ce 95 e5 29 7a 90 05 d2 63 97 de 30 4a 20 58  ....)z...c..0J X
         *        176  19 76 42 b7 e8 9c 74 8c f3 7d a0 99 e3 13 c2 c1  .vB...t..}......
         *        192  59 93 9b 84 c3 ec 64 d7 f7 f2 fb d3 ef 9d a7 f6  Y.....d.........
         *        208  c9 70 22 fc 32 40 c4 45 17 75 ae 2b bb 96 38 50  .p".2@.E.u.+..8P
         *        224  27 be 92 ab c6 04 1f 35 e4 c5 51 0d 0c 7f 85 d6  '......5..Q.....
         *        240  73 23 83 7e 8f 67 54 b0 ed 06 37 c0 16 57 39 1b  s#.~.gT...7..W9.
         *
         * \param ref Verweis auf eine ByteArray-Instanz, die das zu übernehmende Datenfeld enthält.
         * \return _true_, wenn alles funktioniert hat.
         */
        bool Secure::SetPermutationKey(ByteArray &ref) {
                void *p = &ref;
                if ((p == nullptr) || (ref.Size() != 256)) {
                        return false;
                } else {
                        if (IsByteMultipleInArray(ref)) {
                                return false;
                        }
                        permutationKey->Copy(ref, 0, 0, 256);
                        return true;
                }
        }

        /*! \brief Diese Methode `SetRedundancy()` setzt die Redundanzdaten auf einen neuen Wert.
         *
         * Für die in dieser Klasse beschriebene Beispielverschlüsselung werden die Redundanzdaten auf die nachfolgenden 24 Byte gesetzt.
         *
         *      Index  Inhalt (hexadezimal)                             Inhalt (ASCII)
         *          0  22 40 07 8d 3a 2f c1 3a da 3d 2a 0a e3 fa 3e c1  "@..:/.:.=*...>.
         *         16  0b e7 da ce ae 5e 1a 62                          .....^.b
         *
         * \param ref Verweis auf eine ByteArray-Instanz, die das zu übernehmende Datenfeld enthält.
         * \return _true_, wenn alles funktioniert hat.
         */
        bool Secure::SetRedundanz(ByteArray &ref) {
                void *p = &ref;
                if (p == nullptr) {
                        return false;
                } else {
                        redundanz->Copy(ref, 0, 0, ref.Size());
                        redundanz->SetSize(ref.Size());
                        return true;
                }
        }

        /*! \brief Diese Methode `TestAndDeleteRedundancy()` prüft einen 256-Byte-Redundanzdatenblock und speichert die gültigen Datenbytes in einem Zieldatenfeld.
         *
         * Bei dieser Methode wird zunächst in einem 256-Byte-Quellfeld nach der Endemarke gesucht.
         * Anschließend werden die Benutzerdaten und die Endmarkierung in einen 256-Byte-Kontrolldatenblock kopiert.
         * Dann wird im Kontrolldatenblock die Redundanz neu berechnet.
         * Stimmen das Quelldatenfeld und der Kontrolldatenblock überein, werden die Benutzerdaten in das Zieldatenfeld (ab dem Schreibindex) übertragen
         * und der neue Schreibindex wird berechnet.
         *
         * \param source Verweis auf eine ByteArray-Instanz, die ein 256 Byte langes Quelldatenfeld enthält.
         * \param target Verweis auf eine ByteArray-Instanz, in die die Nutzdatenbytes kopiert werden sollen.
         * \param index Aktuelle Schreibposition im Zieldatenfeld.
         * \param arrayNumber Bytewert (zwischen 0 und 255) der die Endmarkierung darstellt.
         * \return _true_, wenn keine Fehler festgestellt wurden (ansonsten _false_).
         */
        bool Secure::TestAndDeleteRedundancy(ByteArray &source, ByteArray &target, size_t &index, unsigned char arrayNumber) {
                size_t endIndex = 256;
                for (size_t i = 0; i < 129; i++) {
                        if (source[i] == (*labelKey)[arrayNumber]) {
                                endIndex = i;
                                break;
                        }
                }
                if (endIndex == 256) {
                        return false;
                }
                ByteArray a(256, true);
                a.SetSize(256);
                for (size_t i = 0; i <= endIndex; i++) {
                        a[i] = source[i];
                }
                InsertRedundancy(a, endIndex + 1);
                for (size_t i = 0; i < 256; i++) {
                        if (source[i] != a[i]) {
                                return false;
                        }
                }
                if (endIndex > 0) {
                        target.Copy(source, 0, index, endIndex);
                        index = index + endIndex;
                }
                return true;
        }


        /*! \brief Konstruktor: Initialisiert eine neue Instanz der Klasse __SHA384__.
         *
         * Die SHA384-Instanz stellt eine Schnittstelle zum SHA384-Hash-Algorithmus in der OpenSSL-Bibliothek bereit.
         */
        SHA384::SHA384() {
                evp_md = EVP_MD_fetch(NULL, "SHA384", NULL);
                evp_md_ctx = EVP_MD_CTX_new();
                hash = new ByteArray(48, true);
        }

        /*! \brief Destruktor: Zerstört die existierende Instanz der Klasse __SHA384__.
         *
         *  Der Destruktor löscht alle sensiblen Daten aus dieser SHA384-Instanz bevor der Heapspeicher
         *  wieder an das Betriebssystem zurück gegeben wird.
         */
        SHA384::~SHA384() {
                delete hash;
                EVP_MD_CTX_free(evp_md_ctx);
                EVP_MD_free(evp_md);
        }

        /*! \brief Dise Methode `AddHash()` erzeugt einen Hash-Wert und hängt diesen an einen Datenblock an.
         *
         * Die Methode erzeugt aus dem übergebenen Datenblock mithilfe des SHA384-Algorithmus einen 48 Byte langen Hashwert
         * und hängt diesen anschließend an das Ende des Datenblocks an.
         *
         * \param ref Verweis auf eine ByteArray-Instanz.
         */
        void SHA384::AddHash(ByteArray &ref) {
                size_t lengthA = ref.Size();
                EVP_DigestInit(evp_md_ctx, evp_md);
                EVP_DigestUpdate(evp_md_ctx, &ref[0], lengthA);
                EVP_DigestFinal(evp_md_ctx, reinterpret_cast<unsigned char*>(&ref.WriteareaReferenz(lengthA, 48)), nullptr);
        }

        /*! \brief Diese Methode `GetHash()` erzeugt aus einem Datenblock einen Hash-Wert und trägt diesen in einen anderen Datenblock ein.
         *
         * Die Methode erzeugt anhand des SHA384-Algorithmus einen 48 Byte langen Hashwert aus dem ersten übergebenen Datenblock
         * und fügt diesen Hashwert anschließend in den zweiten Datenblock ein.
         *
         * \param ref Verweis auf eine ByteArray-Instanz, aus der der Hash-Wert ermittelt werden soll.
         * \param Hash Verweis auf eine ByteArray-Instanz, in die der ermittelte Hash-Wert geschrieben werden soll.
         */
        void SHA384::GetHash(ByteArray &ref, ByteArray &Hash) {
                EVP_DigestInit(evp_md_ctx, evp_md);
                EVP_DigestUpdate(evp_md_ctx, &ref[0], ref.Size());
                EVP_DigestFinal(evp_md_ctx, reinterpret_cast<unsigned char*>(&Hash.WriteareaReferenz(0, 48)), nullptr);
        }

        /*! \brief Diese Methode `TestHash()` prüft in einem Datenblock mit angehängtem Hash-Wert, ob der Hashwert der Daten und der angehängte Hash-Wert übereinstimmen.
         *
         * Die Methode generiert aus dem übergebenen Datenblock (abzüglich der letzten 48 Bytes) einen 48 Byte langen Hashwert gemäß dem SHA384-Algorithmus.
         * Anschließend wird geprüft, ob dieser ermittelte Hash-Wert mit den letzten 48 Bytes im Datenblock übereinstimmt.
         *
         * \param ref Verweis auf eine ByteArray-Instanz mit den Daten und dem zugehörigen Hash-Wert.
         * \return _true_, wenn der Hashwert der Daten und der angehängte Hashwert übereinstimmen.
         */
        bool SHA384::TestHash(ByteArray &ref) {
                size_t lengthA = ref.Size() - 48;
                EVP_DigestInit(evp_md_ctx, evp_md);
                EVP_DigestUpdate(evp_md_ctx, &ref[0], lengthA);
                EVP_DigestFinal(evp_md_ctx, reinterpret_cast<unsigned char*>(&hash->WriteareaReferenz(0, 48)), nullptr);
                return hash->IsEqual(ref, lengthA, 0, 48);
        }

        /*! \brief Diese Methode `TestHash()` ermittelt einen Hashwert aus dem ersten Datenblock und prüft, ob dieser mit dem zweiten Datenblock übereinstimmt.
         *
         * Die Methode generiert anhand des SHA384-Algorithmus einen 48 Byte langen Hashwert aus dem ersten übergebenen Datenblock.
         * Anschließend wird geprüft, ob dieser ermittelte Hashwert mit dem zweiten Datenblock übereinstimmt.
         *
         * \param ref Verweis auf eine ByteArray-Instanz mit den Daten.
         * \param Hash Verweis auf eine ByteArray-Instanz, die den Hash-Wert enthält.
         * \return _true_, wenn der Hashwert der Daten und der übergebene Hashwert übereinstimmen.
         */
        bool SHA384::TestHash(ByteArray &ref, ByteArray &Hash) {
                EVP_DigestInit(evp_md_ctx, evp_md);
                EVP_DigestUpdate(evp_md_ctx, &ref[0], ref.Size());
                EVP_DigestFinal(evp_md_ctx, reinterpret_cast<unsigned char*>(&hash->WriteareaReferenz(0, 48)), nullptr);
                return hash->IsEqual(Hash, 0, 0, 48);
        }



        /*! \brief Konstruktor: Initialisiert eine neue Instanz der Klasse __SHA512__.
         *
         * Die SHA512-Instanz stellt eine Schnittstelle zum SHA512-Hash-Algorithmus in der OpenSSL-Bibliothek bereit.
         */
        SHA512::SHA512() {
                evp_md = EVP_MD_fetch(NULL, "SHA512", NULL);
                evp_md_ctx = EVP_MD_CTX_new();
                hash = new ByteArray(64, true);
        }

        /*! \brief Destruktor: Zerstört die existierende Instanz der Klasse __SHA512__.
         *
         *  Der Destruktor löscht alle sensiblen Daten aus dieser SHA512-Instanz bevor der Heapspeicher
         *  wieder an das Betriebssystem zurück gegeben wird.
         */
        SHA512::~SHA512() {
                delete hash;
                EVP_MD_CTX_free(evp_md_ctx);
                EVP_MD_free(evp_md);
        }

        /*! \brief Dise Methode `AddHash()` erzeugt einen Hash-Wert und hängt diesen an einen Datenblock an.
         *
         * Die Methode erzeugt aus dem übergebenen Datenblock mithilfe des SHA512-Algorithmus einen 64 Byte langen Hashwert
         * und hängt diesen anschließend an das Ende des Datenblocks an.
         *
         * \param ref Verweis auf eine ByteArray-Instanz.
         */
        void SHA512::AddHash(ByteArray &ref) {
                size_t lengthA = ref.Size();
                EVP_DigestInit(evp_md_ctx, evp_md);
                EVP_DigestUpdate(evp_md_ctx, &ref[0], lengthA);
                EVP_DigestFinal(evp_md_ctx, reinterpret_cast<unsigned char*>(&ref.WriteareaReferenz(lengthA, 64)), nullptr);
        }

        /*! \brief Diese Methode `GetHash()` erzeugt aus einem Datenblock einen Hash-Wert und trägt diesen in einen anderen Datenblock ein.
         *
         * Die Methode erzeugt anhand des SHA512-Algorithmus einen 64 Byte langen Hashwert aus dem ersten übergebenen Datenblock
         * und fügt diesen Hashwert anschließend in den zweiten Datenblock ein.
         *
         * \param ref Verweis auf eine ByteArray-Instanz, aus der der Hash-Wert ermittelt werden soll.
         * \param Hash Verweis auf eine ByteArray-Instanz, in die der ermittelte Hash-Wert geschrieben werden soll.
         */
        void SHA512::GetHash(ByteArray &ref, ByteArray &Hash) {
                EVP_DigestInit(evp_md_ctx, evp_md);
                EVP_DigestUpdate(evp_md_ctx, &ref[0], ref.Size());
                EVP_DigestFinal(evp_md_ctx, reinterpret_cast<unsigned char*>(&Hash.WriteareaReferenz(0, 64)), nullptr);
        }

        /*! \brief Diese Methode `TestHash()` prüft in einem Datenblock mit angehängtem Hash-Wert, ob der Hashwert der Daten und der angehängte Hash-Wert übereinstimmen.
         *
         * Die Methode generiert aus dem übergebenen Datenblock (abzüglich der letzten 64 Bytes) einen 64 Byte langen Hashwert gemäß dem SHA512-Algorithmus.
         * Anschließend wird geprüft, ob dieser ermittelte Hash-Wert mit den letzten 64 Bytes im Datenblock übereinstimmt.
         *
         * \param ref Verweis auf eine ByteArray-Instanz mit den Daten und dem zugehörigen Hash-Wert.
         * \return _true_, wenn der Hashwert der Daten und der angehängte Hashwert übereinstimmen.
         */
        bool SHA512::TestHash(ByteArray &ref) {
                size_t lengthA = ref.Size() - 64;
                EVP_DigestInit(evp_md_ctx, evp_md);
                EVP_DigestUpdate(evp_md_ctx, &ref[0], lengthA);
                EVP_DigestFinal(evp_md_ctx, reinterpret_cast<unsigned char*>(&hash->WriteareaReferenz(0, 64)), nullptr);
                return hash->IsEqual(ref, lengthA, 0, 64);
        }

        /*! \brief Diese Methode `TestHash()` ermittelt einen Hashwert aus dem ersten Datenblock und prüft, ob dieser mit dem zweiten Datenblock übereinstimmt.
         *
         * Die Methode generiert anhand des SHA512-Algorithmus einen 64 Byte langen Hashwert aus dem ersten übergebenen Datenblock.
         * Anschließend wird geprüft, ob dieser ermittelte Hashwert mit dem zweiten Datenblock übereinstimmt.
         *
         * \param ref Verweis auf eine ByteArray-Instanz mit den Daten.
         * \param Hash Verweis auf eine ByteArray-Instanz, die den Hash-Wert enthält.
         * \return _true_, wenn der Hashwert der Daten und der übergebene Hashwert übereinstimmen.
         */
        bool SHA512::TestHash(ByteArray &ref, ByteArray &Hash) {
                EVP_DigestInit(evp_md_ctx, evp_md);
                EVP_DigestUpdate(evp_md_ctx, &ref[0], ref.Size());
                EVP_DigestFinal(evp_md_ctx, reinterpret_cast<unsigned char*>(&hash->WriteareaReferenz(0, 64)), nullptr);
                return hash->IsEqual(Hash, 0, 0, 64);
        }

} // end of namespace RH
