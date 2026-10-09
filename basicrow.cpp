/*
HelperClasses.dll, a collection of useful classes and routines.
Copyright (C) 2023 - 2026    Reinhard Hölscher

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/


#include "basicrow.h"

namespace Table {

        /*! \brief Konstruktor: Initialisiert eine neue Instanz der Klasse __BasicRow__.
         *
         * Die BasicRow-Instanz stellt eine Schnittstelle zur abstrakten Datenbanktabelle BasicTable bereit.
         *
         * \param table Verweis auf die Tabelleninstanz, die diese (Zeilen-)Instanz verwaltet.
         * \param isSearchInfo _true_, wenn diese Instanz Suchinformationen enthält; _false_, wenn es sich um eine normale Datensatzinstanz handelt.
         */
        BasicRow::BasicRow(BasicTable &table, const bool isSearchInfo) : myTable { &table }, searchInfo { isSearchInfo } { }

        /*! \brief Kopierkonstruktor: Initialisiert eine neue Instanz der Klasse __BasicRow__ und kopiert die Daten von einer bereits existierenden Instanz in diese neue Instanz.
         *
         * \param ref Die Referenz auf eine bereits vorhandene BasicRow-Instanz, aus der kopiert werden soll.
         */
        BasicRow::BasicRow(const BasicRow &ref) {
                this->myTable = ref.myTable;
                this->searchInfo = ref.searchInfo;
                ref.CopyTo(*this);
        }

        /*! \brief Destruktor: Zerstört die existierende Instanz der Klasse __BasicRow__.
         *
         *  Alle ggf. in einer Ableitung dieser Instanz vorhandene andere Instanzen werden vor der Zerstörung vorher selbst zerstört.
         */
        BasicRow::~BasicRow() { }

        /*! \brief Diese Methode `CloneThis()` dupliziert die Daten dieser Instanz.
         *
         * Führt die tatsächlich Arbeit der Duplizierung dieser Instanz aus.
         * Diese virtuelle Methode `CloneThis()` muss in abgeleiteten Klassen überschrieben und erweitert werden.
         *
         * \returns Zeiger auf eine Kopie dieser Instanz.
         * \sa CopyToDestination()
         */
        BasicRow *BasicRow::CloneThis() const {
                BasicRow *newRow;

                newRow = new BasicRow(*this->myTable, this->searchInfo);
                return newRow;
        }

        /*! \brief Diese Methode `CopyToDestination()` kopiert die Daten dieser Instanz in eine (Ziel-)Instanz.
         *
         * Führt tatsächlich den Vorgang des Kopierens dieser Instanz in eine Zielinstanz durch.
         * `CopyToDestination()` muss in abgeleiteten Klassen sinnvoll überschrieben werden.
         *
         * \param ref Referenz auf die Zielinstanz, in die kopiert wird.
         * \sa CloneThis()
         */
        void BasicRow::CopyToDestination(BasicRow &ref) const {
                // the following code line only serves to prevent a warning message (unused parameter ref).
                ref.searchInfo = false;
        }

        /*! \brief Diese Methode `ExportToByteArray()` exportiert alle Datenfelder dieser Instanz in ein ByteArray-Instanz.
         *
         * Exportiert alle Datenfelder einer Instanz dieser Klasse in ein ByteArray, sodass sie mit der Methode `ImportFromByteArray()` wieder eingelesen werden können.
         * Diese Methode muss in einer abgeleiteten Klasse sinnvoll überschrieben werden.
         *
         * \param ref Verweis auf eine ByteArray-Instanz, in die exportiert werden soll.
         * \sa ImportFromByteArray()
         */
        void BasicRow::ExportToByteArray(ByteArray &ref) const {
                // the following code line only serves to prevent a warning message (unused parameter ref).
                ref.Append(' ');
        }

        /*! \brief Diese Methode `ImportFromByteArray()` importiert alle Datenfelder dieser Instanz aus einer ByteArray-Instanz.
         *
         * Importiert alle Datenfelder aus einer ByteArray-Instanz in eine Instanz dieser Klasse,
         * die zuvor mit der Methode `ExportToByteArray()` dorthin exportiert wurden.
         * Diese Methode muss in einer abgeleiteten Klasse sinnvoll überschrieben werden.
         *
         * \param ref Verweis auf eine ByteArray-Instanz, aus der importiert werden soll.
         * \sa ExportToByteArray()
         */
        void BasicRow::ImportFromByteArray(ByteArray &ref) {
                // the following code lines only serves to prevent a warning message (unused parameter ref).
                ByteArray B;
                B.Append(ref);
        }

        /*! \brief Diese Methode `IsEqual()` prüft, ob Schlüssel gleich sind.
         *
         * Die Methode `IsEqual()` führt den Schlüsselvergleich durch, indem sie die virtuelle Methode `IsEqualKey()` aufruft.
         *
         * \param row Instanz, mit der der Vergleich durchgeführt werden soll.
         * \param keyIndex Index des zu prüfenden Schlüssels (0 .. n).
         * \return _true_, wenn der eigene Schlüssel mit dem Schlüssel der Vergleichsinstanz übereinstimmt.
         * \sa IsLess()
         */
        bool BasicRow::IsEqual(BasicRow &row, const size_t keyIndex) const {
                return IsEqualKey(row, keyIndex);
        }

        /*! \brief Diese Methode `IsEqualKey()` prüft, ob Schlüssel gleich sind.
         *
         * Die virtuelle Methode `IsEqualKey()` muss (in jeder von dieser Klasse abgeleiteten Klasse) für jeden vorhandenen Schlüsselindex
         * eine Vergleichsoperation bereitstellen, die den eigenen Schlüssel mit dem Schlüssel in einer Vergleichsinstanz vergleicht.
         *
         * Wenn der eigene Schlüssel mit dem der Vergleichsinstanz übereinstimmt, wird _true_ zurückgegeben.
         * Wenn beide Schlüssel unterschiedlich sind, wird _false_ zurückgegeben.
         *
         * \param row Instanz, mit der der Vergleich durchgeführt werden soll.
         * \param keyIndex Index des zu prüfenden Schlüssels (0 .. n).
         * \return _true_, wenn der eigene Schlüssel mit dem Schlüssel der Vergleichsinstanz übereinstimmt.
         * \sa IsLessKey()
         */
        bool BasicRow::IsEqualKey(BasicRow &row, const size_t keyIndex) const {
                // the following code lines only serves to prevent a warning message (unused parameter row).
                if (keyIndex == 0) {
                        row.searchInfo = false;
                }
                return true;
        }

        /*! \brief Diese Methode `IsLess()` prüft, ob der eigene Schlüssel kleiner ist als der in der Vergleichsinstanz.
         *
         * Die Methode `IsLess()` führt den Schlüsselvergleich durch, indem sie die virtuelle Methode `IsLesslKey()` aufruft.
         *
         * \param row Instanz, mit der der Vergleich durchgeführt werden soll.
         * \param keyIndex Index des zu prüfenden Schlüssels (0 .. n).
         * \return _true_, wenn der eigene Schlüssel kleiner ist als der Schlüssel der Vergleichsinstanz.
         * \sa IsEqual()
         */
        bool BasicRow::IsLess(BasicRow &row, const size_t keyIndex) const {
                return IsLessKey(row, keyIndex);
        }

        /*! \brief Diese Methode `IsLessKey()` prüft, ob der eigene Schlüssel kleiner ist als der in der Vergleichsinstanz.
         *
         * Die virtuelle Methode `IsLessKey()` muss (in jeder von dieser Klasse abgeleiteten Klasse) für jeden vorhandenen Schlüsselindex
         * eine Vergleichsoperation bereitstellen, die den eigenen Schlüssel mit dem Schlüssel in einer Vergleichsinstanz vergleicht.
         *
         * Ist der eigene Schlüssel kleiner als der der Vergleichsinstanz, wird _true_ zurückgegeben.
         * Sind beide Schlüssel gleich oder ist der eigene Schlüssel größer, wird _false_ zurückgegeben.
         *
         * \param row Instanz, mit der der Vergleich durchgeführt werden soll.
         * \param keyIndex Index des zu prüfenden Schlüssels (0 .. n).
         * \return _true_, wenn der eigene Schlüssel kleiner ist als der Schlüssel der Vergleichsinstanz.
         * \sa IsEqualKey()
         */
        bool BasicRow::IsLessKey(BasicRow &row, const size_t keyIndex) const {
                // the following code lines only serves to prevent a warning message (unused parameter row).
                if (keyIndex == 0) {
                        row.searchInfo = false;
                }
                return true;
        }

} // end of namespace Table
