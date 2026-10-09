/*
HelperClasses.dll, a collection of useful classes and routines.
Copyright (C) 2023 - 2026    Reinhard Hölscher

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/


/*! \file array.t
 * \brief Stellt die Vorlage für die Klasse __Array<T>__ bereit.
 */

#ifndef RH_ARRAY_T
#define RH_ARRAY_T
#include <stdexcept>
#include <QByteArray>

namespace RH {

        /*! \brief Stellt die API für die Klasse __Array<T>__ bereit.
         *
         * Die Klasse __Array<T>__ stellt einen Datenbereich beliebiger Größe für Datenobjekte vom Typ __T__ bereit. Auf die einzelnen Datenobjekte kann über einen
         * Index zugegriffen werden und es stehen Methoden zum Löschen, Vergleichen, Kopieren, Einfügen und Bearbeiten dieser Datenobjekte zur Verfügung.
         *
         * Bei der Instanzierung einer neuen Klasse __Array<T>__ wird für die verwalteten Datenobjekte vom Typ __T__ Heapspeicher vom Betriebssystem belegt. Beim Aufruf
         * einzelner Methoden (z.B. Append, Copy oder Insert) wird dieser Heapspeicher möglicherweise zu klein. Der Heapspeicher kann dann von diesen Methoden automatisch
         * sinnvoll vergrößert werden. Bei der Freigabe einer instanzierten Klasse __Array<T>__ wird der gesamte belegte Heapspeicher des Betriebssystems wieder
         * freigegeben.
         *
         * Soll die Klasse __Array<T>__ für die Verwaltung sensibler Daten (z.B. Passwörter oder Schlüsselmaterial) instanziert werden, so kann über den Parameter
         * __sec=true__ sichergestellt werden, dass diese Daten aus dem Heapspeicher gelöscht werden, bevor der Heapspeicher an das Betriebssystem zurück gegeben wird.
         */
        template<typename T>
        class Array {
        public:
                Array();
                Array(const size_t size, const bool sec = false);
                Array(const Array<T> &ref);
                virtual ~Array();
                void Append(const T &ref);
                void Append(const Array<T> &ref);
                T &At(const size_t index) const;
                size_t Capacity() const;
                void Clear();
                void Copy(const Array<T> &A, const size_t sindex, const size_t tindex, const size_t val);
                int IndexOf(const T &ref) const;
                void Insert(const size_t index, const T &ref);
                bool IsEqual(const Array<T> &A, const size_t sindex, const size_t tindex, const size_t val) const;
                bool IsSecure() const;
                T &operator[](size_t index);
                void RemoveAt(const size_t index);
                void SetCapacity(const size_t val);
                void SetSize(const size_t val);
                size_t Size() const;
                void TestCapacity(const size_t val);
                T &WriteareaReferenz(const size_t index, const size_t val);
        private:
                /*! \brief Die Variable _capacity_ enthält die maximal verwaltbaren Datenobjekte.
                 *
                 * Die Variable _capacity_ enthält die Anzahl der derzeit maximal verwaltbaren Datenobjekte (vom Typ T) in dieser Array-Instanz.
                 */
                size_t capacity;
                /*! \brief Die Variable _data_ verweist auf eine Speicheraddresse auf dem Heap.
                 *
                 * Die Variable _data_ enthält die Adresse des Speicherbereiches auf dem Heap, in dem ein Datenarray (vom Typ T) verwaltet wird.
                 */
                T *data;
                /*! \brief Die Variable _secure_ regelt den Modus wenn Heap-Bereiche freizugebenden sind.
                 *
                 * Ist die Variable _secure_ auf __true__ gesetzt, so werden die freizugebenden Heap-Bereiche vor der Freigabe gelöscht.
                 */
                bool secure;
                /*! \brief Die Variable _size_ enthält die der derzeit tatsächlich vorhandenen Datenobjekte.
                 *
                 * Die Variable _size_ enthält die Anzahl der derzeit tatsächlich vorhandenen Datenobjekte (vom Typ T) in dieser Array-Instanz.
                 */
                size_t size;
                void IncreaseCapacity(size_t size);
        };


