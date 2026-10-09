/*
HelperClasses.dll, a collection of useful classes and routines.
Copyright (C) 2023 - 2026    Reinhard Hölscher

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/


#include "basictable.h"
#include "basicrow.h"
#include <stdexcept>

/*! \file basictable.cpp
 *  \brief In dieser Datei wird die Klasse __BasicTable__ implementiert.
 */


namespace Table {

        /*! \brief Konstruktor: Initialisiert eine neue Instanz der Klasse __BasicTable__.
         *
         * \param numberOfKeyTabs Anzahl der verschiedenen Schlüsseltabellen, die verwaltet werden müssen.
         * \sa ~BasicTable()
         */
        BasicTable::BasicTable(const size_t numberOfKeyTabs) {
                numOfKeyTabs = numberOfKeyTabs;
                lastDelete = new size_t[numOfKeyTabs];
                lastAction = new size_t[numOfKeyTabs];
                lists = new Array<BasicRow*> *[numOfKeyTabs];
                for (size_t i = 0; i < numOfKeyTabs; i++) {
                        lastDelete[i] = 0;
                        lastAction[i] = 0;
                        lists[i] = new Array<BasicRow*>();
                }
                currentKeyTable = 0;
                newRowFlag = false;
                currentRow = nullptr;
                searchInfo = nullptr;
        }

        /*! \brief Destruktor: Zerstört die existierende Instanz der Klasse __BasicTable__.
         * \sa BasicTable()
         */
        BasicTable::~BasicTable() {
                if (lists[0] != nullptr) {
                        ClearAll();
                        for (size_t i = 0; i < numOfKeyTabs; i++) {
                                delete lists[i];
                        }
                }
                delete[] lists;
                delete[] lastAction;
                delete[] lastDelete;
        }

        /*! \brief Diese Methode `ChangeKey()` sortiert den Datensatz korrekt neu in die Tabeele ein, falls eine Schlüsselangabe im Datensatz geändert werden musste.
         *
         * Diese virtuelle Methode muss in abgeleiteten Klassen überschrieben werden, wenn Informationen zu einem Schlüssel in einer Datensatzinstanz geändert werden
         * sollen, die bereits von der Tabelle verwaltet wird.
         * Dann kann ein Löschen und eine Neuanordnung in einer einzelnen Indextabelle erforderlich sein.
         *
         * \param keyIndex Index der Schlüsseltabelle, in der eine Änderung vorgenommen werden soll.
         * \sa Delete() und Insert()
         */
        void BasicTable::ChangeKey(const size_t keyIndex) {
                // the following code lines only serves to prevent a warning message (unused parameter keyIndex).
                size_t i = currentKeyTable;
                currentKeyTable = keyIndex;
                currentKeyTable = i;
        }

        /*! \brief Diese Methode `ClearAll()` löscht alle Datensatz-Instanzen aus der Tabelle und zerstört die Datensatz-Instanzen.
         *  \sa Delete()
         */
        void BasicTable::ClearAll() {
                for (size_t i = 0; i < lists[0]->Size(); i++) {
                        BasicRow *tmp = (*lists[0])[i];
                        delete tmp;
                }
                for (size_t i = 0; i < numOfKeyTabs; i++) {
                        lists[i]->Clear();
                }
        }

        /*! \brief Diese Methode `Delete()` löscht eine Datensatz-Instanz aus der Tabelle und zerstört die Datensatz-Instanz.
         *
         * Löscht die Datensatz-Instanz aus der Tabelle, auf die zuletzt der Cursor positioniert war, und zerstört sie.
         *
         * \param next Gibt an, wohin Der Cursor nach dem Löschvorgang gesetzt werden soll.
         */
        void BasicTable::Delete(const NextPosition next) {
                BasicRow *NextWorkRow = nullptr;
                for (size_t i = 0; i < numOfKeyTabs; i++) {
                        int DeleteIndex = lists[i]->IndexOf(currentRow);
                        if (DeleteIndex > -1) {
                                lastDelete[i] = static_cast<size_t>(DeleteIndex);
                                lists[i]->RemoveAt(static_cast<size_t>(DeleteIndex));
                                if (currentKeyTable == i) {
                                        if (next == SetBefore) {
                                                if ((lastDelete[i] > 0) && (lastDelete[i] < lists[i]->Size())) {
                                                        lastAction[i] = lastDelete[i] - 1;
                                                        NextWorkRow = (*lists[i])[lastAction[i]];
                                                } else {
                                                        NextWorkRow = nullptr;
                                                }
                                        }
                                        if (next == SetNot) {
                                                NextWorkRow = nullptr;
                                        }
                                        if (next == SetAfter) {
                                                if (lastDelete[i] < lists[i]->Size()) {
                                                        lastAction[i] = lastDelete[i];
                                                        NextWorkRow = (*lists[i])[lastAction[i]];
                                                } else {
                                                        NextWorkRow = nullptr;
                                                }
                                        }
                                }
                        } else {
                                throw std::out_of_range("Index");
                        }
                }
                delete currentRow;
                currentRow = NextWorkRow;
        }

