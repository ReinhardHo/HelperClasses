/*
HelperClasses.dll, a collection of useful classes and routines.
Copyright (C) 2023 - 2026    Reinhard Hölscher

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/


/*! \file basictable.h
 *  \brief Stellt die Schnittstelle für die Klasse __BasicTable__ bereit.
 *
 * Die Klasse __BasicTable__ stellt Methoden zur Verwaltung von Datensatzinstanzen (Ableitungen der Klasse __BasicRow__) bereit.
 * Hier können beliebig viele Schlüssel verwaltet werden, und über diese Schlüssel lässt sich (auch bei großen Datensätzen)
 * schnell auf bestimmte Datensatzinstanzen zugreifen.
 */

#ifndef TABLE_BASICTABLE_H
#define TABLE_BASICTABLE_H
#include "HelperClasses_global.h"
#include "bytearray.h"
using namespace RH;

namespace Table {

        class BasicRow;         // forward declaration

        /*!
         * \brief Liste mit Konstanten, die angibt, auf welchen Datensatz nach einer Tabellenaktion positoniert werden soll.
         *
         * _SetBefore_ positioniert auf den vorangegangenen Datensatz.
         *
         * _SetNot_ positioniert überhaupt nicht. Die letzte erfolgte Positionierung bleibt einfach erhalten.
         *
         * _SetAfter_ positioniert auf den nachfolgenden Datensatz.
         */
        enum NextPosition {
                SetBefore = -1,
                SetNot = 0,
                SetAfter = 1
        };

        /*! \brief Die Klasse __BasicTable__ stellt Methoden zur Verwaltung von Datenzeilen (Ableitungen der Klasse __BasicRow__) in einer Tabelle bereit.
         *
         * Die Klasse __BasicTable__ stellt Methoden zur Verwaltung von Datensatzinstanzen (Ableitungen der Klasse __BasicRow__) bereit.
         * Hier können beliebig viele Schlüssel verwaltet werden, und über diese Schlüssel lässt sich (auch bei großen Datensätzen)
         * schnell auf bestimmte Datensatzinstanzen zugreifen.
         */
        class HELPERCLASSES_EXPORT BasicTable {
        public:
                BasicTable() = delete;          // prevent default constructor
                BasicTable(const size_t numberOfKeyTabs);
                virtual ~BasicTable();
                size_t Count() const;
                void ClearAll();
                void CreateNewRow();
                void Delete(const NextPosition next);
                void Delete(BasicRow &row);
                void Delete(const size_t atKeyTable);
                void ExportToByteArray(ByteArray &ref);
                void First();
                size_t GetKeyIndex() const;
                void ImportFromByteArray(ByteArray &ref, const bool change);
                void Insert(const bool savePosition);
                void Insert(const size_t atKeyTable);
                bool IsNewRow() const;
                void Last();
                size_t LastDeleteIndex() const;
                size_t LastInsertIndex() const;
                void Next();
                void Prior();
                bool Seek(const bool savePosition);
                bool SeekNearest(const bool savePosition);
                void SetCapacity(const size_t size);
                void SetKeyIndex(const size_t index);
        protected:
                /*!
                 * \brief Enthält einen Zeiger auf die Datensatzinstanz, die sich in einer Tabelle befindet.
                 *
                 * Enthält einen Zeiger auf die Datensatzinstanz, mit der zuletzt gearbeitet wurde oder mit der als Nächstes fortgefahren werden soll.
                 */
                BasicRow *currentRow;
                /*!
                 * \brief Enthält einen Zeiger auf die Datensatzinstanz, die sich __nicht__ in einer Tabelle befindet.
                 *
                 * Enthält einen Zeiger auf die Datensatzinstanz, die mit anderen Datensatzinstanzen verglichen werden soll.
                 */
                BasicRow *searchInfo;
                /*!
                 * \brief _true_, wenn mit der Methode `NewRow()` eine neue Datensatzinstanz erstellt wurde, die noch nicht in die Tabelle einsortiert wurde.
                 * \sa NewRow()
                 */
                bool newRowFlag;
                virtual void ChangeKey(const size_t keyIndex);
                virtual void NewRow();
        private:
                /*!
                 * \brief Enthält den Index einer Schlüsseltabelle.
                 *
                 * Enthält die Nummer (den Index) der Schlüsseltabelle, mit der gerade gearbeitet wird oder ab nun gearbeitet werden soll.
                 */
                size_t currentKeyTable;
                /*!
                 * \brief Anzahl der verschiedenen Schlüsseltabellen die in dieser Tabelle vorhanden sind.
                 *
                 * Anzahl der verschiedenen Schlüsseltabellen die in dieser Tabelle vorhanden sind, die verwaltet werden müssen und mit denen gearbeitet werden kann.
                 */
                size_t numOfKeyTabs;
                /*!
                 * \brief Zeiger auf ein Array (mit Daten vom Typ `size_t`), mit den Indexen der letzten Löschpositionen.
                 *
                 * Zeiger auf ein Array (mit Daten vom Typ `size_t`), in dem für jede Schlüsseltabelle der Index der letzten bzw. nächsten Löschposition verwaltet wird.
                 */
                size_t *lastDelete;
                /*!
                 * \brief Zeiger auf ein Array (mit Daten vom Typ `size_t`), mit den Indexen der letzten Tabellenoperation.
                 *
                 * Zeiger auf ein Array (mit Daten vom Typ `size_t`), in dem für jede Schlüsseltabelle der Index der letzten bzw. nächsten Tabellenoperation verwaltet wird.
                 */
                size_t *lastAction;
                /*!
                 * \brief Zeiger auf ein Array von Zeigern auf Instanzen der Klasse __BasicRow__ (oder deren Ableitung).
                 *
                 * Zeiger auf ein Array von Zeigern auf Instanzen der Klasse __BasicRow__. Für jeden Schlüssel der Tabelle werden die Verweise auf die Datensätze
                 * (Instanzen von Ableitungen der Klasse BasicRow) sortiert in einem Array von Zeigern verwaltet.
                 */
                Array<BasicRow*> **lists;
        };