        /*! \brief Konstruktor: Initialisiert eine neue Instanz der Klasse __Array<T>__.
         *
         * Die Array-Instanz kann nach ihrer Erstellung bis zu 32 Datenobjekte (vom Typ __T__) aufnehmen und verwalten.
         * Freizugebender Heapspeicher wird vor der Freigabe nicht gelöscht.
         */
        template<typename T>
        Array<T>::Array() {
                data = new T[32];
                size = 0;
                capacity = 32;
                secure = false;
        }

        /*! \brief Konstruktor: Initialisiert eine neue Instanz der Klasse __Array<T>__.
         *
         * Die Array-Instanz kann nach ihrer Erstellung bis zu __val__ (siehe Variable val) Datenobjekte (vom Typ __T__) aufnehmen und verwalten.
         *
         * \param val Die Anzahl der Datenobjekte (vom Typ __T__), die diese Instanz nach ihrer Erstellung verwalten kann.
         * \param sec Wenn _true_ wird der Heapspeicher vor seiner Freigabe gelöscht. Der Standardwert der Variablen _sec_ ist _false_.
         */
        template<typename T>
        Array<T>::Array(const size_t val, const bool sec) {
                data = new T[val];
                size = 0;
                capacity = val;
                secure = sec;
        }

        /*! \brief Kopierkonstruktor: Initialisiert eine neue Instanz der Klasse __Array<T>__ und kopiert die Daten von einer bereits existierenden Instanz in diese neue Instanz.
         *
         * \param ref Enthält die Referenz zu einer bereits existierenden Instanz aus der kopiert werden soll.
         */
        template<typename T>
        Array<T>::Array(const Array<T> &ref) {
                capacity = ref.capacity;
                size = ref.size;
                secure = ref.secure;
                data = new T[capacity];
                for (size_t i = 0; i < size; i++) {
                        data[i] = ref.data[i];
                }
        }

        /*! \brief Destruktor: Zerstört die existierende Instanz der Klasse __Array<T>__.
         *
         * Wenn der Parameter _sec_ bei der Erstellung dieser Instanz auf _true_ gesetzt war, wird der Heapspeicher vor seiner Freigabe gelöscht.
         */
        template<typename T>
        Array<T>::~Array() {
                if (data != nullptr) {
                        if (secure) {
                                for (size_t i = 0; i < capacity; i++) {
                                        data[i] = 0;
                                }
                        }
                        delete[] data;
                        data = nullptr;
                }
        }

        /*! \brief Diese Methode `Append()` fügt ein Datenobjekt vom Typ __T__ hinter den schon vorhandenen Datenobjekten hinzu.
         *
         * Die Methode prüft zunächst, ob in der Array-Instanz noch Platz vorhanden ist. Ist dies nicht der Fall, verdoppelt es zunächst die Kapazität der Array-Instanz.
         * Anschließend fügt die Methode das neue Datenobjekt hinter den schon vorhandenen Datenobjekten hinzu.
         *
         * \param ref Verweis auf das Datenobjekt, das hinzugefügt werden soll.
         */
        template<typename T>
        void Array<T>::Append(const T &ref) {
                if (size == capacity) {
                        IncreaseCapacity(0);
                }
                data[size] = ref;
                size++;
        }

