/*
HelperClasses.dll, a collection of useful classes and routines.
Copyright (C) 2023 - 2026    Reinhard Hölscher

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

/*! \file securefile.h
 *  \brief Stellt die Schnittstelle für die Klasse __SecureFile__ bereit.
 *
 * Die Klasse __SecureFile__ verwaltet Datenfelder beliebiger Länge doppelt verschlüsselt in einer Datei.
 *
 * Von einem Datenfeld dass im SecureFile gespeichert und verwaltet werden soll, wird zunächst der SHA512-Hashwert ermittelt und an das Datenfeld angehängt.
 * Anschliedend wird das Datenfeld mit dem angehängten Hashwert zweifach mit der AES-Verfahren (mit unterschiedlichen Schlüsseln) verschlüsselt und unter
 * einer eindeutigen Indexnummer im SecureFile gespeichert.
 *
 * Mit der Methode `Put()` kann ein Datenfeld dem SecureFile hinzugefügt werden. Die Methode `Put()` liefert (bei Erfolg) eine eindeutige Indexnummer zurück.
 *
 * Mit der Methode `Get()` kann (unter der Angabe der Indexnummer) ein Datenfeld (im Klartext) aus dem SecureFile zurück erlangt werden.
 *
 * Die Metode `Delete()` löscht ein Datenfeld (unter der Angabe der Indexnummer) aus dem SecureFile.
 *
 * Mit der Methode `NewStore()` kann ein neues SecureFile in einer noch nicht vorhandenen Datei angelegt werden. Bei der Neuanlage werden zwei
 * Initialisierungsvektoren (IV) und zwei Schlüssel festgelegt, mit denen das SecureFile arbeiten soll.
 *
 * Mit der Methode `Open()` kann ein vorhandenes SecureFile geöffnet werden. Hierbei müssen die beiden Initialisierungsvektoren und Schlüssel angegeben
 * werden, die bei der Neuanlage des SecureFiles festgelegt wurden. Werden falsche Werte angegeben schlägt der Open-Prozess fehl. Die Methoden `Put()`,
 * `Get()` und `Delete()` funktionieren nur in einem korrekt geöffneten SecureFile.
 *
 * Mit der Methode `Close()` kann ein SecureFile wieder geschlossen werden.
 */


#ifndef CONTAINER_SECURECONTAINER_H
#define CONTAINER_SECURECONTAINER_H
#include "HelperClasses_global.h"
#include "basicrow.h"
#include "basictable.h"
#include "secure.h"
#include "QFile"
using namespace  Table;
using namespace  RH;

namespace Store {


        /*! \brief Die Klassse __IndexFree__ verwaltet die freien Indexwerte im SecureFile.
         *
         * Eine Instanz dieser Klasse __IndexFree__ verwaltet einen nicht zugewiesenen Indexwert, der größer als 1023 und kleiner als der größte zugewiesene Indexwert im SecureFile ist.
         * Ein nicht zugewiesener Indexwert entsteht bei der Löschung eines Datenfeldes aus dem SecureFile.
         */
        class IndexFree : public BasicRow {
        public:
                /*! \brief Enthält eine derzeit nicht zugewiesene Indexnumber.
                 *
                 * Indexnummer (1024 .. n), die derzeit nicht vergeben ist und kleiner ist als die größte vergebene Indexnummer.
                 * (Die Variable _indexNumber_ bildet den Schlüssel Nr. 0 in der Verwaltungstabelleninstanz _IndexFreeTable_).
                 */
                size_t indexNumber;
                IndexFree(BasicTable &table, const bool isSearchInfo);
                virtual ~IndexFree() override;
        protected:
                virtual BasicRow *CloneThis() const override;
                virtual void CopyToDestination(BasicRow &ref) const override;
                virtual void ExportToByteArray(ByteArray &ref) const override;
                virtual void ImportFromByteArray(ByteArray &ref) override;
                virtual bool IsEqualKey(BasicRow &row, const size_t keyIndex) const override;
                virtual bool IsLessKey(BasicRow &row, const size_t keyIndex) const override;
        };