        //******************** inline implementations **********************************

        /*! \brief Diese Methode `CreateNewRow()` Erstellt eine neue Datenzeile (Datensatzinstanz).
         *
         * Diese öffentliche Methode `CreateNewRow()` erstellt eine neue Datensatzinstanz, die von diesem Tabellenobjekt verwaltet wird,
         * und nutzt zu diesem Zweck intern die virtuelle Methode `NewRow()`.
         */
        inline void BasicTable::CreateNewRow() { NewRow(); }


        /*! \brief Diese Methode `Count()` gibt die Anzahl der verschiedenen Schlüssel in dieser Tabelle zurück.
         *
         *  Mindestens einen Schlüssel benötigt die Tabelle zur Verwaltung der Datensätze.
         */
        inline size_t BasicTable::Count() const { return lists[0]->Size(); }


        /*! \brief Diese Methode `GetKeyIndex()` gibt den aktuell aktivierten Schlüssel der Tabelle zurück.
         *
         *  Diese Methode ist dann nützlich, wenn die Tabelle zwei oder noch mehr verschiedene Schlüssel besitzt.
         * \sa SetKeyIndex()
         */
        inline size_t BasicTable::GetKeyIndex() const { return currentKeyTable; }


        /*! \brief Diese Methode `IsNewRow()` gibt _true_ zurück, solange eine neue Datensatzinstanz noch nicht in die Tabelleninstanz einsortiert wurde.
         *  \sa NewRow()
         */
        inline bool BasicTable::IsNewRow() const { return newRowFlag; }


        /*! \brief Diese Methode `LastDeleteIndex()` gibt den Index des letzten gelöschten Datensatzes (in Bezug zum aktivierten Schlüssel) zurück.
         *  \sa LastInsertIndex()
         */
        inline size_t BasicTable::LastDeleteIndex() const { return lastDelete[currentKeyTable]; }


        /*! \brief Diese Methode `LastInsertIndex()` gibt den Index des letzten eingefügten Datensatzes (in Bezug zum aktivierten Schlüssel) zurück.
         *  \sa LastDeleteIndex()
         */
        inline size_t BasicTable::LastInsertIndex() const { return lastAction[currentKeyTable]; }


} // end of namespace Table

#endif // TABLE_BASICTABLE_H