        /*! \brief Diese Methode `Append()` fügt die Datenobjekte einer anderen Array-Instanz hinter den schon vorhandenen Datenobjekten dieser Array-Instanz hinzu.
         *
         * Die Methode prüft zunächst, ob in dieser Array-Instanz noch genügend Platz vorhanden ist. Falls nicht, wird zunächst die Kapazität dieser Array-Instanz erhöht
         * (und mindestens verdoppelt). Anschließend fügt die Methode die neuen Datenobjekte hinter den schon vorhandenen Datenobjekten hinzu.
         *
         * \param ref Verweis auf die Array-Instanz, aus der gelesen werden soll und deren Datenobjekte hinzugefügt werden sollen.
         */
        template<typename T>
        void Array<T>::Append(const Array<T> &ref) {
                size_t length = ref.size;
                size_t newLength = this->size + length;
                while (newLength > this->capacity) {
                        IncreaseCapacity(newLength);
                }
                for (size_t i = 0; i < length; i++) {
                        data[this->size + i] = ref.data[i];
                }
                this->size = newLength;
        }

        /*! \brief Diese Methode `At()` gibt das Datenobjekt zurück, das unter dem nullbasierten Index der Array-Instanz vorhanden ist.
         *
         * Die Methode prüft zunächst, ob der Indexwert innerhalb eines gültigen Wertebereichs liegt (auf ein bereits verwaltetes Datenobjekt verweist).
         * Wenn nicht, wird eine Ausnahme (_out_of_range("Index")_) ausgelöst. Falls ja, wird das gewünschte Datenobjekt zurückgegeben.
         *
         * \param index Der Index im Datenbereich, dessen Datenobjekt zurückgegeben werden soll.
         * \return Die Instanz eines Datenobjekts vom Typ T.
         * \sa operator[]()
         */
        template<typename T>
        T &Array<T>::At(const size_t index) const {
                if (index >= size) {
                        throw std::out_of_range("Index");
                } else {
                        return data[index];
                }
        }


        /*! \brief Diese Methode `Capacity()` gibt die aktuelle Kapazität der Array-Instanz zurück.
         *  \return Die Anzahl der verwaltbaren Datenobjekte.
         */
        template<typename T>
        inline size_t Array<T>::Capacity() const { return capacity; }


        /*! \brief Diese Methode `Clear()` setzt die Anzahl der verwalteten Datenobjekte auf 0.
         *
         * Nach dem Aufruf der Methode `Clear()` beträgt die Anzahl der verwalteten Datenobjekte 0.
         * Die Kapazität der Array-Instanz bleibt aber unverändert. Wenn der Parameter _sec_ bei der Erstellung dieser Instanz auf __true__
         * gesetzt war, wird der Heapspeicher gelöscht aber nicht freigegeben.
         */
        template<typename T>
        void Array<T>::Clear() {
                size = 0;
                if (secure) {
                        for (size_t i = 0; i < capacity; i++) {
                                data[i] = 0;
                        }
                }
        }


        /*! \brief Diese Methode `Copy()` kopiert Datenobjekte von einer anderen Array-Instanz in diese Array-Instanz.
         *
         * Die Methode prüft zunächst, ob die Quell- und Zielindexwerte innerhalb der zulässigen Wertebereiche liegen.
         * Wenn nicht, wird eine Ausnahme (_out_of_range("Index")_) ausgelöst.
         * Die Methode prüft anschließend, ob die Kapazität im Datenfeld für den Schreibvorgang ausreicht.
         * Wenn nicht, wird die Kapazität in dieser Array-Instanz erhöht.
         * Danach erfolgt der eigentliche Kopiervorgang. Falls erforderlich, wird die Variable _size_ angepasst.
         *
         * \param A Verweis auf die Instanz des Quell-Arrays, aus dem kopiert werden soll.
         * \param sindex Der nullbasierte Quellindex in der Quell-Array-Instanz, aus der kopiert werden soll.
         * \param tindex Der nullbasierte Zielindex in dieser Array-Instanz, in die geschrieben werden soll.
         * \param val Die Anzahl der zu kopierenden Datenobjekte.
         */
        template<typename T>
        void Array<T>::Copy(const Array<T> &A, const size_t sindex, const size_t tindex, const size_t val) {
                if ((sindex + val) > A.size) {
                        throw std::out_of_range("Source Index");
                }
                if (tindex > size) {
                        throw std::out_of_range("Target Index");
                }
                size_t newLength = tindex + val;
                if (newLength > capacity) {
                        IncreaseCapacity(newLength);
                }
                for (size_t i = 0; i < val; i++) {
                        data[tindex + i] = A.data[sindex + i];
                }
                if (size < (tindex + val)) {
                        size = tindex + val;
                }
        }

