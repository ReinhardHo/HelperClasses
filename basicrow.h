/*
HelperClasses.dll, a collection of useful classes and routines.
Copyright (C) 2023 - 2026    Reinhard Hölscher

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/


/*! \file basicrow.h
 *  \brief Stellt die Schnittstelle für die Klasse __BasicRow__ bereit.
 *
 * Die Klasse __BasicRow__ stellt eine Datenzeile in einer Datenbank dar, die noch keine Datenfelder enthält.
 * Instanzen von Ableitungen dieser Klasse __BasicRow__ stellen dann Datensätze mit bestimmten Datenfeldern bereit, die in einer Tabelle gespeichert werden können
 * (In Instanzen von Ableitungen der Klasse __BasicTable__).
 *
 * Einzelne Datenfelder (oder Kombinationen von Datenfeldern) können als Schlüssel (oder Datenbankindizes) dienen.
 * Mithilfe der Methoden `IsLess()` und `IsEqual()` lassen sich die Schlüssel miteinander vergleichen und somit in einer Tabelle verwalten.
 */

#ifndef TABLE_BASICROW_H
#define TABLE_BASICROW_H

#include "HelperClasses_global.h"
#include "bytearray.h"
using namespace RH;

namespace Table {


        class BasicTable;       // forward declaration

        /*! \brief Stellt Methoden für das grundlegende Verhalten einer Datenzeile in einer Tabelle vom Typ __BasicTable__ bereit (siehe dort).
         *
         * Instanzen von Ableitungen dieser Klasse __BasicRow__ stellen dann Datensätze mit bestimmten Datenfeldern bereit, die in einer Tabelle gespeichert werden können
         * (In Instanzen von Ableitungen der Klasse __BasicTable__).
         *
         * Einzelne Datenfelder (oder Kombinationen von Datenfeldern) können als Schlüssel (oder Datenbankindizes) dienen.
         * Mithilfe der Methoden `IsLess()` und `IsEqual()` lassen sich die Schlüssel miteinander vergleichen und somit in einer Tabelle verwalten.
         */
        class HELPERCLASSES_EXPORT BasicRow {
        public:
                BasicRow() = delete;          // prevent default constructor
                BasicRow(BasicTable &table, const bool isSearchInfo = false);
                BasicRow(const BasicRow &ref);
                virtual ~BasicRow();
                BasicRow *Clone() const;
                void CopyTo(BasicRow &ref) const;
                void ExportToArray(ByteArray &ref) const;
                void ImportFromArray(ByteArray &ref);
                bool IsEqual(BasicRow &row, const size_t reyIndex) const;
                bool IsLess(BasicRow &row, const size_t keyIndex) const;
        protected:
                /*!
                 * \brief Zeiger auf die Tabelleninstanz.
                 *
                 * Zeiger auf die Tabelleninstanz, die diese Datensatzinstanz verwaltet.
                 */
                BasicTable *myTable;
                /*!
                 * \brief Beschreibt die Art der Datensatzinstanz, um die es sich hier handelt.
                 *
                 * Diese Variable ist _true_, wenn diese Instanz Suchinformationen zu einer anderen Datensatzinstanz enthält.
                 * Diese Variable ist _false_, wenn es sich um eine (normale) Datensatzinstanz handelt.
                 */
                bool searchInfo;
                virtual BasicRow *CloneThis() const;
                virtual void CopyToDestination(BasicRow &ref) const;
                virtual void ExportToByteArray(ByteArray &ref) const;
                virtual void ImportFromByteArray(ByteArray &ref);
                virtual bool IsEqualKey(BasicRow &row, const size_t keyIndex) const;
                virtual bool IsLessKey(BasicRow &row, const size_t keyIndex) const;
        };

        //***************** inline implementations *****************


        /*! \brief Diese Methode `Clone()` dupliziert die Daten dieser Instanz in eine neue Instanz.
         *
         * Die Methode `Clone()` führt die Duplizierung dieser Instanz durch, indem die virtuelle Methode `CloneThis()` aufgerufen wird.
         *
         * \note Der Aufrufer ist dafür verantwortlich, die erstellte Kopie (Instanz) irgendwann wieder frei zu geben.
         *
         * \return Zeiger auf eine Kopie dieser Instanz.
         * \sa CopyTo()
         */
        inline BasicRow *BasicRow::Clone() const { return CloneThis(); }


        /*! \brief Diese Methode `CopyTo()` kopiert die Daten dieser Instanz in eine andere (Ziel-)Instanz.
         *
         * Die Methode `CopyTo()` führt die Duplizierung dieser Instanz durch, indem die virtuelle Methode `CopyToDestination()` aufgerufen wird.
         *
         * \param ref Verweis auf die Zielinstanz, in die kopiert werden soll.
         * \sa Clone()
         */
        inline void BasicRow::CopyTo(BasicRow &ref) const { CopyToDestination(ref); }


        /*! \brief Diese Methode `ExportToArray()` exportiert alle Datenfelder dieser Instanz in ein ByteArray-Instanz.
         *
         * Die Methode `ExportToArray()` führt den Export dieser Instanz durch Aufruf der virtuellen Methode `ExportToByteArray()` durch.
         *
         * \param ref Verweis auf eine ByteArray-Instanz, in die exportiert werden soll.
         * \sa ImportFromArray()
         */
        inline void BasicRow::ExportToArray(ByteArray &ref) const { ExportToByteArray(ref); }


        /*! \brief Diese Methode `ImportFromArray()` importiert alle Datenfelder dieser Instanz aus einer ByteArray-Instanz.
         *
         * Die Methode `ImportFromArray()` führt den Import dieser Instanz durch Aufruf der virtuellen Methode `ImportFromByteArray()` durch.
         *
         * \param ref Verweis auf eine ByteArray-Instanz, aus der importiert werden soll.
         * \sa ExportToArray()
         */
        inline void BasicRow::ImportFromArray(ByteArray &ref) { ImportFromByteArray(ref); }


} // end of namespace Table

#endif // TABLE_BASICROW_H
