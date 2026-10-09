/*
X400_Utilities.dll, a collection of useful classes and routines concerning the X.400 standard.
Copyright (C) 2023 - 2026    Reinhard Hölscher

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

/*! \file securefile.cpp
 *  \brief In dieser Datei wird die Klasse __SecureFile__ implementiert.
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


#include "securefile.h"

namespace Store {

        /*! \brief Konstruktor: Initialisiert eine neue Instanz der Klasse __IndexFree__.
         *
         * Die IndexFree-Instanz stellt eine Schnittstelle zur Datenbanktabelle IndexFreeTable bereit.
         * \param table Verweis auf die Tabelleninstanz, die diese (Zeilen-)Instanz verwaltet.
         * \param isSearchInfo _true_, wenn diese Instanz Suchinformationen enthält; _false_, wenn es sich um eine normale Datensatzinstanz handelt.
         */
        IndexFree::IndexFree(BasicTable &table, const bool isSearchInfo) : BasicRow(table, isSearchInfo) {}

        /*! \brief Destruktor: Zerstört die existierende Instanz der Klasse __IndexFree__.
         *
         *  In die Klasse _IndexFree_ wurden keine Instanzen anderer Klassen geladen. Deshalb ist (außer der eigenen Instanz) nichts zu zerstören.
         */
        IndexFree::~IndexFree() {}

        /*! \brief Diese Methode `CloneThis()` dupliziert die Daten dieser Instanz.
         *
         * Führt die tatsächlich Arbeit der Duplizierung dieser Instanz aus.
         *
         * \returns Zeiger auf eine Kopie dieser Instanz.
         * \sa CopyToDestination()
         */
        BasicRow *IndexFree::CloneThis() const {
                IndexFree *newRow = new IndexFree(*this->myTable, this->searchInfo);
                CopyToDestination(*newRow);
                return newRow;
        }

        /*! \brief Diese Methode `CopyToDestination()` kopiert die Daten dieser Instanz in eine (Ziel-)Instanz.
         *
         * Führt tatsächlich den Vorgang des Kopierens dieser Instanz in eine Zielinstanz durch.
         *
         * \param ref ref Referenz auf die Zielinstanz, in die kopiert wird.
         * \sa CloneThis()
         */
        void IndexFree::CopyToDestination(BasicRow &ref) const {
                IndexFree &targetRow = static_cast<IndexFree &>(ref);
                targetRow.indexNumber = this->indexNumber;
        }

        /*! \brief Diese Methode `ExportToByteArray()` exportiert alle Datenfelder dieser Instanz in eine ByteArray-Instanz.
         *
         * Exportiert alle Datenfelder einer Instanz dieser Klasse in ein ByteArray, sodass sie mit der Methode `ImportFromByteArray()` wieder eingelesen werden können.
         *
         * \param ref Verweis auf eine ByteArray-Instanz, in die exportiert werden soll.
         * \sa ImportFromByteArray()
         */
        void IndexFree::ExportToByteArray(ByteArray &ref) const {
                size_t index = 0;
                ref.WriteX209Int64(index, static_cast<int64_t>(indexNumber));
                ref.SetSize(index);
        }

        /*! \brief Diese Methode `ImportFromByteArray()` importiert alle Datenfelder dieser Instanz aus einer ByteArray-Instanz.
         *
         * Importiert alle Datenfelder aus einer ByteArray-Instanz in eine Instanz dieser Klasse,
         * die zuvor mit der Methode `ExportToByteArray()` dorthin exportiert wurden.
         *
         * \param ref Verweis auf eine ByteArray-Instanz, aus der importiert werden soll.
         * \sa ExportToByteArray()
         */
        void IndexFree::ImportFromByteArray(ByteArray &ref) {
                size_t index = 0;
                indexNumber = static_cast<size_t>(ref.ReadX209Int64(index));
        }

        /*! \brief Diese Methode `IsEqualKey()` prüft, ob Schlüssel gleich sind.
         *
         * Die virtuelle Methode `IsEqualKey()` prüft, ob die eigene _IndexNumber_ (Schlüsselindex 0) gleich ist als diejenige aus einem Vergleichsdatensatz.
         *
         * Wenn der eigene Schlüssel mit dem der Vergleichsinstanz übereinstimmt, wird _true_ zurückgegeben.
         * Wenn beide Schlüssel unterschiedlich sind, wird _false_ zurückgegeben.
         *
         * \param row Instanz, mit der der Vergleich durchgeführt werden soll.
         * \param keyIndex Index des zu prüfenden Schlüssels (0 .. n).
         * \return _true_, wenn der eigene Schlüssel mit dem Schlüssel der Vergleichsinstanz übereinstimmt.
         * \sa IsLessKey()
         */
        bool IndexFree::IsEqualKey(BasicRow &row, const size_t keyIndex) const {
                IndexFree &test = static_cast<IndexFree&>(row);
                switch (keyIndex) {
                case 0:
                        if (this->indexNumber == test.indexNumber) {
                                return true;
                        } else {
                                return false;
                        }
                default:
                        return true;
                }
        }

        /*! \brief Diese Methode `IsLessKey()` prüft, ob der eigene Schlüssel kleiner ist als der in der Vergleichsinstanz.
         *
         * Die virtuelle Methode `IsLessKey()` prüft, ob die eigene _IndexNumber_ (Schlüsselindex 0) kleiner ist als diejenige aus einem Vergleichsdatensatz.
         *
         * Ist der eigene Schlüssel kleiner als der der Vergleichsinstanz, wird _true_ zurückgegeben.
         * Sind beide Schlüssel gleich oder ist der eigene Schlüssel größer, wird _false_ zurückgegeben.
         *
         * \param row Instanz, mit der der Vergleich durchgeführt werden soll.
         * \param keyIndex Index des zu prüfenden Schlüssels (0 .. n).
         * \return _true_, wenn der eigene Schlüssel kleiner ist als der Schlüssel der Vergleichsinstanz.
         * \sa IsEqualKey()
         */
        bool IndexFree::IsLessKey(BasicRow &row, const size_t keyIndex) const {
                IndexFree &test = static_cast<IndexFree&>(row);
                switch (keyIndex) {
                case 0:
                        if (this->indexNumber < test.indexNumber) {
                                return true;
                        } else {
                                return false;
                        }
                default:
                        return true;
                }
        }




        /*! \brief Konstruktor: Initialisiert eine neue Instanz der Klasse __IndexFreeTable__.
         *
         * Die Instanz verwaltet nur einen Schlüssel, der die freien Indexenummern enthält.
         * Die Variable _searchInfo_ wird mit einer neuen Suchinstanz der Klasse __IndexFree__ initialisiert.
         */
        IndexFreeTable::IndexFreeTable() : BasicTable(1) {
                searchInfo = new IndexFree(*this, true);
        }

        /*! \brief Destruktor: Zerstört die existierende Instanz der Klasse __IndexFreeTable__.
         *
         * Der Destruktor löscht die Instanz der Klasse _FreeIndex_, auf die die Variable _searchInfo_ verweist.
         */
        IndexFreeTable::~IndexFreeTable() {
                delete searchInfo;
        }

        /*! \brief Diese Methode `NewRow()` erstellt eine neue Datenzeile (eine Instanz der Klasse _IndexFree_).
         *
         * Erstellt eine neue Datensatzinstanz der Klasse _FreeIndex_ in der Variablen _currentRow_, sofern keine nicht einsortierte Instanz mehr vorhanden ist.
         */
        void IndexFreeTable::NewRow() {
                if (!newRowFlag) {
                        currentRow = new IndexFree(*this, false);
                        newRowFlag = true;
                }
        }

        /*! \brief Diese Methode `Row()` gewährt Zugriff auf die Datensatzinstanz, die gerade verarbeitet wird.
         *  \sa SearchRow()
         */
        IndexFree *IndexFreeTable::Row() {
                return static_cast<IndexFree*>(currentRow);
        }

        /*! \brief Diese Methode `SearchRow()` gewährt Zugriff auf die Datensatzinstanz, die für Suchzwecke in Bezug auf andere Datensatzinstanzen verwendet wird.
         *  \sa Row()
         */
        IndexFree *IndexFreeTable::SearchRow() {
                return static_cast<IndexFree*>(searchInfo);
        }




        /*! \brief Konstruktor: Initialisiert eine neue Instanz der Klasse __Items__.
         *
         * Die Items-Instanz stellt eine Schnittstelle zur Datenbanktabelle ItemTable bereit.
         * \param table Verweis auf die Tabelleninstanz, die diese (Zeilen-)Instanz verwaltet.
         * \param isSearchInfo _true_, wenn diese Instanz Suchinformationen enthält; _false_, wenn es sich um eine normale Datensatzinstanz handelt.
         */
        Items::Items(BasicTable &table, const bool isSearchInfo) : BasicRow(table, isSearchInfo) {
        }

        /*! \brief Destruktor: Zerstört die existierende Instanz der Klasse __Items__.
         *
         *  In die Klasse _Items_ wurden keine Instanzen anderer Klassen geladen. Deshalb ist (außer der eigenen Instanz) nichts zu zerstören.
         */
        Items::~Items() {}

        /*! \brief Diese Methode `CloneThis()` dupliziert die Daten dieser Instanz.
         *
         * Führt die tatsächlich Arbeit der Duplizierung dieser Instanz aus.
         *
         * \returns Zeiger auf eine Kopie dieser Instanz.
         * \sa CopyToDestination()
         */
        BasicRow *Items::CloneThis() const {
                Items *newRow = new Items(*this->myTable, this->searchInfo);
                CopyToDestination(*newRow);
                return newRow;
        }

        /*! \brief Diese Methode `CopyToDestination()` kopiert die Daten dieser Instanz in eine (Ziel-)Instanz.
         *
         * Führt tatsächlich den Vorgang des Kopierens dieser Instanz in eine Zielinstanz durch.
         *
         * \param ref ref Referenz auf die Zielinstanz, in die kopiert wird.
         * \sa CloneThis()
         */
        void Items::CopyToDestination(BasicRow &ref) const {
                Items &targetRow = static_cast<Items &>(ref);
                targetRow.arrayLength = this->arrayLength;
                targetRow.fileIndex = this->fileIndex;
                targetRow.indexNumber = this->indexNumber;
        }

        /*! \brief Diese Methode `ExportToByteArray()` exportiert alle Datenfelder dieser Instanz in eine ByteArray-Instanz.
         *
         * Exportiert alle Datenfelder einer Instanz dieser Klasse in ein ByteArray, sodass sie mit der Methode `ImportFromByteArray()` wieder eingelesen werden können.
         *
         * \param ref Verweis auf eine ByteArray-Instanz, in die exportiert werden soll.
         * \sa ImportFromByteArray()
         */
        void Items::ExportToByteArray(ByteArray &ref) const {
                size_t index = 0;
                ref.WriteX209Int64(index, static_cast<int64_t>(arrayLength));
                ref.WriteX209Int64(index, static_cast<int64_t>(fileIndex));
                ref.WriteX209Int64(index, static_cast<int64_t>(indexNumber));
        }

        /*! \brief Diese Methode `ImportFromByteArray()` importiert alle Datenfelder dieser Instanz aus einer ByteArray-Instanz.
         *
         * Importiert alle Datenfelder aus einer ByteArray-Instanz in eine Instanz dieser Klasse,
         * die zuvor mit der Methode `ExportToByteArray()` dorthin exportiert wurden.
         *
         * \param ref Verweis auf eine ByteArray-Instanz, aus der importiert werden soll.
         * \sa ExportToByteArray()
         */
        void Items::ImportFromByteArray(ByteArray &ref) {
                size_t index = 0;
                arrayLength = static_cast<size_t>(ref.ReadX209Int64(index));
                fileIndex = static_cast<size_t>(ref.ReadX209Int64(index));
                indexNumber = static_cast<size_t>(ref.ReadX209Int64(index));
        }

        /*! \brief Diese Methode `IsEqualKey()` prüft, ob Schlüssel gleich sind.
         *
         * Die virtuelle Methode `IsEqualKey()` prüft, ob die eigene _indexNumber_ (Schlüsselindex 0) gleich ist als diejenige aus einem Vergleichsdatensatz.
         *
         * Die virtuelle Methode `IsEqualKey()` prüft, ob die eigene _fileIndex_ (Schlüsselindex 1) gleich ist als diejenige aus einem Vergleichsdatensatz.
         *
         * Wenn der eigene Schlüssel mit dem der Vergleichsinstanz übereinstimmt, wird _true_ zurückgegeben.
         * Wenn beide Schlüssel unterschiedlich sind, wird _false_ zurückgegeben.
         *
         * \param row Instanz, mit der der Vergleich durchgeführt werden soll.
         * \param keyIndex Index des zu prüfenden Schlüssels (0 .. n).
         * \return _true_, wenn der eigene Schlüssel mit dem Schlüssel der Vergleichsinstanz übereinstimmt.
         * \sa IsLessKey()
         */
        bool Items::IsEqualKey(BasicRow &row, const size_t keyIndex) const {
                Items &test = static_cast<Items&>(row);
                switch (keyIndex) {
                case 0:
                        if (this->indexNumber == test.indexNumber) {
                                return true;
                        } else {
                                return false;
                        }
                case 1:
                        if (this->fileIndex == test.fileIndex) {
                                return true;
                        } else {
                                return false;
                        }
                default:
                        return true;
                }
        }

        /*! \brief Diese Methode `IsLessKey()` prüft, ob der eigene Schlüssel kleiner ist als der in der Vergleichsinstanz.
         *
         * Die virtuelle Methode `IsLessKey()` prüft, ob die eigene _indexNumber_ (Schlüsselindex 0) kleiner ist als diejenige aus einem Vergleichsdatensatz.
         *
         * Die virtuelle Methode `IsLessKey()` prüft, ob die eigene _fileIndex_ (Schlüsselindex 1) kleiner ist als diejenige aus einem Vergleichsdatensatz.
         *
         * Ist der eigene Schlüssel kleiner als der der Vergleichsinstanz, wird _true_ zurückgegeben.
         * Sind beide Schlüssel gleich oder ist der eigene Schlüssel größer, wird _false_ zurückgegeben.
         *
         * \param row Instanz, mit der der Vergleich durchgeführt werden soll.
         * \param keyIndex Index des zu prüfenden Schlüssels (0 .. n).
         * \return _true_, wenn der eigene Schlüssel kleiner ist als der Schlüssel der Vergleichsinstanz.
         * \sa IsEqualKey()
         */
        bool Items::IsLessKey(BasicRow &row, const size_t keyIndex) const {
                Items &test = static_cast<Items&>(row);
                switch (keyIndex) {
                case 0:
                        if (this->indexNumber < test.indexNumber) {
                                return true;
                        } else {
                                return false;
                        }
                case 1:
                        if (this->fileIndex < test.fileIndex) {
                                return true;
                        } else {
                                return false;
                        }
                default:
                        return true;
                }
        }



        /*! \brief Konstruktor: Initialisiert eine neue Instanz der Klasse __ItemTable__.
         *
         * Die Instanz verwaltet zwei Schlüssel.
         * Die Variable _searchInfo_ wird mit einer neuen Suchinstanz der Klasse __Items__ initialisiert.
         */
        ItemTable::ItemTable() : BasicTable(2) {
                searchInfo = new Items(*this, true);
        }

        /*! \brief Destruktor: Zerstört die existierende Instanz der Klasse __ItemTable__.
         *
         * Der Destruktor löscht die Instanz der Klasse _Items_, auf die die Variable _searchInfo_ verweist.
         */
        ItemTable::~ItemTable() {
                delete searchInfo;
        }

        /*! \brief Diese Methode `NewRow()` erstellt eine neue Datenzeile (eine Instanz der Klasse _Items_).
         *
         * Erstellt eine neue Datensatzinstanz der Klasse _Items_ in der Variablen _currentRow_, sofern keine nicht einsortierte Instanz mehr vorhanden ist.
         */
        void ItemTable::NewRow() {
                if (!newRowFlag) {
                        currentRow = new Items(*this, false);
                        newRowFlag = true;
                }
        }

        /*! \brief Diese Methode `Row()` gewährt Zugriff auf die Datensatzinstanz, die gerade verarbeitet wird.
         *  \sa SearchRow()
         */
        Items *ItemTable::Row() {
                return static_cast<Items*>(currentRow);
        }

        /*! \brief Diese Methode `SearchRow()` gewährt Zugriff auf die Datensatzinstanz, die für Suchzwecke in Bezug auf andere Datensatzinstanzen verwendet wird.
         *  \sa Row()
         */
        Items *ItemTable::SearchRow() {
                return static_cast<Items*>(searchInfo);
        }





        /*! \brief Konstruktor: Initialisiert eine neue Instanz der Klasse __SecureFile__.
         *  \sa ~SecureFile()
         */
        SecureFile::SecureFile() {
                openFlag = false;
                freeIndexes = new IndexFreeTable();
                items = new ItemTable();
                checkArray1 = new ByteArray(352, true);
                checkArray2 = new ByteArray(352, true);
                controlArray = new ByteArray(112, true);
                aes1 = new AES();
                aes2 = new AES();
                aes3 = new AES();
                aes4 = new AES();
                sha384 = new SHA384();
                sha512 = new SHA512();
        }


        /*! \brief Destruktor: Zerstört die existierende Instanz der Klasse __SecureFile__.
         *  \sa SecureFile()
         */
        SecureFile::~SecureFile() {
                delete sha512;
                delete sha384;
                delete aes4;
                delete aes3;
                delete aes2;
                delete aes1;
                delete controlArray;
                delete checkArray2;
                delete checkArray1;
                items->ClearAll();
                delete items;
                freeIndexes->ClearAll();
                delete freeIndexes;
        }


        /*! \brief Diese Methode `ClearAll()` löscht alle gespeicherten verschlüsselten Datenfelder aus dem SecureFile.
         *
         *  Das SecureFile muss geöffnet sein, damit diese Methode fehlerfrei arbeiten kann.
         *
         *      Mögliche Fehlernummern:
         *      10:  Das SecureFile war nicht geöffnet!
         *      17:  In der Datei des SecureFiles konnte nicht positioniert werden!
         *      19:  In der Datei des SecureFiles konnte nicht geschrieben werden!
         *      26:  Ein Datenblock konnte nicht fehlerfrei verschlüsselt werden!
         *
         *  \return Fehlernummer (0 wenn alles ok ist).
         *  \sa Delete()
         */
        size_t SecureFile::ClearAll() {
                if (!openFlag) {
                        return 10;
                }
                freeIndexes->ClearAll();
                items->ClearAll();
                nextFreeIndex = 1024;
                userDataEnd = 816;
                return WriteTablesToStore();
        }


        /*! \brief Diese Methode `Close()` schließt ein geöffnetes SecureFile.
         *
         *  Das SecureFile muss geöffnet sein, damit diese Methode fehlerfrei arbeiten kann.
         *
         *      Mögliche Fehlernummern:
         *      10:  Das SecureFile war nicht geöffnet!
         *      20:  Die Datei des SecureFiles konnte nicht gekürzt werden!
         *
         *  \return Fehlernummer (0 wenn alles ok ist).
         *  \sa Open()
         */
        size_t SecureFile::Close() {
                if (!openFlag) {
                        return 10;
                }
                size_t fileend = userDataEnd + freeIndexesLng + itemsLng;
                if (!file0->resize(fileend)) {
                        return 20;
                }
                file0->close();
                delete file0;
                file0 = nullptr;
                aes1->Clear();
                aes2->Clear();
                aes3->Clear();
                aes4->Clear();
                openFlag = false;
                return 0;
        }



        /*! \brief Diese Methode `Decrypt12()` entschlüsselt ein ByteArray und überprüft anschließend den Hashwert der Nutzdaten.
         *
         * Due Methode arbeitet mit den AES-Instanzen 1 und 2 zusammen.
         * Die Daten werden zuerst zweimal mit dem AES-Algorithmus entschlüsselt.
         * Danach wird der SHA512-Hashwert auf Gültigkeit geprüft.
         * Wenn dies funktioniert, wird ein Zeiger auf das entschlüsselte ByteArray zurückgegeben.
         * Wenn ein Fehler aufgetreten ist, wird ein _nullptr_ zurückgegeben.
         *
         * \param ref Verweis auf eine ByteArray-Instanz, die verschlüsselte Daten enthält.
         * \param newArray _true_, wenn das verschlüsselte Array beibehalten werden soll.
         * \return Zeiger auf die entschlüsselte ByteArray-Instanz (oder _nullptr_ im Fehlerfall).
         * \sa Encrypt12() und Encrypt34()
         */
        ByteArray *SecureFile::Decrypt12(ByteArray &ref, bool newArray) {
                if (newArray) {
                        ByteArray *r = new ByteArray(ref.Size(), true);
                        r->Append(ref);
                        if (!aes2->Decrypt(*r)) {
                                delete r;
                                return nullptr;
                        }
                        if (!aes1->Decrypt(*r)) {
                                delete r;
                                return nullptr;
                        }
                        size_t length = ref.Size();
                        if (sha512->TestHash(*r)) {
                                length = length - 64;
                                r->SetSize(length);
                        } else {
                                delete r;
                                return nullptr;
                        }
                        return r;
                } else {
                        if (!aes2->Decrypt(ref)) {
                                return nullptr;
                        }
                        if (!aes1->Decrypt(ref)) {
                                return nullptr;
                        }
                        size_t length = ref.Size();
                        if (sha512->TestHash(ref)) {
                                length = length - 64;
                                ref.SetSize(length);
                        } else {
                                return nullptr;
                        }
                        return &ref;
                }
        }

        /*! \brief Diese Methode `Decrypt34()` entschlüsselt ein ByteArray und überprüft anschließend den Hashwert der Nutzdaten.
         *
         * Due Methode arbeitet mit den AES-Instanzen 3 und 4 zusammen.
         * Die Daten werden zuerst zweimal mit dem AES-Algorithmus entschlüsselt.
         * Danach wird der SHA512-Hashwert auf Gültigkeit geprüft.
         * Wenn dies funktioniert, wird ein Zeiger auf das entschlüsselte ByteArray zurückgegeben.
         * Wenn ein Fehler aufgetreten ist, wird ein _nullptr_ zurückgegeben.
         *
         * \param ref Verweis auf eine ByteArray-Instanz, die verschlüsselte Daten enthält.
         * \param newArray _true_, wenn das verschlüsselte Array beibehalten werden soll.
         * \return Zeiger auf die entschlüsselte ByteArray-Instanz (oder _nullptr_ im Fehlerfall).
         * \sa Encrypt12() und Encrypt34()
         */
        ByteArray *SecureFile::Decrypt34(ByteArray &ref, bool newArray) {
                if (newArray) {
                        ByteArray *r = new ByteArray(ref.Size(), true);
                        r->Append(ref);
                        if (!aes4->Decrypt(*r)) {
                                delete r;
                                return nullptr;
                        }
                        if (!aes3->Decrypt(*r)) {
                                delete r;
                                return nullptr;
                        }
                        size_t length = ref.Size();
                        if (sha512->TestHash(*r)) {
                                length = length - 64;
                                r->SetSize(length);
                        } else {
                                delete r;
                                return nullptr;
                        }
                        return r;
                } else {
                        if (!aes4->Decrypt(ref)) {
                                return nullptr;
                        }
                        if (!aes3->Decrypt(ref)) {
                                return nullptr;
                        }
                        size_t length = ref.Size();
                        if (sha512->TestHash(ref)) {
                                length = length - 64;
                                ref.SetSize(length);
                        } else {
                                return nullptr;
                        }
                        return &ref;
                }
        }

        /*! \brief Diese Methode `Delete()` löscht den verschlüsselten Datenblock mit der übergebenen Indexnummer aus dem SecureFile.
         *
         *  Das SecureFile muss geöffnet sein, damit diese Methode fehlerfrei arbeiten kann.
         *
         *      Mögliche Fehlernummern:
         *      10:  Das SecureFile war nicht geöffnet!
         *      17:  In der Datei des SecureFiles konnte nicht positioniert werden!
         *      18:  In der Datei des SecureFiles konnte nicht gelesen werden!
         *      19:  In der Datei des SecureFiles konnte nicht geschrieben werden!
         *      21:  Die übergebene Indexnummer existiert im SecureFile nicht!
         *      26:  Ein Datenblock konnte nicht fehlerfrei verschlüsselt werden!
         *
         *  \param indexNumber Nummer des Elements, das aus dem SecureFile gelöscht werden soll.
         *  \return Fehlernummer (0 wenn alles ok ist).
         *  \sa Get() und Put()
         */
        size_t SecureFile::Delete(const size_t indexNumber) {
                if (!openFlag) {
                        return 10;
                }
                if (indexNumber < 256) {
                        return 21;
                }
                items->SetKeyIndex(0);
                items->SearchRow()->indexNumber = indexNumber;
                if (items->Seek(false)) {
                        size_t gapindex = items->Row()->fileIndex;
                        size_t gaplength = items->Row()->arrayLength;
                        items->Delete(SetNot);
                        if (indexNumber > 1023) {
                                freeIndexes->SearchRow()->indexNumber = indexNumber;
                                if (!freeIndexes->Seek(false)) {
                                        freeIndexes->CreateNewRow();
                                        freeIndexes->Row()->indexNumber = indexNumber;
                                        freeIndexes->Insert(false);
                                }
                        }
                        size_t readIndex = gapindex + gaplength;
                        size_t length = userDataEnd - readIndex;
                        Array<char> A(length);
                        if (!file0->seek(static_cast<qint64>(readIndex))) {
                                return 17;
                        }
                        if (file0->read(&(A.WriteareaReferenz(0, length)), static_cast<qint64>(length)) != static_cast<qint64>(length)) {
                                return 18;
                        }
                        if (!file0->seek(static_cast<qint64>(gapindex))) {
                                return 17;
                        }
                        if (file0->write(&(A[0]), static_cast<qint64>(length)) != static_cast<qint64>(length)) {
                                return 19;
                        }
                        items->SetKeyIndex(1);
                        items->SearchRow()->fileIndex = readIndex;
                        items->Seek(true);
                        while ((items->Row() != nullptr) && (items->Row()->fileIndex <= userDataEnd)) {
                                items->Row()->fileIndex = items->Row()->fileIndex - gaplength;
                                items->Next();
                        }
                        userDataEnd = userDataEnd - gaplength;
                        return WriteTablesToStore();
                } else {
                        return 21;
                }
        }

        /*! \brief Diese Methode `Encrypt12()` schützt ein Byte-Array mithilfe eines Hashwertes vor Veränderung und verschlüsselt es anschließend.
         *
         *  Das übergebene Byte-Array wird zunächst durch einen angehängten Hashwert vor nachträglichen Manipulationen geschützt.
         *  Dann wird es zweimal mit den AES-Instanzen 1 und 2 verschlüsselt.
         *
         * \param ref Verweis auf eine ByteArray-Instanz, die Klartext enthält.
         * \param newArray _true_, wenn das Klartext-Array beibehalten werden soll.
         * \return Zeiger auf eine ByteArray-Instanz, die verschlüsselte Daten enthält (oder im Fehlerfall einen _nullptr_).
         * \sa Decrypt12() und Decrypt34()
         */
        ByteArray *SecureFile::Encrypt12(ByteArray &ref, bool newArray) {
                if (newArray) {
                        ByteArray *r = new ByteArray(ref.Size() + 128);
                        r->Append(ref);
                        sha512->AddHash(*r);
                        if (!aes1->Encrypt(*r)) {
                                delete r;
                                return nullptr;
                        }
                        if (!aes2->Encrypt(*r)) {
                                delete r;
                                return nullptr;
                        }
                        return r;
                } else {
                        sha512->AddHash(ref);
                        if (!aes1->Encrypt(ref)) {
                                return nullptr;
                        }
                        if (!aes2->Encrypt(ref)) {
                                return nullptr;
                        }
                        return &ref;
                }
        }

        /*! \brief Diese Methode `Encrypt34()` schützt ein Byte-Array mithilfe eines Hashwertes vor Veränderung und verschlüsselt es anschließend.
         *
         *  Das übergebene Byte-Array wird zunächst durch einen angehängten Hashwert vor nachträglichen Manipulationen geschützt.
         *  Dann wird es zweimal mit den AES-Instanzen 3 und 4 verschlüsselt.
         *
         * \param ref Verweis auf eine ByteArray-Instanz, die Klartext enthält.
         * \param newArray _true_, wenn das Klartext-Array beibehalten werden soll.
         * \return Zeiger auf eine ByteArray-Instanz, die verschlüsselte Daten enthält (oder im Fehlerfall einen _nullptr_).
         * \sa Decrypt12() und Decrypt34()
         */
        ByteArray *SecureFile::Encrypt34(ByteArray &ref, bool newArray) {
                if (newArray) {
                        ByteArray *r = new ByteArray(ref.Size() + 128);
                        r->Append(ref);
                        sha512->AddHash(*r);
                        if (!aes3->Encrypt(*r)) {
                                delete r;
                                return nullptr;
                        }
                        if (!aes4->Encrypt(*r)) {
                                delete r;
                                return nullptr;
                        }
                        return r;
                } else {
                        sha512->AddHash(ref);
                        if (!aes3->Encrypt(ref)) {
                                return nullptr;
                        }
                        if (!aes4->Encrypt(ref)) {
                                return nullptr;
                        }
                        return &ref;
                }
        }



        /*! \brief Diese Methode `DumpRecord()` erstellt einen Hexadezimal-Dump eines Datenblocks mit der übergebenen Indexnummer (im Klartext) in einer separaten Datei.
         *
         * Die Methode versucht zunächst die übergebene Datei zum Schreiben zu öffnen.
         * Wenn dies nicht möglich ist, wird mit _false_ zurückgekehrt.
         * Danach wird nach dem gewünschten Index gesucht. Ist er nicht vorhanden,  wird mit _false_ zurückgekehrt.
         * Danach werden die Datenbytes in die Datei geschrieben und die Datei wieder geschlossen.
         * Die Funktion gibt _true_ zurück, wenn der gewünschte Datenblock verarbeitet werden konnte, andernfalls _false_.
         *
         * Nachfolgend ein Beispielausdruck eines Dumps:
         *
         *      decimal Index  hexIndex  Content(hexadezimal)
         *                  0  00000000  d6 00 12 1d 0a 27 09 38 e6 0f 00 80 08 07 41 63  .....'.8......Ac
         *                 16  00000010  72 6f 6e 69 73 12 0d 0c 0b 01 11 e9 0f 00 81 08  ronis...........
         *                 32  00000020  07 41 63 72 6f 6e 69 73 16 1d 0a 28 09 30 e6 0f  .Acronis...(.0..
         *                 48  00000030  00 82 08 0b 41 6b 61 64 65 6d 69 73 63 68 65 16  ....Akademische.
         *                 64  00000040  13 09 0f 0c 1a e9 0f 00 83 08 0b 41 6c 61 72 6d  ...........Alarm
         *                 80  00000050  61 6e 6c 61 67 65 11 1d 0a 29 09 3b e6 0f 00 84  anlage...).;....
         *                 96  00000060  08 06 41 6d 61 7a 6f 6e 12 1c 0c 1f 01 15 ea 0f  ..Amazon........
         *                112  00000070  00 85 08 07 41 72 63 68 69 6f 6e 0f 0c 0e 05 0a  ....Archion.....
         *                128  00000080  25 e7 0f 00 86 08 04 42 61 68 6e 11 06 0b 19 07  %......Bahn.....
         *                144  00000090  22 e9 0f 00 87 08 06 42 61 6e 6b 65 6e 11 1d 0a  "......Banken...
         *                160  000000a0  2c 09 12 e6 0f 00 88 08 06 42 61 72 6d 65 72 19  ,........Barmer.*
         *                176  000000b0  1d 09 28 04 39 e8 0f 00 89 08 0e 42 65 6e 65 66  ..(.9......Benef
         *                192  000000c0  69 74 5f 50 6f 72 74 61 6c 12 1d 0a 2d 09 1a e6  it_Portal...-...
         *                208  000000d0  0f 00 8a 08 07 42 65 72 67 46 65 78 10 0d 0d 2d  .....BergFex...-
         *                224  000000e0  09 29 e9 0f 00 8b 08 05 42 6c 69 6e 6b 13 14 0f  .)......Blink...
         *                240  000000f0  22 0c 19 e7 0f 00 8c 08 08 42 72 65 63 68 74 65  "........Brechte
         *                256  00000100  6c 13 02 11 13 02 1b e7 0f 00 8d 08 08 42 75 67  l............Bug
         *                272  00000110  61 32 30 32 33 11 10 0a 22 02 3b ea 0f 00 8e 08  a2023...".;.....
         *                288  00000120  06 42 75 6e 64 49 44 12 1d 0a 2e 09 39 e6 0f 00  .BundID.....9...
         *                304  00000130  8f 08 07 63 61 70 65 6c 6c 61 15 1d 0a 2f 09 31  ...capella.../.1
         *                320  00000140  e6 0f 00 90 08 0a 43 61 72 53 68 61 72 69 6e 67  ......CarSharing
         *                336  00000150  11 1d 0b 13 09 1d e6 0f 00 91 08 06 43 6f 72 6f  ............Coro
         *                352  00000160  6e 61 12 1d 0b 13 09 3b e6 0f 00 92 08 07 44 45  na.....;......DE
         *                368  00000170  2d 4d 61 69 6c 10 1d 0b 14 09 1b e6 0f 00 93 08  -Mail...........
         *                384  00000180  05 44 65 65 70 4c 11 1d 0b 14 09 34 e6 0f 00 94  .DeepL.....4....
         *                400  00000190  08 06 44 65 65 7a 65 72 13 1d 0b 15 09 1a e6 0f  ..Deezer........
         *                416  000001a0  00 95 08 08 44 6f 63 78 79 67 65 6e 1d 1d 0b 17  ....Docxygen....
         *                432  000001b0  09 0f e6 0f 00 96 08 12 65 64 76 2d 62 75 63 68  ........edv-buch
         *                448  000001c0  76 65 72 73 61 6e 64 2e 64 65 1a 07 12 15 02 0c  versand.de......
         *                464  000001d0  e8 0f 00 97 08 0f 45 6c 69 61 6e 61 2d 53 65 63  ......Eliana-Sec
         *                480  000001e0  75 72 65 44 42 0e 1d 0b 18 09 21 e6 0f 00 98 08  ureDB.....!.....
         *                496  000001f0  03 45 4c 56 0f 1d 0b 19 09 03 e6 0f 00 99 08 04  .ELV............
         *                512  00000200  45 52 47 4f 10 03 0d 2f 07 24 e8 0f 00 9a 08 05  ERGO.../.$......
         *                528  00000210  65 53 49 4d 31 16 05 0a 1d 01 0b e9 0f 00 9b 08  eSIM1...........
         *                544  00000220  0b 45 78 63 65 6c 5f 4d 61 6b 72 6f 13 1d 0b 1a  .Excel_Makro....
         *                560  00000230  09 2b e6 0f 00 9c 08 08 46 61 63 65 62 6f 6f 6b  .+......Facebook
         *                576  00000240  14 1d 0b 1b 09 08 e6 0f 00 9d 08 09 46 69 6e 61  ............Fina
         *                592  00000250  6e 7a 61 6d 74 17 1d 0b 1b 09 28 e6 0f 00 9e 08  nzamt.....(.....
         *                608  00000260  0c 46 6c 61 73 63 68 65 6e 70 6f 73 74 19 1d 0b  .Flaschenpost...
         *                624  00000270  1c 09 14 e6 0f 00 9f 08 0e 46 6c 6f 72 61 49 6e  .........FloraIn
         *                640  00000280  63 6f 67 6e 69 74 61 11 0b 0c 2d 01 15 e9 0f 00  cognita...-.....
         *                656  00000290  a0 08 06 46 72 61 67 65 6e 1b 1d 0b 1e 09 12 e6  ...Fragen.......
         *                672  000002a0  0f 00 a1 08 10 47 61 72 61 67 65 5f 4d c3 b6 72  .....Garage_M..r
         *                688  000002b0  73 64 6f 72 66 11 0c 10 02 01 39 e9 0f 00 a2 08  sdorf.....9.....
         *                704  000002c0  06 47 69 74 48 75 62 11 07 0e 2e 02 3a e9 0f 00  .GitHub.....:...
         *                720  000002d0  a3 08 06 47 6f 6f 67 6c 65 17 1d 0b 20 09 0b e6  ...Google... ...
         *                736  000002e0  0f 00 a4 08 0c 48 61 6e 64 65 6c 73 62 6c 61 74  .....Handelsblat
         *                752  000002f0  74 1a 1d 0b 20 09 27 e6 0f 00 a5 08 0f 48 61 6e  t... .'......Han
         *                768  00000300  64 65 6c 73 72 65 67 69 73 74 65 72 1f 03 0d 2a  delsregister...*
         *                784  00000310  07 34 e8 0f 00 a6 08 14 48 61 6e 64 79 5f 47 61  .4......Handy_Ga
         *                800  00000320  6c 61 78 79 5f 53 32 30 2b 5f 35 47 1b 03 11 30  laxy_S20+_5G...0
         *                816  00000330  07 32 e8 0f 00 a7 08 10 48 61 6e 64 79 5f 47 61  .2......Handy_Ga
         *                832  00000340  6c 61 78 79 5f 53 32 34 16 1d 0b 21 09 09 e6 0f  laxy_S24...!....
         *                848  00000350  00 a8 08 0b 48 61 6e 64 79 45 6c 69 61 6e 61 16  ....HandyEliana.
         *                864  00000360  0f 0c 0b 06 1e e9 0f 00 a9 08 0b 48 61 6e 64 79  ...........Handy
         *                880  00000370  45 6c 69 61 6e 61 17 1d 0b 21 09 25 e6 0f 00 aa  Eliana...!.%....
         *                896  00000380  08 0c 48 61 6e 64 79 4d 61 75 72 69 63 65 17 1d  ..HandyMaurice..
         *                912  00000390  0b 23 09 05 e6 0f 00 ab 08 0c 48 65 69 6d 6e 65  .#........Heimne
         *                928  000003a0  74 7a 77 65 72 6b 15 1f 0e 21 0c 10 e6 0f 00 ac  tzwerk...!......
         *                944  000003b0  08 0a 48 55 4b 2d 43 6f 62 75 72 67 20 1d 0b 29  ..HUK-Coburg ..)
         *                960  000003c0  09 34 e6 0f 00 ad 08 15 48 75 6e 64 65 66 75 74  .4......Hundefut
         *                976  000003d0  74 65 72 62 65 73 74 65 6c 6c 75 6e 67 0f 1d 10  terbestellung...
         *                992  000003e0  3b 09 19 e6 0f 00 ae 08 04 49 4b 45 41 1c 1d 11  ;........IKEA...
         *              1.008  000003f0  00 09 02 e6 0f 00 af 08 11 49 6d 6d 6f 62 69 6c  .........Immobil
         *              1.024  00000400  69 65 6e 5a 65 69 74 75 6e 67 11 1d 11 00 09 20  ienZeitung.....
         *              1.040  00000410  e6 0f 00 b0 08 06 69 54 75 6e 65 73 14 1d 11 01  ......iTunes....
         *              1.056  00000420  09 02 e6 0f 00 b1 08 09 4a 65 73 73 69 63 61 50  ........JessicaP
         *              1.072  00000430  43 0f 1f 0f 01 0c 0e e6 0f 00 b2 08 04 4b 69 6e  C............Kin
         *              1.088  00000440  67 17 06 0a 1b 07 3b e9 0f 00 b3 08 0c 4c 61 70  g.....;......Lap
         *              1.104  00000450  74 6f 70 45 6c 69 61 6e 61 15 1d 11 01 09 2f e6  topEliana...../.
         *              1.120  00000460  0f 00 b4 08 0a 4c 61 70 74 6f 70 47 72 61 66 10  .....LaptopGraf.
         *              1.136  00000470  08 0e 27 0b 1d e6 0f 00 b5 08 05 4c 65 76 69 73  ..'........Levis
         *              1.152  00000480  10 1d 11 03 09 13 e6 0f 00 b6 08 05 4c 6f 74 74  ............Lott
         *              1.168  00000490  6f 11 1d 11 04 09 06 e6 0f 00 b7 08 06 4d 63 41  o............McA
         *              1.184  000004a0  66 65 65 11 1d 11 05 09 01 e6 0f 00 b8 08 06 4d  fee............M
         *              1.200  000004b0  65 64 69 6f 6e 17 1d 0a 19 01 2a e9 0f 00 b9 08  edion.....*.....
         *              1.216  000004c0  0c 4d 65 64 69 6f 6e 50 43 32 30 32 35 10 1d 0b  .MedionPC2025...
         *              1.232  000004d0  3a 01 37 e9 0f 00 ba 08 05 4d 65 69 65 72 19 05  :.7......Meier..
         *              1.248  000004e0  0c 13 09 18 e7 0f 00 bb 08 0e 4d 69 63 72 6f 73  ..........Micros
         *              1.264  000004f0  6f 66 74 4b 6f 6e 74 6f 18 08 10 21 06 30 e9 0f  oftKonto...!.0..
         *              1.280  00000500  00 bc 08 0d 4f 70 65 6e 73 74 72 65 65 74 6d 61  ....Openstreetma
         *              1.296  00000510  70 11 1d 11 09 09 38 e6 0f 00 bd 08 06 50 61 79  p.....8......Pay
         *              1.312  00000520  50 61 6c 11 11 07 23 04 2d e8 0f 00 be 08 06 50  Pal...#.-......P
         *              1.328  00000530  42 65 61 4b 4b 0f 1d 11 0a 09 38 e6 0f 00 bf 08  BeaKK.....8.....
         *              1.344  00000540  04 50 65 72 6c 1a 1c 11 21 02 0e ea 0f 00 c0 08  .Perl...!.......
         *              1.360  00000550  0f 50 65 72 73 6f 6e 61 6c 61 75 73 77 65 69 73  .Personalausweis
         *              1.376  00000560  0d 07 10 2d 0b 1a e9 0f 00 c1 08 02 51 74 21 1d  ...-........Qt!.
         *              1.392  00000570  11 0c 09 3a e6 0f 00 c2 08 16 52 61 74 73 49 6e  ...:......RatsIn
         *              1.408  00000580  66 6f 72 6d 61 74 69 6f 6e 73 53 79 73 74 65 6d  formationsSystem
         *              1.424  00000590  0e 1d 11 0d 09 18 e6 0f 00 c3 08 03 52 4d 56 12  ............RMV.
         *              1.440  000005a0  05 12 17 05 2f e9 0f 00 c4 08 07 53 61 6d 73 75  ..../......Samsu
         *              1.456  000005b0  6e 67 14 1a 10 2d 01 2d ea 0f 00 c5 08 09 53 70  ng...-.-......Sp
         *              1.472  000005c0  61 72 6b 61 73 73 65 19 1d 11 0e 09 38 e6 0f 00  arkasse.....8...
         *              1.488  000005d0  c6 08 0e 53 70 72 69 6e 67 65 72 56 65 72 6c 61  ...SpringerVerla
         *              1.504  000005e0  67 15 0c 0a 39 08 07 e9 0f 00 c7 08 0a 53 74 61  g...9........Sta
         *              1.520  000005f0  64 74 77 65 72 6b 65 14 06 0c 21 06 2b e8 0f 00  dtwerke...!.+...
         *              1.536  00000600  c8 08 09 53 77 61 72 6f 76 73 6b 69 11 18 0b 1c  ...Swarovski....
         *              1.552  00000610  0a 2d e7 0f 00 c9 08 06 54 2d 48 6f 6d 65 1e 1d  .-......T-Home..
         *              1.568  00000620  11 11 09 1c e6 0f 00 ca 08 13 54 2d 4d 6f 62 69  ..........T-Mobi
         *              1.584  00000630  6c 65 2d 52 65 63 68 6e 75 6e 67 65 6e 15 03 11  le-Rechnungen...
         *              1.600  00000640  35 05 1a ea 0f 00 d7 08 0a 54 65 61 6d 56 69 65  5........TeamVie
         *              1.616  00000650  76 65 72 15 1d 11 12 09 00 e6 0f 00 cb 08 0a 54  ver............T
         *              1.632  00000660  68 65 50 69 6f 6e 65 65 72 11 1d 11 13 09 05 e6  hePioneer.......
         *              1.648  00000670  0f 00 cc 08 06 54 6f 6c 69 6e 6f 14 03 10 3a 07  .....Tolino...:.
         *              1.664  00000680  09 e8 0f 00 cd 08 09 56 65 72 61 43 72 79 70 74  .......VeraCrypt
         *              1.680  00000690  1f 1d 11 14 09 0a e6 0f 00 ce 08 14 56 6f 6c 6c  ............Voll
         *              1.696  000006a0  73 74 72 65 63 6b 75 6e 67 73 70 6f 72 74 61 6c  streckungsportal
         *              1.712  000006b0  12 1d 11 14 09 23 e6 0f 00 cf 08 07 56 6f 72 77  .....#......Vorw
         *              1.728  000006c0  65 72 6b 12 0d 11 1c 04 01 e7 0f 00 d0 08 07 56  erk............V
         *              1.744  000006d0  52 4e 2d 41 70 70 14 1d 11 16 09 0e e6 0f 00 d1  RN-App..........
         *              1.760  000006e0  08 09 57 69 6b 69 70 65 64 69 61 0f 1d 11 16 09  ..Wikipedia.....
         *              1.776  000006f0  28 e6 0f 00 d2 08 04 57 4c 41 4e 18 13 0f 27 02  (......WLAN...'.
         *              1.792  00000700  23 ea 0f 00 d3 08 0d 57 4c 41 4e 2d 52 65 70 65  #......WLAN-Repe
         *              1.808  00000710  61 74 65 72 0f 1d 11 17 09 04 e6 0f 00 d4 08 04  ater............
         *              1.824  00000720  5a 41 4b 42                                      ZAKB
         *
         * \note Diese Methode ist nicht für den Wirkbetrieb gedacht. Sie kann aber bei Programmentwicklung manchmal sehr hilfreich sein.
         * \param pathName Pfad und Dateiname der Datei, in die die Daten ausgegeben werden sollen.
         * \param indexNumber Nummer des Datenblockes, für den der Speicherauszug erstellt werden soll.
         * \return _true_, wenn kein Fehler aufgetreten ist, andernfalls _false_.
         * \sa DumpStruct()
         */
        bool SecureFile::DumpRecord(QString &pathName, size_t indexNumber) {
                void *p = &pathName;
                if ((p == nullptr) || (pathName.isEmpty())) {
                        return false;
                }
                if (!openFlag) {
                        return false;
                }
                QFile sout(pathName);
                ByteArray plain(32, true);
                size_t error = 0;
                Get(plain, indexNumber, error);
                if (error == 0) {
                        if (!sout.open(QIODevice::WriteOnly)){
                                return false;
                        }
                        size_t length = plain.Size();
                        size_t position = 0;
                        size_t rest = length;
                        size_t index = 0;
                        ByteArray b(128);
                        unsigned char c;
                        sout.write("  decimal Index  hexIndex  Content(hexadezimal)\r\n", 49);
                        while (rest > 15) {
                                b.Clear();
                                index = 0;
                                b.IntegerToText(index, static_cast<int>(position), 15, true);
                                b.Append(' ');
                                index++;
                                b.Append(' ');
                                index++;
                                b.IntegerToHexText(index, static_cast<int>(position), 8);
                                b.Append(' ');
                                index++;
                                b.Append(' ');
                                index++;
                                for (size_t i = 0; i < 16; i++) {
                                        c = static_cast<unsigned char>((plain)[position + i]);
                                        b.IntegerToHexText(index, static_cast<int>(c), 2);
                                        b.Append(' ');
                                        index++;
                                }
                                b.Append(' ');
                                for (size_t i = 0; i < 16; i++) {
                                        c = static_cast<unsigned char>((plain)[position + i]);
                                        if (c < 32) {
                                                b.Append('.');
                                        } else {
                                                if (c < 127) {
                                                        b.Append(c);
                                                } else {
                                                        b.Append('.');
                                                }
                                        }
                                        index++;
                                }
                                b.Append('\r');
                                b.Append('\n');
                                sout.write(&b[0], b.Size());
                                position = position + 16;
                                rest = rest - 16;
                        }
                        if (rest > 0) {
                                b.Clear();
                                index = 0;
                                b.IntegerToText(index, static_cast<int>(position), 15, true);
                                b.Append(' ');
                                index++;
                                b.Append(' ');
                                index++;
                                b.IntegerToHexText(index, static_cast<int>(position), 8);
                                b.Append(' ');
                                index++;
                                b.Append(' ');
                                index++;
                                for (size_t i = 0; i < rest; i++) {
                                        c = static_cast<unsigned char>((plain)[position + i]);
                                        b.IntegerToHexText(index, static_cast<int>(c), 2);
                                        b.Append(' ');
                                        index++;
                                }
                                for (size_t i = 0; i < (16 - rest); i++) {
                                        b.Append("   ");
                                }
                                b.Append(' ');
                                for (size_t i = 0; i < rest; i++) {
                                        c = static_cast<unsigned char>((plain)[position + i]);
                                        if (c < 32) {
                                                b.Append('.');
                                        } else {
                                                if (c < 127) {
                                                        b.Append(c);
                                                } else {
                                                        b.Append('.');
                                                }
                                        }
                                        index++;
                                }
                                b.Append('\r');
                                b.Append('\n');
                                sout.write(&b[0], b.Size());
                        }
                        sout.close();
                        return true;
                } else {
                        return false;
                }
        }



        /*! \brief Diese Methode `DumpStruct()` erstellt einen Dump mit der Struktur dieses SecureFiles in einer separaten Datei.
         *
         * Die Methode versucht zunächst die übergebene Datei zum Schreiben zu öffnen.
         * Wenn dies nicht möglich ist, wird mit _false_ zurückgekehrt.
         * Danach wird die Struktur des SecureFiles in die Datei geschrieben und die Datei wieder geschlossen.
         * Die Funktion gibt _true_ zurück, wenn alles fehlerfrei verarbeitet werden konnte, andernfalls _false_.
         *
         * Nachfolgend ein Beispielausdruck eines Dumps:
         *
         *      IndexNumber   FileIndex BlockLength ChangeKeys
         *              258     4844912        1920
         *             1024         816       57952
         *             1025       58768       57440
         *             1026      116208       56672
         *             1027      172880       54880
         *             1028      227760       56160
         *             1029      283920       54880
         *             1030      338800       56160
         *             1031      394960       54880
         *             1032      449840       56672
         *             1033      506512       54368
         *             1034      560880       55904
         *             1035      616784       50528
         *             1036      667312       55648
         *             1037      722960       56672
         *             1038      779632       54880
         *             1039      834512       57184
         *             1040      891696       55648
         *             1041      947344       54880
         *             1042     1002224       53856
         *             1043     1056080       61792
         *             1044     1117872       55648
         *             1045     1173520       53856
         *             1046     1227376       54112
         *             1047     1281488       57696
         *             1048     1339184       56672
         *             1049     1395856       56160
         *             1050     1452016       56672
         *             1051     1508688       57184
         *             1052     1565872       54368
         *             1053     1620240       57696
         *             1054     1677936       55904
         *             1055     1733840       62304
         *             1056     1796144       57440
         *             1057     1853584       57440
         *             1058     1911024       54368
         *             1059     1965392       56416
         *             1060     2021808       53856
         *             1061     2075664       55392
         *             1062     2131056       57952
         *             1063     2189008       56928
         *             1064     2245936       57440
         *             1065     2303376       53600
         *             1066     2356976       55392
         *             1067     2412368       57952
         *             1068     2470320       57184
         *             1069     2527504       57696
         *             1070     2585200       58720
         *             1071     2643920       54112
         *             1072     2698032       56160
         *             1073     2754192       57696
         *             1074     2811888       58976
         *             1075     2870864       57440
         *             1076     2928304       56672
         *             1077     2984976       56928
         *             1078     3041904       58464
         *             1079     3100368       55136
         *             1080     3155504       57440
         *             1081     3212944       55392
         *             1082     3268336       57184
         *             1083     3325520       57184
         *             1084     3382704       56672
         *             1085     3439376       58976
         *             1086     3498352       55392
         *             1087     3553744       58208
         *             1088     3611952       55136
         *             1089     3667088       54880
         *             1090     3721968       56416
         *             1091     3778384       56416
         *             1092     3834800       53856
         *             1093     3888656       56928
         *             1094     3945584       54112
         *             1095     3999696       53600
         *             1096     4053296       54624
         *             1097     4107920       58976
         *             1098     4166896       58464
         *             1099     4225360       57952
         *             1100     4283312       57952
         *             1101     4341264       58720
         *             1102     4399984       55648
         *             1103     4455632       55392
         *             1104     4511024       52320
         *             1105     4563344       56672
         *             1106     4620016       56416
         *             1107     4676432       55392
         *             1108     4731824       55904
         *             1111     4787728       57184
         *
         *        FileIndex BlockLength IndexNumber ChangeKeys
         *                0         352      checkArray1
         *              352         352      checkArray2
         *              704         112      controlArray
         *              816       57952        1024
         *            58768       57440        1025
         *           116208       56672        1026
         *           172880       54880        1027
         *           227760       56160        1028
         *           283920       54880        1029
         *           338800       56160        1030
         *           394960       54880        1031
         *           449840       56672        1032
         *           506512       54368        1033
         *           560880       55904        1034
         *           616784       50528        1035
         *           667312       55648        1036
         *           722960       56672        1037
         *           779632       54880        1038
         *           834512       57184        1039
         *           891696       55648        1040
         *           947344       54880        1041
         *          1002224       53856        1042
         *          1056080       61792        1043
         *          1117872       55648        1044
         *          1173520       53856        1045
         *          1227376       54112        1046
         *          1281488       57696        1047
         *          1339184       56672        1048
         *          1395856       56160        1049
         *          1452016       56672        1050
         *          1508688       57184        1051
         *          1565872       54368        1052
         *          1620240       57696        1053
         *          1677936       55904        1054
         *          1733840       62304        1055
         *          1796144       57440        1056
         *          1853584       57440        1057
         *          1911024       54368        1058
         *          1965392       56416        1059
         *          2021808       53856        1060
         *          2075664       55392        1061
         *          2131056       57952        1062
         *          2189008       56928        1063
         *          2245936       57440        1064
         *          2303376       53600        1065
         *          2356976       55392        1066
         *          2412368       57952        1067
         *          2470320       57184        1068
         *          2527504       57696        1069
         *          2585200       58720        1070
         *          2643920       54112        1071
         *          2698032       56160        1072
         *          2754192       57696        1073
         *          2811888       58976        1074
         *          2870864       57440        1075
         *          2928304       56672        1076
         *          2984976       56928        1077
         *          3041904       58464        1078
         *          3100368       55136        1079
         *          3155504       57440        1080
         *          3212944       55392        1081
         *          3268336       57184        1082
         *          3325520       57184        1083
         *          3382704       56672        1084
         *          3439376       58976        1085
         *          3498352       55392        1086
         *          3553744       58208        1087
         *          3611952       55136        1088
         *          3667088       54880        1089
         *          3721968       56416        1090
         *          3778384       56416        1091
         *          3834800       53856        1092
         *          3888656       56928        1093
         *          3945584       54112        1094
         *          3999696       53600        1095
         *          4053296       54624        1096
         *          4107920       58976        1097
         *          4166896       58464        1098
         *          4225360       57952        1099
         *          4283312       57952        1100
         *          4341264       58720        1101
         *          4399984       55648        1102
         *          4455632       55392        1103
         *          4511024       52320        1104
         *          4563344       56672        1105
         *          4620016       56416        1106
         *          4676432       55392        1107
         *          4731824       55904        1108
         *          4787728       57184        1111
         *          4844912        1920         258
         *          4846832          96      freeIndexes
         *          4846928         944      items
         *          4847872                  DateiEnde
         *
         *      NextFreeIndex:     1112
         *
         *      freie IndexNr
         *       1109
         *       1110
         *
         *
         * \note Diese Methode ist nicht für den Wirkbetrieb gedacht. Sie kann aber bei Programmentwicklung manchmal sehr hilfreich sein.
         * \param pathName Pfad und Dateiname der Datei, in die die Daten ausgegeben werden sollen.
         * \return _true_, wenn kein Fehler aufgetreten ist, andernfalls _false_.
         * \sa DumpRecord()
         */
        bool SecureFile::DumpStruct(QString &pathName) {
                void *p = &pathName;
                if ((p == nullptr) || (pathName.isEmpty())) {
                        return false;
                }
                if (!openFlag) {
                        return false;
                }
                QFile sout(pathName);
                if (!sout.open(QIODevice::WriteOnly)){
                        return false;
                }
                ByteArray b(128);
                size_t index = 0;
                sout.write(" IndexNumber   FileIndex BlockLength ChangeKeys\r\n", 49);
                items->SetKeyIndex(0);
                items->First();
                while (items->Row() != nullptr) {
                        b.Clear();
                        index = 0;
                        b.IntegerToText(index, static_cast<int>(items->Row()->indexNumber), 12);
                        b.IntegerToText(index, static_cast<int>(items->Row()->fileIndex), 12);
                        b.IntegerToText(index, static_cast<int>(items->Row()->arrayLength), 12);
                        // if (items->Row()->changeKeys) {
                        //         b.Copy("      True \r\n", 0, index, 13);
                        // } else {
                        //         b.Copy("      False\r\n", 0, index, 13);
                        // }
                        b.Copy("           \r\n", 0, index, 13);
                        sout.write(&b[0], b.Size());
                        items->Next();
                }
                sout.write("\r\n   FileIndex BlockLength IndexNumber ChangeKeys\r\n", 51);
                sout.write("           0         352      checkArray1      \r\n", 49);
                sout.write("         352         352      checkArray2      \r\n", 49);
                sout.write("         704         112      controlArray     \r\n", 49);
                size_t fileIndex = 816;
                items->SetKeyIndex(1);
                items->First();
                while (items->Row() != nullptr) {
                        if (fileIndex == items->Row()->fileIndex) {
                                b.Clear();
                                index = 0;
                                b.IntegerToText(index, static_cast<int>(items->Row()->fileIndex), 12);
                                b.IntegerToText(index, static_cast<int>(items->Row()->arrayLength), 12);
                                b.IntegerToText(index, static_cast<int>(items->Row()->indexNumber), 12);
                                // if (items->Row()->changeKeys) {
                                //         b.Copy("      True \r\n", 0, index, 13);
                                // } else {
                                //         b.Copy("      False\r\n", 0, index, 13);
                                // }
                                b.Copy("           \r\n", 0, index, 13);
                                sout.write(&b[0], b.Size());
                                fileIndex = fileIndex + items->Row()->arrayLength;
                                items->Next();
                        } else {
                                b.Clear();
                                index = 0;
                                size_t freeLength = items->Row()->fileIndex - fileIndex;
                                b.IntegerToText(index, static_cast<int>(fileIndex), 12);
                                b.IntegerToText(index, static_cast<int>(freeLength), 12);
                                b.Copy(" Frei   Frei\r\n", 0, index, 14);
                                sout.write(&b[0], b.Size());
                                fileIndex = fileIndex + freeLength;
                        }
                }
                b.Clear();
                index = 0;
                b.IntegerToText(index, static_cast<int>(userDataEnd), 12);
                b.IntegerToText(index, static_cast<int>(freeIndexesLng), 12);
                b.Copy("      freeIndexes      \r\n", 0, index, 25);
                sout.write(&b[0], b.Size());
                size_t newend = userDataEnd + freeIndexesLng;
                b.Clear();
                index = 0;
                b.IntegerToText(index, static_cast<int>(newend), 12);
                b.IntegerToText(index, static_cast<int>(itemsLng), 12);
                b.Copy("      items            \r\n", 0, index, 25);
                sout.write(&b[0], b.Size());
                newend = newend + itemsLng;
                b.Clear();
                index = 0;
                b.IntegerToText(index, static_cast<int>(newend), 12);
                b.Copy("                  DateiEnde        \r\n", 0, index, 37);
                sout.write(&b[0], b.Size());
                sout.write("\r\nNextFreeIndex:", 16);
                b.Clear();
                index = 0;
                b.IntegerToText(index, static_cast<int>(nextFreeIndex), 9);
                b.Append('\r');
                b.Append('\n');
                sout.write(&b[0], b.Size());
                sout.write("\r\nfreie IndexNr\r\n", 17);
                freeIndexes->First();
                while (freeIndexes->Row() != nullptr) {
                        b.Clear();
                        index = 0;
                        b.IntegerToText(index, static_cast<int>(freeIndexes->Row()->indexNumber), 13);
                        b.Append('\r');
                        b.Append('\n');
                        sout.write(&b[0], b.Size());
                        freeIndexes->Next();
                }
                sout.write("\r\n", 2);
                sout.close();
                return true;
        }


        /*! \brief Diese Methode `First()` positioniert auf den ersten gespeicherten verschlüsselten Datenblock.
         *
         * Setzt den Cursor auf den ersten gespeicherten verschlüsselten Datenblock und gibt dessen Indexnummer zurück.
         *
         * \return Die Indexnummer des ersten gespeicherten verschlüsselten Datenblocks oder 0, falls nichts gespeichert ist.
         * \sa Next()
         */
        size_t SecureFile::First() const {
                items->SetKeyIndex(0);
                items->First();
                if (items->Row() != nullptr) {
                        return items->Row()->indexNumber;
                } else {
                        return 0;
                }
        }

        /*! \brief Diese Methode `Get()` überträgt einen verschlüsselten Datenblock im Klartext in die ByteArray-Instanz unter _ref_.
         *
         * Gibt den verschlüsselten Datenblock mit der übergebenen Indexnummer aus dem SecureSore im Klartext zurück.
         *
         *      Mögliche Fehlernummern:
         *      10:  Das SecureFile war nicht geöffnet!
         *      17:  In der Datei des SecureFiles konnte nicht positioniert werden!
         *      18:  In der Datei des SecureFiles konnte nicht gelesen werden!
         *      21:  Die übergebene Indexnummer existiert im SecureFile nicht!
         *      27:  Ein Datenblock konnte nicht fehlerfrei entschlüsselt werden!
         *
         * \param ref Verweis auf eine ByteArray-Instanz, in die der Klartext geschrieben werden soll.
         * \param indexNumber Indexnummer des Datenblockes, der aus dem SecureSore ausgelesen werden soll.
         * \param error Verweis auf eine Fehlernummer (0, wenn alles in Ordnung ist).
         * \sa Put() und Delete()
         */
        void SecureFile::Get(ByteArray &ref, const size_t indexNumber, size_t &error) {
                if (!openFlag) {
                        error = 10;
                        return;
                }
                items->SetKeyIndex(0);
                items->SearchRow()->indexNumber = indexNumber;
                if (items->Seek(true)) {
                        if (!file0->seek(static_cast<qint64>(items->Row()->fileIndex))) {
                                error = 17;
                                return;
                        }
                        size_t length = items->Row()->arrayLength;
                        ByteArray *a = &ref;
                        if (file0->read(&a->WriteareaReferenz(0, static_cast<size_t>(length)), static_cast<qint64>(length)) != static_cast<qint64>(length)) {
                                error = 18;
                                return;
                        }
                } else {
                        error = 21;
                        return;
                }
                if (Decrypt12(ref, false) == nullptr) {
                        error = 27;
                }
        }

        /*! \brief Diese Methode `Get()` gibt einen verschlüsselten Datenblock im Klartext zurück.
         *
         * Gibt den verschlüsselten Datenblock mit der übergebenen Indexnummer aus dem SecureSore im Klartext zurück.
         *
         *      Mögliche Fehlernummern:
         *      10:  Das SecureFile war nicht geöffnet!
         *      17:  In der Datei des SecureFiles konnte nicht positioniert werden!
         *      18:  In der Datei des SecureFiles konnte nicht gelesen werden!
         *      21:  Die übergebene Indexnummer existiert im SecureFile nicht!
         *      27:  Ein Datenblock konnte nicht fehlerfrei entschlüsselt werden!
         *
         * \note Der Benutzer muss den zurückgegebenen Datenblock selbst freigeben.
         * \param indexNumber Indexnummer des Datenblockes, der aus dem SecureSore ausgelesen werden soll.
         * \param error Verweis auf eine Fehlernummer (0, wenn alles in Ordnung ist).
         * \return Zeiger auf eine ByteArray-Instanz, die den Klartext des gesuchten Datenblockes enthält, oder _nullptr_, falls ein Fehler aufgetreten ist.
         * \sa Put() und Delete()
         */
        ByteArray *SecureFile::Get(const size_t indexNumber, size_t &error) {
                ByteArray *result = new ByteArray(32, true);
                if (!openFlag) {
                        error = 10;
                }
                items->SetKeyIndex(0);
                items->SearchRow()->indexNumber = indexNumber;
                if (items->Seek(true)) {
                        if (!file0->seek(static_cast<qint64>(items->Row()->fileIndex))) {
                                error = 17;
                        }
                        size_t length = items->Row()->arrayLength;
                        if (file0->read(&result->WriteareaReferenz(0, static_cast<size_t>(length)), static_cast<qint64>(length)) != static_cast<qint64>(length)) {
                                error = 18;
                        }
                } else {
                        error = 21;
                }
                if (Decrypt12(*result, false) == nullptr) {
                        error = 27;
                }
                if (error > 0) {
                        delete result;
                        return nullptr;
                } else {
                        return result;
                }
        }

        /*! \brief Diese Methode `NewStore()` erstellt ein neues SecureFile.
         *
         * Mit der Methode `NewStore()` kann ein neues SecureFile in einer noch nicht vorhandenen Datei angelegt werden. Bei der Neuanlage werden zwei
         * Initialisierungsvektoren (IV) und zwei Schlüssel festgelegt, mit denen das SecureFile später arbeiten soll.
         *
         * Die Methode überprüft die übergebenen Parameter und versucht dann eine neue Datei für das neue SecureFile anzulegen.
         *
         * Bei der Neuanlage eines SecureFiles wird die ByteArray-Instanz _checkArray1_ mit 256 zufälligen Bytes gefüllt.
         * Diese 256 Zufallsbytes werden auch in die ByteArray-Instanz _checkArray2_ kopiert.
         * Dann werden die Zufallsbytes durch einen angehängten Hash-Wert (SHA512) vor nachträglichen Manipulationen geschützt.
         * Mit dem übergebenen ersten Initialisierungsvektor und dem ersten Schlüssel werden die Zufallsbytes mitsamt dem angehängten Hashwert (320 Bytes) nun
         * mit dem AES-Algoritmus zum ersten mal verschlüsselt. Das ByteArray ist nun 336 Bytes lang.
         * Mit dem übergebenen zweiten Initialisierungsvektor und dem zweiten Schlüssel werden diese 336 Bytes erneut mit dem AES-Algoritmus verschlüsselt.
         * Die so entstandenen 352 Bytes werden am Anfang der Datei des SecureFiles gespeichert.
         *
         * Auch in der ByteArray-Instanz _checkArray2_ werden die Zufallsbytes durch einen angehängten Hash-Wert (SHA512) vor nachträglichen Manipulationen geschützt.
         * Außerdem wird ein SHA384-Hash-Wert von den 256 Zufallsbytes ermittelt.
         * Aus dem SHA512-Hash-Wert werden die ersten 16 Byte als erster Initialisierungsvektor verwendet.
         * Die nächsten 32 Byte werden als erster Schlüssel verwendet. Die restlichen 16 Byte des SHA512-Hash-Wert bleiben ungenutzt.
         * Mit diesem ersten Initialisierungsvektor und diesem ersten Schlüssel werden die Zufallsbytes mitsamt dem angehängten Hashwert (320 Bytes) nun
         * mit dem AES-Algoritmus zum ersten mal verschlüsselt. Das ByteArray ist nun 336 Bytes lang.
         * Als zweiter Initialisierungsvektor werden die ersten 16 Byte des SHA384-Hash-Wertes benutzt.
         * Die nächsten 32 Byte werden als zweiter Schlüssel verwendet.
         * Mit diesem zweiten Initialisierungsvektor und diesem zweiten Schlüssel werden diese 336 Bytes erneut mit dem AES-Algoritmus verschlüsselt.
         * Die so entstandenen 352 Bytes werden ab dem Index 352 in der Datei des SecureFiles gespeichert.
         *
         * Mit den so zweifach gesicherten Zufallsbytes kann die Methode `Open()` später sicher überprüfen, ob der Benutzer die richtigen Initialisierungsvektoren
         * und Schlüssel kennt, wenn er das SecureFile öffnen und mit ihm arbeiten will.
         *
         * Nach den beiden Check-Arrays wird der Kontrollblock (_controlArray_) ab dem Dateiindex 704 gespeichert. Der Kontrollblock ist im Klartext 16 Byte lang.
         * Mit dem angehängtem Hashwert und der doppelten Verschlüsselung belegt er 112 Byte in der Datei. Er steht immer an dieser Stelle in der Datei.
         *
         * Nach dem Kontrollblock werden später die Nutzerdatenblöcke lückenlos platziert.
         *
         * Bei der Neuerstellung des SecureFiles folgen nun die exportierten Daten der Tabelleninstanz (der Klasse __FreeIndexTable__) zur Verwaltung der nicht zugewiesenen Indexwerte.
         * Da diese Tabelle noch leer ist bestehen die exportierten Daten im Klartext nur aus einem Byte.
         * Mit dem angehängtem Hashwert und der doppelten Verschlüsselung belegt der Block 96 Byte in der Datei.
         * Danach fogen nun die exportierten Daten der Tabelleninstanz (der Klasse __ItemTable__) zur Verwaltung der Nutzerdatenblöcke.
         * Da auch diese Tabelle noch leer ist bestehen die exportierten Daten im Klartext nur aus einem Byte.
         * Mit dem angehängtem Hashwert und der doppelten Verschlüsselung belegt der Block ebenfalls 96 Byte in der Datei.
         * Das leere SecureFile ist damit nach seiner Neuanlage 1.008 Byte lang.
         *
         *      Mögliche Fehlernummern:
         *      11:  Ein SecureFile war schon geöffnet!
         *      12:  Ein Dateiname ist nicht angegeben!
         *      13:  Die angegebene Datei existiert bereits!
         *      15:  Die angegebene Datei konnte nicht geöffnet werden!
         *      19:  In der Datei des SecureFiles konnte nicht geschrieben werden!
         *      22:  Der erste Initialisierungsvektor ist nicht angegeben oder hat eine falsche Länge!
         *      23:  Der erste Schlüssel ist nicht angegeben oder hat eine falsche Länge!
         *      24:  Der zweite Initialisierungsvektor ist nicht angegeben oder hat eine falsche Länge!
         *      25:  Der zweite Schlüssel ist nicht angegeben oder hat eine falsche Länge!
         *      26:  Ein Datenblock konnte nicht fehlerfrei verschlüsselt werden!
         *
         * \param name Zeiger auf eine QString-Instanz, die den Pfad und den Dateinamen enthält, in dem das SecureFile angelegt werden soll.
         * \param iv1 Zeiger auf einen 128 Bit langen Initialisierungsvektor für die erste AES-Verschlüsselung.
         * \param key1 Zeiger auf einen 256 Bit langen Schlüssel für die erste AES-Verschlüsselung.
         * \param iv2 Zeiger auf einen 128 Bit langen Initialisierungsvektor für die zweite AES-Verschlüsselung.
         * \param key2 Zeiger auf einen 256 Bit langen Schlüssel für die zweite AES-Verschlüsselung.
         * \return Fehlernummer (0 wenn alles ok ist).
         */
        size_t SecureFile::NewStore(QString *name, ByteArray *iv1, ByteArray *key1, ByteArray *iv2, ByteArray *key2) {
                if (openFlag) {
                        return 11;
                }
                if ((name == nullptr) || (name->isEmpty())) {
                        return 12;
                }
                if ((iv1 == nullptr) || (iv1->Size() != 16)) {
                        return 22;
                } else {
                        aes1->SetIV(*iv1);
                }
                if ((key1 == nullptr) || (key1->Size() != 32)) {
                        return 23;
                } else {
                        aes1->SetKey(*key1);
                }
                if ((iv2 == nullptr) || (iv2->Size() != 16)) {
                        return 24;
                } else {
                        aes2->SetIV(*iv2);
                }
                if ((key2 == nullptr) || (key2->Size() != 32)) {
                        return 25;
                } else {
                        aes2->SetKey(*key2);
                }
                QString fname;
                fname.append(*name);
                QFile newfile(fname);
                if (newfile.exists()) {
                        return 13;
                }
                if (!newfile.open(QIODevice::WriteOnly)) {
                        return 15;
                }
                checkArray1->Clear();
                Random rand;
                rand.GetRandomBytes(*checkArray1, 256);
                checkArray2->Clear();
                checkArray2->Append(*checkArray1);
                ByteArray hash(64, true);
                sha512->GetHash(*checkArray1, hash);
                ByteArray ivKey(32, true);
                ivKey.Copy(hash, 0, 0, 16);
                aes3->SetIV(ivKey);
                ivKey.Clear();
                ivKey.Copy(hash, 16, 0, 32);
                aes3->SetKey(ivKey);
                sha384->GetHash(*checkArray1, hash);
                ivKey.Clear();
                ivKey.Copy(hash, 0, 0, 16);
                aes4->SetIV(ivKey);
                ivKey.Clear();
                ivKey.Copy(hash, 16, 0, 32);
                aes4->SetKey(ivKey);
                ByteArray *encrypt1 = Encrypt12(*checkArray1, false);
                if (encrypt1 == nullptr) {
                        return 26;
                }
                ByteArray *encrypt2 = Encrypt34(*checkArray2, false);
                if (encrypt2 == nullptr) {
                        return 26;
                }
                size_t index = 0;
                controlArray->Clear();
                controlArray->WriteNumber(index, 1024, 4);      // next Item-Index
                controlArray->WriteNumber(index, 816, 4);       // FileIndexEnd
                freeIndexes->ClearAll();
                ByteArray decFreeIndexes(128);
                freeIndexes->ExportToByteArray(decFreeIndexes);
                ByteArray *encFreeIndexes = Encrypt12(decFreeIndexes, false);
                if (encFreeIndexes == nullptr) {
                        return 26;
                }
                items->ClearAll();
                ByteArray decItems(128);
                items->ExportToByteArray(decItems);

                ByteArray *encItems = Encrypt12(decItems, false);
                if (encItems == nullptr) {
                        return 26;
                }
                controlArray->WriteNumber(index, static_cast<int>(encFreeIndexes->Size()), 4);  // FreeIndexesLength
                controlArray->WriteNumber(index, static_cast<int>(encItems->Size()), 4);        // ItemListLength
                ByteArray *encControlArray = Encrypt12(*controlArray, false);
                if (encControlArray == nullptr) {
                        return 26;
                }
                if (newfile.write(&(*checkArray1)[0], 352) != 352) {
                        return 19;
                }
                if (newfile.write(&(*checkArray2)[0], 352) != 352) {
                        return 19;
                }
                if (newfile.write(&(*controlArray)[0], 112) != 112) {
                        return 19;
                }
                if (newfile.write(&(*encFreeIndexes)[0], static_cast<qint64>(encFreeIndexes->Size())) != static_cast<qint64>(encFreeIndexes->Size())) {
                        return 19;
                }
                if (newfile.write(&(*encItems)[0], static_cast<qint64>(encItems->Size())) != static_cast<qint64>(encItems->Size())) {
                        return 19;
                }
                newfile.close();
                return 0;
        }


        /*! \brief Diese Methode `Next()` positioniert auf den nächsten gespeicherten verschlüsselten Datenblock.
         *
         * Setzt den Cursor auf den nächsten gespeicherten verschlüsselten Datenblock und gibt dessen Indexnummer zurück.
         * Wurde das Ende überschritten, wird 0 zurückgegeben.
         *
         * \return Die Indexnummer des nächsten gespeicherten verschlüsselten Datenblocks oder 0, falls das Ende überschritten wurde.
         * \sa First()
         */
        size_t SecureFile::Next() const {
                items->SetKeyIndex(0);
                items->Next();
                Items *s = items->Row();
                if (s != nullptr) {
                        return s->indexNumber;
                } else {
                        return 0;
                }
        }


        /*! \brief Diese Methode `Open()` öffnet ein vorhandenes SecureFile.
         *
         * Die Methode überprüft ob alle benötigten Parameter angegeben sind und die richtige Länge haben.
         *
         * Die ersten 352 Byte werden aus der Datei des SecureFiles gelesen. Danach wird versucht diese 352 Byte mit dem zweiten Initialisierungsvekter und dem zweiten übergebenen Schlüssel
         * zu entschlüsseln. Wenn dies nicht funktioniert, wird mit Fehlernummer 27 abgebrochen. Andernfalls bleiben 336 Bytes Nutzdaten übrig. Es wird versucht diese 336 Byte Nutzdaten mit
         * dem ersten Initialisierungsvekter und dem ersten übergebenen Schlüssel zu entschlüsseln. Wenn dies nicht funktioniert, wird mit Fehlernummer 27 abgebrochen. Andernfalls bleiben
         * 320 Bytes Nutzdaten übrig, die aus 256 Byte Zufallsdaten mit angehängten 64 Byte Hashwert besten. Es wird überprüft, ob der SHA512-Hashwert der Zufallsdaten mit dem angehängten
         * Hashwert übereinstimmt. Wenn nein, wird mit Fehlernummer 27 abgebrochen. Nun wird auch der SHA384-Hashwert der Zufallsdaten ermittelt.
         *
         * Die nächsten 352 Byte werden aus der Datei des SecureFiles gelesen. Danach wird versucht diese 352 Byte mit den ersten 16 Byte des SHA384-Hashwertes (als Initialisierungsvekter)
         * und den folgenden 32 Byte des SHA384-Hashwertes (als Schlüssel) zu entschlüsseln. Wenn dies nicht funktioniert, wird mit Fehlernummer 27 abgebrochen.
         * Andernfalls bleiben 336 Bytes Nutzdaten übrig. Es wird versucht diese 336 Byte Nutzdaten mit den ersten 16 Byte des SHA512-Hashwertes (als Initialisierungsvekter)
         * und den folgenden 32 Byte des SHA512-Hashwertes (als Schlüssel) zu entschlüsseln. Wenn dies nicht funktioniert, wird mit Fehlernummer 27 abgebrochen.
         * Andernfalls bleiben 336 Bytes Nutzdaten übrig, die aus 256 Byte Zufallsdaten mit angehängten 64 Byte Hashwert besten. Stimmen die ersten 256 Byte aus diesem Block (Byte für Byte)
         * mit den 256 Byte aus dem ersten Block überein, so müssen dem Benutzer die richtigen Initialisierungvektoren und Schlüssel (die bei der Neuanlage des SecureFiles benutzt wurden)
         * bekannt gewesen sein.
         *
         * Als nächstes wird der 112 Byte lange Kontrollblock eingelesen und entschlüsselt. Wenn dies fehlerfrei funktioniert, werden auch die Blöcke der beiden Tabellen eingelesen,
         * entschlüsselt und in die Tabellen importiert. Wenn auch dies fehlerfrei funktioniert, wird das SecureFile eröffnet und die Variable _openFlag_ auf _true_ gesetzt.
         *
         *      Mögliche Fehlernummern:
         *      11:  Ein SecureFile war schon geöffnet!
         *      12:  Ein Dateiname ist nicht angegeben!
         *      14:  Die angegebene Datei existiert nicht!
         *      15:  Die angegebene Datei konnte nicht geöffnet werden!
         *      16:  Das SecureFile konnte nicht geöffnet werden!
         *      17:  In der Datei des SecureFiles konnte nicht positioniert werden!
         *      18:  In der Datei des SecureFiles konnte nicht gelesen werden!
         *      22:  Der erste Initialisierungsvektor ist nicht angegeben oder hat eine falsche Länge!
         *      23:  Der erste Schlüssel ist nicht angegeben oder hat eine falsche Länge!
         *      24:  Der zweite Initialisierungsvektor ist nicht angegeben oder hat eine falsche Länge!
         *      25:  Der zweite Schlüssel ist nicht angegeben oder hat eine falsche Länge!
         *      27:  Ein Datenblock konnte nicht fehlerfrei entschlüsselt werden!
         *
         * \param name Zeiger auf eine QString-Instanz, die den Pfad und den Dateinamen enthält, dessen SecureFile geöffnet werden soll.
         * \param iv1 Zeiger auf einen 128 Bit langen Initialisierungsvektor für die erste AES-Verschlüsselung.
         * \param key1 Zeiger auf einen 256 Bit langen Schlüssel für die erste AES-Verschlüsselung.
         * \param iv2 Zeiger auf einen 128 Bit langen Initialisierungsvektor für die zweite AES-Verschlüsselung.
         * \param key2 Zeiger auf einen 256 Bit langen Schlüssel für die zweite AES-Verschlüsselung.
         * \return Fehlernummer (0 wenn alles ok ist).
         * \sa Close()
         */
        size_t SecureFile::Open(QString *name, ByteArray *iv1, ByteArray *key1, ByteArray *iv2, ByteArray *key2) {
                if (openFlag) {
                        return 11;
                }
                if ((name == nullptr) || (name->isEmpty())) {
                        return 12;
                }
                if ((iv1 == nullptr) || (iv1->Size() != 16)) {
                        return 22;
                } else {
                        aes1->SetIV(*iv1);
                }
                if ((key1 == nullptr) || (key1->Size() != 32)) {
                        return 23;
                } else {
                        aes1->SetKey(*key1);
                }
                if ((iv2 == nullptr) || (iv2->Size() != 16)) {
                        return 24;
                } else {
                        aes2->SetIV(*iv2);
                }
                if ((key2 == nullptr) || (key2->Size() != 32)) {
                        return 25;
                } else {
                        aes2->SetKey(*key2);
                }
                file0 = new QFile(*name);
                if (!file0->exists()) {
                        delete file0;
                        file0 = nullptr;
                        return 14;
                }
                if (!file0->open(QIODevice::ReadWrite)) {
                        delete file0;
                        file0 = nullptr;
                        return 15;
                }
                if (file0->read(&(checkArray1->WriteareaReferenz(0, 352)), 352) != 352) {
                        delete file0;
                        file0 = nullptr;
                        return 18;
                }
                if (file0->read(&(checkArray2->WriteareaReferenz(0, 352)), 352) != 352) {
                        delete file0;
                        file0 = nullptr;
                        return 18;
                }
                ByteArray *decrypt1 = Decrypt12(*checkArray1, false);
                if (decrypt1 == nullptr) {
                        delete file0;
                        file0 = nullptr;
                        return 27;
                }
                ByteArray hash(64, true);
                sha512->GetHash(*checkArray1, hash);
                ByteArray ivKey(32, true);
                ivKey.Copy(hash, 0, 0, 16);
                aes3->SetIV(ivKey);
                ivKey.Clear();
                ivKey.Copy(hash, 16, 0, 32);
                aes3->SetKey(ivKey);
                sha384->GetHash(*checkArray1, hash);
                ivKey.Clear();
                ivKey.Copy(hash, 0, 0, 16);
                aes4->SetIV(ivKey);
                ivKey.Clear();
                ivKey.Copy(hash, 16, 0, 32);
                aes4->SetKey(ivKey);
                ByteArray *decrypt2 = Decrypt34(*checkArray2, false);
                if (decrypt2 == nullptr) {
                        delete file0;
                        file0 = nullptr;
                        return 27;
                }
                for (size_t i = 0; i < 256; i++) {
                        unsigned char c1 = static_cast<unsigned char>((*decrypt1)[i]);
                        unsigned char c2 = static_cast<unsigned char>((*checkArray2)[i]);
                        if (c1 != c2) {
                                file0->close();
                                delete file0;
                                file0 = nullptr;
                                return 16;
                        }
                }
                if (file0->read(&(controlArray->WriteareaReferenz(0, 112)), 112) != 112) {
                        delete file0;
                        file0 = nullptr;
                        return 18;
                }
                checkArray1->Clear();
                checkArray2->Clear();
                ByteArray *decrypt3 = Decrypt12(*controlArray, false);
                if (decrypt3 == nullptr) {
                        delete file0;
                        file0 = nullptr;
                        return 27;
                }
                size_t index = 0;
                nextFreeIndex = static_cast<size_t>(controlArray->ReadNumber(index, 4));
                userDataEnd = static_cast<size_t>(controlArray->ReadNumber(index, 4));
                freeIndexesLng = controlArray->ReadNumber(index, 4);
                itemsLng = controlArray->ReadNumber(index, 4);
                if (userDataEnd > 816) {
                        if (!file0->seek(userDataEnd)) {
                                delete file0;
                                file0 = nullptr;
                                return 17;
                        }
                }
                ByteArray *encrypt = new ByteArray(8192);
                if (file0->read(&(encrypt->WriteareaReferenz(0, static_cast<size_t>(freeIndexesLng))), freeIndexesLng) != freeIndexesLng) {
                        delete encrypt;
                        delete file0;
                        file0 = nullptr;
                        return 18;
                }
                decrypt3 = Decrypt12(*encrypt, false);
                if (decrypt3 == nullptr) {
                        delete encrypt;
                        delete file0;
                        file0 = nullptr;
                        return 27;
                }
                freeIndexes->ClearAll();
                freeIndexes->ImportFromByteArray(*decrypt3, false);
                if (file0->read(&(encrypt->WriteareaReferenz(0, static_cast<size_t>(itemsLng))), itemsLng) != itemsLng) {
                        delete encrypt;
                        delete file0;
                        file0 = nullptr;
                        return 18;
                }
                decrypt3 = Decrypt12(*encrypt, false);
                if (decrypt3 == nullptr) {
                        delete encrypt;
                        delete file0;
                        file0 = nullptr;
                        return 27;
                }
                items->ClearAll();
                items->ImportFromByteArray(*decrypt3, false);
                delete encrypt;
                openFlag = true;
                return 0;
        }


        /*! \brief Diese Methode `Put()` schreibt einen Klartextblock verschlüsselt in das SecureFile.
         *
         * Schreibt einen (im Klartext angegebenen) Datenblock in verschlüsselter Form in das SecureFile.
         * Wird eine gültige Indexnummer angegeben (>256), wird der Block unter dieser Nummer gespeichert und ein eventuell bereits vorhandener Block wird überschrieben.
         * Ist die angegebene Indexnummer 0, wird der nächste freie Index größer als 1023 verwendet und zurückgegeben.
         *
         *      Mögliche Fehlernummern:
         *      10:  Das SecureFile war nicht geöffnet!
         *      17:  In der Datei des SecureFiles konnte nicht positioniert werden!
         *      18:  In der Datei des SecureFiles konnte nicht gelesen werden!
         *      19:  In der Datei des SecureFiles konnte nicht geschrieben werden!
         *      26:  Ein Datenblock konnte nicht fehlerfrei verschlüsselt werden!
         *
         * \param ref Array-Verweis auf den Klartextblock (dieser bleibt unverändert).
         * \param storeIndex ggf. die Indexnummer, unter der der Datenblock geschrieben werden soll; andernfalls 0.
         * \param error Verweis auf eine Fehlernummer (diese ist 0, wenn alles in Ordnung ist).
         * \return Die Indexnummer, unter der der Datenblock gespeichert wurde (>256), oder 0 im Fehlerfall.
         */
        size_t SecureFile::Put(ByteArray &ref, const size_t storeIndex, size_t &error) {
                if (!openFlag) {
                        return 10;
                }
                ByteArray *encrypt = Encrypt12(ref, true);
                if (encrypt == nullptr) {
                        return 26;
                }
                size_t length1 = encrypt->Size();
                size_t indNumber = storeIndex;
                if (indNumber > 255) {
                        items->SetKeyIndex(0);
                        items->SearchRow()->indexNumber = storeIndex;
                        if (items->Seek(false)) {
                                size_t gapindex = items->Row()->fileIndex;
                                size_t gaplength = items->Row()->arrayLength;
                                if (gaplength == length1) {
                                        if (!file0->seek(static_cast<qint64>(gapindex))) {
                                                return 17;
                                        }
                                        if (file0->write(&(*encrypt)[0], static_cast<qint64>(length1)) != static_cast<qint64>(length1)) {
                                                return 19;
                                        }
                                        return indNumber;
                                }
                                items->Delete(SetNot);
                                size_t readIndex = gapindex + gaplength;
                                size_t length2 = userDataEnd - readIndex;
                                Array<char> A(length2);
                                if (!file0->seek(static_cast<qint64>(readIndex))) {
                                        return 17;
                                }
                                if (file0->read(&(A.WriteareaReferenz(0, length2)), static_cast<qint64>(length2)) != static_cast<qint64>(length2)) {
                                        return 18;
                                }
                                if (!file0->seek(static_cast<qint64>(gapindex))) {
                                        return 17;
                                }
                                if (file0->write(&(A[0]), static_cast<qint64>(length2)) != static_cast<qint64>(length2)) {
                                        return 19;
                                }
                                size_t seekPos = userDataEnd - gaplength;
                                items->SetKeyIndex(1);
                                items->SearchRow()->fileIndex = readIndex;
                                items->Seek(true);
                                while ((items->Row() != nullptr) && (items->Row()->fileIndex <= seekPos)) {
                                        items->Row()->fileIndex = items->Row()->fileIndex - gaplength;
                                        items->Next();
                                }
                                if (!file0->seek(static_cast<qint64>(seekPos))) {
                                        return 17;
                                }
                                if (file0->write(&(*encrypt)[0], static_cast<qint64>(length1)) != static_cast<qint64>(length1)) {
                                        return 19;
                                }
                                items->CreateNewRow();
                                items->Row()->fileIndex = seekPos;
                                items->Row()->arrayLength = length1;
                                items->Row()->indexNumber = indNumber;
                                items->Insert(false);
                                userDataEnd = seekPos + length1;
                                error = WriteTablesToStore();
                                if (error > 0) {
                                        return error;
                                }
                                return indNumber;
                        }
                } else {
                        if (freeIndexes->Count() > 0) {
                                freeIndexes->Last();
                                indNumber = freeIndexes->Row()->indexNumber;
                                freeIndexes->Delete(SetNot);
                        } else {
                                indNumber = nextFreeIndex;
                                nextFreeIndex++;
                        }
                }
                if (!file0->seek(static_cast<qint64>(userDataEnd))) {
                        return 17;
                }
                if (file0->write(&(*encrypt)[0], static_cast<qint64>(length1)) != static_cast<qint64>(length1)) {
                        return 19;
                }
                items->CreateNewRow();
                items->Row()->fileIndex = userDataEnd;
                items->Row()->arrayLength = length1;
                items->Row()->indexNumber = indNumber;
                items->Insert(false);
                userDataEnd = userDataEnd + length1;
                error = WriteTablesToStore();
                if (error > 0) {
                        return error;
                }
                return indNumber;
        }


        /*! \brief Diese Methode `WriteTablesToStore()` schreibt verschlüsselte Verwaltungsdaten in das SecureFile.
         *
         * Schreibt den Inhalt der Verwaltungstabellen verschlüsselt an das Ende der Datei des SecureFiles.
         *
         *      Mögliche Fehlernummern:
         *      17:  In der Datei des SecureFiles konnte nicht positioniert werden!
         *      19:  In der Datei des SecureFiles konnte nicht geschrieben werden!
         *      26:  Ein Datenblock konnte nicht fehlerfrei verschlüsselt werden!
         *
         */
        size_t SecureFile::WriteTablesToStore() {
                ByteArray decrypt2(512);
                freeIndexes->ExportToByteArray(decrypt2);
                ByteArray *encrypt2 = Encrypt12(decrypt2, false);
                if (encrypt2 == nullptr) {
                        return 26;
                }
                freeIndexesLng = encrypt2->Size();
                ByteArray decrypt4(8192);
                items->ExportToByteArray(decrypt4);
                ByteArray *encrypt4 = Encrypt12(decrypt4, false);
                if (encrypt4 == nullptr) {
                        return 26;
                }
                itemsLng = encrypt4->Size();
                size_t index = 0;
                controlArray->Clear();
                controlArray->WriteNumber(index, static_cast<int>(nextFreeIndex), 4);
                controlArray->WriteNumber(index, static_cast<int>(userDataEnd), 4);     // FileIndexEnd
                controlArray->WriteNumber(index, static_cast<int>(encrypt2->Size()), 4); // FreeIndexestLength
                controlArray->WriteNumber(index, static_cast<int>(encrypt4->Size()), 4); // ItemListLength
                ByteArray *encrypt5 = Encrypt12(*controlArray, false);
                if (encrypt5 == nullptr) {
                        return 26;
                }
                if (!file0->seek(704)) {
                        return 17;
                }
                if (file0->write(&(*encrypt5)[0], 112) != 112) {
                        return 19;
                }
                if (!file0->seek(userDataEnd)) {
                        return 17;
                }
                if (file0->write(&(*encrypt2)[0], static_cast<qint64>(encrypt2->Size())) != static_cast<qint64>(encrypt2->Size())) {
                        return 19;
                }
                if (file0->write(&(*encrypt4)[0], static_cast<qint64>(encrypt4->Size())) != static_cast<qint64>(encrypt4->Size())) {
                        return 19;
                }
                if (!file0->flush()) {
                        return 19;
                }
                return 0;
        }

} // end of namespace Store


