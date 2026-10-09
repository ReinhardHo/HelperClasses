/*
HelperClasses.dll, a collection of useful classes and routines.
Copyright (C) 2023 - 2026    Reinhard Hölscher

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/


/*! \file bytearray.h
 *  \brief Enthält die Definition der Klasse __ByteArray__.
 */

#ifndef RH_BYTEARRAY_H
#define RH_BYTEARRAY_H
#include "HelperClasses_global.h"
#include "array.t"

namespace RH {

        /*! \brief Stellt Methoden zur Verwaltung von Datenbytes in einem Datenfeld bereit.
         *
         * Die Klasse __ByteArray__ stellt ein Datenfeld beliebiger Größe für Datenbytes (von Typ char) bereit.
         * Auf die einzelnen Elemente kann über einen Index zugegriffen werden und es stehen Methoden zum Kopieren und Bearbeiten dieser Datenbytes zur Verfügung.
         * Wird bei der Instanzerzeugung der Parameter _sec=true_ angegeben, wird der freizugebenden Heapspeicher vor der Freigabe gelöscht.
         */
        class HELPERCLASSES_EXPORT ByteArray {
        public:
                ByteArray();
                ByteArray(const char *pointer, const bool sec = false);
                ByteArray(const size_t size, const bool sec = false);
                ByteArray(const QString &ref, const bool sec = false);
                ByteArray(const ByteArray &ref);
                virtual ~ByteArray();
                void Append(const ByteArray &ref);
                void Append(const char &c);
                char &At(const size_t index) const;
                size_t Capacity() const;
                void Clear();
                int Compare(const ByteArray &ref, const bool caseSensitive = true);
                int Compare(const ByteArray &ref, const size_t sindex, const size_t tindex, const size_t size, const bool caseSensitive = true);
                int Compare(const char *pointer, const size_t sindex, const size_t tindex, const size_t size, const bool caseSensitive = true);
                void Copy(const ByteArray &ref, const size_t sindex, const size_t tindex, const size_t size);
                void Copy(const char *pointer, const size_t sindex, const size_t tindex, const size_t size);
                void Copy(const QByteArray &ref, const size_t sindex, const size_t tindex, const size_t size);
                void Copy(const QString &ref, const size_t sindex, const size_t tindex, const size_t size);
                bool Dump(const QString &pathName, const QString &title, const size_t index = 0, const size_t length = 0);
                int IndexOf(const char &ref) const;
                void Insert(const size_t index, const char &ref);
                void IntegerToHexText(size_t &index, const int value, const size_t length);
                void IntegerToText(size_t &index, const int value, const size_t length, const bool points=false);
                bool IsEqual(const ByteArray &ref, const size_t sindex, const size_t tindex, const size_t size) const;
                bool IsSecure() const;
                char &operator[](size_t index);
                int ReadNumber(size_t &index, const size_t bytes, const bool littleEndian = true, const bool sign = true) const;
                int64_t ReadX209Int64(size_t &index, const bool reverse = false);
                void RemoveAt(const size_t index);
                void SetCapacity(const size_t size);
                void SetSize(const size_t size);
                size_t Size() const;
                void TestCapacity(const size_t size);
                int ToInteger(bool *ok = nullptr, const int base = 10);
                QString *ToQString() const;
                std::string *ToString() const;
                void WriteNumber(size_t &index, const int value, const size_t bytes, const bool littleEndian = true);
                void WriteX209Int64(size_t &index, const int64_t value, const bool reverse = false);
                char &WriteareaReferenz(const size_t index, const size_t size);
        private:
                /*!
                 *  \brief Zeiger auf eine Instanz vom Typ Array<char>.
                 *
                 * Zeiger auf eine Instanz vom Typ Array<char> (vgl. Template __array.t__), in der die Bytes verwaltet werden.
                 */
                Array<char> *ba;
        };

        //******************** inline implementations **********************************

        /*! \brief Die Methode `Capacity()` gibt die aktuelle Kapazität der ByteArray-Instanz zurück.
         *
         *      ByteArray *ba = new ByteArray(4096);
         *      size_t erg = ba->Capacity();
         *      delete ba;
         *
         * In diesem Beispiel enthält die Variable _ba_ einen Zeiger auf die ByteArray-Instanz. Diese hat eine Kapazität von 4096 Bytes.
         * Davon sind 0 Bytes mit Daten belegt. Die Variable _erg_ enthält den Wert __4096__.
         * Vor der Zerstörung der ByteArray-Instanz, wird der Heapspeicher in diesem Beispiel --nicht__ gelöscht.
         *  \return Die Anzahl der verwaltbaren Datenbytes.
         * \sa TestCapacity() SetCapacity() Size() SetSize()
         */
        inline size_t ByteArray::Capacity() const { return ba->Capacity(); }


        /*! \brief Gibt das Datenbyte zurück, das unter dem nullbasierten Index der ByteArray-Instanz vorhanden ist.
         *
         *
         *      ByteArray *ba = new ByteArray("Ein Glas kaltes Bier bitte!");
         *      char erg = (*ba)[12];
         *      delete ba;
         *
         * In diesem Beispiel enthält die Variable _erg_ den Buchstaben __t__, da an der Indexposition __12__ der Buchstabe __t__ steht.
         *
         * \note Die Methode führt keine Indexprüfung durch. Liegt der Indexwert außerhalb des gültigen Wertebereichs,
         * kann möglicherweise auf nicht zugewiesenen Speicher zugegriffen werden.
         *
         * \param index Der Index des Datenbytes, das zurückgegeben werden soll.
         * \return Das gewünschte (indexierte) Datenbyte.
         * \sa At()
         */
        inline char &ByteArray::operator[](size_t index) { return (*this->ba)[index]; }

        /*! \brief Die Methode `IsSecure()` gibt _true_ zurück, wenn die Daten auf dem Heap gelöscht werden, bevor sie freigegeben werden.
         *
         * \return _true_, wenn sicher (ansonsten _false_).
         * \sa { Konstruktor }
         */
        inline bool ByteArray::IsSecure() const { return ba->IsSecure(); }

        /*! \brief Die Methode `Size()` gibt die Anzahl der in der ByteArray-Instanz verwalteten Datenbytes zurück.
         *
         *      ByteArray *ba = new ByteArray("Ein Glas kaltes Bier bitte!");
         *      size_t size = ba->Size();
         *      delete ba;
         *
         * In diesem Beispiel enthält die Variable _ba_ einen Zeiger auf die ByteArray-Instanz. Diese hat eine Kapazität von 28 Bytes.
         * Davon sind 27 Bytes mit Daten belegt (der Bierbestellung).
         * Die Variable _size_ enthält nach dem Aufruf der Methode `Size()` den Wert __27__.
         * \return Die Anzahl der verwalteten Datenbytes.
         * \sa SetSize() Capacity() SetCapacity() TestCapacity()
         */
        inline size_t ByteArray::Size() const { return ba->Size(); }


} // end of namespace RH

#endif // RH_BYTEARRAY_H