        /*! \brief Diese private Methode `IncreaseCapacity()` erhöht die Kapazität der verwaltbaren Datenobjekte in der Array-Instanz.
         *
         * Die Methode reserviert neuen Speicher (der der neuen Kapazitätsgröße entspricht, aber mindestens doppelt so groß ist) aus dem Heap,
         * kopiert die bereits verwalteten Datenobjekte in den neuen Speicher und gibt den alten Speicher auf dem Heap frei. Wenn der Parameter _sec_ bei
         * der Erstellung dieser Instanz auf __true__ gesetzt war, wird der alte Heapspeicher vor seiner Freigabe gelöscht.
         *
         * \param length Die neue gewünschte Mindestgröße für Datenobjekte vom Typ __T__.
         */
        template<typename T>
        void Array<T>::IncreaseCapacity(size_t length) {
                size_t newlength = capacity << 1;
                if (newlength < length) {
                        newlength = length;
                }
                T *tmp = new T[newlength];
                for (size_t i = 0; i < size; i++) {
                        tmp[i] = data[i];
                }
                if (secure) {
                        for (size_t i = 0; i < capacity; i++) {
                                data[i] = 0;
                        }
                }
                delete[] data;
                data = tmp;
                capacity = newlength;
        }

        /*! \brief Diese Methode `IndexOf()` gibt den nullbasierten Index des ersten vorhandenen Verweises aus der Array-Instanz zurück.
         *
         * Es wird nach dem gewünschten Verweis ab dem Anfang in der verwalteten Liste gesucht.
         * Falls er gefunden werden kann, wird sein nullbasierter Index zurückgegeben.
         * Wurde der Verweis nicht gefunden, wird -1 zurückgegeben.
         *
         * \param ref Enthält den gesuchten Verweis.
         * \return Der nullbasierter Index oder -1, wenn der Verweis nicht gefunden werden konnte.
         */
        template<typename T>
        int  Array<T>::IndexOf(const T &ref) const {
                for (size_t i = 0; i < size; i++) {
                        if (data[i] == ref) {
                                return static_cast<int>(i);
                        }
                }
                return -1;
        }

        /*! \brief Diese Methode `Insert()` fügt ein Datenobjekt an der angegebenen (nullbasierten) Indexposition in die Array-Instanz ein.
         *
         * Die Methode prüft zunächst, ob in der Array-Instanz noch Platz vorhanden ist. Ist dies nicht der Fall, verdoppelt es zunächst die Kapazität der Array-Instanz.
         * Die Methode prüft anschließend, ob der Indexwert innerhalb eines gültigen Wertebereichs liegt.
         * Wenn nicht, wird eine Ausnahme (_out_of_range("Index")_) ausgelöst.
         * Falls ja, werden etwaige nachfolgende Datenobjekte im Datenbereich um eine Position nach hinten verschoben, und das neue Datenobjekt wird an der gewünschten
         * Indexposition eingefügt.
         *
         * \param index Der Index in der Liste, an dem das Datenobjekt eingefügt werden soll.
         * \param ref Verweis auf das Datenobjekt, das in diese Array-Instanz eingefügt werden soll.
         */
        template<typename T>
        void Array<T>::Insert(const size_t index, const T &ref) {
                if (size == capacity) {
                        IncreaseCapacity(0);
                }
                if (index > size) {
                        throw std::out_of_range("Index");
                } else {
                        for (size_t j = size; j > index; j--) {
                                data[j] = data[j - 1];
                        }
                        data[index] = ref;
                        size++;
                }
        }