        /*! \brief Die Klassse __IndexFreeTable__ verwaltet die freien Indexwerte im SecureFile.
         *
         * Eine Instanz dieser Klasse verwaltet die freien Indexwerte im SecureFile und hält sie sortiert (unter dem Schlüssel 0).
         */
        class IndexFreeTable : public BasicTable {
        public:
                IndexFreeTable();
                virtual ~IndexFreeTable() override;
                IndexFree *Row();
                IndexFree *SearchRow();
        protected:
                virtual void NewRow() override;
        };



        /*! \brief Die Klassse __Items__ beschreibt einen verschlüsselten ByteArray-Bereich im SecureFile.
         *
         * Eine Instanz dieser Klasse __Items__ beschreibt einen verschlüsselten ByteArray-Bereich
         * in der Datei (die Größe des Arrays in Byte, die in der Datei belegt ist; die dem Datensatz zugewiesene Indexnummer sowie den Dateiindex, an dem sich der Datensatz befindet).
         */
        class Items : public BasicRow {
        public:
                /*! \brief Länge der verschlüsselten ByteArray-Instanz.
                 *
                 * Länge des verschlüsselten Datensatzes (ByteArray) in Bytes
                 */
                size_t arrayLength;
                /*! \brief Byteindex in der Datei, ab dem der Datensatz gespeichert wird.
                 *
                 * Byte-Index in der Datei, ab dem der verschlüsselte Datensatz gespeichert ist.
                 * (_fileIndex_ bildet den Schlüssel Nr. 1 in der Verwaltungstabelleninstanz).
                 */
                size_t fileIndex;
                /*! \brief Eindeutige Nummer unter der ein Datensatz verwaltet wird
                 *
                 * Indexnummer (257 .. n), die diesem verschlüsselten Datensatz zugewiesen wurde und unter der er aus dem SecureFile (im Klartext) ausgelesen werden kann.
                 * (Die _indexNumber_ bildet den Schlüssel Nr. 0 in der Verwaltungstabelleninstanz).
                 */
                size_t indexNumber;
                Items(BasicTable &table, const bool isSearchInfo);
                virtual ~Items() override;
        protected:
                virtual BasicRow *CloneThis() const override;
                virtual void CopyToDestination(BasicRow &ref) const override;
                virtual void ExportToByteArray(ByteArray &ref) const override;
                virtual void ImportFromByteArray(ByteArray &ref) override;
                virtual bool IsEqualKey(BasicRow &row, const size_t keyIndex) const override;
                virtual bool IsLessKey(BasicRow &row, const size_t keyIndex) const override;
        };



        /*! \brief Die Klassse __ItemTable__ verwaltet die verschlüsselten Datenbereiche im SecureFile.
         *
         * Eine Instanz dieser Klasse verwaltet die verschlüsselten Datenbereiche im SecureFile sortiert nach den Variablen _indexNumber_ (Schlüssel 0) und _arraLength_ (Schlüssel 1).
         */
        class ItemTable : public BasicTable {
        public:
                ItemTable();
                virtual ~ItemTable() override;
                Items *Row();
                Items *SearchRow();
        protected:
                virtual void NewRow() override;
        };