        /*! \brief Diese Methode `Delete()` löscht eine Datensatz-Instanz aus der Tabelle und zerstört die Datensatz-Instanz.
         *
         * Löscht die als Parameter übergebene Datensatz-Instanz aus der Tabelle, sofern sie vorhanden ist, und zerstört sie.
         *
         * \param row Die gesuchte Datensatz-Instanz, die gelöscht werden soll.
         */
        void BasicTable::Delete(BasicRow &row) {
                for (size_t i = 0; i < numOfKeyTabs; i++) {
                        int DeleteIndex = lists[i]->IndexOf(&row);
                        if (DeleteIndex > -1) {
                                lastDelete[i] = static_cast<size_t>(DeleteIndex);
                                lists[i]->RemoveAt(static_cast<size_t>(DeleteIndex));
                        } else {
                                throw std::out_of_range("Index");
                        }
                }
                delete &row;
        }

        /*! \brief Diese Methode `Delete()` löscht einen Verweis auf eine Datensatz-Instanz aus der übergebenen Schlüsseltabelle.
         *
         * Löscht die Datensatz-Instanz aus der angegebenen Schlüsseltabelle, auf die zuletzt der Cursor positioniert war.
         *
         * \param atKeyTable Index auf die Schlüsseltabelle, in der eine Löschung vorgenommen werden soll.
         */
        void BasicTable::Delete(const size_t atKeyTable) {
                if (atKeyTable < numOfKeyTabs) {
                        int DeleteIndex = lists[atKeyTable]->IndexOf(currentRow);
                        if (DeleteIndex > -1) {
                                lists[atKeyTable]->RemoveAt(static_cast<size_t>(DeleteIndex));
                        } else {
                                throw std::out_of_range("Index");
                        }
                }
        }

        /*! \brief Diese Methode `ExportToByteArray()` exportiert die Daten aller Datensatz-Instanzen in eine ByteArray-Instanz.
         *
         * \param ref Ein Verweis auf eine ByteArray-Instanz, in die die Daten aller Datensatz-Instanzen exportiert werden sollen.
         * \sa ImportFromByteArray()
         */
        void BasicTable::ExportToByteArray(ByteArray &ref) {
                ByteArray a(1024);
                size_t index = 0;
                ref.WriteX209Int64(index, static_cast<int64_t>(lists[currentKeyTable]->Size()));
                First();
                while (currentRow != nullptr) {
                        a.Clear();
                        currentRow->ExportToArray(a);
                        ref.WriteX209Int64(index, static_cast<int64_t>(a.Size()));
                        ref.Append(a);
                        index = index + a.Size();
                        Next();
                }
        }

        /*! \brief Diese Methode `First()` positioniert den Cursor auf die erste Datensatz-Instanz.
         *
         * Positioniert den Cursor auf den ersten vorhandenen Datensatz, in Abhängigkeit von der aktuell eingestellten Schlüsseltabelle.
         * Wenn in dieser Tabellen-Instanz noch keine Datensatz-Instanzen vorhanden sind, wird _CurrentRow_ auf _nullptr_ gesetzt.
         * \sa Next(), Prior() und Last()
         */
        void BasicTable::First() {
                if (newRowFlag) {
                        newRowFlag = false;
                }
                if (lists[currentKeyTable]->Size() > 0) {
                        currentRow = (*lists[currentKeyTable])[0];
                        lastAction[currentKeyTable] = 0;
                } else {
                        currentRow = nullptr;
                }
        }