        /*! \brief Diese Methode `IsEqual()` vergleicht einen Datenbereich aus der übergebenen Array-Instanz mit einem Datenbereich in dieser Array-Instanz.
         *
         * Die Methode vergleicht einen beliebigen Datenbereich aus der eigenen Instanz mit einem Datenbereich aus einer übergebenen Instanz.
         * Bei der ersten Nichtübereinstimmung zweier Objekte wird die Prüfung abgebrochen und _false_ zurückgegeben.
         * Nur wenn beide Datenbereiche vollständig übereinstimmen, wird _true_ zurückgegeben.
         *
         * \note Da beide Datenbereiche nur gelesen werden, wird nicht geprüft, ob sich der Leseindex auch immer innerhalb der verwalteten Datenbereiche befindet.
         *
         * \param A Der Verweis auf die Array-Instanz, mit der verglichen werden soll.
         * \param sindex Der nullbasierte Index in der übergebenen Array-Instanz, ab dem der Vergleich erfolgen soll.
         * \param tindex Der nullbasierte Index in der eigenen Array-Instanz, ab dem der Vergleich erfolgen soll.
         * \param val Die Anzahl der zu vergleichenden Datenobjekte.
         * \return _true_, wenn beide Datenbereiche gleich sind (ansonsten _false_).
         */
        template<typename T>
        bool Array<T>::IsEqual(const Array<T> &A, const size_t sindex, const size_t tindex, const size_t val) const {
                for (size_t i = 0; i < val; i++) {
                        if (data[tindex + i] != A.data[sindex + i]) {
                                return false;
                        }
                }
                return true;
        }

        /*! \brief Diese Methode `IsSecure()` gibt _true_ zurück, wenn die Daten auf dem Heap gelöscht werden, bevor sie freigegeben werden.
         *
         * \return _true_, wenn sicher (ansonsten _false_).
         * \sa { Konstruktor }
         */
        template<typename T>
        inline bool Array<T>::IsSecure() const { return secure; }

        /*! \brief Gibt das Datenobjekt zurück, das unter dem nullbasierten Index der Array-Instanz vorhanden ist.
         *
         * \note Die Methode führt keine Indexprüfung durch. Liegt der Indexwert außerhalb des gültigen Wertebereichs,
         * kann möglicherweise auf nicht zugewiesenen Speicher zugegriffen werden.
         *
         * \param index Der Index des Datenobjekts, das zurückgegeben werden soll.
         * \return Instanz eines Datenobjekts vom Typ __T__.
         * \sa At()
         */
        template<typename T>
        inline T &Array<T>::operator[](size_t index) { return data[index]; }


        /*! \brief Diese Methode `RemoveAt()` löscht ein Datenobjekt an einer bestimmten Indexposition aus der Array-Instanz.
         *
         * Die Methode prüft zunächst, ob der Indexwert innerhalb eines gültigen Wertebereichs liegt (verweist auf ein Datenobjekt, das bereits verwaltet wird).
         * Wenn nicht, wird eine Ausnahme (_out_of_range("Index")_) ausgelöst.
         * Anschließend werden, falls erforderlich, nachfolgende Datenobjekte im Datenfeld der Indexposition vorgezogen, und die Variable _size_ wird um eins verringert.
         *
         * \param index Der Index im Datenfeld, in dem das Datenobjekt gelöscht werden soll.
         */
        template<typename T>
        void Array<T>::RemoveAt(const size_t index) {
                if (index >= size) {
                        throw std::out_of_range("Index");
                } else {
                        for (size_t i = index + 1; i < size; i++) {
                                data[i - 1] = data[i];
                        }
                        size--;
                }
        }