        /*! \brief Die Klasse __SecureFile__ verwaltet Datenfelder beliebiger Länge doppelt verschlüsselt in einer Datei.
         *
         * Von einem Datenfeld dass im SecureFile gespeichert und verwaltet werden soll, wird zunächst der SHA512-Hashwert ermittelt und an das Datenfeld angehängt.
         * Anschliedend wird das Datenfeld mit dem angehängten Hashwert zweifach mit der AES-Verfahren (mit unterschiedlichen Schlüsseln) verschlüsselt und unter
         * einer eindeutigen Indexnummer im SecureFile gespeichert.
         *
         * Mit der Methode `Put()` kann ein Datenfeld dem SecureFile hinzugefügt werden. Die Methode `Put()` liefert (bei Erfolg) eine eindeutige Indexnummer zurück.
         *
         * Mit der Methode `Get()` kann (unter der Angabe der Indexnummer) ein Datenfeld (im Klartext) aus dem SecureFile zurück erlangt werden.
         *
         * Die Metode `Delete()` löscht ein Datenfeld (unter der Angabe der Indexnummer) aus dem SecureFile.
         *
         * Mit der Methode `NewStore()` kann ein neues SecureFile in einer noch nicht vorhandenen Datei angelegt werden. Bei der Neuanlage werden zwei
         * Initialisierungsvektoren (IV) und zwei Schlüssel festgelegt, mit denen das SecureFile arbeiten soll.
         *
         * Mit der Methode `Open()` kann ein vorhandenes SecureFile geöffnet werden. Hierbei müssen die beiden Initialisierungsvektoren und Schlüssel angegeben
         * werden, die bei der Neuanlage des SecureFiles festgelegt wurden. Werden falsche Werte angegeben schlägt der Open-Prozess fehl. Die Methoden `Put()`,
         * `Get()` und `Delete()` funktionieren nur in einem korrekt geöffneten SecureFile.
         *
         * Mit der Methode `Close()` kann ein SecureFile wieder geschlossen werden.
         */
        class HELPERCLASSES_EXPORT SecureFile {
        public:
                SecureFile();
                virtual ~SecureFile();
                size_t ClearAll();
                size_t Close();
                size_t Count() const;
                size_t Delete(const size_t indexNumber);
                bool DumpRecord(QString &pathName, size_t indexNumber);
                bool DumpStruct(QString &pathName);
                size_t First() const;
                ByteArray *Get(const size_t indexNumber, size_t &error);
                void Get(ByteArray &ref, const size_t indexNumber, size_t &error);
                bool isStoreOpen() const;
                size_t NewStore(QString *name, ByteArray *iv1, ByteArray *key1, ByteArray *iv2, ByteArray *key2);
                size_t Next() const;
                size_t Open(QString *name, ByteArray *iv1, ByteArray *key1, ByteArray *iv2, ByteArray *key2);
                size_t Put(ByteArray &ref, const size_t storeIndex, size_t &error);
        private:
                /*!
                 * \brief Zeiger auf eine Instanz der Klasse _AES_, die Verschlüsselung und Entschlüsselung gemäß dem AES-Standard ermöglicht.
                 *
                 * Diese AES-Instanz arbeitet bei der Ver- und Entschlüsselung mit dem ersten übergebenen Initialisierungsvektor
                 * und dem ersten übergebenen Schlüssel.
                 */
                AES *aes1;
                /*!
                 * \brief Zeiger auf eine Instanz der Klasse _AES_, die Verschlüsselung und Entschlüsselung gemäß dem AES-Standard ermöglicht.
                 *
                 * Diese AES-Instanz arbeitet bei der Ver- und Entschlüsselung mit dem zweiten übergebenen Initialisierungsvektor
                 * und dem zweiten übergebenen Schlüssel.
                 */
                AES *aes2;
                /*!
                 * \brief Zeiger auf eine Instanz der Klasse _AES_, die Verschlüsselung und Entschlüsselung gemäß dem AES-Standard ermöglicht.
                 *
                 * Diese AES-Instanz arbeitet bei der Ver- und Entschlüsselung mit dem ersten Initialisierungsvektor aus dem SHA512-Hash-Wert
                 * und dem ersten Schlüssel aus dem SHA512-Hash-Wert.
                 */
                AES *aes3;
                /*!
                 * \brief Zeiger auf eine Instanz der Klasse _AES_, die Verschlüsselung und Entschlüsselung gemäß dem AES-Standard ermöglicht.
                 *
                 * Diese AES-Instanz arbeitet bei der Ver- und Entschlüsselung mit dem zweiten Initialisierungsvektor aus dem SHA384-Hash-Wert
                 * und dem zweiten Schlüssel aus dem SHA384-Hash-Wert.
                 */
                AES *aes4;
                /*!
                 * \brief Zeiger auf eine ByteArray-Instanz zur Überprüfung der übergebenen Initialisierungsvektoren und Schlüssel beim Öffnen des SecureFiles.
                 *
                 * Bei der Neuanlage eines SecureFiles wird diese ByteArray-Instanz mit 256 zufälligen Bytes gefüllt.
                 * Dann werden die Zufallsbytes durch einen angehängten Hash-Wert (SHA512) vor nachträglichen Manipulationen geschützt.
                 * Mit dem übergebenen ersten Initialisierungsvektor und dem ersten Schlüssel werden die Zufallsbytes mitsamt dem angehängten Hashwert (320 Bytes) nun
                 * mit dem AES-Algoritmus zum ersten mal verschlüsselt. Das ByteArray ist nun 336 Bytes lang.
                 * Mit dem übergebenen zweiten Initialisierungsvektor und dem zweiten Schlüssel werden diese 336 Bytes erneut mit dem AES-Algoritmus verschlüsselt.
                 * Die so entstandenen 352 Bytes werden am Anfang der Datei des SecureFiles gespeichert.
                 *
                 * Nur wer die richtigen Initialisierungsvektoren und Schlüssel kennt, kann das SecureFile wieder öffnen.
                 */
                ByteArray *checkArray1;
                /*!
                 * \brief Zeiger auf eine ByteArray-Instanz zur Überprüfung der übergebenen Initialisierungsvektoren und Schlüssel beim Öffnen des SecureFiles.
                 *
                 * Bei der Neuanlage eines SecureFiles wird diese ByteArray-Instanz mit den 256 Zufallsbytes aus der ByteArray-Instanz _checkArray1_ gefüllt.
                 * Dann werden die Zufallsbytes durch einen angehängten Hash-Wert (SHA512) vor nachträglichen Manipulationen geschützt.
                 * Außerdem wird ein SHA384-Hash-Wert von den 256 Zufallsbytes ermittelt.
                 * Aus dem SHA512-Hash-Wert werden die ersten 16 Byte als erster Initialisierungsvektor verwendet.
                 * Die nächsten 32 Byte werden als erster Schlüssel verwendet. Die restlichen 16 Byte des SHA512-Hash-Wert bleiben ungenutzt.
                 * Mit dem ersten Initialisierungsvektor und dem ersten Schlüssel werden die Zufallsbytes mitsamt dem angehängten Hashwert (320 Bytes) nun
                 * mit dem AES-Algoritmus zum ersten mal verschlüsselt. Das ByteArray ist nun 336 Bytes lang.
                 * Als zweiter Initialisierungsvektor werden die ersten 16 Byte des SHA384-Hash-Wertes benutzt.
                 * Die nächsten 32 Byte werden als zweiter Schlüssel verwendet.
                 * Mit dem zweiten Initialisierungsvektor und dem zweiten Schlüssel werden diese 336 Bytes erneut mit dem AES-Algoritmus verschlüsselt.
                 * Die so entstandenen 352 Bytes werden ab dem Index 352 in der Datei des SecureFiles gespeichert.
                 *
                 * Nur wer die richtigen Initialisierungsvektoren und Schlüssel kennt, kann das SecureFile wieder öffnen.
                 */
                ByteArray *checkArray2;
                /*!
                 * \brief Zeiger auf eine ByteArray-Instanz mit Verwaltungsinformationen zum SecureFile.
                 *
                 * In diesem Block werden die Startadressen und Längen der Verwaltungstabellen in verschlüsselter Form gespeichert.
                 * Im Klartext ist dieser Block 16 Byte lang. Mit angehängtem Hash-Wert und doppelter AES-Verschlüsselung ist er 112 Byte lang.
                 * Der verschlüsselte Block wird ab der Indexposition 704 in der Datei des SecureFiles gespeichert.
                 */
                ByteArray *controlArray;
                /*!
                 * \brief Enthält einen Zeiger auf eine QFile-Instanz für die Ein- und Ausgabe in die Verwaltungsdatei.
                 *
                 * In dieser Datei werden alles Informationen dopppelt verschlüsselt gespeichert.
                 */
                QFile *file0;
                /*!
                 * \brief Enthält einen Zeiger auf eine Tabelleninstanz (der Klasse __FreeIndexTable__) zur Verwaltung der nicht zugewiesenen Indexwerte.
                 *
                 * Hier werden die nicht zugewiesenen Indexwerte verwaltet, der größer als 1024 und kleiner als der höchste zugewiesene Indexwert sind.
                 */
                IndexFreeTable *freeIndexes;
                /*!
                 * \brief Länge der ByteArray-Instanz, die die exportierten und verschlüsselten Daten der Tabellen-Instanz __FreeIndexTable__ enthält.
                 *
                 * Die Minimallänge beträgt 96 Bytes.
                 */
                size_t freeIndexesLng;
                /*!
                 * \brief Enthält einen Zeiger auf eine Tabelleninstanz (der Klasse __ItemTable__) zur Verwaltung der Nutzerdatenblöcke.
                 *
                 * In dieser Tabelle werden die im Securestore gespeicherten Datenfelder mit ihrer Indexnummer und ihrer Lage in der Datei verwaltet.
                 */
                ItemTable *items;
                /*!
                 * \brief Länge der ByteArray-Instanz, die die exportierten und verschlüsselten Daten der Tabellen-Instanz __ItemTable__ enthält.
                 *
                 * Die Minimallänge beträgt 96 Bytes.
                 */
                size_t itemsLng;
                /*!
                 * \brief Enthält die nächste freie Indexnummer größer als 1023, die vergeben werden kann.
                 * \sa Beispielausdruck der Methode `DumpStruct()`
                 */
                size_t nextFreeIndex;
                /*!
                 * \brief _true_, wenn das SecureFile erfolgreich eröffnet wurde. _false_, wenn das SecureFile geschlossen ist.
                 * \sa Open() und Close()
                 */
                bool openFlag;
                /*!
                 * \brief Zeiger auf eine Instanz der Klasse SHA384, die eine Hash-Methode mit einem 48 Byte langen Hash-Code bereitstellt.
                 *
                 * Diese Instanz wird in den Methoden Open() und NewStore() benötigt und benutzt.
                 */
                SHA384 *sha384;
                /*!
                 * \brief Zeiger auf eine Instanz der Klasse SHA512, die eine Hash-Methode mit einem 64 Byte langen Hash-Code bereitstellt.
                 *
                 * Diese Instanz wird in zahlreichen Methoden benötigt und benutzt.
                 */
                SHA512 *sha512;
                /*!
                 * \brief Byte-Index in der Datei, bis zu dem diese mit Nutzerdaten gefüllt ist.
                 *
                 * Wenn das SecureFile noch keine Nutzerdaten enthält beträgt der Indexwert 816 (_checkArray1_ und _checkArray2_ jeweils 352 Bytes
                 * plus _controlArray_ mit 112 Bytes ergibt zusammen 816).
                 *
                 * \sa checkArray1, checkArray2 und controlArray.
                 */
                size_t userDataEnd;
                ByteArray *Decrypt12(ByteArray &ref, bool newArray);
                ByteArray *Decrypt34(ByteArray &ref, bool newArray);
                ByteArray *Encrypt12(ByteArray &ref, bool newArray);
                ByteArray *Encrypt34(ByteArray &ref, bool newArray);
                size_t WriteTablesToStore();
        };


        //******************** inline implementations **********************************

        /*! \brief Diese Methode `Count()` gibt die Anzahl der verwalteten und verschlüsselten Array-Bereiche zurück.
         *
         *  Die Methode benutzt intern die Methode Count() der Instanz der Klasse __ItemTable__.
         */
        inline size_t SecureFile::Count() const { return items->Count(); }

        /*! \brief Diese Methode `isStoreOpen()` gibt _true_ zurück, wenn das SecureFile geöffnet ist.
         *  \sa Die Methoden Open() und Close() und die private Variable _openflag_
         */
        inline bool SecureFile::isStoreOpen() const { return openFlag; }

} // end of namespace Store


#endif // CONTAINER_SECURECONTAINER_H