        /*! \brief Diese Methode `ImportFromByteArray()` importiert Daten aus einer ByteArray-Instanz.
         *
         * Importiert Daten aus einer ByteArray-Instanz, in die zuvor mit der Methode `ExportToByteArray()` exportiert wurde.
         *
         * \param ref Ein Verweis auf eine ByteArray-Instanz, die die Daten aller Datensätze enthält.
         * \param change Ist der Wert _true_, wird ein bereits vorhandener Datensatz (mit diesem Schlüssel) nur geändert; ist der Wert _false_, wird der Datensatz eingefügt.
         * \sa ExportToByteArray()
         */
        void BasicTable::ImportFromByteArray(ByteArray &ref, const bool change) {
                size_t index = 0;
                int count;
                count = ref.ReadX209Int64(index);
                ByteArray a(1024);
                for (int i = 0; i < count; i++) {
                        size_t length;
                        length = static_cast<size_t>(ref.ReadX209Int64(index));
                        a.SetSize(length);
                        a.Copy(ref, index, 0, length);
                        index = index + length;
                        searchInfo->ImportFromArray(a);
                        if (change) {
                                if (Seek(false)) {
                                        searchInfo->CopyTo(*currentRow);
                                } else {
                                        currentRow = searchInfo->Clone();
                                        newRowFlag = true;
                                        Insert(false);
                                }
                        } else {
                                currentRow = searchInfo->Clone();
                                newRowFlag = true;
                                Insert(false);
                        }
                }
        }

        /*! \brief Diese Methode `Insert()` sortiert eine Datensatz-Instanz in diese Tabellen-Instanz ein.
         *
         * Wenn _newRowFlag_ den Wert _true_ hat, wird die unter _currentRow_ referenzierte Datensatz-Instanz in diese Tabellen-Instanz einsortiert.
         *
         * \param savePosition Wird _true_ übergeben, wird die Indexposition des einsortierten Datensatzes gespeichert.
         * \sa Delete()
         */
        void BasicTable::Insert(const bool savePosition) {
                if (newRowFlag) {
                        BasicRow *testinstanz;
                        for (size_t i = 0; i < numOfKeyTabs; i++) {
                                if (lists[i]->Size() == 0) {
                                        lists[i]->Append(currentRow);
                                } else {
                                        size_t testindex = 0;
                                        size_t minindex = 0;
                                        size_t maxindex = lists[i]->Size() - 1;
                                        while (minindex != maxindex) {
                                                testindex = ((minindex + maxindex) >> 1) + 1;
                                                testinstanz = (*lists[i])[testindex];
                                                if (currentRow->IsLess(*testinstanz, i)) {
                                                        testindex--;
                                                        maxindex = testindex;
                                                } else {
                                                        minindex = testindex;
                                                }
                                        }
                                        testinstanz = (*lists[i])[testindex];
                                        if (currentRow->IsLess(*testinstanz, i)) {
                                                lists[i]->Insert(testindex, currentRow);
                                                if (savePosition && (currentKeyTable == i)) {
                                                        lastAction[i] = testindex;
                                                }
                                        } else {
                                                lists[i]->Insert(testindex + 1, currentRow);
                                                if (savePosition && (currentKeyTable == i)) {
                                                        lastAction[i] = testindex + 1;
                                                }
                                        }
                                }
                        }
                        newRowFlag = false;
                }
        }

        /*! \brief Diese Methode `Insert()` sortiert Eine Datensatz-Instanz in eine Schlüsseltabelle ein.
         *
         * Sortiert die unter _currentRow_ referenzierte Datensatz-Instanz in die angegebenen Schlüsseltabelle ein.
         *
         * \param atKeyTable Index der Schlüsseltabelle, in die einsortiert werden soll.
         * \sa Delete()
         */
        void BasicTable::Insert(const size_t atKeyTable) {
                BasicRow *testinstanz;
                if (atKeyTable < numOfKeyTabs) {
                        if (lists[atKeyTable]->Size() == 0) {
                                lists[atKeyTable]->Append(currentRow);
                        } else {
                                size_t testindex = 0;
                                size_t minindex = 0;
                                size_t maxindex = lists[atKeyTable]->Size() - 1;
                                while (minindex != maxindex) {
                                        testindex = ((minindex + maxindex) >> 1) + 1;
                                        testinstanz = (*lists[atKeyTable])[testindex];
                                        if (currentRow->IsLess(*testinstanz, atKeyTable)) {
                                                testindex--;
                                                maxindex = testindex;
                                        } else {
                                                minindex = testindex;
                                        }
                                }
                                testinstanz = (*lists[atKeyTable])[testindex];
                                if (currentRow->IsLess(*testinstanz, atKeyTable)) {
                                        lists[atKeyTable]->Insert(testindex, currentRow);
                                } else {
                                        lists[atKeyTable]->Insert(testindex + 1, currentRow);
                                }
                        }
                }
        }