        /*! \brief Diese Methode `SetCapacity()` ändert die Kapazität (die Anzahl der verwaltbaren Datenobjekte) in der Array-Instanz.
         *
         * Die Methode prüft zunächst, ob der übergebene Kapazitätswert größer ist als die Anzahl der derzeit verwalteten Datenobjekte.
         * Wenn nicht, wird eine Ausnahme (_out_of_range("size")_) ausgelöst.
         * Anschließend wird neuer Speicher (mit der gewünschten Kapazität) aus dem Heap zugewiesen, die bereits verwalteten Daten werden in den neuen Speicher
         * kopiert, und der alte Speicherplatz auf dem Heap wird wieder freigegeben. Wenn der Parameter _sec_ bei
         * der Erstellung dieser Instanz auf __true__ gesetzt war, wird der alte Heapspeicher vor seiner Freigabe gelöscht.
         *
         * \param val Die neue Kapazitätsgröße der Array-Instanz.
         */
        template<typename T>
        void Array<T>::SetCapacity(const size_t val) {
                if (val <= size) {
                        throw std::out_of_range("size");
                } else {
                        T *tmp = new T[val];
                        for (size_t i = 0; i < size; i++) {
                                tmp[i] = data[i];
                        }
                        if (secure) {
                                for (size_t i = 0; i < capacity; i++) {
                                        data[i] = 0;
                                }
                        }
                        delete[] data;
                        data = tmp;
                        capacity = val;
                }
        }

        /*! \brief Diese Methode `SetSize()` setzt die Anzahl der verwalteten Datenobjekte in der Array-Instanz auf einen neuen Wert.
         *
         * Die Methode prüft zunächst, ob der übergebene Wert größer ist als die aktuelle Kapazität.
         * Wenn ja, wird eine Ausnahme (_out_of_range("size")_) ausgelöst.
         * Anschließend wird die Variable _size_ auf den übergebenen Wert gesetzt.
         *
         * \param val Die neue Anzahl der verwalteten Datenobjekte.
         */
        template<typename T>
        void Array<T>::SetSize(const size_t val) {
                if (val <= capacity) {
                        size = val;
                } else {
                        throw std::out_of_range("size");
                }

        }

        /*! \brief Diese Methode `Size()` gibt die Anzahl der in der Array-Instanz verwalteten Datenobjekte zurück.
         *  \return Die Anzahl der verwalteten Datenobjekte.
         */
        template<typename T>
        inline size_t Array<T>::Size() const { return size; }


        /*! \brief Diese Methode `TestCapacity()` prüft, ob die Kapazität (die Anzahl der verwaltbaren Datenobjekte) in der Array-Instanz mindestens den erforderlichen Mindestwert aufweist.
         *
         * Wenn die aktuelle Kapazität der Array-Instanz unter dem erforderlichen Mindestwert liegt, wird sie erhöht.
         *
         * \param val Die erforderliche Mindestkapazität der Array-Instanz.
         */
        template<typename T>
        void Array<T>::TestCapacity(const size_t val) {
                if (val <= capacity) {
                        return;
                }
                IncreaseCapacity(val);
        }

        /*! \brief Diese Methode `WriteareaReference()` garantiert die gewünschte Feldgröße und gibt eine Referenz (zum Schreiben) in die Array-Instanz zurück.
         *
         * Die Methode prüft zunächst, ob die Kapazität im verwalteten Datenbereich für den Schreibvorgang ausreicht.
         * Ist dies nicht der Fall, wird die Kapazität erhöht (zumindest verdoppelt), bis sie ausreicht.
         * Ist der Schreibindex am Ende des Vorgangs größer als die Variable _size_, wird _size_ auf das Schreibende gesetzt.
         *
         * \param index Nullbasierter Schreibindex im Datenbereich, in dem geschrieben werden soll.
         * \param val Anzahl der zu schreibenden Datenobjekte.
         * \return Verweis auf die Addresse im Datenbereich, ab der geschrieben werden soll.
         */
        template<typename T>
        T &Array<T>::WriteareaReferenz(const size_t index, const size_t val) {
                size_t newLength = index + val;
                if (newLength > capacity) {
                        IncreaseCapacity(newLength);
                }
                size = index + val;
                return data[index];
        }

} // end of namespace RH

#endif // RH_ARRAY_T