        /*! \brief Diese Methode `Last()` positioniert den Cursor auf die letzte Datensatz-Instanz.
         *
         * Positioniert den Cursor auf den ersten vorhandenen Datensatz, in Abhängigkeit von der aktuell eingestellten Schlüsseltabelle.
         * Wenn in dieser Tabellen-Instanz noch keine Datensatz-Instanzen vorhanden sind, wird _CurrentRow_ auf _nullptr_ gesetzt.
         * \sa First(), Next() und Prior()
         */
        void BasicTable::Last() {
                if (newRowFlag) {
                        newRowFlag = false;
                }
                if (lists[currentKeyTable]->Size() > 0) {
                        lastAction[currentKeyTable] = lists[currentKeyTable]->Size() - 1;
                        currentRow = (*lists[currentKeyTable])[lastAction[currentKeyTable]];
                } else {
                        currentRow = nullptr;
                }
        }

        /*! \brief Diese virtuelle Methode `NewRow()` muss in abgeleiteten Klassen überschrieben werden, um eine neue Datensatz-Instanz zu erstellen.
         *  \sa newRowFlag
         */
        void BasicTable::NewRow() {
                if (!newRowFlag) {
                        // CurrentRow = new ... create a new record instance here
                        newRowFlag = true;
                }
        }

        /*! \brief Diese Methode `Next()` positioniert den Cursor auf die nächste Datensatz-Instanz.
         *
         * Positioniert den Cursor auf den nächsten vorhandenen Datensatz, in Abhängigkeit von der aktuell eingestellten Schlüsseltabelle.
         * Wenn in dieser Tabellen-Instanz noch keine Datensatz-Instanzen vorhanden sind, wird _CurrentRow_ auf _nullptr_ gesetzt.
         * \sa First(), Last() und Prior()
         */
        void BasicTable::Next() {
                if (newRowFlag) {
                        newRowFlag = false;
                }
                if (lastAction[currentKeyTable] < lists[currentKeyTable]->Size() - 1) {
                        lastAction[currentKeyTable]++;
                        currentRow = (*lists[currentKeyTable])[lastAction[currentKeyTable]];
                } else {
                        currentRow = nullptr;
                }
        }

        /*! \brief Diese Methode `Prior()` positioniert den Cursor auf die vorherige Datensatz-Instanz.
         *
         * Positioniert den Cursor auf den vorherigen vorhandenen Datensatz, in Abhängigkeit von der aktuell eingestellten Schlüsseltabelle.
         * Wenn in dieser Tabellen-Instanz noch keine Datensatz-Instanzen vorhanden sind, wird _CurrentRow_ auf _nullptr_ gesetzt.
         * \sa First(), Next() und Last()
         */
        void BasicTable::Prior() {
                if (newRowFlag) {
                        newRowFlag = false;
                }
                if (lastAction[currentKeyTable] > 0) {
                        lastAction[currentKeyTable]--;
                        currentRow = (*lists[currentKeyTable])[lastAction[currentKeyTable]];
                } else {
                        currentRow = nullptr;
                }
        }

        /*! \brief Diese Methode `Seek()` sucht nach einer Datensatzinstanz in der Tabelle.
         *
         * Sucht nach einem Datensatz mit den in _searchInfo_ angegebenen Schlüsseln. Wird ein passender Datensatz gefunden,
         * wird dieser unter _currentRow_ bereitgestellt und _true_ zurückgegeben (ansonsten _false_).
         *
         * \param savePosition Wird _true_ übergeben, wird die Indexposition des gefundenen Datensatzes gespeichert.
         * \return _true_, wenn ein Datensatz mit dem gesuchten Schlüssel gefunden wurde.
         */
        bool BasicTable::Seek(const bool savePosition) {
                if (newRowFlag) {
                        currentRow = nullptr;
                        newRowFlag = false;
                }
                if (lists[currentKeyTable]->Size() == 0) {
                        return false;
                } else {
                        size_t testindex = 0;
                        size_t minindex = 0;
                        BasicRow *testinstanz;
                        size_t maxindex = lists[currentKeyTable]->Size() - 1;
                        while (minindex != maxindex) {
                                testindex = ((minindex + maxindex) >> 1) + 1;
                                BasicRow *testinstanz = (*lists[currentKeyTable])[testindex];
                                if (searchInfo->IsLess(*testinstanz, currentKeyTable)) {
                                        testindex--;
                                        maxindex = testindex;
                                } else {
                                        minindex = testindex;
                                }
                        }
                        testinstanz = (*lists[currentKeyTable])[testindex];
                        if (searchInfo->IsEqual(*testinstanz, currentKeyTable)) {
                                currentRow = testinstanz;
                                if (savePosition) {
                                        lastAction[currentKeyTable] = testindex;
                                }
                                return true;
                        } else {
                                return false;
                        }
                }
        }

        /*! \brief Diese Methode `SeekNearest()` sucht nach einer Datensatzinstanz in der Tabelle.
         *
         * Setzt _currentRow_ auf den Datensatz, der dem gesuchten Schlüssel am nächsten liegt (entweder identisch oder der nächstgrößere).
         * Wenn unter _currentRow_ ein Datensatz vorhanden ist, wird _true_ zurückgegeben (ansonsten _false_).
         *
         * \param savePosition Wird _true_ übergeben, wird die Indexposition des gefundenen Datensatzes gespeichert.
         * \return _true_, wenn ein Datensatz mit dem gesuchten oder einem größeren Schlüssel gefunden wurde.
         */
        bool BasicTable::SeekNearest(const bool savePosition) {
                BasicRow *testinstanz;
                if (newRowFlag) {
                        currentRow = nullptr;
                        newRowFlag = false;
                }
                if (lists[currentKeyTable]->Size() > 0) {
                        size_t testindex = 0;
                        size_t minindex = 0;
                        size_t maxindex = lists[currentKeyTable]->Size() - 1;
                        while (minindex != maxindex) {
                                testindex = ((minindex + maxindex) >> 1) + 1;
                                testinstanz = (*lists[currentKeyTable])[testindex];
                                if (searchInfo->IsLess(*testinstanz, currentKeyTable)) {
                                        testindex--;
                                        maxindex = testindex;
                                } else {
                                        minindex = testindex;
                                }
                        }
                        testinstanz = (*lists[currentKeyTable])[testindex];
                        if (searchInfo->IsEqual(*testinstanz, currentKeyTable)) {
                                currentRow = testinstanz;
                                if (savePosition) {
                                        lastAction[currentKeyTable] = testindex;
                                }
                                return true;
                        } else {
                                if (searchInfo->IsLess(*testinstanz, currentKeyTable)) {
                                        currentRow = testinstanz;
                                        if (savePosition) {
                                                lastAction[currentKeyTable] = testindex;
                                        }
                                        return true;
                                } else {
                                        if (testindex < (lists[currentKeyTable]->Size() - 1)) {
                                                currentRow = (*lists[currentKeyTable])[testindex + 1];
                                                if (savePosition) {
                                                        lastAction[currentKeyTable] = testindex + 1;
                                                }
                                                return true;
                                        } else {
                                                currentRow = nullptr;
                                                return false;
                                        }
                                }
                        }
                } else {
                        return false;
                }
        }

        /*! \brief Diese Methode `SetCapacity()` legt die Kapazität der Schlüssellisten fest.
         *
         * Stellt die Kapazität in den Sortierlisten auf einen bestimmten Wert ein, um häufige Neuanordnungen zu vermeiden.
         *
         * \param size Anzahl der Elemente in den Sortierlisten.
         */
        void BasicTable::SetCapacity(const size_t size) {
                for (size_t i = 0; i < numOfKeyTabs; i++) {
                        lists[i]->SetCapacity(size);
                }
        }

        /*! \brief Diese Methode `SetKeyIndex()` aktiviert einen bestimmten Schlüssel der Tabelle.
         *
         * Aktiviert einen bestimmten Schlüsselindex, um Datensätze in der Reihenfolge dieses Schlüsselindexes bearbeiten zu können.
         *
         * \param index Index (0..n) des gewünschten Schlüsselindexes.
         * \sa GetKeyIndex()
         */
        void BasicTable::SetKeyIndex(const size_t index) {
                if (index < numOfKeyTabs) {
                        currentKeyTable = index;
                } else {
                        throw std::out_of_range("Index");
                }
        }

} // end of namespace Table
